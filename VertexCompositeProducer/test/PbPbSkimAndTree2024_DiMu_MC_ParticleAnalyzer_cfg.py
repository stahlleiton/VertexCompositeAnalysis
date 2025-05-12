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

# Define the input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://xrootd-cms.infn.it//store/group/phys_heavyions/dyano/OORun/Jpsitodimu/MonteCarlo/Reco/mc_jpsidimu_Reco_250105_192808/CRAB_UserFiles/mc_jpsidimu_Reco_250105_192808/250105_182852/0000/step3_STARlight_Reco_9.root"),
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Set the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('141X_mcRun3_2024_realistic_HI_v13')

# Add the Particle producer
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles

# DiMu selection
process.diMu = generalParticles.clone(
    pdgId = cms.uint32(443),
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(13), charge = cms.int32(+1)),
        cms.PSet(pdgId = cms.uint32(13), charge = cms.int32(-1)),
    ]),
    muons = cms.InputTag('patMuons')
)

# Add muons
from VertexCompositeAnalysis.VertexCompositeProducer.PATAlgos_cff import doPATMuons
doPATMuons(process)

from RecoMuon.MuonIdentification.calomuons_cfi import calomuons
process.mergedMuons = cms.EDProducer("CaloMuonMerger",
    muons    = cms.InputTag("muons::RECO"),
    caloMuons = cms.InputTag("calomuons"),
    minCaloCompatibility = calomuons.minCaloCompatibility,
    mergeTracks = cms.bool(True),
    tracks = cms.InputTag("generalTracks"),
    mergeCaloMuons = cms.bool(False),
    caloMuonsCut = cms.string(""),
    muonsCut     = cms.string(""),
    tracksCut    = cms.string(""),
)

# Add PbPb collision event selection
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.load('VertexCompositeAnalysis.VertexCompositeProducer.hfCoincFilter_cff')
process.colEvtSel = cms.Sequence(process.hiClusterCompatibility * process.primaryVertexFilter)

# Define the analysis steps
process.diMu_rereco_step = cms.Path(process.mergedMuons * process.patMuonSequence *  process.diMu)

# Add the VertexComposite tree
from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna_mc
process.diMuAna = particleAna_mc.clone(
  recoParticles = cms.InputTag("diMu"),
  eventFilterNames = cms.untracked.vstring(
      'Flag_colEvtSel',
      'Flag_primaryVertexFilter',
  ),
  triggerInfo = cms.untracked.VPSet([
    # UPC muon triggers
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuCosmic_BptxAND_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuCosmic_NotMBHF2AND_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuCosmic_NotMBHF2AND_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuCosmic_NotMBHF2OR_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuCosmic_NotMBHF2OR_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_BptxAND_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_NotMBHF2AND_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_NotMBHF2AND_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_NotMBHF2OR_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_NotMBHF2OR_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_OR_SingleMuCosmic_EMTF_BptxAND_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_OR_SingleMuCosmic_EMTF_NotMBHF2AND_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_OR_SingleMuCosmic_EMTF_NotMBHF2AND_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_OR_SingleMuCosmic_EMTF_NotMBHF2OR_MaxPixelCluster1000_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_SingleMuOpen_OR_SingleMuCosmic_EMTF_NotMBHF2OR_v*')),
    # UPC zero bias triggers
    cms.PSet(path = cms.string('HLT_HIZeroBias_v*')),
    cms.PSet(path = cms.string('HLT_HIZeroBias_HighRate_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_SinglePixelTrack_MaxPixelTrack_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_SinglePixelTrackLowPt_MaxPixelCluster400_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_MinPixelCluster400_MaxPixelCluster10000_v*')),
    # UPC ZDC triggers
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_SinglePixelTrack_MaxPixelTrack_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_SinglePixelTrackLowPt_MaxPixelCluster400_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_MinPixelCluster400_MaxPixelCluster10000_v*')),
  ]),
  maxGenDeltaR = cms.untracked.double(0.3),
  maxGenDeltaPtRel = cms.untracked.double(1.0),
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('diMu_ana_mc.root'))
process.p = cms.EndPath(process.diMuAna)

# Define the process schedule
process.schedule = cms.Schedule(
    process.diMu_rereco_step,
    process.p
)

# Add the event selection filters
process.Flag_colEvtSel = cms.Path(process.colEvtSel)
process.Flag_hfCoincFilter2Th4 = cms.Path(process.hfCoincFilter2Th4)
process.Flag_primaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.Flag_hfPosFilterNTh3 = cms.Path(process.hfPosFilterNTh3_seq)
process.Flag_hfPosFilterNTh4 = cms.Path(process.hfPosFilterNTh4_seq)
process.Flag_hfPosFilterNTh5 = cms.Path(process.hfPosFilterNTh5_seq)
process.Flag_hfPosFilterNTh6 = cms.Path(process.hfPosFilterNTh6_seq)
process.Flag_hfPosFilterNTh7 = cms.Path(process.hfPosFilterNTh7_seq)
process.Flag_hfPosFilterTh8 = cms.Path(process.hfPosFilterTh8_seq)
process.Flag_hfPosFilterNTh8 = cms.Path(process.hfPosFilterNTh8_seq)
process.Flag_hfPosFilterNTh7p3 = cms.Path(process.hfPosFilterNTh7p3_seq)
process.Flag_hfPosFilterNTh200 = cms.Path(process.hfPosFilterNTh200_seq)
process.Flag_hfNegFilterNTh3 = cms.Path(process.hfNegFilterNTh3_seq)
process.Flag_hfNegFilterNTh4 = cms.Path(process.hfNegFilterNTh4_seq)
process.Flag_hfNegFilterNTh5 = cms.Path(process.hfNegFilterNTh5_seq)
process.Flag_hfNegFilterNTh6 = cms.Path(process.hfNegFilterNTh6_seq)
process.Flag_hfNegFilterNTh7 = cms.Path(process.hfNegFilterNTh7_seq)
process.Flag_hfNegFilterTh8 = cms.Path(process.hfNegFilterTh8_seq)
process.Flag_hfNegFilterNTh8 = cms.Path(process.hfNegFilterNTh8_seq)
process.Flag_hfNegFilterNTh7p6 = cms.Path(process.hfNegFilterNTh7p6_seq)
process.Flag_hfNegFilterNTh200 = cms.Path(process.hfNegFilterNTh200_seq)

eventFilterPaths = [ process.Flag_colEvtSel , process.Flag_hfCoincFilter2Th4 , process.Flag_primaryVertexFilter, process.Flag_hfPosFilterNTh3, process.Flag_hfNegFilterNTh3,process.Flag_hfPosFilterNTh4,
                     process.Flag_hfNegFilterNTh4, process.Flag_hfPosFilterNTh5, process.Flag_hfNegFilterNTh5, process.Flag_hfPosFilterNTh6, process.Flag_hfNegFilterNTh6, process.Flag_hfPosFilterNTh7, process.Flag_hfNegFilterNTh7,
                     process.Flag_hfPosFilterTh8, process.Flag_hfPosFilterNTh8, process.Flag_hfNegFilterTh8, process.Flag_hfNegFilterNTh8, process.Flag_hfPosFilterNTh7p3, process.Flag_hfNegFilterNTh7p6 ]

for P in eventFilterPaths:
    process.schedule.insert(0, P)

# Add recovery for offline primary vertex
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
process = MassReplaceInputTag(process, "muons", "mergedMuons")
process.mergedMuons.muons = cms.InputTag("muons")
