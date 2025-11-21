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
  pdgId_(theParameters.getParameter<unsigned int>("pdgId")),
  doSwap_(theParameters.getParameter<bool>("doSwap")),
  matchVertex_(theParameters.getParameter<bool>("matchVertex")),
  vtxSortByTrkSize_(theParameters.getParameter<bool>("vtxSortByTrkSize")),
  fitAlgoV_(theParameters.getParameter<std::vector<UInt_t> >("fitAlgo")),
  puMap_(matchVertex_ ? theParameters.getParameter<std::vector<double> >("puMap") : std::vector<double>()),
  bField_esToken_(iC.esConsumes<MagneticField, IdealMagneticFieldRecord>()),
  token_beamSpot_(iC.consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"))),
  token_vertices_(iC.consumes<reco::VertexCollection>(theParameters.getParameter<edm::InputTag>("primaryVertices"))),
  token_tracks_(iC.consumes<reco::TrackCollection>(theParameters.getParameter<edm::InputTag>("tracks"))),
  token_pfParticles_(iC.consumes<reco::PFCandidateCollection>(theParameters.getParameter<edm::InputTag>("pfParticles"))),
  token_electrons_(iC.consumes<pat::ElectronCollection>(theParameters.getParameter<edm::InputTag>("electrons"))),
  token_muons_(iC.consumes<pat::MuonCollection>(theParameters.getParameter<edm::InputTag>("muons"))),
  token_taus_(iC.consumes<pat::TauCollection>(theParameters.getParameter<edm::InputTag>("taus"))),
  token_photons_(iC.consumes<pat::PhotonCollection>(theParameters.getParameter<edm::InputTag>("photons"))),
  token_convPhotons_(iC.consumes<reco::ConversionCollection>(theParameters.getParameter<edm::InputTag>("conversions"))),
  token_jets_(iC.consumes<pat::JetCollection>(theParameters.getParameter<edm::InputTag>("jets"))),
  token_met_(iC.consumes<pat::METCollection>(theParameters.getParameter<edm::InputTag>("met"))),
  preSelection_(theParameters.getParameter<std::string>("preSelection")),
  preMassSelection_(theParameters.getParameter<std::string>("preMassSelection")),
  pocaSelection_(theParameters.getParameter<std::string>("pocaSelection")),
  postSelection_(theParameters.getParameter<std::string>("postSelection")),
  finalSelection_(theParameters.getParameter<std::string>("finalSelection"))
{
  // get candidate information
  if (theParameters.existsAs<double>("mass")) {
    mass_ = theParameters.getParameter<double>("mass");
  }
  else if (MASS_.find(pdgId_)!=MASS_.end()) {
    mass_ = MASS_.at(pdgId_);
  }
  else { mass_ = -1; }
  if (doSwap_ && mass_<0.) { throw std::logic_error("[ERROR] Can't swap with undefined input mass!"); }
  if (theParameters.existsAs<double>("width")) {
    width_ = theParameters.getParameter<double>("width");
  }
  else { width_ = mass_*0.15; }

  // get daughter information
  const auto daughterVPset = theParameters.getParameter<std::vector<edm::ParameterSet> >("daughterInfo");
  daughters_.resize(daughterVPset.size());
  for (size_t i=0; i<daughterVPset.size(); i++) {
    daughters_[i].fillInfo(daughterVPset[i], theParameters, iC);
  }

  // initialize attributes
  vertex_ = reco::Vertex();
  beamSpot2D_ = reco::Vertex();
  beamSpot_ = reco::BeamSpot();
  candidates_ = {};
  daughterColl_ = {};
  vertices_ = {};
  priVertices_ = {};
  vertexRefMap_ = {};
  particleRefMap_ = {};
};


ParticleFitter::~ParticleFitter()
{
};

// Method containing the algorithm for vertex reconstruction
void ParticleFitter::fitAll(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // get the magnetic field
  bFieldHandle_ = iSetup.getHandle(bField_esToken_);
  // fill daughters particles
  fillDaughters(iEvent, iSetup);
  // create candidates
  makeCandidates();
};


void ParticleFitter::clear()
{
  vertex_ = reco::Vertex();
  beamSpot2D_ = reco::Vertex();
  beamSpot_ = reco::BeamSpot();
  clear(candidates_);
  clear(daughterColl_);
  clear(vertices_);
  clear(priVertices_);
  vertexRefMap_.clear();
  particleRefMap_.clear();
  std::for_each(daughters_.begin(), daughters_.end(), [](ParticleDaughter &d){ d.clear(); });
};


reco::VertexRef ParticleFitter::getVertexRef(const reco::Vertex& vertex)
{
  const VertexTuple tuple(vertex.x(), vertex.y(), vertex.z(), vertex.tracksSize(), !vertex.isFake());
  auto& ref = vertexRefMap_[tuple];
  if (ref.isNull()) {
    vertices_.emplace_back(vertex);
    ref = reco::VertexRef(vtxProd_, vertices_.size()-1);
  }
  return ref;
};


math::XYZTLorentzVector ParticleFitter::getP4(const GlobalVector& p, const double& m)
{
  const auto en = std::sqrt(p.mag2() + m*m);
  //const auto en = ROOT::Math::Mag(AlgebraicVector4(p.x(), p.y(), p.z(), m));
  return math::XYZTLorentzVector(p.x(), p.y(), p.z(), en);
};


void ParticleFitter::setVertex(const edm::Event& iEvent)
{
  // extract the input collections
  edm::Handle<reco::VertexCollection> vertexHandle;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(token_vertices_, vertexHandle);
  iEvent.getByToken(token_beamSpot_, beamSpotHandle);
  // initialize containers
  priVertices_.reserve(vertexHandle->size());
  // select primary vertices (sort based on track multiplicity)
  for (size_t iVtx=0; iVtx<vertexHandle->size(); iVtx++) {
    const auto& pv = vertexHandle->at(iVtx);
    if (!pv.isFake() && pv.tracksSize() >= 2) {
      priVertices_.emplace_back(pv);
      const VertexTuple tp(pv.x(), pv.y(), pv.z(), pv.tracksSize(), true);
      vertexRefMap_[tp] = reco::VertexRef(vertexHandle, iVtx);
    }
  }
  if (vtxSortByTrkSize_) {
    auto byTracksSize = [] (const reco::Vertex& v1, const reco::Vertex& v2) -> bool { return v1.tracksSize() > v2.tracksSize(); };
    std::sort(priVertices_.begin(), priVertices_.end(), byTracksSize);
  }
  // set the primary vertex
  const auto beamSpotVertex = reco::Vertex(beamSpotHandle->position(), beamSpotHandle->rotatedCovariance3D());
  vertex_ = (priVertices_.empty() ? beamSpotVertex : priVertices_[0]);
  // set the beam spot
  beamSpot_ = *beamSpotHandle;
  // set the 2D beam spot
  const auto beamSpot2DValue = reco::Vertex::Point(beamSpotHandle->x0(), beamSpotHandle->y0(), 0.);
  reco::Vertex::Error beamSpot2DError;
  beamSpot2DError(0,0) = std::pow(beamSpotHandle->BeamWidthX(), 2.);
  beamSpot2DError(1,1) = std::pow(beamSpotHandle->BeamWidthY(), 2.);
  beamSpot2D_ = reco::Vertex(beamSpot2DValue, beamSpot2DError);
};


void ParticleFitter::addParticles(ParticleDaughter& d, const edm::Event& iEvent)
{
  const auto& sid = d.sourceId();
  const auto& vertex = (vertex_.isFake() ? beamSpot2D_ : vertex_);
  if      (sid==Token::GenericParticle) { d.addParticles(iEvent); }
  else if (sid==Token::Track          ) { d.addParticles(iEvent, token_tracks_, vertex);      }
  else if (sid==Token::ParticleFlow   ) { d.addParticles(iEvent, token_pfParticles_, vertex); }
  else if (sid==Token::Jet            ) { d.addParticles(iEvent, token_jets_, vertex);        }
  else if (sid==Token::Electron       ) { d.addParticles(iEvent, token_electrons_, vertex);   }
  else if (sid==Token::Muon           ) { d.addParticles(iEvent, token_muons_, vertex);       }
  else if (sid==Token::Tau            ) { d.addParticles(iEvent, token_taus_, vertex);        }
  else if (sid==Token::Photon         ) { d.addParticles(iEvent, token_photons_);             }
  else if (sid==Token::Conversion     ) { d.addParticles(iEvent, token_convPhotons_, vertex); }
  else if (sid==Token::MET            ) { d.addParticles(iEvent, token_met_);                 }
  else { throw std::logic_error(Form("[ERROR] Source ID %u is not valid!", sid)); }
};


void ParticleFitter::fillDaughters(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  for (auto& daughter : daughters_) {
    daughter.init(iSetup);
    addParticles(daughter, iEvent);
  }
};


pat::GenericParticleRef ParticleFitter::addDaughter(const pat::GenericParticleRef& daughter)
{
  const auto& p = *daughter;
  const ParticleTuple tuple(p.pt(), p.eta(), p.phi(), p.mass(), p.charge());
  auto& ref = particleRefMap_[tuple];
  if (ref.isNonnull()) { return ref; }
  daughterColl_.emplace_back(p);
  const auto dauIdx = daughterColl_.size() - 1;
  const auto& particle = daughterColl_.back();
  if (particle.hasUserData("daughters")) {
    auto& daughters = *const_cast<pat::GenericParticleRefVector*>(particle.userData<pat::GenericParticleRefVector>("daughters"));
    pat::GenericParticleRefVector dauColl(dauProd_.id());
    dauColl.reserve(daughters.size());
    for (const auto& dau : daughters) { dauColl.push_back(addDaughter(dau)); }
    daughters = dauColl;
  }
  if (particle.hasUserData("primaryVertex")) {
    auto& priVtx = *const_cast<reco::VertexRef*>(particle.userData<reco::VertexRef>("primaryVertex"));
    priVtx = getVertexRef(*priVtx);
  }
  ref = pat::GenericParticleRef(dauProd_, dauIdx);
  return ref;
};


bool ParticleFitter::isUniqueDaughter(ParticleRefSet& set, const pat::GenericParticleRef& dau)
{
  if (dau->hasUserData("daughters")) {
    const auto& gDauColl = *dau->userData<pat::GenericParticleRefVector>("daughters");
    for (const auto& gDau : gDauColl) {
      if (!isUniqueDaughter(set, gDau)) { return false; }
    }
  }
  else if (!set.insert(dau).second) { return false; }
  return true;
};


void ParticleFitter::makeCandidates()
{
  // define daughter container
  const auto& nDaughters = daughters_.size();
  std::vector<GenericParticleRefCollection> dauColls(nDaughters);
  for (size_t i=0; i<daughters_.size(); i++) {
    dauColls[i] = daughters_[i].particlesRef();
    if (dauColls[i].empty()) return;
  }
  // make daughter combinations
  const auto combinations = make_combinations(dauColls);
  if (combinations.empty()) return;
  candidates_.reserve(std::min(4000UL, combinations.size()));
  daughterColl_.reserve(std::min(4000UL, 3*combinations.size()));
  // loop over all combinations of daughters
  std::unordered_set<ParticleTuple, ParticleTupleHash> candidates;
  for (const auto& combination : combinations) {
    // get unique set of daughters
    ParticleRefSet daughters;
    for (const auto& daughter : combination) {
      if (!daughters.insert(*daughter).second) break;
    }
    // check if all daughters are unique (are in set)
    if (daughters.size()!=nDaughters) continue;
    // check if candidate has not been found
    int charge(0);
    math::XYZTLorentzVector p4(0,0,0,0);
    for (const auto& daughter : daughters) {
      charge += daughter->charge();
      p4 += daughter->p4();
    }
    const ParticleTuple candTuple(p4.pt(), p4.eta(), p4.phi(), (doSwap_ ? 0.0 : p4.mass()), charge);
    if (!candidates.insert(candTuple).second) continue;
    // create candidate
    pat::GenericParticle cand;
    cand.setCharge(charge);
    cand.setP4(p4);
    cand.setPdgId(pdgId_);
    cand.setStatus(3);
    // add daughter related information
    float dauPtSum(0.);
    for (const auto& daughter : daughters) {
      dauPtSum += daughter->pt();
    }
    cand.addUserFloat("dauPtSum", dauPtSum);
    if (nDaughters==2) {
      auto dau1 = daughters.cbegin();
      cand.addUserFloat("dauEtaDiff", std::abs((*std::next(dau1))->eta()-(*dau1)->eta()));
    }
    // make preselection
    if (!preSelection_(cand)) continue;
    // check if grandaughters are unique
    bool hasDuplicate = false;
    ParticleRefSet finalStateColl;
    for (const auto& dau : daughters) {
      if (!isUniqueDaughter(finalStateColl, dau)) { hasDuplicate = true; break; }
    }
    if (hasDuplicate) continue;
    // make swapped daughters
    GenericParticleRefCollection dauColl(daughters.begin(), daughters.end());
    DoubleMap swapDauColls;
    swapDaughters(swapDauColls, cand, dauColl);
    setBestMass(cand, swapDauColls);
    // make mass preselection
    if (!preMassSelection_(cand)) continue;
    // extract daughter information
    LorentzVectorColl daughtersP4(nDaughters);
    for (size_t i=0; i<nDaughters; i++) {
      daughtersP4[i] = dauColl[i]->p4();
    }
    cand.addUserData<LorentzVectorColl>("daughtersP4", daughtersP4);
    // fit candidate and make postselection
    if (!fitCandidate(cand, dauColl)) continue;
    // add extra information
    addExtraInfo(cand);
    // make final selection
    if (!finalSelection_(cand)) continue;
    // add candidate
    pat::GenericParticleCollection candidates;
    candidates.reserve(swapDauColls.size()+1);
    if (width_<=0. || std::abs(cand.mass()-mass_) < width_) {
      candidates.emplace_back(cand);
    }
    // add swapped candidates
    pat::GenericParticleCollection swapCandColl(swapDauColls.size());
    addSwapCandidates(swapCandColl, cand, swapDauColls);
    for (const auto& swapCand : swapCandColl) {
      if (width_<=0. || std::abs(swapCand.mass()-mass_) < width_) {
        candidates.emplace_back(swapCand);
      }
    }
    // insert candidates
    std::vector<size_t> idxV(nDaughters);
    pat::GenericParticleRefVector dauRefColl(dauProd_.id());
    dauRefColl.reserve(nDaughters);
    if (!candidates.empty()) {
      std::iota(idxV.begin()+1, idxV.end(), 1);
      std::sort(idxV.begin(), idxV.end(), [&](size_t i, size_t j) { return ParticleMassComparator()(dauColl[i], dauColl[j]); });
      auto dR=dauColl; std::transform(idxV.begin(), idxV.end(), dauColl.begin(), [&](size_t i){ return dR[i]; });
      for (const auto& dau : dauColl) dauRefColl.push_back(addDaughter(dau));
    }
    for (auto& c : candidates) {
      auto& dP4Coll = *const_cast<LorentzVectorColl*>(c.userData<LorentzVectorColl>("daughtersP4"));
      auto dR=dP4Coll; std::transform(idxV.begin(), idxV.end(), dP4Coll.begin(), [&](size_t i){ return dR[i]; });
      c.addUserData<pat::GenericParticleRefVector>("daughters", dauRefColl);
      candidates_.emplace_back(c);
    }
  }
  // sort candidates
  std::sort(candidates_.begin(), candidates_.end(), ParticleMassComparator());
};


void ParticleFitter::swapDaughters(DoubleMap& swapDauColls, const pat::GenericParticle& cand, const GenericParticleRefCollection& dauColl)
{
  if (!doSwap_) return;
  const auto& nDau = dauColl.size();
  auto nPer = nDau;
  for (size_t i=1; i<nDau; i++) nPer *= i;
  std::vector<size_t> dauIdx(nDau);
  std::iota(dauIdx.begin()+1, dauIdx.end(), 1);
  // loop over permutations of daughters
  for (size_t iPer=0; iPer<nPer-1; iPer++) {
    auto perIdx = dauIdx;
    for (size_t i=1, k=iPer; i<nDau; i++) {
      const auto u = k % (i + 1);
      if (u!=i) std::swap(perIdx[u], perIdx[i]);
      k = k / (i + 1);
    }
    std::vector<double> swapDauColl(nDau, -1.);
    math::XYZTLorentzVector p4(0,0,0,0);
    for (size_t i=0; i<nDau; i++) {
      const auto& dau = *dauColl[i];
      const auto& per = *dauColl[perIdx[i]];
      if (cand.charge()!=0 && dau.charge()!=per.charge()) break;
      if (!dau.hasUserInt("sourceId") || !per.hasUserInt("sourceId")) { throw std::logic_error("[ERROR] Missing sourceId when swapping!"); }
      const auto& sourceId1 = dau.userInt("sourceId");
      const auto& sourceId2 = per.userInt("sourceId");
      if (sourceId1!=sourceId2) break;
      if (sourceId1>Token::Track && dau.mass()!=per.mass()) break; // sourceId > Token::Track means that the daughter has PID, not necessary to swap it with another mass
      swapDauColl[i] = per.mass();
      p4 += math::XYZTLorentzVector(dau.px(), dau.py(), dau.pz(), std::sqrt(dau.p4().P2()+per.massSqr()));
    }
    if (swapDauColl.back()>=0. && p4.mass()!=cand.mass()) {
      swapDauColls.insert({p4.mass(), swapDauColl});
    }
  }
};


void ParticleFitter::setBestMass(pat::GenericParticle& cand, const DoubleMap& swapDauColls)
{
  if (!doSwap_) return;
  // find mass closest to expected value
  auto mass = cand.mass();
  for (const auto& swapM : swapDauColls) {
    mass = ((std::abs(swapM.first - mass_) < std::abs(mass - mass_)) ? swapM.first : mass);
  }
  cand.addUserFloat("bestMass", mass);
};


void ParticleFitter::addSwapCandidates(pat::GenericParticleCollection& swapCandColl, const pat::GenericParticle& cand, const DoubleMap& swapDauColls)
{
  if (!doSwap_) return;
  // add swapped candidates
  size_t j(0);
  for (const auto& swapM : swapDauColls) {
    pat::GenericParticle swapCand = cand;
    auto& daughtersP4 = *const_cast<LorentzVectorColl*>(swapCand.userData<LorentzVectorColl>("daughtersP4"));
    math::XYZTLorentzVector p4(0,0,0,0);
    for (size_t i=0; i<daughtersP4.size(); i++) {
      auto& dau = daughtersP4[i];
      dau = getP4(GlobalVector(dau.px(), dau.py(), dau.pz()), swapM.second[i]);
      p4 += dau;
    }
    swapCand.setP4(p4);
    swapCandColl[j++] = swapCand;
  }
};


RefCountedKinematicTree ParticleFitter::fitVertex(const ParticleInfo& parInfo, const int& fitAlgo, GlobalPoint decP, const reco::Vertex& priVtx)
{
  RefCountedKinematicTree res;
  auto& algoConf = algoConf_[fitAlgo];
  // perform unconstrained vertex fit
  if (fitAlgo==0) {
    if (algoConf.empty()) {
      algoConf.addParameter("maxDistance", 0.01); // default: 0.01
      algoConf.addParameter("maxNbrOfIterations", 100); // default: 100
    }
    KinematicParticleVertexFitter fitter(algoConf);
    res = fitter.fit(std::get<0>(parInfo));
  }
  // perform constrained vertex fit
  else if (fitAlgo>0 && !(decP==GlobalPoint())) {
    if (algoConf.empty()) {
      algoConf.addParameter("maxDelta", 0.01); // default: 0.01
      algoConf.addParameter("maxNbrOfIterations", 1000); // default: 1000
      algoConf.addParameter("maxReducedChiSq", 225.); // default: 225.
      algoConf.addParameter("minChiSqImprovement", 50.); // default: 50.
    }
    KinematicConstrainedVertexFitter fitter;
    fitter.setParameters(algoConf);
    if (fitAlgo==1) {
      GlobalPoint pv = GlobalPoint(priVtx.x(), priVtx.y(), priVtx.z());
      MultiTrackPointingKinematicConstraint constrain(pv);
      res = fitter.fit(std::get<0>(parInfo), &constrain, &decP);
    }
    else if (fitAlgo==2) {
      if (mass_<0.) { throw std::logic_error("[ERROR] Trying to do constrain mass fit with unknown input mass!"); }
      MultiTrackMassKinematicConstraint constrain(mass_, std::get<0>(parInfo).size());
      res = fitter.fit(std::get<0>(parInfo), &constrain, &decP);
    }
  }
  // check fit result
  if (!res || !res->isValid() || !res->currentDecayVertex()->vertexIsValid() || !res->currentParticle()->currentState().isValid()) return RefCountedKinematicTree();
  // return fit result
  return res;
};


RefCountedKinematicTree ParticleFitter::fitVertex(const ParticleInfo& parInfo, const int& fitAlgo)
{
  CachingVertex<5> fitVertex;
  auto& algoConf = algoConf_[fitAlgo];
  // perform vertex fit
  if (fitAlgo==3) {
    if (algoConf.empty()) {
      algoConf.addParameter("maxDistance", 0.01); // default: 0.01
      algoConf.addParameter("maxNbrOfIterations", 10); // default: 10
    }
    KalmanVertexFitter fitter(algoConf, true);
    fitVertex = fitter.vertex(std::get<1>(parInfo));
  }
  else if (fitAlgo==4) {
    if (algoConf.empty()) {
      algoConf.addParameter("maxshift", 0.0001); // default: 0.0001
      algoConf.addParameter("maxlpshift", 0.1); // default: 0.1
      algoConf.addParameter("maxstep", 30); // default: 30
      algoConf.addParameter("weightthreshold", 0.001); // default 0.001
    }
    auto fitter = AdaptiveVertexFitter(GeometricAnnealing(), DefaultLinearizationPointFinder(), KalmanVertexUpdator<5>(), KalmanVertexTrackCompatibilityEstimator<5>(), KalmanVertexSmoother());
    fitter.setParameters(algoConf);
    fitVertex = fitter.vertex(std::get<1>(parInfo));
  }
  else if (fitAlgo==5) {
    if (algoConf.empty()) {
      algoConf.addParameter("maxDistance", 0.01); // default: 0.01
      algoConf.addParameter("maxNbrOfIterations", 10); // default: 10
      algoConf.addParameter("limitComponents", false);
      algoConf.addParameter("smoothTracks", true);
    }
    GsfVertexFitter fitter(algoConf);
    fitVertex = fitter.vertex(std::get<1>(parInfo));
  }
  else if (fitAlgo==6) {
    if (algoConf.empty()) {
      algoConf.addParameter("maxshift", 0.0001); // default: 0.0001
      algoConf.addParameter("maxlpshift", 0.1); // default: 0.1
      algoConf.addParameter("maxstep", 30); // default: 30
      algoConf.addParameter("weightthreshold", 0.001); // default 0.001
      algoConf.addParameter("limitComponents", false);
    }
    AdaptiveGsfVertexFitter fitter(algoConf);
    fitVertex = fitter.vertex(std::get<1>(parInfo));
  }
  if (!fitVertex.isValid() || !fitVertex.vertexState().isValid()) return RefCountedKinematicTree();
  // convert to kinematic particle vertex
  const auto& refTracks = fitVertex.tracks();
  typedef ReferenceCountingPointer<VertexTrack<6> > RefCountedVertexTrack;
  std::vector<RefCountedVertexTrack> tks(refTracks.size());
  for (size_t iTrk=0; iTrk<refTracks.size(); iTrk++) {
    const auto& trk = refTracks[iTrk];
    const auto& ifs = trk->linearizedTrack()->track().initialFreeState();
    const ParticleTuple tuple(ifs.momentum().x(), ifs.momentum().y(), ifs.momentum().z(), 0, ifs.charge());
    auto par = std::get<0>(parInfo)[std::get<2>(parInfo).at(tuple)];
    // full covariance
    auto parFullCov = par->currentState().kinematicParametersError().matrix();
    parFullCov.Place_at(trk->fullCovariance(), 0, 0);
    // refitted track state
    const auto& fts = trk->refittedState()->freeTrajectoryState();
    AlgebraicVector3 trkRefMV;
    if (fitAlgo<5) { trkRefMV = trk->refittedState()->momentumVector(); }
    else {
      const auto& mom = math::XYZVector(fts.parameters().vector()[3], fts.parameters().vector()[4], fts.parameters().vector()[5]);
      const auto& bOverQ = (fts.charge()==0 ? 1.0 : -fts.parameters().magneticFieldInInverseGeV().z()/fts.charge());
      trkRefMV = AlgebraicVector3(bOverQ/mom.rho(), mom.theta(), mom.phi());
    }
    const auto& parRefMV = AlgebraicVector4(trkRefMV[0], trkRefMV[1], trkRefMV[2], par->currentState().mass());
    const auto& parRefKS = KinematicPerigeeConversions().kinematicState(parRefMV, fts.position(), fts.charge(), parFullCov, bFieldHandle_.product());
    const auto& parRefSta = ReferenceCountingPointer<RefittedTrackState<6> >(new KinematicRefittedTrackState(parRefKS, parRefMV));
    // linearized track state
    const auto& parLinSta = ParticleKinematicLinearizedTrackStateFactory().linearizedTrackState(fts.position(), par);
    // vertex track
    tks[iTrk].reset(new VertexTrack<6>(parLinSta, trk->vertexState(), trk->weight(), parRefSta, trk->smoothedChi2(), parFullCov));
  }
  // track-track covariance map
  std::map<RefCountedVertexTrack, std::map<RefCountedVertexTrack, AlgebraicMatrix44> > covMap;
  for (size_t iTrk=0; iTrk<tks.size(); iTrk++) {
    for (size_t jTrk=0; jTrk<tks.size(); jTrk++) {
      if (iTrk==jTrk) continue;
      auto& cov = covMap[tks[iTrk]][tks[jTrk]];
      if (fitVertex.tkToTkCovarianceIsAvailable()) {
        cov.Place_at(fitVertex.tkToTkCovariance(refTracks[iTrk], refTracks[jTrk]), 0, 0);
      }
    }
  }
  CachingVertex<6> parVertex(fitVertex.vertexState(), tks, fitVertex.totalChiSquared(), covMap);
  // get kinematic tree
  const auto& res = FinalTreeBuilder().buildTree(parVertex, std::get<0>(parInfo));
  // check fit result
  if (!res || !res->isValid() || !res->currentDecayVertex()->vertexIsValid() || !res->currentParticle()->currentState().isValid()) return RefCountedKinematicTree();
  // return fit result
  return res;
};


FreeTrajectoryState ParticleFitter::getFTS(const pat::GenericParticle& p)
{
  GlobalTrajectoryParameters pars(GlobalPoint(p.vx(), p.vy(), p.vz()), GlobalVector(p.px(), p.py(), p.pz()), p.charge(), bFieldHandle_.product());
  if (p.hasUserData("kinematicParametersError")) {
    const auto& error = p.userData<KinematicParametersError>("kinematicParametersError")->matrix().Sub<AlgebraicSymMatrix66>(0,0);
    return FreeTrajectoryState(pars, CartesianTrajectoryError(error));
  }
  else if (p.track().isNonnull()) {
    return FreeTrajectoryState(pars, CurvilinearTrajectoryError(p.track()->covariance()));
  }
  return FreeTrajectoryState(pars);
};
  

bool ParticleFitter::fitCandidate(pat::GenericParticle& cand, const GenericParticleRefCollection& dauColl)
{
  if (fitAlgoV_.empty() || daughters_.size()<2) return true;
  const auto& magField = bFieldHandle_.product();
  // get the daughters transient tracks
  std::vector<std::pair<std::vector<std::pair<reco::TransientTrack, double> >, size_t> > daughters;
  daughters.reserve(dauColl.size());
  for (size_t iDau=0; iDau<dauColl.size(); iDau++) {
    const auto& daughter = *dauColl[iDau];
    const auto& nTracks = daughter.numberOfTracks();
    std::vector<std::pair<reco::TransientTrack, double> > tracks;
    if (nTracks>0) {
      tracks.resize(nTracks);
      for (size_t iTrk=0; iTrk<nTracks; iTrk++) {
        const auto& trackMass = daughter.userFloat(Form("trackMass%lu", iTrk));
        tracks[iTrk] = {reco::TransientTrack(*daughter.track(iTrk), magField), trackMass};
      }
    }
    else if (daughter.track().isNonnull()) {
      tracks.emplace_back(reco::TransientTrack(*daughter.track(), magField), daughter.mass());
    }
    else if (daughter.hasUserData("kinematicParametersError")) {
      tracks.emplace_back(TransientTrackFromFTSFactory().build(getFTS(daughter)), daughter.mass());
    }
    if (!tracks.empty() && tracks[0].second>=0.) {
      daughters.emplace_back(tracks, iDau);
    }
  }
  if (daughters.size()<2) return true;
  // measure distance between daughter tracks at their point of closest approach
  if (daughters.size()==2 && daughters[0].first.size()==1 && daughters[1].first.size()==1 &&
      daughters[0].first[0].first.isValid() && daughters[0].first[0].first.impactPointTSCP().isValid() &&
      daughters[1].first[0].first.isValid() && daughters[1].first[0].first.impactPointTSCP().isValid()) {
    ClosestApproachInRPhi cApp;
    const auto& stateDau1 = daughters[0].first[0].first.impactPointTSCP().theState();
    const auto& stateDau2 = daughters[1].first[0].first.impactPointTSCP().theState();
    cApp.calculate(stateDau1, stateDau2);
    if (!cApp.status()) return false;
    const auto& cxPt = cApp.crossingPoint();
    math::XYZTLorentzVector p4(0,0,0,0), swapP4(0,0,0,0);
    for (size_t i=0; i<2; i++) {
      const auto& tscpDau = daughters[i].first[0].first.trajectoryStateClosestToPoint(cxPt);
      const auto& tscpDauMom = tscpDau.momentum();
      if (!tscpDau.isValid()) return false;
      for (size_t j=0; j<2; j++) {
        if (!doSwap_ && i!=j) continue;
        const auto& daughter = dauColl[daughters[j].second];
        const auto& tscpDauP4 = getP4(tscpDauMom, daughter->mass());
        if (i==j) { p4 += tscpDauP4; }
        else { swapP4 += tscpDauP4; }
      }
    }
    cand.setP4(p4);
    if (doSwap_) {
      const auto& mass = math::PtEtaPhiMLorentzVector(swapP4).M();
      cand.addUserFloat("bestMass", ((std::abs(cand.mass() - mass_) < std::abs(mass - mass_)) ? cand.mass() : mass), true);
    }
    cand.addUserFloat("dca", cApp.distance());
    if (!pocaSelection_(cand)) return false;
  }
  // prepare particles for decay vertex fit
  ParticleInfo parInfo;
  std::map<size_t, std::vector<size_t> > dauParIdx;
  for (const auto& d : daughters) {
    const auto& daughter = *dauColl[d.second];
    if (daughter.hasUserData("kinematicParametersError")) {
      const auto& kinError = *daughter.userData<KinematicParametersError>("kinematicParametersError");
      const auto& kinPars = KinematicParameters(daughter.vx(), daughter.vy(), daughter.vz(), daughter.px(), daughter.py(), daughter.pz(), daughter.mass());
      const auto& kinState = KinematicState(kinPars, kinError, daughter.charge(), magField);
      float chi = 0., ndf = 0.;
      std::get<0>(parInfo).emplace_back(VirtualKinematicParticleFactory().particle(kinState, chi, ndf, NULL));
      std::get<1>(parInfo).emplace_back(std::get<0>(parInfo).back()->refittedTransientTrack());
      const ParticleTuple tuple(daughter.px(), daughter.py(), daughter.pz(), 0, daughter.charge());
      std::get<2>(parInfo)[tuple] = std::get<0>(parInfo).size()-1;
      dauParIdx[d.second].emplace_back(std::get<0>(parInfo).size()-1);
    }
    else if (d.first[0].first.isValid() && d.first[0].first.numberOfValidHits()>0) {
      for (const auto& t : d.first) {
        if (!t.first.isValid() || !t.first.impactPointTSCP().isValid()) return false;
        float chi = 0., ndf = 0., width = daughter.userFloat("width");
        std::get<0>(parInfo).emplace_back(KinematicParticleFactoryFromTransientTrack().particle(t.first, t.second, chi, ndf, width));
        std::get<1>(parInfo).emplace_back(t.first);
        const ParticleTuple tuple(t.first.track().px(), t.first.track().py(), t.first.track().pz(), 0, t.first.charge());
        std::get<2>(parInfo)[tuple] = std::get<0>(parInfo).size()-1;
        dauParIdx[d.second].emplace_back(std::get<0>(parInfo).size()-1);
      }
    }
  }
  if (dauParIdx.size()<2) return false;
  // fit decay vertex
  std::pair<RefCountedKinematicTree, bool> fit0({}, false);
  for (const auto& a : fitAlgoV_) {
    // ignore if already done
    const auto lbl = (a==fitAlgoV_[0] ? "" : Form("_%d",a));
    if (cand.hasUserData(Form("decayVertex%s",lbl))) continue;
    // perform prior fit for a<3
    if (a<3 && !fit0.second) { fit0 = std::make_pair(fitVertex(parInfo, 0, {}), true); }
    const auto& priorP = (fit0.first && fit0.first->isValid()) ? fit0.first->currentDecayVertex()->position() : GlobalPoint();
    // perform vertex fit
    RefCountedKinematicTree fitResult;
    if (a==0) { fitResult = fit0.first; }
    else if (a>=3) { fitResult = fitVertex(parInfo, a); }
    else { fitResult = fitVertex(parInfo, a, priorP, vertex_); }
    // check fit result
    if (!fitResult || fitResult->isEmpty()) { if (a==fitAlgoV_[0]) { return false; } else continue; }
    // add decay vertex
    const auto& fV = fitResult->currentDecayVertex();
    const auto  nDoF = std::max(std::round(fV->degreesOfFreedom()), 0.f);
    const auto& decP = reco::Particle::Point(fV->position().x(), fV->position().y(), fV->position().z());
    const auto& decVtx = reco::Vertex(decP, fV->error().matrix(), fV->chiSquared(), nDoF, std::get<0>(parInfo).size());
    cand.addUserData<reco::Vertex>(Form("decayVertex%s",lbl), decVtx);
    // keep extra information only if main fit (fitAlgoV_[0])
    if (a!=fitAlgoV_[0]) continue;
    // update particle kinematics from fit
    math::XYZTLorentzVector candP4(0, 0, 0, 0);
    auto& daughtersP4 = *const_cast<LorentzVectorColl*>(cand.userData<LorentzVectorColl>("daughtersP4"));
    for (size_t iDau=0; iDau<dauColl.size(); iDau++) {
      if (dauParIdx.find(iDau)!=dauParIdx.end()) {
        math::XYZTLorentzVector dauP4(0, 0, 0, 0);
        for (const auto& iPar : dauParIdx.at(iDau)) {
          const auto& fitDau = fitResult->daughterParticles()[iPar];
          if (!fitDau->currentState().isValid()) return false;
          dauP4 += getP4(fitDau->currentState().kinematicParameters().momentum(), fitDau->currentState().mass());
        }
        daughtersP4[iDau] = dauP4;
      }
      candP4 += daughtersP4[iDau];
    }
    cand.setP4(candP4);
    // apply post-fit selection
    cand.setVertex(decVtx.position());
    cand.addUserFloat("normChi2", decVtx.chi2()/decVtx.ndof());
    cand.addUserFloat("vertexProb", TMath::Prob(decVtx.chi2(), decVtx.ndof()));
    if (!postSelection_(cand)) return false;
    // add information to fitted candidate
    const auto& candState = fitResult->currentParticle()->currentState();
    cand.addUserData<KinematicParametersError>("kinematicParametersError", candState.kinematicParametersError());
  }
  matchPrimaryVertex(cand, std::get<1>(parInfo), {});
  return true;
};


void ParticleFitter::matchPrimaryVertex(pat::GenericParticle& cand, const TransTrackColl& tracks, FreeTrajectoryState fts, const double& thr)
{
  if (cand.hasUserData("primaryVertex")) return;
  // initialise the candidate PV to main vertex
  auto candPV = vertex_;
  // find primary vertex
  const auto& vtxProb = (cand.hasUserFloat("vertexProb") ? cand.userFloat("vertexProb") : 1.f);
  if (matchVertex_ && priVertices_.size()>1 && vtxProb>thr) {
    // compute free trajectory state
    if (!fts.hasError()) fts = getFTS(cand);
    // extrapolate trajectory to beam spot
    GlobalPoint pca;
    if (fts.hasError()) {
      auto track = TransientTrackFromFTSFactory().build(fts);
      track.setBeamSpot(beamSpot_);
      const auto& res = track.stateAtBeamLine();
      if (res.isValid() && res.trackStateAtPCA().hasError()) {
        const auto prob = TMath::Erfc(res.transverseImpactParameter().significance()/std::sqrt(2));
        if (prob>thr) { pca = res.beamLinePCA(); }
      }
    }
    // fit vertex constrained to beam spot
    if (pca==GlobalPoint() && !tracks.empty()) {
      const auto& res = KalmanVertexFitter().vertex(tracks, beamSpot_);
      if (res.isValid() && res.vertexState().isValid()) {
        const auto prob = TMath::Prob(res.totalChiSquared(), res.degreesOfFreedom());
        if (prob>thr) { pca = res.position(); }
      }
    }
    // match primary vertex
    if (!(pca==GlobalPoint())) {
      for (const auto& pv : priVertices_) {
        const bool& isGoodPV = (pv.position() == vertex_.position() || pv.tracksSize() >= puMap_.size() || std::abs(pv.z()-vertex_.z()) > puMap_[pv.tracksSize()]);
        if (isGoodPV && std::abs(pv.z()-pca.z()) < std::abs(candPV.z()-pca.z())-0.4) { candPV = pv; }
      }
    }
  }
  // store primary vertex
  cand.addUserData<reco::VertexRef>("primaryVertex", getVertexRef(candPV));
};


void ParticleFitter::addExtraInfo(pat::GenericParticle& cand)
{
  if (!cand.hasUserData("decayVertex") || !cand.hasUserData("primaryVertex")) return;
  const auto& decayVertex = *cand.userData<reco::Vertex>("decayVertex");
  const auto& priVtx = *cand.userData<reco::VertexRef>("primaryVertex");
  const auto& primaryVertex = (priVtx.isAvailable() ? *priVtx : vertices_.at(priVtx.key()));
  // compute lifetime information
  const auto lineOfFlight = decayVertex.position() - primaryVertex.position();
  const auto lVtxMag = lineOfFlight.r();
  const auto rVtxMag = lineOfFlight.rho();
  const auto angle3D = angle(lineOfFlight.x(), lineOfFlight.y(), lineOfFlight.z(), cand.px(), cand.py(), cand.pz());
  const auto angle2D = angle(lineOfFlight.x(), lineOfFlight.y(), 0.0, cand.px(), cand.py(), 0.0);
  const auto distanceVector3D = AlgebraicVector3(lineOfFlight.x(), lineOfFlight.y(), lineOfFlight.z());
  const auto distanceVector2D = AlgebraicVector3(lineOfFlight.x(), lineOfFlight.y(), 0.0);
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
};


// ParticleDaughter: constructor and (empty) destructor
ParticleDaughter::ParticleDaughter()
{
  pdgId_ = 0;
  charge_ = -99;
  mass_ = -1.;
  width_ = 0.;
  selection_ = "";
  finalSelection_ = "";
  particles_ = {};
  particlesRef_ = {};
  propToMuonSetup_ = 0;
  source_id_ = Token::Unknown;
};


ParticleDaughter::ParticleDaughter(const edm::ParameterSet& pSet, const edm::ParameterSet& config, edm::ConsumesCollector&& iC) :
  ParticleDaughter()
{
  fillInfo(pSet, config, iC);
};


ParticleDaughter::~ParticleDaughter()
{
  if (propToMuonSetup_) delete propToMuonSetup_;
};


void ParticleDaughter::clear()
{
  clear(particles_);
  clear(particlesRef_);
};


void ParticleDaughter::fillInfo(const edm::ParameterSet& pSet, const edm::ParameterSet& config, edm::ConsumesCollector& iC)
{
  if (pSet.existsAs<unsigned int>("pdgId")) {
    pdgId_ = pSet.getParameter<unsigned int>("pdgId");
  }
  else if (pSet.existsAs<int>("pdgId")) {
    throw std::logic_error("ERROR: Duaghter pdgId should be unsigned integer (uint32)");
  }
  else {
    throw std::logic_error("ERROR: Duaghter pdgId not defined!");
  }
  if (pSet.existsAs<int>("charge")) {
    charge_ = pSet.getParameter<int>("charge");
  }
  if (pSet.existsAs<double>("mass")) {
    mass_ = pSet.getParameter<double>("mass");
  }
  else if (MASS_.find(pdgId_)!=MASS_.end()) {
    mass_ = MASS_.at(pdgId_);
  }
  if (pSet.existsAs<double>("width")) {
    width_ = pSet.getParameter<double>("width");
  }
  else if (WIDTH_.find(pdgId_)!=WIDTH_.end()) {
    width_ = WIDTH_.at(pdgId_);
  }
  else { width_ = mass_*1e-6; }
  if (pSet.existsAs<std::string>("selection")) {
    selection_ = pSet.getParameter<std::string>("selection");
  }
  if (pSet.existsAs<std::string>("finalSelection")) {
    finalSelection_ = pSet.getParameter<std::string>("finalSelection");
  }
  if (pSet.existsAs<edm::InputTag>("source")) {
    token_source_ = iC.consumes<pat::GenericParticleCollection>(pSet.getParameter<edm::InputTag>("source"));
    source_id_ = Token::GenericParticle;
  }
  else if (pSet.existsAs<unsigned int>("sourceId")) {
    const auto& sid = pSet.getParameter<unsigned int>("sourceId");
    if (sid>=Token::All) { throw std::logic_error(Form("[ERROR] Source ID %u is not valid!", sid)); }
    source_id_ = Token(sid);
  }
  if (source_id_==Token::Unknown) {
    getSourceId(source_id_, pdgId_, config, pSet);
  }
  const auto& mvaSet = (pSet.existsAs<edm::InputTag>("mva") ? pSet : config);
  if (mvaSet.existsAs<edm::InputTag>("mva")) {
    token_mva_ = iC.consumes<std::vector<float> >(mvaSet.getParameter<edm::InputTag>("mva"));
  }
  if (config.existsAs<std::vector<edm::InputTag> >("dEdxInputs")) {
    for (const auto& input : config.getParameter<std::vector<edm::InputTag> >("dEdxInputs")){
      const auto& label = input.instance()!="" ? input.instance() : input.label();
      tokens_dedx_.insert(std::make_pair(label, iC.consumes<edm::ValueMap<reco::DeDxData> >(input)));
    }
  }
  if (source_id_==Token::Muon && (pSet.existsAs<bool>("propToMuon") && pSet.getParameter<bool>("propToMuon"))) {
    edm::ParameterSet conf;
    conf.addParameter("useSimpleGeometry", (pSet.existsAs<bool>("useSimpleGeometry") ? pSet.getParameter<bool>("useSimpleGeometry") : true)); // default: true
    conf.addParameter("useTrack", (pSet.existsAs<std::string>("useTrack") ? pSet.getParameter<std::string>("useTrack") : "none")); // default: none
    conf.addParameter("useState", (pSet.existsAs<std::string>("useState") ? pSet.getParameter<std::string>("useState") : "atVertex")); // default: atVertex
    conf.addParameter("fallbackToME1", (pSet.existsAs<bool>("fallbackToME1") ? pSet.getParameter<bool>("fallbackToME1") : true)); // default: true
    conf.addParameter("useMB2InOverlap", (pSet.existsAs<bool>("useMB2InOverlap") ? pSet.getParameter<bool>("useMB2InOverlap") : true)); // default: true
    conf.addParameter("useStation2", (pSet.existsAs<bool>("useStation2") ? pSet.getParameter<bool>("useStation2") : true)); // default: true
    conf.addParameter("cosmicPropagationHypothesis", (pSet.existsAs<bool>("cosmicPropagationHypothesis") ? pSet.getParameter<bool>("cosmicPropagationHypothesis") : false)); // default: false
    conf.addParameter("propagatorAlong", edm::ESInputTag("", "SteppingHelixPropagatorAlong"));
    conf.addParameter("propagatorAny", edm::ESInputTag("", "SteppingHelixPropagatorAny"));
    conf.addParameter("propagatorOpposite", edm::ESInputTag("", "SteppingHelixPropagatorOpposite"));
    propToMuonSetup_ = new PropagateToMuonSetup(conf, iC);
  }
  if (config.existsAs<edm::InputTag>("recoToSimTrackMap")) {
    token_recoToSimTrackMap_ = iC.consumes<reco::RecoToSimCollection>(config.getParameter<edm::InputTag>("recoToSimTrackMap"));
  }
};


void ParticleDaughter::init(const edm::EventSetup& iSetup)
{
  if (propToMuonSetup_) {
    propToMuon_ = propToMuonSetup_->init(iSetup);
  }
};


template <class T>
void ParticleDaughter::addParticles(const edm::Event& event, const edm::EDGetTokenT<std::vector<T> >& token, const reco::Vertex& vertex, const bool embedInfo)
{
  // extract input collections
  const auto& handle = event.getHandle(token);
  std::map<std::string, edm::Handle<edm::ValueMap<reco::DeDxData> > > dEdxMaps;
  for (const auto& tokenMap : tokens_dedx_) {
    dEdxMaps.emplace(tokenMap.first, event.getHandle(tokenMap.second));
  }
  const auto& mvaColl = event.getHandle(token_mva_);
  const auto& recoToSimTrackMap = event.getHandle(token_recoToSimTrackMap_);
  // set selections
  StringCutObjectSelector<T, true> selection(selection_);
  StringCutObjectSelector<pat::GenericParticle, true> finalSelection(finalSelection_);
  // add particles
  ParticleMassSet particles;
  for (size_t i=0; i<handle->size(); i++) {
    const auto& p = edm::Ref<std::vector<T> >(handle, i);
    if (!selection(*p)) continue;
    pat::GenericParticle cand;
    addInfo(cand, *p);
    if (charge_!=-99 && cand.charge()!=charge_) continue;
    cand.setPdgId(pdgId_);
    cand.setStatus(1);
    cand.addUserInt("sourceId", source_id_);
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
    }
    else if (cand.charge()!=0) continue; // ignore if charged particle has no track
    cand.addUserFloat("width", width_);
    setDeDx(cand, dEdxMaps);
    setMVA(cand, i, mvaColl);
    setSim(cand, recoToSimTrackMap);
    if (finalSelection(cand)) {
      particles.insert(cand);
    }
  }
  particles_.assign(particles.begin(), particles.end());
  particlesRef_.resize(particles_.size());
  for (size_t i=0; i<particles_.size(); i++) { particlesRef_[i] = pat::GenericParticleRef(&particles_, i); }
};


void ParticleDaughter::addParticles(const edm::Event& event)
{
  const auto& source = event.get(token_source_);
  StringCutObjectSelector<pat::GenericParticle, true> selection(selection_);
  StringCutObjectSelector<pat::GenericParticle, true> finalSelection(finalSelection_);
  ParticleMassSet particles;
  for (const auto& p : source) {
    if (!selection(p)) continue;
    pat::GenericParticle cand(p);
    if (charge_!=-99 && cand.charge()!=charge_) continue;
    if (cand.hasUserData("daughters")) {
      cand.setStatus(2);
    }
    if (!cand.hasUserInt("sourceId")) {
      cand.addUserInt("sourceId", source_id_);
    }
    if (finalSelection(cand)) {
      particles.insert(cand);
    }
  }
  particles_.assign(particles.begin(), particles.end());
  particlesRef_.resize(particles_.size());
  for (size_t i=0; i<particles_.size(); i++) { particlesRef_[i] = pat::GenericParticleRef(&particles_, i); }
};


template <class T>
void ParticleDaughter::addInfo(pat::GenericParticle& c, const T& p)
{
  if (pdgId_!=0 && mass_<0) { throw std::logic_error(Form("[ERROR] Mass not set for particle %d !", pdgId_)); }
  const auto p4 = math::PtEtaPhiMLorentzVector(p.pt(), p.eta(), p.phi(), mass_);
  c.setP4(p4);
  c.setCharge(p.charge());
  c.setVertex(p.vertex());
};


void ParticleDaughter::addInfo(pat::GenericParticle& c, const reco::Conversion& p)
{
  const auto& vtx = p.conversionVertex();
  reco::TrackCollection tracks;
  if (vtx.isValid() && vtx.nTracks(0.5)==2) {
    tracks = vtx.refittedTracks();
  }
  else {
    const auto& trks = p.tracks();
    tracks.resize(trks.size());
    for (size_t i=0; i<trks.size(); i++) { tracks[i] = *trks[i]; }
  }
  int charge = 0;
  math::PtEtaPhiMLorentzVector p4(0, 0, 0, 0);
  for (const auto& t: tracks) {
    charge += t.charge();
    p4 += math::PtEtaPhiMLorentzVector(t.pt(), t.eta(), t.phi(), 0.000511);
  }
  reco::TrackRefVector trackRefs;
  trackRefs.reserve(tracks.size());
  for (size_t i=0; i<tracks.size(); i++) {
    trackRefs.push_back(reco::TrackRef(&tracks, i));
    c.addUserFloat(Form("trackMass%lu", i), 0.000511);
  }
  c.setP4(p4);
  c.setCharge(charge);
  c.setVertex(vtx.position());
  //c.setTracks(trackRefs, true); // CONVERSION TRACK FIT DOES NOT WORK
};


template <class T>
void ParticleDaughter::addData(pat::GenericParticle& c, const edm::Ref<std::vector<T> >& p, const bool& embedInfo)
{
  c.addUserData<T>("src", *p);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const reco::TrackRef& p, const bool& embedInfo)
{
  c.setTrack(p, embedInfo);
  if (embedInfo) c.addUserData<reco::TrackRef>("trackRef", p);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const reco::PFCandidateRef& p, const bool& embedInfo)
{
  if (c.pdgId()!=p->pdgId() && p->pdgId()>=11 && p->pdgId()<=22) {
    c.setPdgId(p->pdgId());
    math::PtEtaPhiMLorentzVector p4(c.pt(), c.eta(), c.phi(), MASS_.at(p->pdgId()));
    c.setP4(p4);
  }
  c.setTrack(p->trackRef(), embedInfo);
  if (embedInfo) c.addUserData<reco::TrackRef>("trackRef", p->trackRef());
  c.addUserData<reco::PFCandidate>("src", *p);
};


void ParticleDaughter::addData(pat::GenericParticle& c, const pat::MuonRef& p, const bool& embedInfo)
{
  auto track = p->track();
  if (!track.id().isValid() && p->originalObjectRef().id().isValid() && p->originalObjectRef().isAvailable() && dynamic_cast<const reco::Muon*>(p->originalObject())) {
    const auto& t = dynamic_cast<const reco::Muon*>(p->originalObject())->track();
    if (t.isNonnull() && t.id().isValid() && t.isAvailable()) { track = t; }
  }
  c.setTrack(track, embedInfo);
  if (embedInfo && track.id().isValid()) c.addUserData<reco::TrackRef>("trackRef", track);
  c.addUserData<pat::Muon>("src", *p);
  // propagate inner track to 2nd muon station (for L1 trigger matching)
  if (!p->hasUserInt("prop") && propToMuonSetup_ && track.isNonnull()) {
    const auto& fts = propToMuon_.extrapolate(*track);
    if (fts.isValid()) {
      const_cast<pat::Muon*>(&*p)->addUserFloat("l1Eta", fts.globalPosition().eta());
      const_cast<pat::Muon*>(&*p)->addUserFloat("l1Phi", fts.globalPosition().phi());
    }
    const_cast<pat::Muon*>(&*p)->addUserInt("prop", 1);
  }
  if (p->hasUserFloat("l1Eta") && p->hasUserFloat("l1Phi")) {
    c.addUserFloat("l1Eta", p->userFloat("l1Eta"));
    c.addUserFloat("l1Phi", p->userFloat("l1Phi"));
  }
};


void ParticleDaughter::addData(pat::GenericParticle& c, const pat::ElectronRef& p, const bool& embedInfo)
{
  const auto& trkC = reco::TrackCollection({*dynamic_cast<const reco::Track*>(p->gsfTrack().get())});
  c.setTrack(reco::TrackRef(&trkC, 0), true);
  c.addUserData<reco::TrackRef>("trackRef", p->core()->ctfTrack());
  c.addUserData<pat::Electron>("src", *p);
};


void ParticleDaughter::setMVA(pat::GenericParticle& c, const size_t& i, const edm::Handle<std::vector<float> >& mvaColl)
{
  if (mvaColl.isValid() && i<mvaColl->size()) {
    c.addUserFloat("mva", mvaColl->at(i));
  }
};


void ParticleDaughter::setDeDx(pat::GenericParticle& c,
    const std::map<std::string, edm::Handle<edm::ValueMap<reco::DeDxData> > >& dEdxMaps)
{
  const auto& track = (c.hasUserData("trackRef") ? *c.userData<reco::TrackRef>("trackRef") : c.track());
  if (track.isNull()) return;
  for (const auto& dEdxMapPair : dEdxMaps) {
    const auto& dEdxMap = dEdxMapPair.second;
    if (dEdxMap->contains(track.id()) || track.key() < dEdxMap->size()) {
      const auto& par = dEdxMap->contains(track.id()) ? (*dEdxMap)[track] : dEdxMap->get(track.key());
      c.addUserFloat("dEdx_"+dEdxMapPair.first, par.dEdx());
      c.addUserFloat("dEdxErr_"+dEdxMapPair.first, par.dEdxError());
    }
  }
};


void ParticleDaughter::setSim(pat::GenericParticle& c, const edm::Handle<reco::RecoToSimCollection>& recToSimMap)
{
  if (not recToSimMap.isValid()) return;
  const auto& track = (c.hasUserData("trackRef") ? *c.userData<reco::TrackRef>("trackRef") : c.track());
  if (track.isNull() || track.id()!=recToSimMap->refProd().key.id()) return;
  const auto& matchedSim = recToSimMap->find(edm::RefToBase<reco::Track>(track));
  if (matchedSim != recToSimMap->end())
    c.addUserData<TrackingParticleRef>("simRef", matchedSim->val.front().first);
};
