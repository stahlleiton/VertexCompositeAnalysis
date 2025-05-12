import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
import FWCore.PythonUtilities.LumiList as LumiList
process = cms.Process('ANASKIM', eras.Run3_2024_UPC)

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')

# Limit the output messages
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 200
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

#Setup FWK for multithreaded
#process.options.numberOfThreads = 8
#process.options.numberOfStreams = 0

# Define the input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://cms-xrd-global.cern.ch///store/hidata/HIRun2024A/HIForward0/AOD/PromptReco-v1/000/388/004/00000/09b2e419-c1c4-4343-8cc1-e9bb6d9bda65.root"),
)
#process.source.lumisToProcess = LumiList.LumiList(filename = 'Cert_Collisions2023HI_374288_375823_Muon.json').getVLuminosityBlockRange()
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Set the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('141X_dataRun3_Prompt_v3')

# Set ZDC information
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

# Add PbPb centrality

process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.cent_seq = cms.Sequence(process.centralityBin)

# Add the Particle producer
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles

# DiPi selection
pionSelection = cms.string("(pt > 0.0 ) && quality(\"highPurity\")")
pionFinalSelection = cms.string("abs(userFloat(\"dzSig\"))<3.0 && abs(userFloat(\"dxySig\"))<3.0")
diPiSelection = cms.string("charge==0")
process.diPi = generalParticles.clone(
    pdgId = cms.uint32(113),
    preSelection = diPiSelection,
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(211), charge = cms.int32(+1), selection = pionSelection),
        cms.PSet(pdgId = cms.uint32(211), charge = cms.int32(-1), selection = pionSelection),
    ]),
    dEdxInputs = cms.vstring('dedxHarmonic2', 'dedxPixelHarmonic2'),
)
process.oneDiPi = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diPi"), minNumber = cms.uint32(1))

# DiMu selection
SoftIdReco = "(innerTrack.isNonnull && innerTrack.quality(\"highPurity\") && innerTrack.hitPattern.trackerLayersWithMeasurement > 5 && innerTrack.hitPattern.pixelLayersWithMeasurement > 0)"
muonSelection = cms.string("(pt > 0.0 )")
diMuSelection = cms.string("charge==0 && mass > 2.5")
process.diMu = generalParticles.clone(
    pdgId = cms.uint32(443),
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(13), charge = cms.int32(+1)),
        cms.PSet(pdgId = cms.uint32(13), charge = cms.int32(-1)),
    ]),
    muons = cms.InputTag('patMuons')
)
process.oneDiMu = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diMu"), minNumber = cms.uint32(1))

# DiPi-DiMu selection
diPiDiMuSelection = cms.string("charge==0")
process.diPiDiMu = generalParticles.clone(
    pdgId = cms.uint32(100443),
    preSelection = diPiDiMuSelection,
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.uint32(443), source = cms.InputTag("diMu")),
        cms.PSet(pdgId = cms.uint32(113), source = cms.InputTag("diPi")),
    ]),
)
process.oneDiPiDiMu = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("diPiDiMu"), minNumber = cms.uint32(1))

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
    muonsCut     = muonSelection,
    tracksCut    = cms.string("quality(\"highPurity\") && hitPattern.trackerLayersWithMeasurement > 5 && hitPattern.pixelLayersWithMeasurement > 0"),
)

# Add diPi event selection
process.twoTracks = cms.EDFilter("TrackCountFilter", src = cms.InputTag("generalTracks"), minNumber = cms.uint32(2))
process.goodTrack = cms.EDFilter("TrackSelector",
            src = cms.InputTag("generalTracks"),
            cut = pionSelection,
            )
process.twoGoodTracks = cms.EDFilter("TrackCountFilter", src = cms.InputTag("goodTrack"), minNumber = cms.uint32(2))
process.goodPion = cms.EDProducer("ChargedCandidateProducer",
            src = cms.InputTag("goodTrack"),
            particleType = cms.string('pi+')
            )
process.goodDiPi = cms.EDProducer("CandViewShallowCloneCombiner",
            cut = diPiSelection,
            checkCharge = cms.bool(True),
            decay = cms.string('goodPion@- goodPion@+')
            )
process.oneGoodDiPi = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodDiPi"), minNumber = cms.uint32(1))
process.diPiEvtSel = cms.Sequence(process.twoTracks * process.goodTrack * process.twoGoodTracks * process.goodPion * process.goodDiPi * process.oneGoodDiPi)

# Add diMu event selection
process.twoMuons = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("muons"), minNumber = cms.uint32(2))
process.goodMuons = cms.EDFilter("MuonSelector",
            src = cms.InputTag("muons"),
            cut = muonSelection,
            )
process.twoGoodMuons = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodMuons"), minNumber = cms.uint32(2))
process.goodDiMu = cms.EDProducer("CandViewShallowCloneCombiner",
            cut = diMuSelection,
            checkCharge = cms.bool(False),
            decay = cms.string('goodMuons@+ goodMuons@-')
            )
process.oneGoodDiMu = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodDiMu"), minNumber = cms.uint32(1))
process.diMuEvtSel = cms.Sequence(process.mergedMuons * process.twoMuons * process.goodMuons * process.twoGoodMuons * process.goodDiMu * process.oneGoodDiMu)

# Add diPi-diMu event selection
process.hpTracks = cms.EDFilter("TrackSelector", src = cms.InputTag("generalTracks"), cut = cms.string("quality(\"highPurity\")"))
process.hpCands = cms.EDProducer("ChargedCandidateProducer", src = cms.InputTag("hpTracks"), particleType = cms.string('pi+'))
process.maxFourHPTracks = cms.EDFilter("PATCandViewCountFilter", src = cms.InputTag("hpCands"), minNumber = cms.uint32(0), maxNumber = cms.uint32(4))
process.goodDiPiDiMu = cms.EDProducer("CandViewShallowCloneCombiner",
            cut = diPiDiMuSelection,
            checkCharge = cms.bool(False),
            decay = cms.string('goodDiMu@+ goodDiPi@-')
            )
process.oneGoodDiPiDiMu = cms.EDFilter("CandViewCountFilter", src = cms.InputTag("goodDiPiDiMu"), minNumber = cms.uint32(1))
process.diPiDiMuEvtSel = cms.Sequence(process.hpTracks * process.hpCands * process.maxFourHPTracks * process.diPiEvtSel * process.diMuEvtSel * process.goodDiPiDiMu * process.oneGoodDiPiDiMu )

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
process.colEvtSel = cms.Sequence(process.primaryVertexFilter)

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
    process.diPiDiMuEvtSel
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

# Define the analysis steps
process.diPiDiMu_rereco_step = cms.Path(process.eventFilter_HM * process.diPi * process.oneDiPi * process.mergedMuons * process.patMuonSequence * process.diMu * process.oneDiMu * process.diPiDiMu)# * process.oneDiPiDiMu)# * process.cent_seq)

# Add the VertexComposite tree
from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.diPiDiMuAna = particleAna.clone(
  recoParticles = cms.InputTag("diPiDiMu"),
  selectEvents = cms.string("diPiDiMu_rereco_step"),
  eventFilterNames = cms.untracked.vstring(
      'Flag_colEvtSel',
      'Flag_hfCoincFilter2Th4',
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
    cms.PSet(path = cms.string('HLT_HIUPC_ZeroBias_SinglePixelTrackLowPt_MaxPixelCluster400_v*')),
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_MinPixelCluster400_MaxPixelCluster10000_v*')),
    # UPC ZDC triggers
    cms.PSet(path = cms.string('HLT_HIUPC_ZDC1nOR_SinglePixelTrackLowPt_MaxPixelCluster400_v*')),
  ]),
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('diPiDiMu_ana.root'))
process.p = cms.EndPath(process.diPiDiMuAna)

process.output_HM = cms.OutputModule("PoolOutputModule",
   # outputCommands = process.analysisSkimContent.outputCommands,
    fileName = cms.untracked.string('pPb_HM_JPsi.root'),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('diPiDiMu_rereco_step')),
    dataset = cms.untracked.PSet(
      dataTier = cms.untracked.string('AOD')
    )
)

process.output_HM_step = cms.EndPath(process.output_HM)

# Define the process schedule
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.diPiDiMu_rereco_step,
    #process.output_HM_step
    process.p
)

# Add the event selection filters
process.Flag_colEvtSel = cms.Path(process.eventFilter_HM * process.colEvtSel)
process.Flag_hfCoincFilter2Th4 = cms.Path(process.eventFilter_HM * process.hfCoincFilter2Th4)
process.Flag_primaryVertexFilter = cms.Path(process.eventFilter_HM * process.primaryVertexFilter)

eventFilterPaths = [ process.Flag_colEvtSel , process.Flag_hfCoincFilter2Th4 , process.Flag_primaryVertexFilter ]

process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
    process.primaryVertexFilter *
    process.hfPosFilterNTh8_seq *
    process.hfNegFilterNTh8_seq *
    process.diPiDiMuEvtSel
)

for P in eventFilterPaths:
    process.schedule.insert(0, P)

# Add recovery for offline primary vertex
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
process = MassReplaceInputTag(process, "muons", "mergedMuons")
process.mergedMuons.muons = cms.InputTag("muons")
