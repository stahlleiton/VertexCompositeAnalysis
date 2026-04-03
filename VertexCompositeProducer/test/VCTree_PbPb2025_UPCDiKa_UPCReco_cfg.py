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
kaonSelection = cms.string("")#(pt > 0.0 && abs(eta) < 3.0) && quality(\"highPurity\")")
kaonFinalSelection = cms.string("")#abs(userFloat(\"dzSig\"))<3.0 && abs(userFloat(\"dxySig\"))<3.0")
diKaSelection = cms.string("charge==0")
process.diKa = generalParticles.clone(
    pdgId = cms.uint32(333),
    preSelection = diKaSelection,
    fitAlgo = [],
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(321), charge = cms.int32(+1), selection = kaonSelection, finalSelection = kaonFinalSelection),
        cms.PSet(pdgId = cms.uint32(321), charge = cms.int32(-1), selection = kaonSelection, finalSelection = kaonFinalSelection),
    ]),
    dEdxInputs = cms.VInputTag('dedxAllLikelihood', 'dedxPixelLikelihood', 'dedxStripLikelihood', 'dedxPixelHarmonic2')
)
process.oneDiKa = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diKa"), minNumber = cms.uint32(1))

# Add diKa event selection
process.twoTracks = cms.EDFilter("TrackCountFilter", src = cms.InputTag("generalTracks"), minNumber = cms.uint32(2))
process.hpTracks = cms.EDFilter("TrackSelector", src = cms.InputTag("generalTracks"), cut = cms.string("quality(\"highPurity\")"))
process.hpCands = cms.EDProducer("ChargedCandidateProducer", src = cms.InputTag("hpTracks"), particleType = cms.string('pi+'))
process.maxTwoHPCands = cms.EDFilter("PATCandViewCountFilter", src = cms.InputTag("hpCands"), minNumber = cms.uint32(0), maxNumber = cms.uint32(2))
process.goodTracks = cms.EDFilter("TrackSelector",
    src = cms.InputTag("generalTracks"),
    cut = kaonSelection,
)
process.twoGoodTracks = cms.EDFilter("TrackCountFilter", src = cms.InputTag("goodTracks"), minNumber = cms.uint32(2))
process.goodKaons = cms.EDProducer("ChargedCandidateProducer",
    src = cms.InputTag("goodTracks"),
    particleType = cms.string('pi+')
)
process.goodDiKaons = cms.EDProducer("CandViewShallowCloneCombiner",
    cut = diKaSelection,
    checkCharge = cms.bool(False),
    decay = cms.string('goodKaons@+ goodKaons@-')
)
process.oneGoodDiKa = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodDiKaons"), minNumber = cms.uint32(1))
process.diKaEvtSel = cms.Sequence(process.twoTracks * process.hpTracks * process.hpCands * process.maxTwoHPCands * process.goodTracks * process.twoGoodTracks * process.goodKaons * process.goodDiKaons * process.oneGoodDiKa)

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
    process.diKaEvtSel
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

# Define the analysis steps
process.diKa_rereco_step = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh20_seq * process.hfNegFilterNTh20_seq * process.diKa * process.oneDiKa * process.cent_seq)

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
process.diKaAna = particleAna.clone(
  recoParticles = cms.InputTag("diKa"),
  selectEvents = cms.string("diKa_rereco_step"),
  eventFilterNames = event_filter,
  addTrgObj = cms.untracked.bool(True),
  triggerInfo = trig_info,
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('diKa_ana.root'))
process.p = cms.EndPath(process.diKaAna)

#! Define the process schedule !!!!!!!!!!!!!!!!!!
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.diKa_rereco_step,
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
