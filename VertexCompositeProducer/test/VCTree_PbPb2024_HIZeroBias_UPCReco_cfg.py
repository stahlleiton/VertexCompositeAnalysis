import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
process = cms.Process('ANASKIM', eras.Run3_2024_UPC)

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
    fileNames = cms.untracked.vstring("root://xrootd-cms.infn.it///store/hidata/HIRun2024B/HIZeroBias0/MINIAOD/PromptReco-v1/000/388/317/00000/787458d1-1d9a-4a1f-8eb7-5d528aaf72f5.root"),
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Set the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('141X_dataRun3_Prompt_v4')


## ##############################################################################################################################
## Variables Production #########################################################################################################

#* Set ZDC information
process.load("VertexCompositeAnalysis.VertexCompositeProducer.ZDCRun3_cfg")
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.cent_seq = cms.Sequence(process.centralityBin * process.zdcreco)

# Add Zero Bias event selection
process.load('VertexCompositeAnalysis.VertexCompositeProducer.hfCoincFilter_cff')
process.hpTracks = cms.EDFilter("TrackSelector", src = cms.InputTag("generalTracks"), cut = cms.string("quality(\"highPurity\")"))
process.hpCands = cms.EDProducer("ChargedCandidateProducer", src = cms.InputTag("hpTracks"), particleType = cms.string('pi+'))
process.noHPCands = cms.EDFilter("PATCandViewCountFilter", src = cms.InputTag("hpCands"), minNumber = cms.uint32(0), maxNumber = cms.uint32(0))
process.hiZeroBiasEvtSel = cms.Sequence(process.hpTracks * process.hpCands * process.noHPCands * process.hfPosFilterNTh20_seq * process.hfNegFilterNTh20_seq)

# Add trigger selection
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltFilter.andOr = cms.bool(True)
process.hltFilter.throw = cms.bool(False)
process.hltFilter.HLTPaths = [
    'HLT_HIZeroBias_HighRateRAW_v*'
]

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
    process.hiZeroBiasEvtSel *
    process.cent_seq
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

## Adding the VertexComposite tree ################################################################################################

event_filter = cms.untracked.vstring(
    "Flag_colEvtSel",
    "Flag_clusterCompatibilityFilter",
    "Flag_primaryVertexFilter",
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
    # Zero Bias trigger
    cms.PSet(path = cms.string('HLT_HIZeroBias_HighRateRAW_v*')),
])

from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.hiZeroBiasAna = particleAna.clone(
  eventFilterNames = event_filter,
  triggerInfo = trig_info,
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('hiZeroBias.root'))
process.p = cms.EndPath(process.hiZeroBiasAna)

#! Define the process schedule !!!!!!!!!!!!!!!!!!
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.p
)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

## Add the event selection filters ###############################################################################################
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.Flag_colEvtSel = cms.Path(process.eventFilter_HM * process.hiClusterCompatibility)
process.Flag_clusterCompatibilityFilter = cms.Path(process.eventFilter_HM * process.hiClusterCompatibility)
process.Flag_primaryVertexFilter = cms.Path(process.eventFilter_HM * process.primaryVertexFilter)

eventFilterPaths = [ process.Flag_clusterCompatibilityFilter , process.Flag_primaryVertexFilter ]

#! Adding the process schedule !!!!!!!!!!!!!!!!!!
for P in eventFilterPaths:
    process.schedule.insert(0, P)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# Change to MiniAOD
from VertexCompositeAnalysis.VertexCompositeProducer.PATAlgos_cff import changeToMiniAOD
changeToMiniAOD(process)
