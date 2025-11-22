import FWCore.ParameterSet.Config as cms

particleAna = cms.EDAnalyzer('ParticleAnalyzer',

  # reconstructed information
  beamSpot = cms.InputTag("offlineBeamSpot"),
  primaryVertices = cms.InputTag("offlinePrimaryVertices"),
  recoParticles = cms.InputTag("generalParticles"),
  nTracksVMap = cms.untracked.InputTag("generalParticles:nTracks"),

  # trigger information
  stageL1Trigger = cms.uint32(2),
  triggerResults = cms.untracked.InputTag("TriggerResults::HLT"),
  triggerEvent   = cms.untracked.InputTag("hltTriggerSummaryAOD::HLT"),
  triggerInfo = cms.untracked.VPSet([
      #cms.PSet(path = cms.string(''), filter = cms.string(''), minN = cms.int32(), isL1OR = cms.bool(), lumiInfo = cms.InputTag(''))
  ]),

  # trigger-reco matching information
  # default values:
  # L1 muons:  deltaR < 0.3, deltaEta < 0.2, deltaPhi < 6.0
  # L2 muons:  deltaR < 0.3, deltaPtRel < 10.0
  # L3 muons:  deltaR < 0.1, deltaPtRel < 10.0
  # any other: deltaR < 0.3, deltaPtRel < 10.0
  matchInfo = cms.untracked.VPSet([
      #cms.PSet(collection = cms.string(''), maxDeltaR = cms.double(), maxDeltaPtRes = cms.double(), maxDeltaEta = cms.double(), maxDeltaPhi = cms.double()),
  ]),

  #Filter info
  eventFilterResults = cms.untracked.InputTag("TriggerResults"),
  eventFilterNames = cms.untracked.vstring(
      'Flag_colEvtSel',
      'Flag_hfCoincFilter2Th4',
      'Flag_primaryVertexFilter',
      'Flag_clusterCompatibilityFilter',
  ),
  selectEvents = cms.string("eventFilter_HM_step"),

  # centrality and event plane information
  centralityBin = cms.untracked.InputTag("centralityBin","HFtowers"),
  centrality    = cms.untracked.InputTag("hiCentrality"),
  eventPlane    = cms.untracked.InputTag("hiEvtPlaneFlat"),
  recoTracks    = cms.untracked.InputTag("generalTracks"),
  pfCandidates  = cms.untracked.InputTag("particleFlow"),

  # luminosity information
  lumiInfo    = cms.untracked.InputTag("lumiInfo", "brilcalc"),
  lumiScalers = cms.untracked.InputTag("scalersRawToDigi"),
  lumiRecord  = cms.untracked.InputTag("onlineMetaDataDigis"),

  # options
  saveTree  = cms.untracked.bool(True),
  addTrgObj = cms.untracked.bool(False),
)

particleAna_mc = particleAna.clone(
  selectEvents = cms.string(""),
  # generated information
  genParticles = cms.untracked.InputTag("genParticles"),
  genInfo      = cms.untracked.InputTag("generator"),
  genPdgId     = cms.untracked.vuint32(),
)
