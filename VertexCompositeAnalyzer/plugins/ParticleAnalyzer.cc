// system include files
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "TTree.h"
#include "TRegexp.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/Registry.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include "DataFormats/HeavyIonEvent/interface/CentralityBins.h"
#include "DataFormats/HeavyIonEvent/interface/Centrality.h"
#include "DataFormats/HeavyIonEvent/interface/EvtPlane.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Luminosity/interface/LumiInfo.h"
#include "DataFormats/OnlineMetaData/interface/OnlineLuminosityRecord.h"
#include "DataFormats/EgammaCandidates/interface/HIPhotonIsolation.h"
#include "DataFormats/Math/interface/angle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/Associations/interface/TrackAssociation.h"
#include "CondFormats/DataRecord/interface/L1TGlobalPrescalesVetosRcd.h"
#include "CondFormats/L1TObjects/interface/L1TGlobalPrescalesVetos.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/PatternTools/interface/trackingParametersAtClosestApproachToBeamSpot.h"
#include "VertexCompositeAnalysis/VertexCompositeAnalyzer/plugins/ZDCHardCodeHelper.h"
#include "HepPDT/ParticleID.hh"

#include "VertexCompositeAnalysis/VertexCompositeProducer/interface/ParticleFitter.h"
#include "ParticleContainer.h"

//
// constants, enums and typedefs
//

//
// class decleration
//

class ParticleAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchRuns> {
public:
  explicit ParticleAnalyzer(const edm::ParameterSet&);
  ~ParticleAnalyzer();

private:
  virtual void beginJob();
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void endRun(const edm::Run&, const edm::EventSetup&) {};
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void getEventData(const edm::Event&, const edm::EventSetup&);
  virtual void getTriggerData(const edm::Event&, const edm::EventSetup&);
  virtual void fillEventInfo(const edm::Event&);
  virtual void fillTriggerInfo(const edm::Event&);
  virtual void fillLumiInfo(const edm::Event&);
  virtual void fillRecoParticleInfo(const edm::Event&);
  virtual void fillGenParticleInfo(const edm::Event&);
  virtual void fillSimParticleInfo(const edm::Event&);
  virtual void endJob();
  virtual void initTree();
  virtual void initNTuple();
  virtual void addParticleToNtuple(const size_t&, const std::pair<int, int>&);
  virtual void fillNTuple(const bool& fill=true);
  virtual void check();

  UShort_t fillTriggerObjectInfo(const pat::TriggerObjectStandAlone&, const UShort_t&, const bool&, const UInt_t& candIdx=UINT_MAX);
  UInt_t   fillRecoParticleInfo(const pat::GenericParticle&, const UInt_t& momIdx=UINT_MAX);
  UShort_t fillTrackInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillSourceInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillMuonInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillElectronInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillPhotonInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillConversionInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillJetInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillTauInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillPFCandidateInfo(const pat::GenericParticle&, const UInt_t&, const bool& force=false);
  UShort_t fillGenParticleInfo(const reco::GenParticleRef&, const UInt_t& candIdx=UINT_MAX, const bool& force=false);
  UShort_t fillSimParticleInfo(const TrackingParticleRef&, const UInt_t& candIdx=UINT_MAX, const bool& force=false);

  void initParticleInfo(const std::string&, const Token& sid=Token::Unknown);
  void addTriggerObject(pat::GenericParticle&);
  bool addTriggerObject(pat::GenericParticle&, const math::XYZTLorentzVector&, const TriggerIndexMap&, const std::string&, const std::string&, const int&);
  bool addGenParticle(pat::GenericParticle&, const math::XYZTLorentzVector&, const reco::GenParticleRefVector&);
  void findGenDaughters(reco::GenParticleRefVector&, const reco::GenParticleRef&, const pat::GenericParticle&, const short& iter=0);
  reco::GenParticleRef findGenDaughter(const reco::GenParticleRef&, const size_t&);
  reco::GenParticleRef findGenMother(const reco::GenParticleRef&);

  int   getInt  (const pat::GenericParticle& c, const std::string& n, const int&   d=-99  ) const { return (c.hasUserInt(n)   ? c.userInt(n)   : d); }
  float getFloat(const pat::GenericParticle& c, const std::string& n, const float& d=-99.9) const { return (c.hasUserFloat(n) ? c.userFloat(n) : d); }
  template <class T> Char_t   getChar  (const T& n, const std::string& p) const { if (!(n >= CHAR_MIN && n < CHAR_MAX )) { throw(cms::Exception("Overflow")<<p<<" invalid char: "  <<n); }; return Char_t(n);   }
  template <class T> UChar_t  getUChar (const T& n, const std::string& p) const { if (!(n >= 0        && n < UCHAR_MAX)) { throw(cms::Exception("Overflow")<<p<<" invalid uchar: " <<n); }; return UChar_t(n);  }
  template <class T> Short_t  getShort (const T& n, const std::string& p) const { if (!(n >= SHRT_MIN && n < SHRT_MAX )) { throw(cms::Exception("Overflow")<<p<<" invalid short: " <<n); }; return Short_t(n);  }
  template <class T> UShort_t getUShort(const T& n, const std::string& p) const { if (!(n >= 0        && n < USHRT_MAX)) { throw(cms::Exception("Overflow")<<p<<" invalid ushort: "<<n); }; return UShort_t(n); }
  template <class T> Int_t    getInt   (const T& n, const std::string& p) const { if (!(n >= INT_MIN  && n < INT_MAX  )) { throw(cms::Exception("Overflow")<<p<<" invalid int: "   <<n); }; return Int_t(n);    }
  template <class T> UInt_t   getUInt  (const T& n, const std::string& p) const { if (!(n >= 0        && n < UINT_MAX )) { throw(cms::Exception("Overflow")<<p<<" invalid uint: "  <<n); }; return UInt_t(n);   }
  
  template <class T1, class T2>
  bool contain (const T1& v, const T2& o) const { return (std::find(v.begin(), v.end(), o)!=v.end()); }
  void insert (reco::GenParticleRefVector& v, const reco::GenParticleRef& p) { if (p.isNonnull() && !contain(v, p)) { v.push_back(p); } }

  bool isCompatible(const reco::Candidate& p1, const reco::Candidate& p2) const
  {
    return ((p1.status()==1)==(p2.status()==1) && p1.charge()==p2.charge() && ((p1.status()==1 && p2.pdgId()!=0) ? std::abs(p1.pdgId())==std::abs(p2.pdgId()) : true));
  }
  double deltaPt(const double& pT1, const double& pT2) const
  {
    return std::abs(pT1 - pT2)/std::sqrt(pT1*pT2);
  }
  bool isMatched(const math::XYZTLorentzVector& c1, const math::XYZTLorentzVector& c2, const double& maxDeltaR, const double& maxDeltaPtRel)
  {
    const auto deltaR = reco::deltaR(c1.Eta(), c1.Phi(), c2.Eta(), c2.Phi());
    const auto dPtRel = deltaPt(c1.Pt(), c2.Pt());
    return (deltaR < maxDeltaR && dPtRel < maxDeltaPtRel);
  }
  bool isL1MuMatched(const math::PtEtaPhiMLorentzVector& c, const math::XYZTLorentzVector& t, const double& maxDeltaR, const double& maxDeltaEta, const double& maxDeltaPhi)
  {
    const auto deltaR = reco::deltaR(c.Eta(), c.Phi(), t.Eta(), t.Phi());
    const auto delEta = std::abs(c.Eta() - t.Eta());
    const auto delPhi = std::abs(reco::deltaPhi(c.Phi(), t.Phi()));
    return (deltaR < maxDeltaR && delEta < maxDeltaEta && delPhi < maxDeltaPhi);
  }
  bool passGenStatus(const reco::GenParticleRef& p) const
  {
    return (p->status()==1 || p->statusFlags().isDecayedLeptonHadron());
  }

  edm::ParameterSet getConfiguration(const std::string&, const std::string&, const edm::Run&);
  edm::ParameterSet getConfiguration(const edm::EDGetToken&, const edm::Run&);
  void              loadConfiguration(const edm::ParameterSet&, const edm::Run&);

  // ----------member data ---------------------------

  // input tokens
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> bField_esToken_;
  const edm::ESGetToken<L1TGlobalPrescalesVetosFract, L1TGlobalPrescalesVetosFractRcd> l1Pres_esToken_;
  const edm::EDGetTokenT<reco::BeamSpot> tok_offlineBS_;
  const edm::EDGetTokenT<reco::VertexCollection> tok_offlinePV_;
  const edm::EDGetTokenT<pat::GenericParticleCollection> tok_recParticle_;
  const edm::EDGetTokenT<reco::GenParticleCollection> tok_genParticle_;
  const edm::EDGetTokenT<GenEventInfoProduct> tok_genInfo_;
  const edm::EDGetTokenT<LHEEventProduct> tok_lheInfo_;
  const edm::EDGetTokenT<int> tok_centBin_;
  const edm::EDGetTokenT<reco::Centrality> tok_centSrc_;
  const edm::EDGetTokenT<reco::EvtPlaneCollection> tok_eventPlaneSrc_;
  const edm::EDGetTokenT<edm::TriggerResults> tok_triggerResults_;
  const edm::EDGetTokenT<trigger::TriggerEvent> tok_triggerEvent_;
  const edm::EDGetTokenT<edm::TriggerResults> tok_filterResults_;
  const edm::EDGetTokenT<LumiInfo> tok_lumiInfo_;
  const edm::EDGetTokenT<OnlineLuminosityRecord> tok_lumiRecord_;
  std::vector< edm::EDGetTokenT<LumiInfo> > tok_triggerLumiInfo_;
  const edm::EDGetTokenT<edm::ValueMap<int> > tok_nTracksVMap_;
  const edm::EDGetTokenT<reco::TrackCollection> tok_trackSrc_;
  const edm::EDGetTokenT<QIE10DigiCollection> tok_zdcDigiSrc_;
  const edm::EDGetTokenT<edm::SortedCollection<ZDCRecHit> > tok_zdcRecHitSrc_;
  const edm::EDGetTokenT<edm::View<reco::Candidate> > tok_pfCandSrc_;
  const edm::EDGetTokenT<CaloTowerCollection> tok_towerSrc_;
  const edm::EDGetTokenT<TrackingParticleCollection> tok_simParticle_;

  // input data
  const ZDCHardCodeHelper HardCodeZDC_;
  const std::vector<edm::ParameterSet> triggerInfo_, matchInfo_;
  const std::vector<std::string> eventFilters_;
  const std::string dataset_, selectEvents_;
  const bool saveTree_;
  const int maxGenIter_;
  const double maxGenDeltaR_, maxGenDeltaPtRel_;
  const std::vector<UInt_t> genPdgIdV_;
  std::map<std::string, bool> addInfo_;
  std::set<Token> sourceId_;
  std::set<UInt_t> genPdgId_;
  std::vector<edm::InputTag> dedxInfo_;

  HLTPrescaleProvider hltPrescaleProvider_;
  std::vector<std::vector<double> > l1PrescaleTable_;
  std::map<std::string, std::vector<size_t> > hltPathIdx_;
  std::vector<size_t> datasetPathIdx_;
  char datasetN_;

  // attributes
  edm::Service<TFileService> fileService_;
  TTree* tree_ = 0;
  TTree* ntuple_ = 0;

  bool isMC_, vtxSortByTrkSize_;
  reco::Vertex vertex_;
  reco::Particle::Point genVertex_;
  reco::VertexCollection vertices_;
  reco::GenParticleRefVector genParticlesToKeep_, genParticlesToMatch_;
  const MagneticField* magField_;
  reco::BeamSpot beamSpot_;
  edm::ValueMap<int> nTracksVMap_;

  // class container attributes
  Container eventInfo_, ntupleInfo_;
  TriggerContainer triggerData_;
  MatchContainer matchData_;
  ParticleContainerMap particleInfo_;
  std::map<std::string, std::map<size_t, pat::TriggerObjectStandAlone> > triggerObjectMap_;
};

//
// static data member definitions
//

//
// constructors and destructor
//

ParticleAnalyzer::ParticleAnalyzer(const edm::ParameterSet& iConfig) :
  bField_esToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>()),
  l1Pres_esToken_(esConsumes<edm::Transition::BeginRun>()),
  tok_offlineBS_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
  tok_offlinePV_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("primaryVertices"))),
  tok_recParticle_(consumes<pat::GenericParticleCollection>(iConfig.getParameter<edm::InputTag>("recoParticles"))),
  tok_genParticle_(consumes<reco::GenParticleCollection>(iConfig.getUntrackedParameter<edm::InputTag>("genParticles", edm::InputTag("genParticles")))),
  tok_genInfo_(consumes<GenEventInfoProduct>(iConfig.getUntrackedParameter<edm::InputTag>("genInfo", edm::InputTag("generator")))),
  tok_lheInfo_(consumes<LHEEventProduct>(iConfig.getUntrackedParameter<edm::InputTag>("lheInfo", edm::InputTag("externalLHEProducer","")))),
  tok_centBin_(consumes<int>(iConfig.getUntrackedParameter<edm::InputTag>("centralityBin"))),
  tok_centSrc_(consumes<reco::Centrality>(iConfig.getUntrackedParameter<edm::InputTag>("centrality"))),
  tok_eventPlaneSrc_(consumes<reco::EvtPlaneCollection>(iConfig.getUntrackedParameter<edm::InputTag>("eventPlane"))),
  tok_triggerResults_(consumes<edm::TriggerResults>(iConfig.getUntrackedParameter<edm::InputTag>("triggerResults", edm::InputTag("TriggerResults::HLT")))),
  tok_triggerEvent_(consumes<trigger::TriggerEvent>(iConfig.getUntrackedParameter<edm::InputTag>("triggerEvent", edm::InputTag("hltTriggerSummaryAOD::HLT")))),
  tok_filterResults_(consumes<edm::TriggerResults>(iConfig.getUntrackedParameter<edm::InputTag>("eventFilterResults", edm::InputTag("TriggerResults")))),
  tok_lumiInfo_(consumes<LumiInfo>(iConfig.getUntrackedParameter<edm::InputTag>("lumiInfo"))),
  tok_lumiRecord_(consumes<OnlineLuminosityRecord>(iConfig.getUntrackedParameter<edm::InputTag>("lumiRecord", edm::InputTag("onlineMetaDataDigis")))),
  tok_nTracksVMap_(consumes<edm::ValueMap<int> >(iConfig.getUntrackedParameter<edm::InputTag>("nTracksVMap", edm::InputTag()))),
  tok_trackSrc_(consumes<reco::TrackCollection>(iConfig.getUntrackedParameter<edm::InputTag>("recoTracks", edm::InputTag("generalTracks")))),
  tok_zdcDigiSrc_(consumes<QIE10DigiCollection>(iConfig.getUntrackedParameter<edm::InputTag>("zdcDigis", edm::InputTag("hcalDigis", "ZDC")))),
  tok_zdcRecHitSrc_(consumes<edm::SortedCollection<ZDCRecHit> >(iConfig.getUntrackedParameter<edm::InputTag>("zdcRecHits", edm::InputTag("zdcreco")))),
  tok_pfCandSrc_(consumes<edm::View<reco::Candidate> >(iConfig.getUntrackedParameter<edm::InputTag>("pfCandidates", edm::InputTag("particleFlow")))),
  tok_towerSrc_(consumes<CaloTowerCollection>(iConfig.getUntrackedParameter<edm::InputTag>("towers", edm::InputTag("towerMaker")))),
  tok_simParticle_(consumes<TrackingParticleCollection>(iConfig.getUntrackedParameter<edm::InputTag>("towers", edm::InputTag("mix:MergedTrackTruth")))),
  triggerInfo_(iConfig.getUntrackedParameter<std::vector<edm::ParameterSet> >("triggerInfo")),
  matchInfo_(iConfig.getUntrackedParameter<std::vector<edm::ParameterSet> >("matchInfo")),
  eventFilters_(iConfig.getUntrackedParameter<std::vector<std::string> >("eventFilterNames")),
  dataset_(iConfig.getUntrackedParameter<std::string>("dataset", "")),
  selectEvents_(iConfig.getParameter<std::string>("selectEvents")),
  saveTree_(iConfig.getUntrackedParameter<bool>("saveTree", true)),
  maxGenIter_(iConfig.getUntrackedParameter<int>("maxGenIter", 0)),
  maxGenDeltaR_(iConfig.getUntrackedParameter<double>("maxGenDeltaR", 0.03)),
  maxGenDeltaPtRel_(iConfig.getUntrackedParameter<double>("maxGenDeltaPtRel", 0.5)),
  genPdgIdV_(iConfig.getUntrackedParameter<std::vector<UInt_t> >("genPdgId", {})),
  hltPrescaleProvider_(iConfig, consumesCollector(), *this)
{
  for (const auto& data : triggerInfo_)
  {
    tok_triggerLumiInfo_.emplace_back(consumes<LumiInfo>(data.existsAs<edm::InputTag>("lumiInfo") ? data.getParameter<edm::InputTag>("lumiInfo") : edm::InputTag()));
  }
  addInfo_["trgObj"] = iConfig.getUntrackedParameter<bool>("addTrgObj", false);
}


ParticleAnalyzer::~ParticleAnalyzer()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
ParticleAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //check event
  if (selectEvents_!="")
  {
    const auto& filterResults = iEvent.getHandle(tok_filterResults_);
    const auto& filterNames = iEvent.triggerNames(*filterResults);
    const auto& index = filterNames.triggerIndex(selectEvents_);
    if (index >= filterNames.size()) throw(cms::Exception("Analyzer")<<"Invalid selection filter: "<<selectEvents_);
    if (index < filterNames.size() && filterResults->wasrun(index) && !filterResults->accept(index)) return;
  }

  // clear information
  vertex_ = reco::Vertex();
  genVertex_ = reco::Vertex();
  vertices_.clear();
  genParticlesToKeep_.clear();
  genParticlesToMatch_.clear();
  eventInfo_.clear();
  triggerData_.clear();
  triggerObjectMap_.clear();
  for (auto& p : particleInfo_) { p.second.clear(); }

  // get event data
  getEventData(iEvent, iSetup);
  getTriggerData(iEvent, iSetup);

  // fill information
  fillEventInfo(iEvent);
  fillTriggerInfo(iEvent);
  fillLumiInfo(iEvent);
  fillRecoParticleInfo(iEvent);
  fillGenParticleInfo(iEvent);
  fillSimParticleInfo(iEvent);

  // check information
  check();

  // fill tree or ntuple
  if (saveTree_)
  {
    if (!tree_) initTree();
    tree_->Fill();
  }
  else
  {
    fillNTuple();
  }
}


void
ParticleAnalyzer::getEventData(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // general information
  isMC_ = !iEvent.isRealData();

  // magnetic field information
  const auto& bFieldHandle = iSetup.getHandle(bField_esToken_);
  magField_ = bFieldHandle.product();

  // beamspot information
  beamSpot_ = iEvent.get(tok_offlineBS_);

  // nTracks value map
  const auto& nTracksVMap = iEvent.getHandle(tok_nTracksVMap_);
  if (nTracksVMap.isValid()) { nTracksVMap_ = *nTracksVMap; }

  // primary vertex information
  const auto& vertices = iEvent.getHandle(tok_offlinePV_);
  if (vertices.isValid()) {
    for (const auto& pv : *vertices)
    {
      if (!pv.isFake() && pv.tracksSize() >= 2)
      {
        vertices_.emplace_back(pv);
      }
    }
  }
  if (vtxSortByTrkSize_) {
    auto byTracksSize = [] (const reco::Vertex& v1, const reco::Vertex& v2) -> bool { return v1.tracksSize() > v2.tracksSize(); };
    std::sort(vertices_.begin(), vertices_.end(), byTracksSize);
  }
  if (vertices_.empty())
  {
    vertex_ = reco::Vertex(beamSpot_.position(), beamSpot_.rotatedCovariance3D());
  }
  else { vertex_ = vertices_[0]; }

  // generated particle information
  const auto& genParticles = iEvent.getHandle(tok_genParticle_);
  if (isMC_ && genParticles.isValid())
  {
    // generated primary vertex information
    for (const auto& p : *genParticles)
    {
      if (p.statusFlags().isLastCopy() && (p.pdgId()==21 || std::abs(p.pdgId())<=6 || p.numberOfMothers()==0))
      {
        genVertex_ = p.vertex();
        break;
      }
    }
    // initialize generated particle container
    initParticleInfo("gen");
    int nGenTracks = 0;
    // extract generated particles
    for (size_t i=0; i<genParticles->size(); i++)
    {
      const auto& p = reco::GenParticleRef(genParticles, i);
      // sum charged tracks within CMS acceptance
      if (p->isLastCopy() && p->status()==1 && p->charge()!=0 && p->pt()>0.4 && std::abs(p->eta())<2.4)
      {   
        nGenTracks += 1;
      }
      // add generated particles for reco-gen matching
      if (p->isLastCopy() && passGenStatus(p) && ((genPdgId_.empty() || *genPdgId_.begin()!=0) ? contain(genPdgId_, std::abs(p->pdgId())) : true))
      {
        const auto& mom = ((p->status()==1 && genPdgId_.size()>1) ? findGenMother(p) : reco::GenParticleRef());
        if (mom.isNull() || contain(genPdgId_, std::abs(mom->pdgId())))
        {
          genParticlesToMatch_.push_back(p);
        }
      }
    }
    for (const auto& p : genParticlesToMatch_)
    {
      // keep generated particle
      insert(genParticlesToKeep_, p);
      // keep its mother
      insert(genParticlesToKeep_, findGenMother(p));
      // keep its daughters
      for (size_t iDau=0; iDau<p->numberOfDaughters(); iDau++)
      {
        insert(genParticlesToKeep_, findGenDaughter(p, iDau));
      }
    }
    // fill generated particle multiplicity
    eventInfo_.add("Ntrkgen", nGenTracks);
  }

  // simulated particle information
  if (isMC_ && addInfo_["sim"])
  {
    // initialize simulated particle container
    initParticleInfo("sim");
  }
}


void
ParticleAnalyzer::getTriggerData(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if (triggerInfo_.empty()) return;

  const auto& triggerResults = iEvent.getHandle(tok_triggerResults_);

  // get current dataset
  datasetN_ = -1;
  if (triggerResults.isValid())
  {
    for (const auto& dsIdx : datasetPathIdx_)
    {
      if (triggerResults->accept(dsIdx)) { datasetN_ = (dsIdx - datasetPathIdx_[0]); break; }
    }
  }

  // get trigger data
  const auto& triggerEvent = iEvent.getHandle(tok_triggerEvent_);
  const auto& isTrgEvtValid = triggerEvent.isValid();
  if (triggerResults.isValid())
  {
    // initialize trigger container
    triggerData_.resize(triggerInfo_.size());
    // retrieve trigger information
    const auto& l1tGlobalUtil = hltPrescaleProvider_.l1tGlobalUtil();
    const auto& hltConfig = hltPrescaleProvider_.hltConfigProvider();
    const bool validPrescale = (hltConfig.inited() && hltPrescaleProvider_.prescaleSet(iEvent, iSetup) >= 0);
    const auto& hltPaths = hltConfig.triggerNames();
    // extract matching information
    std::vector<std::string> objCol;
    if (isTrgEvtValid)
    {
      objCol = std::vector<std::string>(triggerEvent->getObjects().size());
      const auto& cK = triggerEvent->collectionKeys();
      for (size_t i=0; i<cK.size(); i++)
      {
        const auto& coll = triggerEvent->collectionTag(i).encode();
        // add default limits
        double maxDeltaR = 1.E6, maxDeltaPtRel = 1.E6, maxDeltaEta = 1.E6, maxDeltaPhi = 1.E6;
        if (coll.find("Stage2Digis:Muon")!=std::string::npos) { maxDeltaR = 0.3; maxDeltaEta = 0.2; maxDeltaPhi = 6.0; } // L1 muon
        else if (coll.find("L2Muon")!=std::string::npos) { maxDeltaR = 0.3; maxDeltaPtRel = 10.0; } // L2 muon
        else if (coll.find("L3Muon")!=std::string::npos) { maxDeltaR = 0.1; maxDeltaPtRel = 10.0; } // L3 muon
        else { maxDeltaR = 0.3; maxDeltaPtRel = 10.0; } // default
        // set user defined limits
        for (const auto& pSet : matchInfo_)
        {
          if (coll.find(pSet.getParameter<std::string>("collection"))==std::string::npos) continue;
          if (pSet.existsAs<double>("maxDeltaR"    )) maxDeltaR     = pSet.getParameter<double>("maxDeltaR");
          if (pSet.existsAs<double>("maxDeltaPtRel")) maxDeltaPtRel = pSet.getParameter<double>("maxDeltaPtRel");
          if (pSet.existsAs<double>("maxDeltaEta"  )) maxDeltaEta   = pSet.getParameter<double>("maxDeltaEta");
          if (pSet.existsAs<double>("maxDeltaPhi"  )) maxDeltaPhi   = pSet.getParameter<double>("maxDeltaPhi");
          break;
        }
        // store trigger matching information
        matchData_[coll].setInfo(coll, maxDeltaR, maxDeltaPtRel, maxDeltaEta, maxDeltaPhi);
        // fill trigger object - collection map
        for (size_t j=(i<1?0:cK[i-1]); j<cK[i]; j++) { objCol[j] = coll; }
      }
    }
    // extract trigger information
    for (size_t iTrg=0; iTrg<triggerInfo_.size(); iTrg++)
    {
      const auto& pSet = triggerInfo_[iTrg];
      const auto& minN = (pSet.existsAs<int>("minN") ? pSet.getParameter<int>("minN") : 0);
      const auto& isL1OR = (pSet.existsAs<bool>("isL1OR") ? pSet.getParameter<bool>("isL1OR") : true);
      const auto& pathLabel = (pSet.existsAs<std::string>("path") ? pSet.getParameter<std::string>("path") : std::string());
      const auto& filterLabel = (pSet.existsAs<std::string>("filter") ? pSet.getParameter<std::string>("filter") : std::string());
      const auto& tok_lumiInfo = tok_triggerLumiInfo_[iTrg];
      // initialize trigger data
      int triggerIndex=-1, filterIndex=-1;
      // extract the trigger index
      const auto& trgIdxFound = pathLabel!="" ? hltPathIdx_.at(pathLabel) : std::vector<size_t>();
      if (pathLabel!="" && trgIdxFound.empty()) continue;
      // extract the filter index
      std::vector<size_t> filterIdxFound;
      if (filterLabel!="" && isTrgEvtValid)
      {
        for (size_t filterIdx=0; filterIdx<triggerEvent->sizeFilters(); filterIdx++)
        {
          const auto& filterName = std::string(triggerEvent->filterLabel(filterIdx));
          if (TString(filterName).Contains(TRegexp(TString(filterLabel)))) { filterIdxFound.emplace_back(filterIdx); }
        }
      }
      // match the trigger and filter
      if (pathLabel!="" && !filterIdxFound.empty())
      {
        std::vector<std::pair<size_t, size_t> > trgFilterIdxFound;
        for (const auto& trgIdx : trgIdxFound)
        {
          for (const auto& filterIdx : filterIdxFound)
          {
            const auto& filterName = std::string(triggerEvent->filterLabel(filterIdx));
            if (hltConfig.moduleIndex(trgIdx, filterName)!=hltConfig.size(trgIdx)) { trgFilterIdxFound.emplace_back(trgIdx, filterIdx); }
          }
        }
        if (trgFilterIdxFound.empty()) continue;
        // determine the trigger and filter index
        auto triggerFilterIndex = trgFilterIdxFound[0];
        for (const auto& trgFilterIdx : trgFilterIdxFound) { if (triggerResults->accept(trgFilterIdx.first)) { triggerFilterIndex = trgFilterIdx; break; } }
        triggerIndex = triggerFilterIndex.first;
        filterIndex = triggerFilterIndex.second;
      }
      // if no filter provided, select the last filter module ran
      else if (pathLabel!="" && filterIdxFound.empty())
      {
        // determine the trigger index
        triggerIndex = trgIdxFound[0];
        for (const auto& trgIdx : trgIdxFound) { if (triggerResults->accept(trgIdx)) { triggerIndex = trgIdx; break; } }
        // find last filter module
        if (filterLabel=="" && triggerResults->accept(triggerIndex) && isTrgEvtValid)
        {
          for (int j = hltConfig.moduleLabels(triggerIndex).size()-1; j >= 0; j--)
          {
            const auto& filter = hltConfig.moduleLabels(triggerIndex).at(j);
            const auto& filterIdx = triggerEvent->filterIndex(edm::InputTag(filter, "", hltConfig.processName()));
            if (filterIdx < triggerEvent->sizeFilters()) { filterIndex = filterIdx; break; }
          }
          if (filterIndex<0) { throw(cms::Exception("Trigger")<<"Invalid filter index for: "<<pathLabel<<" , "<<filterLabel); }
        }
      }
      // if no path provided, select the first filter module found
      else if (pathLabel=="" && !filterIdxFound.empty())
      {
        filterIndex = filterIdxFound[0];
      }
      else if (pathLabel=="" && filterIdxFound.empty()) continue;
      // determine the trigger/filter name
      const auto triggerName = (triggerIndex>=0 ? hltPaths.at(triggerIndex) : std::string());
      const auto filterName  = (isTrgEvtValid && filterIndex>=0 ? std::string(triggerEvent->filterLabel(filterIndex)) : std::string());
      // determine the trigger decision
      std::array<bool,4> bit;
      bit[0] = (triggerIndex>=0 ? triggerResults->accept(triggerIndex) : false);
      // extract prescale information
      double hltPrescale=0, l1Prescale=0;
      if (validPrescale && triggerIndex>=0)
      {
        // HLT information
        const auto& presInfo = hltPrescaleProvider_.prescaleValuesInDetail<double>(iEvent, iSetup, triggerName);
        hltPrescale = presInfo.second;
        const auto& lastModule = hltConfig.moduleLabel(triggerIndex, triggerResults->index(triggerIndex));
        bit[2] = hltConfig.moduleType(lastModule)!="HLTPrescaler";
        // L1 information
        if (!presInfo.first.empty())
        {
          // L1 prescale
          const auto& pCol = l1tGlobalUtil.prescaleColumn();
          const auto& l1DecisionsFinal = const_cast<l1t::L1TGlobalUtil*>(&l1tGlobalUtil)->decisionsFinal();
          typedef std::tuple<bool, double, int> l1T_t;
          std::vector<l1T_t> l1Info;
          for (const auto& p : presInfo.first)
          {
            int l1Bit; if (!l1tGlobalUtil.getAlgBitFromName(p.first, l1Bit)) continue;
            auto pres = (isMC_ ? 1 : (pCol<l1PrescaleTable_.size() && l1PrescaleTable_[pCol][l1Bit]>0 ? l1PrescaleTable_[pCol][l1Bit] : 1E9));
            bool fail = !l1DecisionsFinal[l1Bit].second;
            l1Info.emplace_back(fail, pres, l1Bit);
          }
          std::sort(l1Info.begin(), l1Info.end(), [=](const l1T_t& i, const l1T_t& j) { return (isL1OR ? i < j : i > j); });
          l1Prescale = std::get<1>(l1Info[0]); if (std::abs(l1Prescale)==1E9) { l1Prescale = 0; }
          // L1 decision
          const auto& l1Bit = std::get<2>(l1Info[0]);
          const auto& l1DecisionsInitial = const_cast<l1t::L1TGlobalUtil*>(&l1tGlobalUtil)->decisionsInitial();
          bit[1] = l1DecisionsInitial[l1Bit].second;
          bit[3] = (l1DecisionsInitial[l1Bit].second == l1DecisionsFinal[l1Bit].second);
        }
      }
      // extract filter objects
      std::map<std::string, std::vector<size_t> > filterObjects;
      const auto& filterKeys = (isTrgEvtValid && filterIndex>=0 ? triggerEvent->filterKeys(filterIndex) : std::vector<UShort_t>());
      const auto& filterIds = (isTrgEvtValid && filterIndex>=0 ? triggerEvent->filterIds(filterIndex) : std::vector<int>());
      for (size_t iKey=0; iKey<filterKeys.size() && minN>0; iKey++)
      {
        const auto& col = objCol[filterKeys[iKey]];
        // if map does not have collection, add all associated trigger objects
        if (triggerObjectMap_.find(col)==triggerObjectMap_.end())
        {
          const auto& i = triggerEvent->collectionIndex(col);
          const auto& cK = triggerEvent->collectionKeys();
          const auto& triggerObjects = triggerEvent->getObjects();
          for (size_t j=(i<1?0:cK[i-1]); j<cK[i]; j++)
          {
            triggerObjectMap_[col][j] = triggerObjects[j];
            triggerObjectMap_[col][j].setCollection(col);
          }
        }
        // add trigger information to object
        auto& obj = triggerObjectMap_.at(col).at(filterKeys[iKey]);
        obj.addFilterId(filterIds[iKey]);
        obj.addFilterLabel(filterName);
        filterObjects[col].emplace_back(filterKeys[iKey]);
      }
      // store trigger information
      triggerData_[iTrg].setInfo(triggerIndex, filterIndex, triggerName, filterName, minN, validPrescale, hltPrescale, l1Prescale, bit, filterObjects);
      // store lumi information per trigger
      const auto& lumiInfo = iEvent.getHandle(tok_lumiInfo);
      if (!isMC_ && lumiInfo.isValid())
      {
        triggerData_[iTrg].setLumiInfo(lumiInfo->recordedLuminosity(), lumiInfo->integLuminosity());
      }
    }
  }
}


void
ParticleAnalyzer::fillEventInfo(const edm::Event& iEvent)
{
  // fill general information
  eventInfo_.add("RunNb", iEvent.id().run());
  eventInfo_.add("EventNb", getUInt(iEvent.id().event(), "EventNb"));
  eventInfo_.add("LSNb", getUInt(iEvent.luminosityBlock(), "LSNb"));
  const auto& bx = iEvent.bunchCrossing();
  eventInfo_.add("BXNb", getUShort(bx>=0 ? bx : 0, "BXNb"));

  // fill vertex information
  eventInfo_.add("nPV", getUChar(vertices_.size(), "nPV"));
  eventInfo_.add("bestvtxX", vertex_.x());
  eventInfo_.add("bestvtxY", vertex_.y());
  eventInfo_.add("bestvtxZ", vertex_.z());
  eventInfo_.add("vtxXerr", vertex_.xError());
  eventInfo_.add("vtxYerr", vertex_.yError());
  eventInfo_.add("vtxZerr", vertex_.zError());
  eventInfo_.add("isValid", bool(vertex_.isValid() && !vertex_.isFake()));

  // fill event selection information
  const auto& filterResults = iEvent.getHandle(tok_filterResults_);
  if (filterResults.isValid() && !eventFilters_.empty())
  {
    std::vector<bool> evtSel;
    evtSel.reserve(eventFilters_.size());
    const auto& filterNames = iEvent.triggerNames(*filterResults);
    for (const auto& eventFilter : eventFilters_)
    {
      const auto& index = filterNames.triggerIndex(eventFilter);
      evtSel.emplace_back(index < filterNames.size() && filterResults->wasrun(index) && filterResults->accept(index));
    }
    eventInfo_.add("evtSel", evtSel);
  }

  // fill centrality information
  const auto& cent = iEvent.getHandle(tok_centSrc_);
  if (cent.isValid())
  {
    eventInfo_.add("HFsumETPlus", cent->EtHFtowerSumPlus());
    eventInfo_.add("HFsumETMinus", cent->EtHFtowerSumMinus());
    eventInfo_.add("Npixel", cent->multiplicityPixel());
    eventInfo_.add("NpixelTracks", cent->NpixelTracks());
    eventInfo_.add("ZDCPlus", cent->zdcSumPlus());
    eventInfo_.add("ZDCMinus", cent->zdcSumMinus());
    eventInfo_.add("Ntrkoffline", getUShort(cent->Ntracks(), "Ntrkoffline"));
  }
  const auto& centBin = iEvent.getHandle(tok_centBin_);
  if (centBin.isValid())
  {
    eventInfo_.add("centrality", getUChar(*centBin, "centrality"));
  }

  // fill event track information
  const auto& tracks = iEvent.getHandle(tok_trackSrc_);
  if (tracks.isValid())
  {
    size_t nHP(0);
    for (const auto& track : *tracks)
      if (track.quality(reco::TrackBase::highPurity))
        nHP += 1;
    eventInfo_.add("NtrkHP", getUShort(nHP, "NtrkHP"));
  }

  // fill event plane information
  const auto& eventPlanes = iEvent.getHandle(tok_eventPlaneSrc_);
  if (eventPlanes.isValid())
  {
    for (const auto& iEP : {0, 6})
    {
      eventInfo_.push("ephfmAngle",      eventPlanes->at(iEP  ).angle(2));
      eventInfo_.push("ephfpAngle",      eventPlanes->at(iEP+1).angle(2));
      eventInfo_.push("eptrackmidAngle", eventPlanes->at(iEP+3).angle(2));
      eventInfo_.push("ephfmQ",          eventPlanes->at(iEP  ).q(2));
      eventInfo_.push("ephfpQ",          eventPlanes->at(iEP+1).q(2));
      eventInfo_.push("eptrackmidQ",     eventPlanes->at(iEP+3).q(2));
    }
    eventInfo_.add("ephfmSumW",      eventPlanes->at(0).sumw());
    eventInfo_.add("ephfpSumW",      eventPlanes->at(1).sumw());
    eventInfo_.add("eptrackmidSumW", eventPlanes->at(3).sumw());
  }

  // fill ZDC information
  const auto& zdcDigis = iEvent.getHandle(tok_zdcDigiSrc_);
  const auto& zdcRecHits = iEvent.getHandle(tok_zdcRecHitSrc_);
  if (zdcRecHits.isValid())
  {
    float ZDCMinus(0), ZDCPlus(0);
    for (const auto& rh : *zdcRecHits) {
      HcalZDCDetId zdcid(rh.id());
      if ((zdcid.section() == 1 && zdcid.channel() <= 5) || zdcid.section() == 2)
        (zdcid.zside() < 0 ? ZDCMinus : ZDCPlus) += rh.energy();
    }
    eventInfo_.add("ZDCMinus", ZDCMinus);
    eventInfo_.add("ZDCPlus",  ZDCPlus);
    eventInfo_.add("hasZDC", not zdcRecHits->empty());
  }
  else if (zdcDigis.isValid())
  {
    float ZDCMinus(0), ZDCPlus(0);
    for (const auto& d : *zdcDigis) {
      const auto digi = static_cast<const QIE10DataFrame>(d);
      const auto chargefC1 = HardCodeZDC_.charge(digi[1].adc(), digi[1].capid());
      const auto chargefC2 = HardCodeZDC_.charge(digi[2].adc(), digi[2].capid());
      HcalZDCDetId zdcid(digi.id());
      const auto factor = (zdcid.zside() < 0 ? 0.5031 : 0.9397) * (zdcid.section() == 1 ? 0.1 : 1.);
      if (zdcid.section() == 1 || zdcid.section() == 2)
        (zdcid.zside() < 0 ? ZDCMinus : ZDCPlus) += (chargefC2 - chargefC1) * factor;
    }
    eventInfo_.add("ZDCMinus", ZDCMinus);
    eventInfo_.add("ZDCPlus",  ZDCPlus);
    eventInfo_.add("hasZDC", not zdcDigis->empty());
  }

  // fill PF information
  const auto& pfCandidates = iEvent.getHandle(tok_pfCandSrc_);
  if (pfCandidates.isValid())
  {
    float PFHFmaxEPlus(-1), PFHFmaxEMinus(-1), PFHFsumETPlus(0), PFHFsumETMinus(0);
    for (const auto& pf : *pfCandidates) {
      if (pf.pdgId() != 1 && pf.pdgId() != 2) continue;
      (pf.eta() > 0 ? PFHFsumETPlus : PFHFsumETMinus) += pf.pt();
      const auto aeta = std::abs(pf.eta());
      if (aeta < 3.0 || aeta > 6.0) continue;
      if (pf.eta() > 0 && PFHFmaxEPlus < pf.energy())
        PFHFmaxEPlus = pf.energy();
      if (pf.eta() < 0 && PFHFmaxEMinus < pf.energy())
        PFHFmaxEMinus = pf.energy();
    }
    eventInfo_.add("PFHFsumETPlus", PFHFsumETPlus);
    eventInfo_.add("PFHFsumETMinus", PFHFsumETMinus);
    eventInfo_.add("PFHFmaxEPlus", PFHFmaxEPlus);
    eventInfo_.add("PFHFmaxEMinus", PFHFmaxEMinus);
  }

  // fill Tower information
  const auto& towers = iEvent.getHandle(tok_towerSrc_);
  if (towers.isValid())
  {
    float TWHFmaxEPlus(-1), TWHFmaxEMinus(-1), TWHFsumETPlus(0), TWHFsumETMinus(0);
    for (const auto& tower : *towers) {
      if (tower.ietaAbs() <= 29) continue;
      (tower.eta() > 0 ? TWHFsumETPlus : TWHFsumETMinus) += tower.pt();
      const auto aeta = std::abs(tower.eta());
      if (aeta < 3.0 || aeta > 6.0) continue;
      if (tower.eta() > 0 && TWHFmaxEPlus < tower.energy())
        TWHFmaxEPlus = tower.energy();
      if (tower.eta() < 0 && TWHFmaxEMinus < tower.energy())
        TWHFmaxEMinus = tower.energy();
    }
    eventInfo_.add("TWHFsumETPlus", TWHFsumETPlus);
    eventInfo_.add("TWHFsumETMinus", TWHFsumETMinus);
    eventInfo_.add("TWHFmaxEPlus", TWHFmaxEPlus);
    eventInfo_.add("TWHFmaxEMinus", TWHFmaxEMinus);
  }
}


void
ParticleAnalyzer::fillLumiInfo(const edm::Event& iEvent)
{
  if (isMC_) return;

  // offline luminosity information
  const auto& lumiInfo = iEvent.getHandle(tok_lumiInfo_);
  if (lumiInfo.isValid())
  {
    eventInfo_.add("totalLumi", lumiInfo->integLuminosity());
    eventInfo_.add("recordLumi", lumiInfo->recordedLuminosity());
  }

  // online luminosity information
  const auto& lumiRecord = iEvent.getHandle(tok_lumiRecord_);
  if (lumiRecord.isValid())
  {
    eventInfo_.add("pileup", lumiRecord->avgPileUp());
    eventInfo_.add("rawInstLumi", lumiRecord->instLumi());
  }
}


void
ParticleAnalyzer::fillTriggerInfo(const edm::Event& iEvent)
{
  // initialize trigger object container
  initParticleInfo("trig");

  // fill trigger information
  for (UShort_t idx=0; idx<triggerData_.size(); idx++)
  {
    const auto& data = triggerData_[idx];
    eventInfo_.push("passHLT", data.triggerBit(0));
    eventInfo_.push("passL1", data.triggerBit(1));
    if (!isMC_)
    {
      eventInfo_.push("passHLTPrescaler", data.triggerBit(2));
      eventInfo_.push("passL1Prescaler", data.triggerBit(3));
      eventInfo_.push("hltPrescale", data.hltPrescale());
      eventInfo_.push("l1Prescale", data.l1Prescale());
    }
    for (const auto& c: data.filterObjects())
    {
      for (const auto& o: triggerObjectMap_.at(c.first))
      {
        fillTriggerObjectInfo(o.second, idx, o.second.hasFilterLabel(data.filterName()));
      }
    }
    if (data.validLumi())
    {
      eventInfo_.push("hltTotalLumi", data.totalLumi());
      eventInfo_.push("hltRecordLumi", data.recordLumi());
    }
  }

  // fill dataset information
  if (dataset_!="")
  {
    eventInfo_.add("datasetN", datasetN_);
  }
}


void
ParticleAnalyzer::initParticleInfo(const std::string& type, const Token& sid)
{
  // return if already initialized
  if (particleInfo_.find(type)!=particleInfo_.end()) return;
  // check type
  if      (type=="trig" && (triggerData_.empty() || !addInfo_.at("trgObj"))) return;
  else if (type=="trk"  && !addInfo_.at("track")) return;
  else if (type=="gen"  && !isMC_) return;
  else if (type=="sim"  && !(isMC_ && addInfo_.at("sim"))) return;
  else if (type=="src"  && !addInfo_.at("source")) return;
  // proceed to initialize with dummy value
  pat::GenericParticle cand; cand.addUserInt("sourceId", sid);
  if      (type=="cand") fillRecoParticleInfo(pat::GenericParticle(), 0);
  else if (type=="trig") fillTriggerObjectInfo(pat::TriggerObjectStandAlone(), 0, 0, 0);
  else if (type=="gen" ) fillGenParticleInfo(reco::GenParticleRef(), 0, true);
  else if (type=="sim" ) fillSimParticleInfo(TrackingParticleRef(), 0, true);
  else if (type=="trk" ) fillTrackInfo(pat::GenericParticle(), 0, true);
  else if (type=="src" ) fillSourceInfo(cand, 0, true);
  // clear the initialized info
  if (type!="src") { particleInfo_[type].clear(); }
  else
  {
    for (auto& p : particleInfo_)
    {
      if (p.first!="cand" && p.first!="trig" && p.first!="gen" && p.first!="sim" && p.first!="trk") { p.second.clear(); }
    }
  }
}


UShort_t
ParticleAnalyzer::fillTriggerObjectInfo(const pat::TriggerObjectStandAlone& obj, const UShort_t& trigIdx, const bool& pass, const UInt_t& candIdx)
{
  if (triggerData_.empty() || !addInfo_.at("trgObj")) return USHRT_MAX;

  // fill trigger information
  auto& info = particleInfo_["trig"];

  // add input information
  size_t index;
  const bool found = info.getIndex(index, obj);
  info.push(index, "candIdx", candIdx, true);
  const auto& idx = getUShort(index, "trig_idx");

  // set trigger pass
  for (size_t i=0; i<triggerData_.size() && !found; i++)
  {
    if (triggerData_[i].minN()>0) info.add(Form("pass%lu", i), false);
  }
  if (triggerData_[trigIdx].minN()>0) info.add(index, Form("pass%d", trigIdx), pass);

  // return if already added
  if (found) return idx;

  // basic information
  info.add("pT", obj.pt());
  info.add("eta", obj.eta());
  info.add("phi", obj.phi());
  info.add("mass", obj.mass());
  info.add("pdgId", obj.pdgId());
  auto filterId = (obj.filterIds().empty() ? 0 : obj.filterIds()[0]);
  if (obj.collection().rfind("hltL2Muon",0)==0) filterId = 80;
  info.add("filterId", getChar(filterId, "filterId"));

  // push data and return index
  info.pushData(obj);
  return idx;
}


void
ParticleAnalyzer::fillRecoParticleInfo(const edm::Event& iEvent)
{
  // fill reconstructed particle information
  const auto& particles = iEvent.getHandle(tok_recParticle_);
  if (particles.isValid())
  {
    // initialize reconstructed particle containers
    initParticleInfo("cand");
    initParticleInfo("trk");
    for (const auto& sid : sourceId_) { initParticleInfo("src", sid); }
    // loop over reconstructed particles
    for (const auto& cand : *particles) { fillRecoParticleInfo(cand); }
  }
}


UInt_t
ParticleAnalyzer::fillRecoParticleInfo(const pat::GenericParticle& cand, const UInt_t& momIdx)
{
  // fill reconstructed particle information
  auto& info = particleInfo_["cand"];

  // add input information
  size_t index;
  const bool found = info.getIndex(index, cand);
  info.push(index, "momIdx", momIdx, true);
  const bool& momMatchGEN = (isMC_ ? info.get(momIdx, "matchGEN", false) : false);
  if (momMatchGEN)
  {
    info.add(index, "momMatchGEN", true);
    info.add(index, "momMatchIdx", momIdx);
  }
  const auto& idx = getUInt(index, "cand_idx");
  // return if already added
  if (found) return idx;

  // basic information
  info.add("p", cand.p());
  info.add("y", cand.rapidity());
  info.add("pT", cand.pt());
  info.add("eta", cand.eta());
  info.add("phi", cand.phi());
  info.add("mass", cand.mass());
  info.add("charge", getChar(cand.charge(), "cand_charge"));
  info.add("pdgId", cand.pdgId());
  info.add("status", getUChar(cand.status(), "cand_status"));
  info.add("sourceId", getUChar(getInt(cand, "sourceId", Token::Unknown), "cand_sourceId"));

  // decay vertex information
  info.add("vtxChi2", getFloat(cand, "normChi2", -1.));
  info.add("vtxProb", getFloat(cand, "vertexProb", -1.));

  // decay angle information
  info.add("angle3D", getFloat(cand, "angle3D", -10.));
  info.add("angle2D", getFloat(cand, "angle2D", -10.));

  // DCA between two daughters, valid when nDaughters == 2
  info.add("dca", getFloat(cand, "dca", -99.9));

  // decay length information
  const auto& lVtxMag = getFloat(cand, "lVtxMag", -99.9);
  const auto& lVtxSig = getFloat(cand, "lVtxSig", -99.9);
  info.add("decayLength3D", lVtxMag);
  info.add("decayLengthError3D", (lVtxSig<-99 ? -99.9 : lVtxMag/lVtxSig));
  const auto& rVtxMag = getFloat(cand, "rVtxMag", -99.9);
  const auto& rVtxSig = getFloat(cand, "rVtxSig", -99.9);
  info.add("decayLength2D", rVtxMag);
  info.add("decayLengthError2D", (rVtxSig<-99 ? -99.9 : rVtxMag/rVtxSig));

  double sigmaPseudoLvtxMag = -99.9, sigmaPseudoRvtxMag = -99.9;
  if (cand.hasUserData("decayVertex"))
  {
    const auto& decayVertex = *cand.userData<reco::Vertex>("decayVertex");
    const auto& primaryVertex = *cand.userData<reco::VertexRef>("primaryVertex");
    const auto totalCov = decayVertex.covariance() + primaryVertex->covariance();
    typedef ROOT::Math::SVector<double, 3> SV3;
    sigmaPseudoLvtxMag = std::sqrt(ROOT::Math::Similarity(totalCov, SV3(cand.px(), cand.py(), cand.pz()))) / cand.p();
    sigmaPseudoRvtxMag = std::sqrt(ROOT::Math::Similarity(totalCov, SV3(cand.px(), cand.py(), 0        ))) / cand.pt();
  }
  info.add("pseudoDecayLengthError3D", sigmaPseudoLvtxMag);
  info.add("pseudoDecayLengthError2D", sigmaPseudoRvtxMag);

  // track multiplicity
  if (!nTracksVMap_.empty())
  {
    auto nTrk = (cand.hasUserData("primaryVertex") ? nTracksVMap_[*cand.userData<reco::VertexRef>("primaryVertex")] : 0);
    info.add("Ntrkoffline", getUShort(nTrk, "Ntrkoffline"));
  }

  // mva information
  if (addInfo_.at("mva")) info.add("mva", getFloat(cand, "mva"));

  // trigger information
  if (!triggerData_.empty())
  {
    std::vector<UShort_t> trigIdx;
    addTriggerObject(*const_cast<pat::GenericParticle*>(&cand));
    for (UShort_t i=0; i<triggerData_.size(); i++)
    {
      if (triggerData_[i].minN()==0) continue;
      pat::TriggerObjectStandAloneCollection triggerObjects;
      if (triggerData_[i].filterName()!="") triggerObjects = cand.triggerObjectMatchesByFilter(triggerData_[i].filterName());
      else triggerObjects = cand.triggerObjectMatchesByPath(triggerData_[i].triggerName());
      info.add(Form("matchTRG%d",i), !triggerObjects.empty());
      if (cand.status()==1 && addInfo_.at("trgObj"))
      {
        for (const auto& obj : triggerObjects)
        {
          trigIdx.emplace_back(fillTriggerObjectInfo(obj, i, true, idx));
        }
      }
    }
    if (addInfo_.at("trgObj")) info.add("trigIdx", trigIdx);
  }

  // track information
  if (addInfo_.at("track")) info.add("trkIdx", fillTrackInfo(cand, idx));

  // source information
  if (addInfo_.at("source")) info.add("srcIdx", fillSourceInfo(cand, idx));

  // generated particle information
  if (isMC_)
  {
    if (!cand.hasUserInt("isGenMatched")) { addGenParticle(*const_cast<pat::GenericParticle*>(&cand), cand.p4(), genParticlesToMatch_); }
    const auto& genPar = cand.genParticleRef();
    info.add("genIdx", fillGenParticleInfo(genPar, idx));
    info.add("matchGEN", genPar.isNonnull());
    info.add("isSwap", (cand.hasUserInt("isSwap") ? cand.userInt("isSwap") : false));
    info.add("genPdgId", (genPar.isNonnull() ? genPar->pdgId() : 0));
    info.add("momMatchGEN", momMatchGEN);
    info.add("momMatchIdx", momMatchGEN ? momIdx : UShort_t(-1));
  }

  // simulated particle information
  if (isMC_ && addInfo_.at("sim")) {
    const auto& simPar = cand.hasUserData("simRef") ? *cand.userData<TrackingParticleRef>("simRef") : TrackingParticleRef();
    info.add("simIdx", fillSimParticleInfo(simPar, idx));
    info.add("matchSIM", simPar.isNonnull());
    info.add("simPdgId", (simPar.isNonnull() ? simPar->pdgId() : 0));
  }

  // initialize daughter information
  info.add("dauIdx", std::vector<UInt_t>());
  info.add("pTDau", std::vector<float>());
  info.add("etaDau", std::vector<float>());
  info.add("phiDau", std::vector<float>());
  info.add("massDau", std::vector<float>());

  // push data
  info.pushData(cand);

  // add daughter information
  if (cand.hasUserData("daughters"))
  {
    const auto& dauColl = *cand.userData<pat::GenericParticleRefVector>("daughters");
    const auto& daughtersP4 = *cand.userData<std::vector<math::XYZTLorentzVector> >("daughtersP4");
    for (size_t iDau=0; iDau<dauColl.size(); iDau++)
    {
      const auto& dau = *dauColl[iDau];
      const auto& p4 = daughtersP4[iDau];
      info.push(idx, "dauIdx", fillRecoParticleInfo(dau, idx));
      info.push(idx, "pTDau", p4.Pt());
      info.push(idx, "etaDau", p4.Eta());
      info.push(idx, "phiDau", p4.Phi());
      info.push(idx, "massDau", p4.M());
    }
  }

  // return index
  return idx;
}


void
ParticleAnalyzer::addTriggerObject(pat::GenericParticle& cand)
{
  if (triggerData_.empty()) return;

  // check if already matched
  if (cand.hasUserInt("isTrigMatched") || !cand.triggerObjectMatches().empty()) return;

  // initialize information
  cand.addUserInt("isTrigMatched", 1);

  // loop over trigger data
  for (const auto& data : triggerData_)
  {
    addTriggerObject(cand, cand.p4(), data.filterObjects(), data.triggerName(), data.filterName(), data.minN());
  }
}


bool
ParticleAnalyzer::addTriggerObject(pat::GenericParticle& cand, const math::XYZTLorentzVector& p4, const TriggerIndexMap& filterObjects,
                                   const std::string& triggerName, const std::string& filterName, const int& minN)
{
  if (minN<=0 || (triggerName=="" && filterName=="")) return false;

  // check if cand has already been matched
  if (filterName!="" && cand.hasUserInt(filterName)) return !cand.triggerObjectMatchesByFilter(filterName).empty();
  if (cand.hasUserInt(triggerName)) return !cand.triggerObjectMatchesByPath(triggerName).empty();

  // initialize trigger objects
  pat::TriggerObjectStandAloneCollection triggerObjects;
  cand.addUserInt(filterName!="" ? filterName : triggerName, 1);

  // if already matched, copy them
  if (cand.hasUserData("src") && cand.userData<pat::Muon>("src"))
  {
    for (const auto& obj : cand.userData<pat::Muon>("src")->triggerObjectMatches())
    {
      const bool& isMatched = (filterName!="" ? obj.hasFilterLabel(filterName) : obj.hasPathName(triggerName));
      if (isMatched) triggerObjects.emplace_back(obj);
    }
  }

  // do trigger-reco matching
  if (triggerObjects.empty() && !filterObjects.empty() && filterName!="")
  {
    // case: final state particle (direct matching)
    if (cand.status()==1)
    {
      for (const auto& c : filterObjects)
      {
        if (!cand.hasUserInt(c.first))
        {
          cand.addUserInt(c.first, 1);
          const auto& m = matchData_.at(c.first);
          auto deltaR = m.maxDeltaR();
          pat::TriggerObjectStandAlone mObj;
          for (const auto& o : triggerObjectMap_.at(c.first))
          {
            // general case: match by deltaR and deltaPtRel, select lowest deltaR
            if (c.first.find("Stage2Digis:Muon")==std::string::npos && !o.second.id(-81))
            {
              if (!isMatched(p4, o.second.p4(), deltaR, m.maxDeltaPtRel())) continue;
              deltaR = reco::deltaR(p4.eta(), p4.phi(), o.second.eta(), o.second.phi());
              mObj = o.second;
            }
            // L1 muon case: match by deltaR, deltaEta and deltaPhi, select lowest deltaR
            else if (cand.hasUserFloat("l1Eta"))
            {
              math::PtEtaPhiMLorentzVector propP4(p4.pt(), cand.userFloat("l1Eta"), cand.userFloat("l1Phi")-(M_PI/144.), 0); // L1 phi offset: 1.25*pi/180
              if (!isL1MuMatched(propP4, o.second.p4(), deltaR, m.maxDeltaEta(), m.maxDeltaPhi())) continue;
              deltaR = reco::deltaR(propP4.eta(), propP4.phi(), o.second.eta(), o.second.phi());
              mObj = o.second;
            }
          }
          cand.addTriggerObjectMatch(mObj);
        }
        const auto& mObj = cand.triggerObjectMatchByCollection(c.first);
        if (mObj && mObj->hasFilterLabel(filterName)) triggerObjects.emplace_back(*mObj);
      }
    }
    // case: decayed particle (daughter matching)
    else if (cand.status()>1)
    {
      int n=0;
      const auto& dauColl = *cand.userData<pat::GenericParticleRefVector>("daughters");
      const auto& dauP4V = *cand.userData<std::vector<math::XYZTLorentzVector> >("daughtersP4");
      for (size_t i=0; i<dauColl.size(); i++)
      {
        auto& dau = *const_cast<pat::GenericParticle*>(dauColl[i].get());
        if (addTriggerObject(dau, dauP4V[i], filterObjects, triggerName, filterName, minN)) { n+=1; }
        if (n==minN) { triggerObjects.emplace_back(pat::TriggerObjectStandAlone()); break; }
      }
      if (n==minN) triggerObjects.back().addFilterLabel(filterName);
    }
  }
  // check if matches found
  if (triggerObjects.empty()) return false;

  // add trigger objects
  for (const auto& trigObj : triggerObjects)
  {
    cand.addTriggerObjectMatch(trigObj);
  }
  return true;
}


UShort_t
ParticleAnalyzer::fillTrackInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasTrack = cand.track().isNonnull();
  if (!force && !hasTrack) return USHRT_MAX;

  // fill track information
  auto& info = particleInfo_["trk"];

  const auto& track = (hasTrack ? *cand.track() : reco::Track());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, track);
  if (force || candIdx!=UINT_MAX)
    info.push(index, "candIdx", candIdx, true);
  const auto& idx = getUShort(index, "trk_idx");
  // return if already added
  if (found) return idx;

  // general information
  info.add("isHP", track.quality(reco::TrackBase::highPurity));
  info.add("nChi2", track.normalizedChi2());
  info.add("pTErr", track.ptError());
  info.add("nHit", track.numberOfValidHits());
  info.add("nHitPixel", track.hitPattern().numberOfValidPixelHits());
  info.add("nHitPixelBarrel", track.hitPattern().numberOfValidPixelBarrelHits());
  info.add("nHitPixelEndcap", track.hitPattern().numberOfValidPixelEndcapHits());
  info.add("nHitStrip", track.hitPattern().numberOfValidStripHits());
  info.add("nLayer", track.hitPattern().trackerLayersWithMeasurement());
  info.add("nLayerPixel", track.hitPattern().pixelLayersWithMeasurement());
  info.add("nLayerPixelBarrel", track.hitPattern().pixelBarrelLayersWithMeasurement());
  info.add("nLayerPixelEndcap", track.hitPattern().pixelEndcapLayersWithMeasurement());
  info.add("nLayerStrip", track.hitPattern().stripLayersWithMeasurement());
  info.add("algo", getUShort(track.algo(), "algo"));

  // dca information
  info.add("zDCASignificance", getFloat(cand, "dzSig"));
  info.add("xyDCASignificance", getFloat(cand, "dxySig"));

  // dEdx information
  if (addInfo_.at("dEdxs")) {
    for (const auto& tag : dedxInfo_) {
      const auto& input = tag.instance()!="" ? tag.instance() : tag.label();
      std::string dedxName = "dEdx_" + input;
      std::replace(dedxName.begin(), dedxName.end(), ':', '_');
      info.add(dedxName, getFloat(cand, "dEdx_"+input));
      std::string dedxErrName = "dEdxErr_" + input;
      std::replace(dedxErrName.begin(), dedxErrName.end(), ':', '_');
      info.add(dedxErrName, getFloat(cand, "dEdxErr_"+input));
    }
  }

  // track information
  std::vector<float> parV, covV;
  for (const auto& p : track.parameters()) parV.emplace_back(p);
  for (size_t i=0; i<5; i++) for (size_t j=0; j<=i; j++) covV.emplace_back(track.covariance(i,j));
  info.add("trackParameters", parV);
  info.add("trackCovariance", covV);

  // push data and return index
  info.pushData(track);
  return idx;
}


UShort_t
ParticleAnalyzer::fillSourceInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  // fill source information
  const auto& sid = getInt(cand, "sourceId", Token::Unknown);
  if      (sid==Token::ParticleFlow) { return fillPFCandidateInfo(cand, candIdx, force); }
  else if (sid==Token::Electron    ) { return fillElectronInfo(cand, candIdx, force);    }
  else if (sid==Token::Muon        ) { return fillMuonInfo(cand, candIdx, force);        }
  else if (sid==Token::Tau         ) { return fillTauInfo(cand, candIdx, force);         }
  else if (sid==Token::Photon      ) { return fillPhotonInfo(cand, candIdx, force);      }
  else if (sid==Token::Conversion  ) { return fillConversionInfo(cand, candIdx, force);  }
  else if (sid==Token::Jet         ) { return fillJetInfo(cand, candIdx, force);         }
  return USHRT_MAX;
}


UShort_t
ParticleAnalyzer::fillMuonInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<pat::Muon>("src"));
  if (!force && !hasSrc) return USHRT_MAX;

  // fill muon information
  auto& info = particleInfo_["muon"];

  const auto& muon = (hasSrc ? *cand.userData<pat::Muon>("src") : pat::Muon());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, muon);
  info.add(index, "candIdx", candIdx);
  const auto& idx = getUShort(index, "muon_idx");
  // return if already added
  if (found) return idx;

  const auto& vertex = (cand.hasUserData("primaryVertex") ? *cand.userData<reco::Vertex>("primaryVertex") : vertex_).position();
  const auto& gTrack = muon.globalTrack();
  const auto& iTrack = muon.innerTrack();
  const auto& mTrack = muon.muonBestTrack();

  // general information
  UInt_t selector = 0;
  for (size_t i=0; i<27; i++) { selector += (muon.passed(i) ? 1U<<i : 0); }
  info.add("selector", selector);

  UInt_t selectionType = 0;
  for (size_t i=0; i<30; i++) { selectionType += (muon::isGoodMuon(muon, muon::SelectionType(i)) ? 1U<<i : 0); }
  info.add("selectionType", selectionType);

  // tight ID muon POG Run 2
  info.add("isGlobal", muon.isGlobalMuon());
  info.add("isPF", muon.isPFMuon());
  info.add("glbTrkNChi2", (gTrack.isNonnull() ? gTrack->normalizedChi2() : 99.9));
  info.add("nMuonHit", getChar(gTrack.isNonnull() ? gTrack->hitPattern().numberOfValidMuonHits() : -1, "nMuonHit"));
  info.add("nMatchedStation", getUChar(muon.numberOfMatchedStations(), "nMatchedStation"));
  info.add("nPixelHit", getChar(iTrack.isNonnull() ? iTrack->hitPattern().numberOfValidPixelHits() : -1, "nPixelHit"));
  info.add("nTrackerLayer", getChar(iTrack.isNonnull() ? iTrack->hitPattern().trackerLayersWithMeasurement() : -1, "nTrackerLayer"));
  info.add("bestTrkdXY", (mTrack.isNonnull() ? mTrack->dxy(vertex) : -99.9));
  info.add("bestTrkdZ", (mTrack.isNonnull() ? mTrack->dz(vertex) : -99.9));
  const auto tightmuon = (
                          muon.isGlobalMuon() &&
                          muon.isPFMuon() &&
                          gTrack.isNonnull() &&
                          gTrack->normalizedChi2() < 10. &&
                          gTrack->hitPattern().numberOfValidMuonHits() > 0 &&
                          muon.numberOfMatchedStations() > 1 &&
                          iTrack.isNonnull() &&
                          iTrack->hitPattern().numberOfValidPixelHits() > 0 &&
                          iTrack->hitPattern().trackerLayersWithMeasurement() > 5 &&
                          mTrack.isNonnull() &&
                          std::abs(mTrack->dxy(vertex)) < 0.2 &&
                          std::abs(mTrack->dz(vertex)) < 0.5
                         );
  info.add("tightID", tightmuon);

  // soft ID muon POG Run >= 2
  info.add("isOneStTight", bool(selectionType & (1U<<muon::SelectionType::TMOneStationTight)));
  info.add("nPixelLayer", getChar(iTrack.isNonnull() ? iTrack->hitPattern().pixelLayersWithMeasurement() : -1, "nPixelLayer"));
  info.add("dXY", (iTrack.isNonnull() ? iTrack->dxy(vertex) : -99.9));
  info.add("dZ", (iTrack.isNonnull() ? iTrack->dz(vertex) : -99.9));
  const auto softmuon = (
                          bool(selectionType & (1U<<muon::SelectionType::TMOneStationTight)) &&
                          iTrack.isNonnull() &&
                          iTrack->hitPattern().trackerLayersWithMeasurement() > 5 &&
                          iTrack->hitPattern().pixelLayersWithMeasurement() > 0 &&
                          iTrack->quality(reco::TrackBase::highPurity) &&
                          std::abs(iTrack->dxy(vertex)) < 0.3 &&
                          std::abs(iTrack->dz(vertex)) < 20.
                        );
  info.add("softID", softmuon);

  // hybrid soft ID HIN PAG Run >= 2 PbPb
  info.add("isTracker", muon.isTrackerMuon());
  const auto hybridmuon = (
                           muon.isTrackerMuon() &&
                           muon.isGlobalMuon() &&
                           iTrack.isNonnull() &&
                           iTrack->hitPattern().trackerLayersWithMeasurement() > 5 &&
                           iTrack->hitPattern().pixelLayersWithMeasurement() > 0 &&
                           std::abs(iTrack->dxy(vertex)) < 0.3 &&
                           std::abs(iTrack->dz(vertex)) < 20.
                          );
  info.add("hybridSoftID", hybridmuon);

  // muon extra information
  info.add("nMatches", getUChar(muon.numberOfMatches(), "nMatches"));
  info.add("hadMax", muon.calEnergy().hadMax);

  float d_seg = 1.E9; 
  float dx_seg = -99.9, dy_seg = -99.9, dxSig_seg = -99.9, dySig_seg = -99.9;
  float ddxdz_seg = -99.9, ddydz_seg = -99.9, ddxdzSig_seg = -99.9, ddydzSig_seg = -99.9;
  for (const auto& exp : muon.matches())
  {
    for (const auto& seg : exp.segmentMatches)
    {
      const auto dseg = std::sqrt((seg.x-exp.x)*(seg.x-exp.x) + (seg.y-exp.y)*(seg.y-exp.y));
      if(dseg < d_seg)
      {
        d_seg = dseg;
        dx_seg = seg.x - exp.x;
        dy_seg = seg.y - exp.y;
        dxSig_seg = dx_seg / std::sqrt(seg.xErr*seg.xErr + exp.xErr*exp.xErr);
        dySig_seg = dy_seg / std::sqrt(seg.yErr*seg.yErr + exp.yErr*exp.yErr);
        ddxdz_seg = seg.dXdZ - exp.dXdZ;
        ddydz_seg = seg.dYdZ - exp.dYdZ;
        ddxdzSig_seg = ddxdz_seg / std::sqrt(seg.dXdZErr*seg.dXdZErr + exp.dXdZErr*exp.dXdZErr);
        ddydzSig_seg = ddydz_seg / std::sqrt(seg.dYdZErr*seg.dYdZErr + exp.dYdZErr*exp.dYdZErr);
      }
    }
  }
  info.add("dX_seg", dx_seg);
  info.add("dY_seg", dy_seg);
  info.add("dXSig_seg", dxSig_seg);
  info.add("dYSig_seg", dySig_seg);
  info.add("ddXdZ_seg", ddxdz_seg);
  info.add("ddYdZ_seg", ddydz_seg);
  info.add("ddXdZSig_seg", ddxdzSig_seg);
  info.add("ddYdZSig_seg", ddydzSig_seg);

  // muon L1 info
  if (!triggerData_.empty() && addInfo_.at("trgObj") && addInfo_.at("muonL1"))
  {
    info.add("l1Eta", (cand.hasUserFloat("l1Eta") ? cand.userFloat("l1Eta") : -99.9));
    info.add("l1Phi", (cand.hasUserFloat("l1Phi") ? cand.userFloat("l1Phi") : -99.9));
  }

  // push data and return index
  info.pushData(muon);
  return idx;
}


UShort_t
ParticleAnalyzer::fillElectronInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<pat::Electron>("src"));
  if (!force && !hasSrc) return USHRT_MAX;

  // fill electron information
  auto& info = particleInfo_["elec"];

  const auto& electron = (hasSrc ? *cand.userData<pat::Electron>("src") : pat::Electron());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, electron);
  info.add(index, "candIdx", candIdx);
  const auto& idx = getUShort(index, "elec_idx");
  // return if already added
  if (found) return idx;

  const auto& vertex = (cand.hasUserData("primaryVertex") ? *cand.userData<reco::Vertex>("primaryVertex") : vertex_);
  const auto& sCluster = (electron.core().isNonnull() ? electron.superCluster() : reco::SuperClusterRef());
  const auto& gsfTrack = (electron.core().isNonnull() ? electron.gsfTrack() : reco::GsfTrackRef());

  // electron information
  info.add("sigmaIEtaIEta", electron.full5x5_sigmaIetaIeta());
  info.add("sigmaIPhiIPhi", electron.full5x5_sigmaIphiIphi());
  info.add("dEtaSeedAtVtx", (sCluster.isNonnull() ? electron.deltaEtaSeedClusterTrackAtVtx() : 99.9));
  info.add("dEtaAtVtx", electron.deltaEtaSuperClusterTrackAtVtx());
  info.add("dPhiAtVtx", electron.deltaPhiSuperClusterTrackAtVtx());
  info.add("eOverPInv", (1./electron.ecalEnergy() - 1./electron.trackMomentumAtVtx().R()));
  info.add("hOverEBc", electron.hcalOverEcalBc());
  info.add("hOverE", electron.hcalOverEcal());

  // super cluster information
  info.add("sCEta", (sCluster.isNonnull() ? sCluster->eta() : -99.9));
  info.add("sCPhi", (sCluster.isNonnull() ? sCluster->phi() : -99.9));

  // gsf track information
  const auto ip3D = (gsfTrack.isNonnull() ? IPTools::absoluteImpactParameter3D(reco::TransientTrack(*gsfTrack, magField_), vertex).second.value() : 99.9);
  info.add("ip3D", ip3D);
  info.add("nMissTrackHits", getUChar(gsfTrack.isNonnull() ? gsfTrack->numberOfLostHits() : 254, "nMissTrackHits"));
  info.add("nMissInnerHits", getUChar(gsfTrack.isNonnull() ? gsfTrack->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS) : 254, "nMissInnerHits"));

  // UPC ID electron Run 2
  const bool upcID = (gsfTrack.isNonnull() &&
                      gsfTrack->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
                      electron.deltaEtaSuperClusterTrackAtVtx() < 0.1 &&
                      electron.hcalOverEcal() < 0.005);
  info.add("upcID", upcID);

  // push data and return index
  info.pushData(electron);
  return idx;
}


UShort_t
ParticleAnalyzer::fillPhotonInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<pat::Photon>("src"));
  if (!force && !hasSrc) return USHRT_MAX;

  // fill photon information
  auto& info = particleInfo_["pho"];

  const auto& photon = (hasSrc ? *cand.userData<pat::Photon>("src") : pat::Photon());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, photon);
  info.add(index, "candIdx", candIdx);
  const auto& idx = getUShort(index, "pho_idx");
  // return if already added
  if (found) return idx;
  
  const auto& phoIso   = cand.userData<reco::HIPhotonIsolation>("iso");
  const auto& sCluster = (photon.photonCore().isNonnull() ? photon.superCluster() : reco::SuperClusterRef());

  // photon information
  info.add("sigmaIEtaIEta", photon.full5x5_sigmaIetaIeta());
  info.add("hoverE", photon.hadronicOverEm());

  // super cluster information
  info.add("sCEta", (sCluster.isNonnull() ? sCluster->eta() : -99.9));
  info.add("sCPhi", (sCluster.isNonnull() ? sCluster->phi() : -99.9));

  // photon isolation information
  info.add("swissCrx", (phoIso ? phoIso->swissCrx() : -99.9));
  info.add("seedTime", (phoIso ? phoIso->seedTime() : -99.9));
  info.add("trackIsoR3PtCut20", (phoIso ? phoIso->trackIsoR3PtCut20() : 99.9));
  info.add("ecalClusterIsoR3", (phoIso ? phoIso->ecalClusterIsoR3() : 99.9));
  info.add("hcalRechitIsoR3", (phoIso ? phoIso->hcalRechitIsoR3() : 99.9));

  // push data and return index
  info.pushData(photon);
  return idx;
}


UShort_t
ParticleAnalyzer::fillConversionInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<reco::Conversion>("src"));
  if (!force && !hasSrc) return USHRT_MAX;

  // fill conversion information
  auto& info = particleInfo_["conv"];

  const auto& conversion = (hasSrc ? *cand.userData<reco::Conversion>("src") : reco::Conversion());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, cand);
  info.add(index, "candIdx", candIdx);
  const auto& idx = getUShort(index, "conv_idx");
  // return if already added
  if (found) return idx;

  // converted photon information
  info.add("nTracks", conversion.nTracks());

  UInt_t quality = 0;
  for (size_t i=0; i<12; i++) { quality += (conversion.quality(reco::Conversion::ConversionQuality(i)) ? 1U<<i : 0); }
  info.add("quality", quality);

  // push data and return index
  info.pushData(cand);
  return idx;
}


UShort_t
ParticleAnalyzer::fillJetInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<pat::Jet>("src"));
  if (!force && !hasSrc) return USHRT_MAX;

  // fill jet information
  auto& info = particleInfo_["jet"];

  const auto& jet = (hasSrc ? *cand.userData<pat::Jet>("src") : pat::Jet());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, jet);
  info.push(index, "candIdx", candIdx, true);
  const auto& idx = getUShort(index, "jet_idx");
  // return if already added
  if (found) return idx;

  const bool hasJEC = jet.jecSetsAvailable();
  const auto& rawP4 = (hasJEC ? jet.correctedJet("Uncorrected") : pat::Jet());
  const auto& l2P4 = (hasJEC ? jet.correctedJet("L2Relative") : pat::Jet());
  const auto& l3P4 = (hasJEC ? jet.correctedJet("L3Absolute") : pat::Jet());

  // jet information
  info.add("rawPt", rawP4.pt());
  info.add("l2", l2P4.pt()/rawP4.pt());
  info.add("l3", l3P4.pt()/l2P4.pt());
  info.add("area", jet.jetArea());
  info.add("pileup", jet.pileup());
  info.add("hadronFlavour", getChar(jet.hadronFlavour(), "hadronFlavour"));
  info.add("partonFlavour", getShort(jet.partonFlavour(), "partonFlavour"));

  // PF jet information
  const auto& isPF = jet.isPFJet();
  info.add("isPF", isPF);
  info.add("CHF", (isPF ? jet.chargedHadronEnergyFraction() : -1.));
  info.add("NHF", (isPF ? jet.neutralHadronEnergyFraction() : -1.));
  info.add("CEF", (isPF ? jet.chargedEmEnergyFraction() : -1.));
  info.add("NEF", (isPF ? jet.neutralEmEnergyFraction() : -1.));
  info.add("MUF", (isPF ? jet.muonEnergyFraction() : -1.));
  info.add("CHM", getUShort(isPF ? jet.chargedHadronMultiplicity() : 0, "CHM"));
  info.add("NHM", getUShort(isPF ? jet.neutralHadronMultiplicity() : 0, "NHM"));
  info.add("CEM", getUShort(isPF ? jet.electronMultiplicity() : 0, "CEM"));
  info.add("NEM", getUShort(isPF ? jet.photonMultiplicity() : 0, "NEM"));
  info.add("MUM", getUShort(isPF ? jet.muonMultiplicity() : 0, "MUM"));

  // b-tagging information
  UInt_t bTag = 0;
  const auto& v = jet.getPairDiscri();
  const auto n = std::min(size_t(32), v.size());
  for (size_t i=0; i<n; i++) { bTag += (std::next(v.begin(), i)->second ? 1U<<i : 0); }
  info.add("bTagging", bTag);

  // push data and return index
  info.pushData(jet);
  return idx;
}


UShort_t
ParticleAnalyzer::fillTauInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<pat::Tau>("src"));
  if (!force && !hasSrc) return USHRT_MAX; 

  // fill tau information
  auto& info = particleInfo_["tau"];

  const auto& tau = (hasSrc ? *cand.userData<pat::Tau>("src") : pat::Tau());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, tau);
  info.add(index, "candIdx", candIdx);
  const auto& idx = getUShort(index, "tau_idx");
  // return if already added
  if (found) return idx;

  // tau ID information
  UInt_t selector = 0;
  const auto& v = tau.tauIDs();
  const size_t n = std::min(size_t(32), v.size());
  for (size_t i=0; i<n; i++) { selector += (std::next(v.begin(), i)->second>0.5 ? 1U<<i : 0); }
  info.add("selector", selector);
  info.add("isPF", tau.isPFTau());

  // push data and return index
  info.pushData(tau);
  return idx;
}


UShort_t
ParticleAnalyzer::fillPFCandidateInfo(const pat::GenericParticle& cand, const UInt_t& candIdx, const bool& force)
{
  const bool hasSrc = (cand.hasUserData("src") && cand.userData<reco::PFCandidate>("src"));
  if (!force && !hasSrc) return USHRT_MAX;

  // fill jet information
  auto& info = particleInfo_["pf"];

  const auto& pfCand = (hasSrc ? *cand.userData<reco::PFCandidate>("src") : reco::PFCandidate());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, pfCand);
  if (force || candIdx!=UINT_MAX)
    info.push(index, "candIdx", candIdx, true);
  const auto& idx = getUShort(index, "pf_idx");
  // return if already added
  if (found) return idx;

  // general information
  info.add("id", getUChar(pfCand.particleId(), "id"));

  // calo information
  info.add("ecalE", pfCand.ecalEnergy());
  info.add("rawEcalE", pfCand.rawEcalEnergy());
  info.add("hcalE", pfCand.hcalEnergy());
  info.add("rawHcalE", pfCand.rawHcalEnergy());

  // push data and return index
  info.pushData(pfCand);
  return idx;
}


void
ParticleAnalyzer::fillGenParticleInfo(const edm::Event& iEvent)
{
  if (!isMC_) return;

  // fill generated particle information
  if (!genParticlesToKeep_.empty())
  {
    for (const auto& cand : genParticlesToKeep_) { fillGenParticleInfo(cand); }
  }

  // fill generator weight
  const auto& genInfo = iEvent.getHandle(tok_genInfo_);
  if (genInfo.isValid())
  {
    eventInfo_.add("genWeight", genInfo->weight());
    if (genInfo->hasBinningValues())
    {
      eventInfo_.add("pTHat",  genInfo->binningValues()[0]);
    }
  }

  // fill LHE weights
  const auto& lheInfo = iEvent.getHandle(tok_lheInfo_);
  if (lheInfo.isValid() && genInfo.isValid())
  {
    std::vector<float> weightLHE;
    weightLHE.reserve(lheInfo->weights().size());
    const auto& asdd = lheInfo->originalXWGTUP();
    for (const auto& w : lheInfo->weights())
    {
      const auto& asdde = w.wgt;
      weightLHE.emplace_back(genInfo->weight()*asdde/asdd);
    }
    eventInfo_.add("lheWeight", weightLHE);
  }
}


UShort_t
ParticleAnalyzer::fillGenParticleInfo(const reco::GenParticleRef& candR, const UInt_t& candIdx, const bool& force)
{
  const bool hasGen = candR.isNonnull() && contain(genParticlesToKeep_, candR);
  if (!force && !hasGen) return USHRT_MAX;

  // fill generated particle information
  auto& info = particleInfo_["gen"];

  const auto& cand = (hasGen ? *candR : reco::GenParticle());

  // add input information
  size_t index;
  const bool found = info.getIndex(index, cand);
  if (force || candIdx!=UINT_MAX)
    info.push(index, "candIdx", candIdx, true);
  const auto& idx = getUShort(index, "gen_idx");
  // return if already added
  if (found) return idx;

  // basic information
  info.add("p", cand.p());
  info.add("y", cand.rapidity());
  info.add("pT", cand.pt());
  info.add("eta", cand.eta());
  info.add("phi", cand.phi());
  info.add("mass", cand.mass());
  info.add("charge", getChar(cand.charge(), "gen_charge"));
  info.add("pdgId", cand.pdgId());
  info.add("status", getUChar(cand.status(), "gen_status"));

  UShort_t statusBit = 0;
  const auto& flags = cand.statusFlags().flags_;
  for (size_t i=0; i<flags.size(); i++) { i += (flags[i] ? 1U<<i : 0); }
  info.add("statusBit", statusBit);

  // decay vertex information
  float dl3D = -99.9, angle3D = -10., dl2D = -99.9, angle2D = -10.;
  if (cand.numberOfDaughters()>0 && cand.daughter(0))
  {
    const auto lineOfFlight = cand.daughter(0)->vertex() - genVertex_;
    dl3D = lineOfFlight.r();
    dl2D = lineOfFlight.rho();
    angle3D = (dl3D>0. ? angle(lineOfFlight.x(), lineOfFlight.y(), lineOfFlight.z(), cand.px(), cand.py(), cand.pz()) : -10.);
    angle2D = (dl2D>0. ? angle(lineOfFlight.x(), lineOfFlight.y(), 0.0, cand.px(), cand.py(), 0.0) : -10.);
  }
  info.add("angle3D", angle3D);
  info.add("angle2D", angle2D);
  info.add("decayLength3D", dl3D);
  info.add("decayLength2D", dl2D);

  // gen track information
  std::vector<float> parV;
  if (hasGen && cand.charge()!=0 && std::abs(cand.eta())<5)
  {
    const auto& sPar = reco::trackingParametersAtClosestApproachToBeamSpot(Basic3DVector<double>(cand.vertex()), Basic3DVector<double>(cand.momentum()), cand.charge(), *magField_, beamSpot_).second;
    for (const auto& p : sPar) parV.emplace_back(p);
  }
  info.add("trackParameters", parV);

  // initialize daughter and mother information
  info.add("dauIdx", std::vector<UShort_t>());
  info.add("momIdx", std::vector<UShort_t>());

  // push data
  info.pushData(cand);

  // add daughter information
  if (cand.numberOfDaughters()>0)
  {
    for (size_t iDau=0; iDau<cand.numberOfDaughters(); iDau++)
    {
      const auto& dau = findGenDaughter(candR, iDau);
      const auto& dauIdx = fillGenParticleInfo(dau);
      if (dauIdx!=USHRT_MAX) { info.push(idx, "dauIdx", dauIdx); }
    }
  }

  // mother information
  if (cand.numberOfMothers()>0)
  {
    const auto& mom = findGenMother(candR);
    const auto& momIdx = fillGenParticleInfo(mom);
    if (momIdx!=USHRT_MAX) { info.push(idx, "momIdx", momIdx); }
  }

  // return index
  return idx;
}


bool
ParticleAnalyzer::addGenParticle(pat::GenericParticle& cand, const math::XYZTLorentzVector& p4, const reco::GenParticleRefVector& genColl)
{
  if (genColl.empty() || cand.status()==0) return false;

  // check that it is in input collection if already matched
  if (cand.hasUserInt("isGenMatched")) return cand.genParticle() && std::find(genColl.begin(), genColl.end(), cand.genParticleRef())!=genColl.end();

  // initialize information
  bool isSwap = false;
  reco::GenParticleRef genCand;
  cand.addUserInt("isGenMatched", 1);

  // do gen-reco matching
  double dPtRel = maxGenDeltaPtRel_;
  for (const auto& genPar : genColl)
  {
    // check if compatible
    if (!isCompatible(*genPar, cand)) continue;
    // case: final state particle (direct matching)
    if (cand.status()==1)
    {
      if (!isMatched(genPar->p4(), p4, maxGenDeltaR_, dPtRel)) continue;
      dPtRel = deltaPt(genPar->pt(), p4.pt());
      genCand = genPar;
      isSwap = (std::abs(genCand->mass()-p4.mass()) > 0.01);
    }
    // case: decayed particle (daughter matching)
    else if (cand.status()>1)
    {
      if (genPar->numberOfDaughters()==0) continue;
      // loop over daughters
      genCand = genPar;
      const auto& dauColl = *cand.userData<pat::GenericParticleRefVector>("daughters");
      const auto& dauP4V = *cand.userData<std::vector<math::XYZTLorentzVector> >("daughtersP4");
      for (size_t i=0; i<dauColl.size(); i++)
      {
        auto& dau = *const_cast<pat::GenericParticle*>(dauColl[i].get());
        reco::GenParticleRefVector genDauColl;
        findGenDaughters(genDauColl, genPar, dau, maxGenIter_);
        if (!addGenParticle(dau, dauP4V[i], genDauColl)) { genCand = reco::GenParticleRef(); break; }
        if (dau.userInt("isSwap")) { isSwap = true; }
      }
      if (genCand.isNonnull()) break;
    }
  }
  if (genCand.isNull()) return false;

  // add generated particle information
  cand.addGenParticleRef(genCand);
  cand.addUserInt("isSwap", isSwap);
  return true;
}


void
ParticleAnalyzer::findGenDaughters(reco::GenParticleRefVector& genColl, const reco::GenParticleRef& genPar, const pat::GenericParticle& cand, const short& iter)
{
  if (iter<0 || genPar.isNull()) return;
  for (size_t i=0; i<genPar->numberOfDaughters(); i++)
  {
    const auto& dau = findGenDaughter(genPar, i);
    if (isCompatible(*dau, cand)) { genColl.push_back(dau); }
    findGenDaughters(genColl, dau, cand, iter-1);
  }
}


reco::GenParticleRef
ParticleAnalyzer::findGenDaughter(const reco::GenParticleRef& par, const size_t& idx)
{
  if (par.isNull() || par->numberOfDaughters()<=idx) return reco::GenParticleRef();
  auto dau = par->daughterRef(idx);
  const auto pdgId = dau->pdgId();
  while (dau->numberOfDaughters()>0 && dau->daughterRef(0)->pdgId()==pdgId)
  {
    dau = dau->daughterRef(0);
  }
  return dau;
}


reco::GenParticleRef
ParticleAnalyzer::findGenMother(const reco::GenParticleRef& par)
{
  if (par.isNull() || par->numberOfMothers()==0) return reco::GenParticleRef();
  auto mom = par->motherRef(0);
  const auto pdgId = par->pdgId();
  while (mom->numberOfMothers()>0 && mom->pdgId()==pdgId)
  {
    mom = mom->motherRef(0);
  }
  if (mom->pdgId()==pdgId) return reco::GenParticleRef();
  return mom;
}


void
ParticleAnalyzer::fillSimParticleInfo(const edm::Event& iEvent)
{
  if (!isMC_ || !addInfo_.at("sim")) return;

  // fill simulated particle information
  const auto& simParticles = iEvent.getHandle(tok_simParticle_);
  if (simParticles.isValid())
  {
    for (size_t i=0; i<simParticles->size(); i++)
    {
      const auto& simParticle = TrackingParticleRef(simParticles, i);
      if (simParticle->decayVertices().empty())
        fillSimParticleInfo(simParticle);
    }
  }
}


UShort_t
ParticleAnalyzer::fillSimParticleInfo(const TrackingParticleRef& candR, const UInt_t& candIdx, const bool& force)
{
  const bool hasSim = candR.isNonnull();
  if (!force && !hasSim) return USHRT_MAX;

  // fill simulated particle information
  auto& info = particleInfo_["sim"];

  const auto& cand = (hasSim ? *candR : TrackingParticle({}, {}));

  // add input information
  size_t index;
  const bool found = info.getIndex(index, cand);
  if (force || candIdx!=UINT_MAX)
    info.push(index, "candIdx", candIdx, true);
  const auto& idx = getUShort(index, "sim_idx");
  // return if already added
  if (found) return idx;

  // basic information
  info.add("pT", cand.pt());
  info.add("eta", cand.eta());
  info.add("phi", cand.phi());
  info.add("mass", cand.mass());
  info.add("charge", getChar(cand.charge(), "gen_charge"));
  info.add("pdgId", cand.pdgId());
  info.add("status", getUChar(std::abs(cand.status()), "gen_status"));

  // push data
  info.pushData(cand);

  // return index
  return idx;
}


void
ParticleAnalyzer::initTree()
{
  if (tree_) return;
  tree_ = fileService_->make<TTree>("ParticleTree","ParticleTree");

  // add event branches
  eventInfo_.initTree(*tree_);

  // add particle branches
  for (const auto& p : std::vector<std::string>({"cand", "trk", "elec", "muon", "tau", "pho", "conv", "jet", "pf", "gen", "sim", "trig"}))
  {
    if (particleInfo_.find(p)!=particleInfo_.end())
    {
      particleInfo_.at(p).initTree(*tree_, (p+"_"));
    }
  }
}


void
ParticleAnalyzer::initNTuple()
{
  if (ntuple_) return;
  ntuple_ = fileService_->make<TTree>("ParticleNTuple","ParticleNTuple");
  // add branches
  ntupleInfo_.initTree(*ntuple_);
}


void
ParticleAnalyzer::addParticleToNtuple(const size_t& i, const std::pair<int, int>& dau)
{
  const auto& cand = particleInfo_["cand"];
  const auto sid = cand.get(i, "sourceId", UChar_t(0));
  // define label
  std::string label;
  if (dau.first>0)
  {
    for (int j=0; j<dau.first-1; j++) { label += "g"; }
    label += Form("dau%d_", dau.second);
  }
  // add particle information
  for (const auto& p : particleInfo_)
  {
    auto idx = UShort_t(-1);
    if      (p.first=="cand") { p.second.copyData(ntupleInfo_, UInt_t(i), "cand_"+label); }
    else if (p.first=="trig") { idx = cand.get(i, "trigIdx", idx); }
    else if (p.first=="gen" ) { idx = cand.get(i, "genIdx",  idx); }
    else if (p.first=="sim" ) { idx = cand.get(i, "simIdx",  idx); }
    else if (p.first=="trk" ) { idx = cand.get(i, "trkIdx",  idx); }
    else if (p.first=="jet"  && sid==Token::Jet       ) { idx = cand.get(i, "srcIdx",  idx); }
    else if (p.first=="elec" && sid==Token::Electron  ) { idx = cand.get(i, "srcIdx",  idx); }
    else if (p.first=="muon" && sid==Token::Muon      ) { idx = cand.get(i, "srcIdx",  idx); }
    else if (p.first=="tau"  && sid==Token::Tau       ) { idx = cand.get(i, "srcIdx",  idx); }
    else if (p.first=="pho"  && sid==Token::Photon    ) { idx = cand.get(i, "srcIdx",  idx); }
    else if (p.first=="conv" && sid==Token::Conversion) { idx = cand.get(i, "srcIdx",  idx); }
    if (p.first=="trig" || p.first=="gen" || p.first=="sim" || p.first=="trk" || idx<USHRT_MAX)
    {
      p.second.copyData(ntupleInfo_, idx, p.first+"_"+label);
    }
  }
  // remove meaningless variables
  if (!ntuple_)
  {
    // detectable particles such as pions do not have decay info
    if (cand.get(i, "status", UChar_t(1))==1)
    {
      ntupleInfo_.erase(label+"decayLength", {"float", "floatV"});
      ntupleInfo_.erase(label+"pseudoDecayLength", {"float", "floatV"});
      ntupleInfo_.erase(label+"angle", {"float", "floatV"});
      ntupleInfo_.erase(label+"dca", {"float", "floatV"});
      ntupleInfo_.erase(label+"vtx", {"float", "floatV"});
    }
    // intermediate particles such as Ks mesons do not have track info
    else
    {
      ntupleInfo_.erase("trk_"+label, {"float", "floatV", "bool", "boolV", "ushort", "ushortV"});
    }
  }
  // add daughter information
  const auto& dauIdx = cand.get(i, "dauIdx", std::vector<UInt_t>());
  for (size_t iDau=0; iDau<dauIdx.size(); iDau++)
  {
    addParticleToNtuple(dauIdx[iDau], {dau.first+1, iDau});
  }
}


void
ParticleAnalyzer::fillNTuple(const bool& fill)
{
  // loop over candidates
  const auto& cand = particleInfo_["cand"];
  for (size_t i=0; i<cand.size(); i++)
  {
    const auto status = cand.get(i, "status", UChar_t(0));
    if (status!=3) continue;
    // clear
    ntupleInfo_.clear();
    // copy event information
    eventInfo_.copyData(ntupleInfo_);
    // copy particle information
    addParticleToNtuple(i, {0, 0});
    // initialize ntuple
    if (!ntuple_)
    {
      ntupleInfo_.erase("Idx", {"ushort"});
      ntupleInfo_.erase("cand_matchTRG", {"bool"});
      ntupleInfo_.erase("cand_momMatch", {"bool"});
      ntupleInfo_.erase("lheWeight", {"floatV"});
      initNTuple();
    }
    // fill ntuple
    if (fill) ntuple_->Fill();
  }
}


void
ParticleAnalyzer::check()
{
  if (particleInfo_.find("cand")==particleInfo_.end())
    return;
  const auto& cand_size = particleInfo_.at("cand").size();
  for (const auto& p : particleInfo_) {
    p.second.check(p.first);
    if (p.first!="cand") {
      const std::string n = ((p.first=="gen" || p.first=="sim" || p.first=="trk") ? p.first+"Idx" : "srcIdx");
      const auto& s = p.second.size();
      if (p.second.hasUIntV("candIdx"))
        for (size_t i=0; i<s; i++)
          for (const auto& v : p.second.get(i, "candIdx", std::vector<UInt_t>({UINT_MAX})))
            if (v>=cand_size) throw std::logic_error(Form("[ERROR] Invalid cand index in %s (%ld, %ld): %d >= %ld", p.first.c_str(), i, s, v, cand_size));
      if (particleInfo_.at("cand").hasUShortV(n))
        for (size_t i=0; i<cand_size; i++)
          for (const auto& v : particleInfo_.at("cand").get(i, n, std::vector<UShort_t>({54321})))
            if (v!=USHRT_MAX && v>=s) throw std::logic_error(Form("[ERROR] Invalid %s index in cand (%ld, %ld): %d >= %ld", p.first.c_str(), i, cand_size, v, s));
    }
    else {
      for (const auto& c : {"dauIdx", "momIdx"})
        if (p.second.hasUIntV(c))
          for (size_t i=0; i<cand_size; i++)
            for (const auto& v : p.second.get(i, c, std::vector<UInt_t>({987654321})))
              if (v!=UINT_MAX && v>=cand_size) throw std::logic_error(Form("[ERROR] Invalid %s in %s (%ld): %d >= %ld", c, p.first.c_str(), i, v, cand_size));
    }
  }
}


edm::ParameterSet
ParticleAnalyzer::getConfiguration(const std::string& module, const std::string& process, const edm::Run& iRun)
{
  for (size_t i=iRun.processHistory().size(); i>0 && module!=""; i--)
  {
    const auto& c = iRun.processHistory().at(i-1);
    const auto& p = *edm::pset::Registry::instance()->getMapped(c.parameterSetID());
    const auto& ps = p.retrieveUnknownParameterSet(module);
    if (ps && (process=="" || process==c.processName())) { return ps->pset(); } 
  }
  return edm::ParameterSet();
}


edm::ParameterSet
ParticleAnalyzer::getConfiguration(const edm::EDGetToken& token, const edm::Run& iRun)
{
  EDConsumerBase::Labels label;
  EDConsumerBase::labelsForToken(token, label);
  return getConfiguration(label.module, label.process, iRun);
}


void
ParticleAnalyzer::loadConfiguration(const edm::ParameterSet& config, const edm::Run& iRun)
{
  if (config.empty()) return;

  if (config.existsAs<bool>("vtxSortByTrkSize"))
  {
    vtxSortByTrkSize_ = config.getParameter<bool>("vtxSortByTrkSize");
  } else {
    vtxSortByTrkSize_ = true;
  }

  UInt_t pdgId(0);
  if (config.existsAs<UInt_t>("pdgId"))
  {
    pdgId = config.getParameter<UInt_t>("pdgId");
    if (genPdgId_.empty()) { std::copy(genPdgIdV_.begin(), genPdgIdV_.end(), std::inserter(genPdgId_, genPdgId_.end())); }
    genPdgId_.insert(pdgId);
  }

  Token sid(Token::Unknown);
  if (!config.existsAs<std::vector<edm::ParameterSet> >("daughterInfo") || config.getParameter<std::vector<edm::ParameterSet> >("daughterInfo").empty())
  {
    if (config.existsAs<UInt_t>("sourceId"))
    {
      sid = Token(config.getParameter<UInt_t>("sourceId"));
    }
    if (sid==Token::Unknown) {
      getSourceId(sid, pdgId, config, config);
    }
  }
  if (sid>=Token::ParticleFlow && sid<=Token::Jet) {
    sourceId_.insert(sid);
  }

  if (!addInfo_["source"])
  {
    addInfo_["source"] = !sourceId_.empty();
  }

  if (!addInfo_["track"])
  {
    addInfo_["track"] = (sid>=Token::Track && sid<=Token::Muon);
  }

  if (!addInfo_["dEdxs"] && config.existsAs<std::vector<edm::InputTag>>("dEdxInputs"))
  {
    dedxInfo_ = config.getParameter<std::vector<edm::InputTag> >("dEdxInputs");
    addInfo_["dEdxs"] = dedxInfo_.size();
  }

  if (!addInfo_["mva"] && config.existsAs<edm::InputTag>("mva"))
  {
    addInfo_["mva"] = config.getParameter<edm::InputTag>("mva").label()!="";
  }

  if (!addInfo_["muonL1"] && sid==Token::Muon)
  {
    addInfo_["muonL1"] = (!config.existsAs<bool>("propToMuon") || config.getParameter<bool>("propToMuon"));
  }

  if (!addInfo_["sim"] && addInfo_.at("track") && config.existsAs<edm::InputTag>("recoToSimTrackMap"))
  {
    addInfo_["sim"] = config.getParameter<edm::InputTag>("recoToSimTrackMap").label()!="";
  }

  if (config.existsAs<edm::InputTag>("source"))
  {
    const auto& source = config.getParameter<edm::InputTag>("source");
    const auto& pSet = getConfiguration(source.label(), source.process(), iRun);
    loadConfiguration(pSet, iRun);
  }

  if (config.existsAs<std::vector<edm::ParameterSet> >("daughterInfo"))
  {
    const auto& daughterVPset = config.getParameter<std::vector<edm::ParameterSet> >("daughterInfo");
    for (const auto& pSet : daughterVPset)
    {
      loadConfiguration(pSet, iRun);
    }
  }
}


// ------------ method called once each job just before starting event
//loop  ------------
void
ParticleAnalyzer::beginJob()
{
}


//--------------------------------------------------------------------------------------------------
void 
ParticleAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
  bool changed = true;
  EDConsumerBase::Labels triggerResultsLabel;
  EDConsumerBase::labelsForToken(tok_triggerResults_, triggerResultsLabel);
  if (!hltPrescaleProvider_.init(iRun, iSetup, triggerResultsLabel.process, changed)) { throw(cms::Exception("Trigger")<<"HLT provider failed init"); }
  l1PrescaleTable_.clear();
  const auto& l1Data = iSetup.tryToGet<L1TGlobalPrescalesVetosFractRcd>();
  if (l1Data)
  {
    const auto& l1GtPrescalesVetoes = l1Data->getHandle(l1Pres_esToken_);
    if (l1GtPrescalesVetoes.isValid())
    {
      l1PrescaleTable_ = l1GtPrescalesVetoes->prescale_table_;
    }
  }
  // extract paths
  std::map<std::string, size_t> hltPaths, datasetPaths;
  const auto& paths = hltPrescaleProvider_.hltConfigProvider().triggerNames();
  for (size_t idx=0; idx<paths.size(); idx++)
  {
    const auto& path = paths[idx];
    if (!triggerInfo_.empty() && path.rfind("HLT_",0)==0) { hltPaths[path] = idx; }
    else if (dataset_!="" && path.rfind("Dataset_",0)==0) { datasetPaths[path] = idx; }
  }
  // extract the trigger index
  for (size_t iTrg=0; iTrg<triggerInfo_.size(); iTrg++)
  {
    const auto& pSet = triggerInfo_[iTrg];
    const auto& pathLabel = (pSet.existsAs<std::string>("path") ? pSet.getParameter<std::string>("path") : std::string());
    if (pathLabel!="")
    {
      hltPathIdx_[pathLabel].clear();
      const TRegexp reg(pathLabel.c_str(), pathLabel.rfind("*")!=std::string::npos);
      for (const auto& h : hltPaths)
      {
        if (TString(h.first).Contains(reg)) { hltPathIdx_[pathLabel].emplace_back(h.second); }
      }
    }
  }
  // extract the dataset index
  if (dataset_!="")
  {
    datasetPathIdx_.clear();
    const TRegexp reg(dataset_.c_str(), dataset_.rfind("*")!=std::string::npos);
    for (const auto& d : datasetPaths)
    {
      if (TString(d.first).Contains(reg)) { datasetPathIdx_.emplace_back(d.second); }
    }
  }
  // load configuration
  if (sourceId_.empty())
  {
    const auto& config = getConfiguration(tok_recParticle_, iRun);
    loadConfiguration(config, iRun);
  }
}


// ------------ method called once each job just after ending the event
//loop  ------------
void 
ParticleAnalyzer::endJob()
{
}


//define this as a plug-in
DEFINE_FWK_MODULE(ParticleAnalyzer);
