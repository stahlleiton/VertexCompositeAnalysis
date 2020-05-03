import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
process = cms.Process('D0PbPb2018SKIM',eras.Run2_2018_pp_on_AA)

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load("CondCore.CondDB.CondDB_cfi")
process.load('Configuration.EventContent.EventContent_cff')


# Limit the output messages
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True ),
)

process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(
###'/store/hidata/HIRun2018A/HIMinimumBias1/AOD/PromptReco-v1/000/326/577/00000/532A6440-350D-2446-89FB-3CA9E8335E4F.root'
#'/store/hidata/HIRun2018A/HIMinimumBias6/AOD/04Apr2019-v1/70012/FEB2298F-D3AA-5A41-A999-02AEC7648569.root'
'file:output_numEvent100.root'
   ),
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))


# Set the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = cms.string('103X_dataRun2_v6')


# Add PbPb centrality
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.load('RecoHI.HiCentralityAlgos.HiCentrality_cfi')
process.hiCentrality.produceHFhits = False
process.hiCentrality.produceHFtowers = False
process.hiCentrality.produceEcalhits = False
process.hiCentrality.produceZDChits = False
process.hiCentrality.produceETmidRapidity = False
process.hiCentrality.producePixelhits = False
process.hiCentrality.produceTracks = False
process.hiCentrality.producePixelTracks = False
process.hiCentrality.reUseCentrality = True
process.hiCentrality.srcReUse = cms.InputTag("hiCentrality","","reRECO")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
process.centralityBin.nonDefaultGlauberModel = cms.string("")
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run2v1031x02_offline"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        label = cms.untracked.string("HFtowers")
        ),
    ])
process.cent_seq = cms.Sequence(process.hiCentrality * process.centralityBin)


# Add PbPb event plane
process.load("RecoHI.HiEvtPlaneAlgos.HiEvtPlane_cfi")
process.load("RecoHI.HiEvtPlaneAlgos.hiEvtPlaneFlat_cfi")
process.hiEvtPlane.trackTag = cms.InputTag("generalTracks")
process.hiEvtPlane.vertexTag = cms.InputTag("offlinePrimaryVerticesRecovery")
process.hiEvtPlane.loadDB = cms.bool(True)
process.hiEvtPlane.useNtrk = cms.untracked.bool(False)
process.hiEvtPlane.caloCentRef = cms.double(-1)
process.hiEvtPlane.caloCentRefWidth = cms.double(-1)
process.hiEvtPlaneFlat.caloCentRef = cms.double(-1)
process.hiEvtPlaneFlat.caloCentRefWidth = cms.double(-1)
process.hiEvtPlaneFlat.vertexTag = cms.InputTag("offlinePrimaryVerticesRecovery")
process.hiEvtPlaneFlat.useNtrk = cms.untracked.bool(False)
process.CondDB.connect = "sqlite_file:HeavyIonRPRcd_PbPb2018_offline.db"
process.PoolDBESSource = cms.ESSource("PoolDBESSource",
                                       process.CondDB,
                                       toGet = cms.VPSet(cms.PSet(record = cms.string('HeavyIonRPRcd'),
                                                                  tag = cms.string('HeavyIonRPRcd_PbPb2018_offline')
                                                                  )
                                                         )
                                      )
process.es_prefer_flatparms = cms.ESPrefer('PoolDBESSource','')
process.evtplane_seq = cms.Sequence(process.hiEvtPlane * process.hiEvtPlaneFlat)


# D0 candidate rereco
process.load("VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff")
process.generalD0CandidatesNew = process.generalD0Candidates.clone(
    preSelection = cms.string(""
       "mass<2.01 && mass> 1.72 && pt > 1.0"
       "&& userFloat('tkPtSum')>1.6 && userFloat('tkEtaDiff')<1.0"
       ),
    postSelection = cms.string(""
       "userFloat('vertexProb')>0.02"
       ),
    finalSelection = cms.string(""
       "userFloat('rVtxSig') > 2.0 "
       "&& userFloat('lVtxSig') > 3.0"
       "&& abs(userFloat('angle3D')) < 0.2 && abs(userFloat('angle2D')) < 0.2"
       "&& abs(mass-1.86484)<0.15"
       ),
#
    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(321), charge = cms.int32(-1),
           selection = cms.string(
              "pt>1.0 && abs(eta)<2.4"
              "&& quality('highPurity') && ptError/pt<0.1"
              "&& (normalizedChi2/hitPattern.trackerLayersWithMeasurement)<0.18"
              "&& numberOfValidHits >=11"
              ),
           finalSelection = cms.string(''
              'userFloat("dzSig") < 3.0 && userFloat("dxySig") < 3.0'
              )
           ),
        cms.PSet(pdgId = cms.int32(211), charge = cms.int32(+1),
           selection = cms.string(
              "pt>1.0 && abs(eta)<2.4"
              "&& quality('highPurity') && ptError/pt<0.1"
              "&& (normalizedChi2/hitPattern.trackerLayersWithMeasurement)<0.18"
              "&& numberOfValidHits >=11"
              ),
           finalSelection = cms.string(''
              'userFloat("dzSig") < 3.0 && userFloat("dxySig") < 3.0'
              )
           )
    ])
  )
#process.generalD0CandidatesNew.tkPtSumCut = cms.double(1.6)
#process.generalD0CandidatesNew.tkEtaDiffCut = cms.double(1.0)
#process.generalD0CandidatesNew.tkNhitsCut = cms.int32(11)
#process.generalD0CandidatesNew.tkPtErrCut = cms.double(0.1)
#process.generalD0CandidatesNew.tkPtCut = cms.double(1.0)
#process.generalD0CandidatesNew.tkChi2Cut = cms.double(0.18) ###cesar: changed chi2/dof/nlayers in D0Fitter and cut according to Tracking group
#process.generalD0CandidatesNew.dPtCut = cms.double(1.0)
#process.generalD0CandidatesNew.alphaCut = cms.double(0.2)
#process.generalD0CandidatesNew.vtxSignificance3DCut = cms.double(3.0)
#process.generalD0CandidatesNew.alpha2DCut = cms.double(0.2)
#process.generalD0CandidatesNew.vtxSignificance2DCut = cms.double(2.0)
#process.generalD0CandidatesNew.VtxChiProbCut = cms.double(0.02)
#process.generalD0CandidatesNew.tkEtaCut = cms.double(2.4)
#process.generalD0CandidatesNew.mvaTrackRecoSrc = cms.InputTag("generalTracks","MVAValues") ###cesar:to change iter6 tracking mva cut



# Add PbPb collision event selection
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesRecovery_cfi")
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
process.load('HeavyIonsAnalysis.Configuration.hfCoincFilter_cff')


# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.offlinePrimaryVerticesRecovery *
    process.hfCoincFilter2Th4 *
    process.primaryVertexFilter *
    process.clusterCompatibilityFilter
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

# Define the analysis steps
process.pcentandep_step = cms.Path(process.eventFilter_HM * process.cent_seq * process.evtplane_seq)
process.d0rereco_step = cms.Path(process.eventFilter_HM * process.generalD0CandidatesNew)


# Define the output
process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('d0ana_PbPb2018.root')
                                  )


process.load("VertexCompositeAnalysis.VertexCompositeProducer.ppanalysisSkimContentD0_cff")
process.output_HM = cms.OutputModule("PoolOutputModule",
    outputCommands = process.analysisSkimContent.outputCommands,
    fileName = cms.untracked.string('PbPb2018_SKIM_AOD.root'),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('eventFilter_HM_step')),
    dataset = cms.untracked.PSet(
      dataTier = cms.untracked.string('AOD')
    )
)


process.output_HM.outputCommands = cms.untracked.vstring('drop *',
      'keep *_generalD0CandidatesNew__D0PbPb2018SKIM',
      'keep *_offlinePrimaryVerticesRecovery_*_*',
      'keep *_hiEvtPlane_*_*',
      'keep *_centralityBin_*_*',
      'keep *_hiCentrality_*_D0PbPb2018SKIM',
)
process.output_HM_step = cms.EndPath(process.output_HM)


# Define the process schedule
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.pcentandep_step,
    process.d0rereco_step,
    process.output_HM_step
)


# peripheral pv recovery
from HLTrigger.Configuration.CustomConfigs import MassReplaceInputTag
process = MassReplaceInputTag(process,"offlinePrimaryVertices","offlinePrimaryVerticesRecovery")
process.offlinePrimaryVerticesRecovery.oldVertexLabel = "offlinePrimaryVertices"
