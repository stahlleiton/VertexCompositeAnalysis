import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
process = cms.Process('ANASKIM', eras.Run3_2025_UPC)

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')

# Limit the output messages
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 200
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.options.numberOfThreads=cms.untracked.uint32(1)

# Define the input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://xrootd-cms.infn.it///store/hidata/HIRun2025A/HIForward0/MINIAOD/PromptReco-v1/000/400/414/00000/cbe12437-6f44-4f77-840e-c070b74bd39e.root"),
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Set the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('151X_dataRun3_Prompt_v1')


## ##############################################################################################################################
## Variables Production #########################################################################################################

#* Set ZDC information
process.load("VertexCompositeAnalysis.VertexCompositeProducer.ZDCRun3_cfg")
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.cent_seq = cms.Sequence(process.centralityBin * process.zdcreco)

#* Add the Particle producer
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles

# DiKa selection
kaonBandPl = "(log(userFloat(\"dEdx_dedxAllLikelihood\")) > 1.3) && (log(userFloat(\"dEdx_dedxAllLikelihood\")) > (0.8 + exp(1.0 - 3.8*p - 0.7*p*   p))) && (log(userFloat(\"dEdx_dedxAllLikelihood\")) < (1.5 + exp(1.0 - 1.3*p - 5.8*p*p)))"
kaonBandMi = "(log(userFloat(\"dEdx_dedxAllLikelihood\")) > 1.3) && (log(userFloat(\"dEdx_dedxAllLikelihood\")) > (0.8 + exp(0.9 - 3.0*p - 1.0*p*   p))) && (log(userFloat(\"dEdx_dedxAllLikelihood\")) < (1.5 + exp(0.9 - 1.5*p - 4.9*p*p)))"
kaonSelection = cms.string("(pt > 0.0 && abs(eta) < 3.0) && quality(\"highPurity\")")
kaonFinalSelectionPl = cms.string("abs(userFloat(\"dzSig\"))<3.0 && abs(userFloat(\"dxySig\"))<3.0 && "+kaonBandPl)
kaonFinalSelectionMi = cms.string("abs(userFloat(\"dzSig\"))<3.0 && abs(userFloat(\"dxySig\"))<3.0 && "+kaonBandMi)
diKaSelection = cms.string("charge==0")
process.diKa = generalParticles.clone(
    pdgId = cms.uint32(333),
    preSelection = diKaSelection,
    fitAlgo = [],
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(321), charge = cms.int32(+1), selection = kaonSelection, finalSelection = kaonFinalSelectionPl),
        cms.PSet(pdgId = cms.uint32(321), charge = cms.int32(-1), selection = kaonSelection, finalSelection = kaonFinalSelectionMi),
    ]),
    dEdxInputs = cms.VInputTag('dedxAllLikelihood', 'dedxPixelLikelihood', 'dedxStripLikelihood', 'dedxPixelHarmonic2')
)
process.oneDiKa = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diKa"), minNumber = cms.uint32(1))

# DiPi selection
pionBandPl = "((log(userFloat(\"dEdx_dedxAllLikelihood\")) < 1.3) || (log(userFloat(\"dEdx_dedxAllLikelihood\")) < (0.8 + exp(1.0 - 3.8*p - 0.7*p*  p))))"
pionBandMi = "((log(userFloat(\"dEdx_dedxAllLikelihood\")) < 1.3) || (log(userFloat(\"dEdx_dedxAllLikelihood\")) < (0.8 + exp(0.9 - 3.0*p - 1.0*p*  p))))"
pionSelection = cms.string("(pt > 0.0 && abs(eta) < 3.0) && quality(\"highPurity\")")
pionFinalSelectionPl = cms.string("abs(userFloat(\"dzSig\"))<3.0 && abs(userFloat(\"dxySig\"))<3.0 && "+pionBandPl)
pionFinalSelectionMi = cms.string("abs(userFloat(\"dzSig\"))<3.0 && abs(userFloat(\"dxySig\"))<3.0 && "+pionBandMi)
diPiSelection = cms.string("charge==0")
process.diPi = generalParticles.clone(
    pdgId = cms.uint32(770),
    preSelection = diPiSelection,
    fitAlgo = [],
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(211), charge = cms.int32(+1), selection = pionSelection, finalSelection = pionFinalSelectionPl),
        cms.PSet(pdgId = cms.uint32(211), charge = cms.int32(-1), selection = pionSelection, finalSelection = pionFinalSelectionMi),
    ]),
    dEdxInputs = cms.VInputTag('dedxAllLikelihood', 'dedxPixelLikelihood', 'dedxStripLikelihood', 'dedxPixelHarmonic2')
)
process.oneDiPi = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diPi"), minNumber = cms.uint32(1))

# DiKaDiPi selection
diKaDiPiSelection = cms.string("charge==0")
process.diKaDiPi = generalParticles.clone(
    pdgId = cms.uint32(2170),
    preSelection = diKaDiPiSelection,
    fitAlgo = [],
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(333), source = cms.InputTag("diKa")),
        cms.PSet(pdgId = cms.uint32(770), source = cms.InputTag("diPi")),
    ]),
)
process.oneDiKaDiPi = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diKaDiPi"), minNumber = cms.uint32(1))

# Add diKaDiPi event selection
process.fourTracks = cms.EDFilter("TrackCountFilter", src = cms.InputTag("generalTracks"), minNumber = cms.uint32(4))
process.hpTracks = cms.EDFilter("TrackSelector", src = cms.InputTag("generalTracks"), cut = cms.string("quality(\"highPurity\")"))
process.hpCands = cms.EDProducer("ChargedCandidateProducer", src = cms.InputTag("hpTracks"), particleType = cms.string('pi+'))
process.maxFourHPCands = cms.EDFilter("PATCandViewCountFilter", src = cms.InputTag("hpCands"), minNumber = cms.uint32(0), maxNumber = cms.uint32(4))
process.diKaDiPiEvtSel = cms.Sequence(process.fourTracks * process.hpTracks * process.hpCands * process.maxFourHPCands)

# Add trigger selection
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltFilter.andOr = cms.bool(True)
process.hltFilter.throw = cms.bool(False)
process.hltFilter.HLTPaths = [
    # UPC ZB triggers
    'HLT_HIUPC_ZeroBias_MaxPixelCluster10000_v*',
    'HLT_HIUPC_ZeroBias_MinPixelCluster400_MaxPixelCluster10000_v*',
    'HLT_HIUPC_ZeroBias_SinglePixelTrackLowPt_MaxPixelCluster400_v*',
    'HLT_HIUPC_ZeroBias_SinglePixelTrack_MaxPixelTrack_v*',
    # UPC ZDC OR triggers
    'HLT_HIUPC_ZDC1nOR_MaxPixelCluster10000_v*',
    'HLT_HIUPC_ZDC1nOR_MinPixelCluster400_MaxPixelCluster10000_v*',
    'HLT_HIUPC_ZDC1nOR_SinglePixelTrackLowPt_MaxPixelCluster400_v*',
    'HLT_HIUPC_ZDC1nOR_SinglePixelTrack_MaxPixelTrack_v*',
    # UPC ZDC AND triggers
    'HLT_HIUPC_ZDC1nAND_NotMBHF2_MaxPixelCluster10000_v*',
]

# Add PbPb collision event selection
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.load('VertexCompositeAnalysis.VertexCompositeProducer.hfCoincFilter_cff')
process.colEvtSel = cms.Sequence(process.hiClusterCompatibility)

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
    process.hfPosFilterNTh20_seq *
    process.hfNegFilterNTh20_seq *
    process.diKaDiPiEvtSel
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

# Define the analysis steps
process.diKaDiPi_rereco_step = cms.Path(process.eventFilter_HM * process.diKa * process.oneDiKa * process.diPi * process.oneDiPi * process.diKaDiPi * process.oneDiKaDiPi * process.cent_seq)

## Adding the VertexComposite tree ################################################################################################

event_filter = cms.untracked.vstring(
    "Flag_colEvtSel",
    "Flag_clusterCompatibilityFilter",
    "Flag_primaryVertexFilter",
    "Flag_hfPosFilterNTh7",
    "Flag_hfPosFilterNTh9p2",
    "Flag_hfPosFilterNTh8",
    "Flag_hfPosFilterNTh20",
    "Flag_hfNegFilterNTh7",
    "Flag_hfNegFilterNTh8p6",
    "Flag_hfNegFilterNTh8",
    "Flag_hfNegFilterNTh20",
)

trig_info = cms.untracked.VPSet([
    # UPC ZB triggers
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_SinglePixelTrack_MaxPixelTrack_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_SinglePixelTrackLowPt_MaxPixelCluster400_v*'), filter = cms.string('hltSinglePixelTrackLowPtForUPC'), minN = cms.int32(1)),
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_MinPixelCluster400_MaxPixelCluster10000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_MaxPixelCluster10000_v*')),
    # UPC ZDC OR triggers
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_SinglePixelTrack_MaxPixelTrack_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_SinglePixelTrackLowPt_MaxPixelCluster400_v*'), filter = cms.string('hltSinglePixelTrackLowPtForUPC'), minN = cms.int32(1)),
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_MinPixelCluster400_MaxPixelCluster10000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_MaxPixelCluster10000_v*')),
    # UPC ZDC AND triggers
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nAND_NotMBHF2_MaxPixelCluster10000_v*')),
])

from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.diKaDiPiAna = particleAna.clone(
  recoParticles = cms.InputTag("diKaDiPi"),
  selectEvents = cms.string("diKaDiPi_rereco_step"),
  eventFilterNames = event_filter,
  addTrgObj = cms.untracked.bool(True),
  triggerInfo = trig_info,
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('diKaDiPi_ana.root'))
process.p = cms.EndPath(process.diKaDiPiAna)

#! Define the process schedule !!!!!!!!!!!!!!!!!!
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.diKaDiPi_rereco_step,
    process.p
)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

## Add the event selection filters ###############################################################################################
process.Flag_colEvtSel = cms.Path(process.colEvtSel)
process.Flag_clusterCompatibilityFilter = cms.Path(process.eventFilter_HM * process.hiClusterCompatibility)
process.Flag_primaryVertexFilter = cms.Path(process.eventFilter_HM * process.primaryVertexFilter)
process.Flag_hfPosFilterNTh7 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh7_seq)
process.Flag_hfPosFilterNTh9p2 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh9p2_seq)
process.Flag_hfPosFilterNTh8 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh8_seq)
process.Flag_hfPosFilterNTh20 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh20_seq)
process.Flag_hfNegFilterNTh7 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh7_seq)
process.Flag_hfNegFilterNTh8p6 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh8p6_seq)
process.Flag_hfNegFilterNTh8 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh8_seq)
process.Flag_hfNegFilterNTh20 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh20_seq)

eventFilterPaths = [ process.Flag_colEvtSel , process.Flag_clusterCompatibilityFilter , process.Flag_primaryVertexFilter , process.Flag_hfPosFilterNTh7 , process.Flag_hfPosFilterNTh9p2 , process.Flag_hfPosFilterNTh8 , process.Flag_hfPosFilterNTh20 , process.Flag_hfNegFilterNTh7 , process.Flag_hfNegFilterNTh8p6 , process.Flag_hfNegFilterNTh8 , process.Flag_hfNegFilterNTh20 ]

#! Adding the process schedule !!!!!!!!!!!!!!!!!!
for P in eventFilterPaths:
    process.schedule.insert(0, P)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# Change to MiniAOD
from VertexCompositeAnalysis.VertexCompositeProducer.PATAlgos_cff import changeToMiniAOD
changeToMiniAOD(process)
