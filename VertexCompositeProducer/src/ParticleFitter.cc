// -*- C++ -*-
//
// Package:    ParticleProducer
// Class:      ParticleFitter
// 
/**\class ParticleFitter ParticleFitter.cc VertexCompositeAnalysis/VertexCompositeProducer/src/ParticleFitter.cc
//
*/

#include "VertexCompositeAnalysis/VertexCompositeProducer/interface/ParticleFitter.h"


// ParticleFitter: constructor and (empty) destructor
ParticleFitter::ParticleFitter(const edm::ParameterSet& theParameters, edm::ConsumesCollector && iC) :
  pdgId_(theParameters.getParameter<int>("pdgId")),
  preSelection_(theParameters.getParameter<std::string>("preSelection")),
  postSelection_(theParameters.getParameter<std::string>("postSelection")),
  finalSelection_(theParameters.getParameter<std::string>("finalSelection"))
{
  // get daughter information
  const auto daughterVPset = theParameters.getParameter<std::vector<edm::ParameterSet> >("daughterInfo");
  for (const auto& pSet : daughterVPset) {
    ParticleDaughter daughter;
    daughter.fillInfo(pSet, theParameters, iC);
    daughters_.push_back(daughter);
  }

  // get input tags
  token_beamSpot_ = iC.consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));
  token_vertices_ = iC.consumes<reco::VertexCollection>(theParameters.getParameter<edm::InputTag>("primaryVertices"));
  token_electrons_ = iC.consumes<pat::ElectronCollection>(theParameters.getParameter<edm::InputTag>("electrons"));
  token_muons_ = iC.consumes<pat::MuonCollection>(theParameters.getParameter<edm::InputTag>("muons"));
  token_taus_ = iC.consumes<pat::TauCollection>(theParameters.getParameter<edm::InputTag>("taus"));
  token_photons_ = iC.consumes<pat::PhotonCollection>(theParameters.getParameter<edm::InputTag>("photons"));
  token_tracks_ = iC.consumes<reco::TrackCollection>(theParameters.getParameter<edm::InputTag>("tracks"));
  token_pfParticles_ = iC.consumes<reco::PFCandidateCollection>(theParameters.getParameter<edm::InputTag>("pfParticles"));
  token_jets_ = iC.consumes<pat::JetCollection>(theParameters.getParameter<edm::InputTag>("jets"));

  // initialize attributes
  fitDone_ = false;
  vertex_ = reco::Vertex();
  beamSpot2D_ = reco::Vertex();
  candidates_ = {};
};


ParticleFitter::~ParticleFitter() {
};


// Method containing the algorithm for vertex reconstruction
void ParticleFitter::fitAll(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // fill daughters particles
  fillDaughters(iEvent);
  //std::cout << "daughters have been filled" << std::endl;
  // create candidates
  makeCandidates();
  //std::cout << "candidates have beeen made" << std::endl;
  // fit candidates
  fitCandidates(iSetup);
  //std::cout << "candidates have beeen fitted" << std::endl;
  // add extra information and make final selection
  selectCandidates();
  //std::cout << "post selectoins for candidates have beeen done" << std::endl;
};


void ParticleFitter::clear() {
  fitDone_ = false;
  vertex_ = reco::Vertex();
  beamSpot2D_ = reco::Vertex();
  candidates_.clear();
  std::for_each(daughters_.begin(), daughters_.end(), [](ParticleDaughter &d){ d.clear(); });
};


void ParticleFitter::setVertex(const edm::Event& iEvent) {
  // extract the input collections
  edm::Handle<reco::VertexCollection> vertexHandle;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(token_vertices_, vertexHandle);
  iEvent.getByToken(token_beamSpot_, beamSpotHandle);
  // set the primary vertex
  const auto beamSpotVertex = reco::Vertex(beamSpotHandle->position(), beamSpotHandle->rotatedCovariance3D());
  const auto primaryVertex = (vertexHandle->size()>0 ? vertexHandle->at(0) : reco::Vertex());
  const bool isGoodVtx = !primaryVertex.isFake() && primaryVertex.tracksSize()>=2;
  vertex_ = (isGoodVtx ? primaryVertex : beamSpotVertex);
  // set the 2D beam spot
  const auto beamSpot2DValue = reco::Vertex::Point(beamSpotHandle->position().x(), beamSpotHandle->position().y(), 0.);
  reco::Vertex::Error beamSpot2DError;
  beamSpot2DError(0,0) = std::pow(beamSpotHandle->BeamWidthX(), 2.);
  beamSpot2DError(1,1) = std::pow(beamSpotHandle->BeamWidthY(), 2.);
  beamSpot2D_ = reco::Vertex(beamSpot2DValue, beamSpot2DError);
};


void ParticleFitter::addParticles(ParticleDaughter& d, const edm::Event& iEvent) {
  const auto pdgId = std::abs(d.pdgId());
  const auto charge = d.charge();
  const auto& vertex = (vertex_.isFake() ? beamSpot2D_ : vertex_);
  if (d.useSource()) { d.addParticles(iEvent); }
  else if (pdgId==0 ) { d.addParticles(iEvent, token_pfParticles_, vertex); }
  else if (pdgId<=6 ) { d.addParticles(iEvent, token_jets_, vertex);        }
  else if (pdgId==11) { d.addParticles(iEvent, token_electrons_, vertex);   }
  else if (pdgId==13) { d.addParticles(iEvent, token_muons_, vertex);       }
  else if (pdgId==15) { d.addParticles(iEvent, token_taus_, vertex);        }
  else if (pdgId==22) { d.addParticles(iEvent, token_photons_, vertex);     }
  else if (charge!=0) { d.addParticles(iEvent, token_tracks_, vertex);      }
  else                { d.addParticles(iEvent, token_pfParticles_, vertex); }
};


void ParticleFitter::fillDaughters(const edm::Event& iEvent) {
  for (auto& daughter : daughters_) {
    addParticles(daughter, iEvent);
  }
};


void ParticleFitter::makeCandidates() {
  // define daughter container
  const auto nDaughters = daughters_.size();
  std::vector<pat::GenericParticleCollection> daughterColls;
  for (const auto& daughter : daughters_) {
    if (daughter.particles().empty()) continue;
    daughterColls.push_back(daughter.particles());
  }
  if (daughterColls.size()!=nDaughters) return;
  // make daughter combinations
  const auto combinations = make_combinations(daughterColls);
  if (combinations.empty()) return;
  // loop over all combinations of daughters
  ParticleSet candidates;
  for (const auto& combination : combinations) {
    pat::GenericParticle cand;
    auto charge = cand.charge();
    auto p4 = cand.p4();
    ParticleDaughterSet daughters;
    float tkPtSum = 0;
    for (const auto& daughter : combination) {
      charge += daughter->charge();
      p4 += daughter->p4();
      tkPtSum += daughter->pt();
      daughters.insert(*daughter);
    }
    // check if all daughters are unique (are in set)
    if (daughters.size()==nDaughters) {
      cand.setCharge(charge);
      cand.setP4(p4);
      cand.setPdgId(pdgId_);
      cand.addUserFloat("tkPtSum", tkPtSum);

      if (nDaughters == 2) {
         auto it2 = daughters.cbegin();
         auto it1 = it2++;
         
         const float tkEtaDiff = std::fabs(it2->eta() - it1->eta());
         cand.addUserFloat("tkEtaDiff", tkEtaDiff);

      }

      if (preSelection_(cand)) {
        pat::GenericParticleCollection daughterColl(daughters.begin(), daughters.end());
        cand.addUserData<pat::GenericParticleCollection>("daughters", daughterColl);
        cand.addUserData<reco::Vertex>("primaryVertex", vertex_);
        candidates.insert(cand);
      }
    }
  }
  // insert all unique candidates
  candidates_.assign(candidates.begin(), candidates.end());
};


void ParticleFitter::fitCandidates(const edm::EventSetup& iSetup) {
  if (candidates_.empty() || daughters_.size()<2) return;
  // get the magnetic field
  edm::ESHandle<MagneticField> bFieldHandle;
  iSetup.get<IdealMagneticFieldRecord>().get(bFieldHandle);
  const auto& magField = bFieldHandle.product();
  // loop over candidates
  pat::GenericParticleCollection candidates;
  for (auto cand : candidates_) {
    // get the daughters transient tracks
    auto& daughterColl = *const_cast<pat::GenericParticleCollection*>(cand.userData<pat::GenericParticleCollection>("daughters"));
    std::vector<std::pair<reco::TransientTrack, size_t> > daughters;
    for (size_t iDau=0; iDau<daughterColl.size(); iDau++) {
      const auto& trk = daughterColl[iDau].track();
      if (trk.isNonnull()) {
        daughters.push_back({reco::TransientTrack(*trk, magField), iDau});
      }
      else if (daughterColl[iDau].hasUserData("state")) {
        daughters.push_back({reco::TransientTrack(), iDau});
      }
    }
    if (daughters.size()<2) return;
    // measure distance between daughter tracks at their closest approach
    if (daughters.size()==2 &&
        daughters[0].first.impactPointTSCP().isValid() &&
        daughters[1].first.impactPointTSCP().isValid()) {
      ClosestApproachInRPhi cApp;
      const auto& stateDau1 = daughters[0].first.impactPointTSCP().theState();
      const auto& stateDau2 = daughters[1].first.impactPointTSCP().theState();
      cApp.calculate(stateDau1, stateDau2);
      if (!cApp.status()) continue;
      const auto& cxPt = cApp.crossingPoint();
      const auto& tscpDau1 = daughters[0].first.trajectoryStateClosestToPoint(cxPt);
      const auto& tscpDau2 = daughters[1].first.trajectoryStateClosestToPoint(cxPt);
      if (!tscpDau1.isValid() || !tscpDau2.isValid()) continue;
      cand.addUserFloat("dca", cApp.distance());
    }
    // prepare particles for decay vertex fit
    std::vector<RefCountedKinematicParticle> particles;
    for (const auto& d : daughters) {
      const auto& daughter = daughterColl[d.second];
      if (d.first.impactPointTSCP().isValid()) {
        float chi = 0., ndf = 0., width = daughter.userFloat("width");
        KinematicParticleFactoryFromTransientTrack pFactory;
        particles.push_back(pFactory.particle(d.first, daughter.mass(), chi, ndf, width));
      }
      else {
        float chi = 0.0, ndf = 0.0;
        const auto& state = *daughter.userData<KinematicState>("state");
        VirtualKinematicParticleFactory pFactory;
        particles.push_back(pFactory.particle(state, chi, ndf, NULL));
      }
    }
    // fit decay vertex
    KinematicParticleVertexFitter fitter;
    const auto& result = fitter.fit(particles);
    if (!result->isValid()) continue;
    const auto& fitVertex = result->currentDecayVertex();
    if (!fitVertex->vertexIsValid()) continue;
    // check fitted final state particle
    const auto& candState = result->currentParticle()->currentState();
    if (!candState.isValid()) continue;
    // check fitted daughter particles
    std::vector<RefCountedKinematicParticle> daughterParticles;
    for (const auto& p : result->daughterParticles()) {
      if (p->currentState().isValid()) {
        daughterParticles.push_back(p);
      }
    }
    if (daughterParticles.size()!=daughters.size()) continue;
    // update particle kinematics from fit
    cand.addUserData<reco::Candidate::PolarLorentzVector>("initialP4", cand.polarP4());
    for (size_t iDau=0; iDau<daughters.size(); iDau++) {
      auto& daughter = daughterColl[daughters[iDau].second];
      const auto iniP4 = daughter.polarP4();
      const auto fitKP = daughterParticles[iDau]->currentState().kinematicParameters();
      const auto fitP4 = reco::Candidate::PolarLorentzVector(fitKP.momentum().perp(), fitKP.momentum().eta(), fitKP.momentum().phi(), fitKP.mass());
      daughter.addUserData<reco::Candidate::PolarLorentzVector>("initialP4", iniP4);
      daughter.setP4(fitP4);
      cand.setP4((cand.polarP4() + fitP4) - iniP4);
    }
    // set data
    const auto decayVertexPos = reco::Particle::Point(fitVertex->position().x(), fitVertex->position().y(), fitVertex->position().z());
    reco::Vertex decayVertex(decayVertexPos, fitVertex->error().matrix(), fitVertex->chiSquared(), fitVertex->degreesOfFreedom(), daughters.size());
    cand.setVertex(decayVertexPos);
    cand.addUserFloat("vertexProb", TMath::Prob(decayVertex.chi2(), decayVertex.ndof()));
    cand.addUserData<reco::Vertex>("decayVertex", decayVertex);
    cand.addUserData<KinematicState>("state", candState);
    // add fitted candidates
    if (postSelection_(cand)) {
      candidates.push_back(cand);
    }
  }
  candidates_ = candidates;
  fitDone_ = true;
};


void ParticleFitter::selectCandidates() {
  if (!candidates_.empty() || !fitDone_) return;
  // loop over candidates
  pat::GenericParticleCollection candidates;
  for (auto cand : candidates_) {
    if (!cand.hasUserData("decayVertex") || !cand.hasUserData("primaryVertex")) continue;
    const auto& decayVertex = *cand.userData<reco::Vertex>("decayVertex");
    const auto& primaryVertex = *cand.userData<reco::Vertex>("primaryVertex");
    // compute lifetime information
    const auto lineOfFlight = decayVertex.position() - primaryVertex.position();
    const auto lVtxMag = lineOfFlight.r();
    const auto rVtxMag = lineOfFlight.rho();
    const auto angle3D = angle(lineOfFlight.x(), lineOfFlight.y(), lineOfFlight.z(), cand.px(), cand.py(), cand.pz());
    const auto angle2D = angle(lineOfFlight.x(), lineOfFlight.y(), 0.0, cand.px(), cand.py(), 0.0);
    const auto distanceVector3D = SVector3(lineOfFlight.x(), lineOfFlight.y(), lineOfFlight.z());
    const auto distanceVector2D = SVector3(lineOfFlight.x(), lineOfFlight.y(), 0.0);
    const auto totalCov = decayVertex.covariance() + primaryVertex.covariance();
    const auto sigmaLvtxMag = std::sqrt(ROOT::Math::Similarity(totalCov, distanceVector3D)) / lVtxMag;
    const auto sigmaRvtxMag = std::sqrt(ROOT::Math::Similarity(totalCov, distanceVector2D)) / rVtxMag;
    // set lifetime infomation
    cand.addUserFloat("lVtxMag", lVtxMag);
    cand.addUserFloat("rVtxMag", rVtxMag);
    cand.addUserFloat("lVtxSig", (lVtxMag/sigmaLvtxMag));
    cand.addUserFloat("rVtxSig", (rVtxMag/sigmaRvtxMag));
    cand.addUserFloat("angle3D", angle3D);
    cand.addUserFloat("angle2D", angle2D);
    cand.addUserFloat("collinearity3D", std::cos(angle3D));
    cand.addUserFloat("collinearity2D", std::cos(angle2D));
    // select final candidates
    if (finalSelection_(cand)) {
      candidates.push_back(cand);
    }
  }
  candidates_ = candidates;
};


// ParticleDaughter: constructor and (empty) destructor
ParticleDaughter::ParticleDaughter()
{
  pdgId_ = 0;
  charge_ = -99;
  mass_ = 0.;
  selection_ = "";
  finalSelection_ = "";
  source_ = edm::EDGetTokenT<pat::GenericParticleCollection>();
  particles_ = {}; 
};


ParticleDaughter::ParticleDaughter(const edm::ParameterSet& pSet, const edm::ParameterSet& config, edm::ConsumesCollector&& iC) :
  ParticleDaughter()
{
  fillInfo(pSet, config, iC);
};


ParticleDaughter::~ParticleDaughter() {
};

void ParticleDaughter::clear() {
  particles_.clear();
};


void ParticleDaughter::fillInfo(const edm::ParameterSet& pSet, const edm::ParameterSet& config, edm::ConsumesCollector& iC) { 
  if (pSet.existsAs<int>("pdgId")) {
    pdgId_ = pSet.getParameter<int>("pdgId");
  }
  if (pSet.existsAs<int>("charge")) {
    charge_ = pSet.getParameter<int>("charge");
  }
  if (pSet.existsAs<double>("mass")) {
    mass_ = pSet.getParameter<double>("mass");
  }
  else if (MASS_.count(pdgId_)) {
    mass_ = MASS_.at(pdgId_);
  }
  if (pSet.existsAs<std::string>("selection")) {
    selection_ = pSet.getParameter<std::string>("selection");
  }
  if (pSet.existsAs<std::string>("finalSelection")) {
    finalSelection_ = pSet.getParameter<std::string>("finalSelection");
  }
  if (pSet.existsAs<edm::InputTag>("source")) {
    source_ = iC.consumes<pat::GenericParticleCollection>(pSet.getParameter<edm::InputTag>("source"));
  }
  if (pSet.existsAs<bool>("usePID")) {
    usePID_ = pSet.getParameter<bool>("usePID");
  } else {
    usePID_ =false;
  }
  if (pSet.existsAs<edm::InputTag>("dedxHarmonic2")) {
    token_dedx_ = iC.consumes<edm::ValueMap<reco::DeDxData>>(pSet.getParameter<edm::InputTag>("dedxHarmonic2"));
  } 
  else if (config.existsAs<edm::InputTag>("dedxHarmonic2")) {
    token_dedx_ = iC.consumes<edm::ValueMap<reco::DeDxData>>(config.getParameter<edm::InputTag>("dedxHarmonic2"));
  }
  if (pSet.existsAs<edm::InputTag>("mvaTrackRecoSrc")) {
     mvaTrackRecoSrc_ = iC.consumes<std::vector<float>>(pSet.getParameter<edm::InputTag>("mvaTrackRecoSrc"));
  }
  else if (config.existsAs<edm::InputTag>("mvaTrackRecoSrc")) {
     mvaTrackRecoSrc_ = iC.consumes<std::vector<float>>(config.getParameter<edm::InputTag>("mvaTrackRecoSrc"));
  }
};


template <class T>
void ParticleDaughter::addParticles(const edm::Event& event, const edm::EDGetTokenT<std::vector<T> >& token, const reco::Vertex& vertex, const bool embedInfo) {
  edm::Handle<std::vector<T> > handle;
  event.getByToken(token, handle);

  edm::Handle<std::vector<float>> mvaTrackRecoOutputHandle;
  if (!mvaTrackRecoSrc_.isUninitialized()) event.getByToken(mvaTrackRecoSrc_, mvaTrackRecoOutputHandle);

  edm::Handle<edm::ValueMap<reco::DeDxData> > dEdxHandle;
  if (!token_dedx_.isUninitialized()) event.getByToken(token_dedx_, dEdxHandle);

  StringCutObjectSelector<T, true> selection(selection_);
  StringCutObjectSelector<pat::GenericParticle, true> finalSelection(finalSelection_);
  if (handle.isValid()) {
    ParticleSet particles;
    for (size_t i=0; i<handle->size(); i++) {
      const auto& p = edm::Ref<std::vector<T> >(&(*handle), i);
      if (!selection(*p)) continue;
      if (charge_!=-99 && p->charge()!=charge_) continue;
      const auto p4 = reco::Candidate::PolarLorentzVector(p->pt(), p->eta(), p->phi(), mass_);
      pat::GenericParticle cand;
      cand.setPdgId(pdgId_);
      cand.setP4(p4);
      cand.setCharge(p->charge());
      cand.setVertex(p->vertex());
      addData(cand, p, embedInfo);
      if (cand.track().isNonnull()) {
        const auto& trk = *cand.track();
        const float dz = trk.dz(vertex.position());
        const float dxy = trk.dxy(vertex.position());
        const float dzerror  = std::sqrt(trk.dzError()*trk.dzError()+vertex.zError()*vertex.zError());
        const float dxyerror = std::sqrt(trk.d0Error()*trk.d0Error()+vertex.xError()*vertex.yError());
        cand.addUserFloat("dz", dz);
        cand.addUserFloat("dxy", dxy);
        cand.addUserFloat("dzSig", dz/dzerror);
        cand.addUserFloat("dxySig", dxy/dxyerror);

        if (mvaTrackRecoOutputHandle.isValid()) {
          const float mvaTrackReco = (*mvaTrackRecoOutputHandle)[i];
          if(trk.algo() == 6){
            if(mvaTrackReco < 0.98) continue;
            //std::cout<<"mvaTrackReco : "<<mvaTrackReco<<std::endl;
          }
        }

        if (dEdxHandle.isValid()) {
          const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle.product();
          const float dedx_value = dEdxTrack[cand.track()].dEdx();
          cand.addUserFloat("dedx", dedx_value);
          // do PID with dedx
          if (usePID_) {
             // not complete
             cand.addUserInt("isProton", 0);
             cand.addUserInt("isKaon", 0);
             cand.addUserInt("isPion", 0);
          }
        } 
      }
      else if (cand.charge()!=0) continue; // ignore if charged particle has no track
      cand.addUserFloat("width", cand.mass()*1.e-6);
      if (finalSelection(cand)) {
        particles.insert(cand);
      }
    }
    particles_.assign(particles.begin(), particles.end());
  }
};


void ParticleDaughter::addParticles(const edm::Event& event) {
  edm::Handle<pat::GenericParticleCollection> handle;
  event.getByToken(source_, handle);
  StringCutObjectSelector<pat::GenericParticle, true> selection(selection_);
  StringCutObjectSelector<pat::GenericParticle, true> finalSelection(finalSelection_);
  if (handle.isValid()) {
    ParticleSet particles;
    for (const auto& p : *handle) {
      if (!selection(p)) continue;
      if (charge_!=-99 && p.charge()!=charge_) continue;
      if (finalSelection(p)) {
        particles.insert(p);
      }
    }
    particles_.assign(particles.begin(), particles.end());
  }
};


template <class T>
void ParticleDaughter::addData(pat::GenericParticle& c, const edm::Ref<std::vector<T> >& p, const bool& embedInfo) {
  c.addUserData<T>("src", *p);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const reco::TrackRef& p, const bool& embedInfo) {
  c.setTrack(p, embedInfo);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const reco::PFCandidateRef& p, const bool& embedInfo) {
  if (c.pdgId()==0) {
    c.setPdgId(p->pdgId());
  }
  c.setTrack(p->trackRef(), embedInfo);
  c.addUserData<reco::PFCandidate>("src", *p);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const pat::MuonRef& p, const bool& embedInfo) {
  c.setTrack(p->track(), embedInfo);
  c.setCombinedMuon(p->combinedMuon(), embedInfo);
  c.addUserData<pat::Muon>("src", *p);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const pat::ElectronRef& p, const bool& embedInfo) {
  c.setTrack(p->track(), embedInfo);
  c.setGsfTrack(p->gsfTrack(), embedInfo);
  c.setSuperCluster(p->superCluster(), embedInfo);
  c.setEcalIso(p->ecalIso());
  c.setHcalIso(p->hcalIso());
  c.addUserData<pat::Electron>("src", *p);
};
