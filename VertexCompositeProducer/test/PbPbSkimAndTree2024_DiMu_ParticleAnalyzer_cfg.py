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
    fileNames = cms.untracked.vstring("root://cms-xrd-global.cern.ch///store/hidata/HIRun2024A/HIForward0/AOD/PromptReco-v1/000/388/004/00000/09b2e419-c1c4-4343-8cc1-e9bb6d9bda65.root"),
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Set the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('141X_dataRun3_Prompt_v3')

# Set ZDC information
process.load('VertexCompositeAnalysis.VertexCompositeProducer.QWZDC2018Producer_cfi')
process.load('VertexCompositeAnalysis.VertexCompositeProducer.QWZDC2018RecHit_cfi')
process.zdcdigi.SOI = cms.untracked.int32(2)
'''
process.es_pool = cms.ESSource("PoolDBESSource",
    timetype = cms.string('runnumber'),
    toGet = cms.VPSet(cms.PSet(record = cms.string("HcalElectronicsMapRcd"), tag = cms.string("HcalElectronicsMap_2021_v2.0_data"))),
    connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
    authenticationMethod = cms.untracked.uint32(1)
)
process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')
process.es_ascii = cms.ESSource('HcalTextCalibrations',
    input = cms.VPSet(cms.PSet(object = cms.string('ElectronicsMap'), file = cms.FileInPath("VertexCompositeAnalysis/VertexCompositeProducer/data/emap_2023_newZDC_v3.txt")))
)
'''
# Add the Particle producer
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles

# DiMu selection
SoftIdReco = "(innerTrack.isNonnull && innerTrack.quality(\"highPurity\") && innerTrack.hitPattern.trackerLayersWithMeasurement > 5 && innerTrack.hitPattern.pixelLayersWithMeasurement > 0 && isTrackerMuon)"
muonSelection = cms.string("(pt > 0.0 && abs(eta) < 2.5) && "+SoftIdReco)
diMuSelection = cms.string("charge==0")
hpMuSelection = cms.string("innerTrack.isNonnull && innerTrack.quality(\"highPurity\")")
process.diMu = generalParticles.clone(
    pdgId = cms.uint32(443),
    preSelection = diMuSelection,
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(13), charge = cms.int32(+1), selection = muonSelection),
        cms.PSet(pdgId = cms.uint32(13), charge = cms.int32(-1), selection = muonSelection),
    ]),
    muons = cms.InputTag('patMuons')
)
process.oneDiMu = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diMu"), minNumber = cms.uint32(1))

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
    muonsCut     = hpMuSelection,
    tracksCut    = cms.string("quality(\"highPurity\")"),
)

# Add diMu event selection
process.twoMuons = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("muons"), minNumber = cms.uint32(2))
process.hpMuons = cms.EDFilter("MuonSelector", src = cms.InputTag("muons"), cut = hpMuSelection)
process.maxTwoHPMuons = cms.EDFilter("PATCandViewCountFilter", src = cms.InputTag("hpMuons"), minNumber = cms.uint32(0), maxNumber = cms.uint32(2))
process.goodMuons = cms.EDFilter("MuonSelector",
            src = cms.InputTag("muons"),
            cut = muonSelection,
            )
process.twoGoodMuons = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodMuons"), minNumber = cms.uint32(2))
process.goodDiMuons = cms.EDProducer("CandViewShallowCloneCombiner",
            cut = diMuSelection,
            checkCharge = cms.bool(False),
            decay = cms.string('goodMuons@+ goodMuons@-')
            )
process.oneGoodDiMu = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodDiMuons"), minNumber = cms.uint32(1))
process.diMuEvtSel = cms.Sequence(process.mergedMuons * process.twoMuons * process.hpMuons * process.maxTwoHPMuons * process.goodMuons * process.twoGoodMuons * process.goodDiMuons * process.oneGoodDiMu)

# Add trigger selection
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltFilter.andOr = cms.bool(True)
process.hltFilter.throw = cms.bool(False)
process.hltFilter.HLTPaths = [
    # UPC muon triggers
    'HLT_HIUPC_SingleMu*',
    # UPC zero bias triggers
    'HLT_HIZeroBias_v*',
    'HLT_HIZeroBias_HighRate_v*',
    'HLT_HIUPC_ZeroBias_SinglePixelTrack_MaxPixelTrack_v*',
    'HLT_HIUPC_ZeroBias_SinglePixelTrackLowPt_MaxPixelCluster400_v*',
    'HLT_HIUPC_ZeroBias_MinPixelCluster400_MaxPixelCluster10000_v*',
    # UPC SDC triggers
    'HLT_HIUPC_ZDC1nOR_SinglePixelTrack_MaxPixelTrack_v*',
    'HLT_HIUPC_ZDC1nOR_SinglePixelTrackLowPt_MaxPixelCluster400_v*',
    'HLT_HIUPC_ZDC1nOR_MinPixelCluster400_MaxPixelCluster10000_v*',
]

# Add PbPb collision event selection
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.load('VertexCompositeAnalysis.VertexCompositeProducer.hfCoincFilter_cff')
process.colEvtSel = cms.Sequence(process.hfCoincFilter2Th4 * process.primaryVertexFilter)

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
    process.diMuEvtSel
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

# Define the analysis steps
process.diMu_rereco_step = cms.Path(process.eventFilter_HM * process.mergedMuons * process.patMuonSequence *  process.diMu * process.oneDiMu)

# Add the VertexComposite tree
from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.diMuAna = particleAna.clone(
  recoParticles = cms.InputTag("diMu"),
  selectEvents = cms.string("diMu_rereco_step"),
  eventFilterNames = cms.untracked.vstring(
      'Flag_colEvtSel',
      'Flag_hfCoincFilter2Th4',
      'Flag_primaryVertexFilter',
      'Flag_hfPosFilterNTh3',
      'Flag_hfNegFilterNTh3',
      'Flag_hfPosFilterNTh4',
      'Flag_hfNegFilterNTh4',
      'Flag_hfPosFilterNTh5',
      'Flag_hfNegFilterNTh5',
      'Flag_hfPosFilterNTh6',
      'Flag_hfNegFilterNTh6',
      'Flag_hfPosFilterNTh7',
      'Flag_hfNegFilterNTh7',
      'Flag_hfPosFilterNTh8',
      'Flag_hfNegFilterNTh8',
      'Flag_hfPosFilterNTh7p3',
      'Flag_hfNegFilterNTh7p6'
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
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('diMu_ana.root'))
process.p = cms.EndPath(process.diMuAna)

# Define the process schedule
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.diMu_rereco_step,
    process.p
)

# Add the event selection filters
process.Flag_colEvtSel = cms.Path(process.eventFilter_HM * process.colEvtSel)
process.Flag_hfCoincFilter2Th4 = cms.Path(process.eventFilter_HM * process.hfCoincFilter2Th4)
process.Flag_primaryVertexFilter = cms.Path(process.eventFilter_HM * process.primaryVertexFilter)
process.Flag_hfPosFilterNTh3 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh3_seq)
process.Flag_hfPosFilterNTh4 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh4_seq)
process.Flag_hfPosFilterNTh5 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh5_seq)
process.Flag_hfPosFilterNTh6 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh6_seq)
process.Flag_hfPosFilterNTh7 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh7_seq)
process.Flag_hfPosFilterTh8 = cms.Path(process.eventFilter_HM * process.hfPosFilterTh8_seq)
process.Flag_hfPosFilterNTh8 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh8_seq)
process.Flag_hfPosFilterNTh7p3 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh7p3_seq)
process.Flag_hfPosFilterNTh200 = cms.Path(process.eventFilter_HM * process.hfPosFilterNTh200_seq)
process.Flag_hfNegFilterNTh3 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh3_seq)
process.Flag_hfNegFilterNTh4 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh4_seq)
process.Flag_hfNegFilterNTh5 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh5_seq)
process.Flag_hfNegFilterNTh6 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh6_seq)
process.Flag_hfNegFilterNTh7 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh7_seq)
process.Flag_hfNegFilterTh8 = cms.Path(process.eventFilter_HM * process.hfNegFilterTh8_seq)
process.Flag_hfNegFilterNTh8 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh8_seq)
process.Flag_hfNegFilterNTh7p6 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh7p6_seq)
process.Flag_hfNegFilterNTh200 = cms.Path(process.eventFilter_HM * process.hfNegFilterNTh200_seq)

eventFilterPaths = [ process.Flag_colEvtSel , process.Flag_hfCoincFilter2Th4 , process.Flag_primaryVertexFilter, process.Flag_hfPosFilterNTh3, process.Flag_hfNegFilterNTh3,process.Flag_hfPosFilterNTh4,
                     process.Flag_hfNegFilterNTh4, process.Flag_hfPosFilterNTh5, process.Flag_hfNegFilterNTh5, process.Flag_hfPosFilterNTh6, process.Flag_hfNegFilterNTh6, process.Flag_hfPosFilterNTh7, process.Flag_hfNegFilterNTh7,
                     process.Flag_hfPosFilterTh8, process.Flag_hfPosFilterNTh8, process.Flag_hfNegFilterTh8, process.Flag_hfNegFilterNTh8, process.Flag_hfPosFilterNTh7p3, process.Flag_hfNegFilterNTh7p6 ]

process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
    process.primaryVertexFilter *
    process.hfPosFilterNTh200_seq *
    process.hfNegFilterNTh200_seq *
    process.diMuEvtSel
)

for P in eventFilterPaths:
    process.schedule.insert(0, P)

# Add recovery for offline primary vertex
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
process = MassReplaceInputTag(process, "muons", "mergedMuons")
process.mergedMuons.muons = cms.InputTag("muons")
