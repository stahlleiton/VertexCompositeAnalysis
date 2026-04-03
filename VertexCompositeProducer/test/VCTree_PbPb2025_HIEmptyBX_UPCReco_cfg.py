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
    fileNames = cms.untracked.vstring("root://xrootd-cms.infn.it///store/hidata/HIRun2025A/HIEmptyBX/MINIAOD/PromptReco-v1/000/400/265/00000/915f3db6-dfbf-4802-905c-82deb8340b96.root"),
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

# Add trigger selection
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltFilter.andOr = cms.bool(True)
process.hltFilter.throw = cms.bool(False)
process.hltFilter.HLTPaths = [
    # Empty BX triggers
    'HLT_HIL1NotBptxOR_v*',
    'HLT_HIL1UnpairedBunchBptxMinus_v*',
    'HLT_HIL1UnpairedBunchBptxPlus_v*',
]

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
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
    # EmptyBX triggers
    cms.PSet(path = cms.string('HLT_HIL1NotBptxOR_v*')),
    cms.PSet(path = cms.string('HLT_HIL1UnpairedBunchBptxMinus_v*')),
    cms.PSet(path = cms.string('HLT_HIL1UnpairedBunchBptxPlus_v*')),
])

from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.hiEmptyBXAna = particleAna.clone(
  eventFilterNames = event_filter,
  triggerInfo = trig_info,
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('hiEmptyBX.root'))
process.p = cms.EndPath(process.hiEmptyBXAna)

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
