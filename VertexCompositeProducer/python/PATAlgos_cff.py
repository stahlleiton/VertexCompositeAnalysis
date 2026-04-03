import FWCore.ParameterSet.Config as cms

def doPATMuons(process):
    # Check if sequence already ran
    if hasattr(process, 'patMuons'): return

    # Make PAT Muons
    from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cfi import patMuons
    process.patMuons = patMuons.clone(
        muonSource = cms.InputTag('muons'),
        # track information
        embedTrack = cms.bool(True),
        embedCombinedMuon = cms.bool(True),
        embedStandAloneMuon = cms.bool(True),
        embedMuonBestTrack = cms.bool(True),
        forceBestTrackEmbedding = cms.bool(False),
        embedTunePMuonBestTrack = cms.bool(False),
        embedCaloMETMuonCorrs = cms.bool(False),
        embedTcMETMuonCorrs = cms.bool(False),
        embedPickyMuon = cms.bool(False),
        embedTpfmsMuon = cms.bool(False),
        embedDytMuon = cms.bool(False),
        # high level information
        embedHighLevelSelection = cms.bool(True),
        # gen information
        addGenMatch = cms.bool(False),
        embedGenMatch = cms.bool(False),
        # PF information
        useParticleFlow = cms.bool(False),
        embedPFCandidate = cms.bool(False),
        embedPfEcalEnergy = cms.bool(False),
        # extra information
        addInverseBeta = cms.bool(False),
        addEfficiencies = cms.bool(False),
        addResolutions = cms.bool(False),
        userIsolation = cms.PSet(),
        isoDeposits = cms.PSet(),
        isolationValues = cms.PSet(),
    )

    process.patMuons.userData.userInts.src    = []
    process.patMuons.userData.userFloats.src  = []
    process.patMuons.userData.userCands.src   = []
    process.patMuons.userData.userClasses.src = []

    # Make a sequence
    process.patMuonSequence = cms.Sequence( process.patMuons )


def doPATElectrons(process):
    # Check if sequence already ran
    if hasattr(process, 'patElectrons'): return

    # Make PAT Electrons
    from PhysicsTools.PatAlgos.producersLayer1.electronProducer_cfi import patElectrons
    process.patElectrons = patElectrons.clone(
        electronSource = cms.InputTag('gedGsfElectrons'),
        # track information
        embedTrack                  = cms.bool(True),
        embedGsfElectronCore        = cms.bool(True),
        embedGsfTrack               = cms.bool(True),
        embedSuperCluster           = cms.bool(True),
        embedSeedCluster            = cms.bool(True),
        embedBasicClusters          = cms.bool(True),
        embedPreshowerClusters      = cms.bool(False),
        embedRecHits                = cms.bool(False),
        # high level information
        embedHighLevelSelection = cms.bool(True),
        # gen information
        addGenMatch = cms.bool(False),
        embedGenMatch = cms.bool(False),
        # PF information
        embedPflowSuperCluster      = cms.bool(False),
        embedPflowBasicClusters     = cms.bool(False),
        embedPflowPreshowerClusters = cms.bool(False),
        embedPFCandidate            = cms.bool(False),
        # extra information
        addMVAVariables         = cms.bool(True),
        isoDeposits             = cms.PSet(),
        isolationValues         = cms.PSet(),
        isolationValuesNoPFId   = cms.PSet(),
    )

    process.patElectrons.userData.userInts.src    = []
    process.patElectrons.userData.userFloats.src  = []
    process.patElectrons.userData.userCands.src   = []
    process.patElectrons.userData.userClasses.src = []

    process.patLowPtElectrons = process.patElectrons.clone(electronSource = cms.InputTag('lowPtGsfElectrons'))

    # Make a sequence
    process.patElectronSequence = cms.Sequence( process.patElectrons * process.patLowPtElectrons )


def doPATPhotons(process):
    # Check if sequence already ran
    if hasattr(process, 'patPhotons'): return

    # Make PAT Electrons
    from PhysicsTools.PatAlgos.producersLayer1.photonProducer_cfi import patPhotons
    process.patPhotons = patPhotons.clone(
        photonSource = cms.InputTag('gedPhotons'),
        electronSource = cms.InputTag('gedGsfElectrons'),
        # photon information
        embedSuperCluster           = cms.bool(True),
        embedSeedCluster            = cms.bool(True),
        embedBasicClusters          = cms.bool(True),
        embedPreshowerClusters      = cms.bool(True),
        embedRecHits                = cms.bool(True),
        saveRegressionData          = cms.bool(True),
        # high level information
        addPhotonID                 = cms.bool(False),
        # gen information
        addGenMatch = cms.bool(False),
        embedGenMatch = cms.bool(False),
        # PF information
        addPFClusterIso             = cms.bool(False),
        addPuppiIsolation           = cms.bool(False),
        # extra information
        isoDeposits             = cms.PSet(),
        userIsolation           = dict()
    )

    process.patPhotons.userData.userInts.src    = []
    process.patPhotons.userData.userFloats.src  = []
    process.patPhotons.userData.userCands.src   = []
    process.patPhotons.userData.userClasses.src = []

    # Make a sequence
    process.patPhotonSequence = cms.Sequence( process.patPhotons )


def doPATMET(process):
    # Check if sequence already ran
    if hasattr(process, 'patMETs'): return

    # Make PAT MET
    from PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi import patMETs
    process.patMETs = patMETs.clone(
        metSource = cms.InputTag("pfMet"),
        # gen information
        addGenMET = cms.bool(False),
        genMETSource = cms.InputTag(""),
    )

    process.patPhotons.userData.userInts.src    = []
    process.patPhotons.userData.userFloats.src  = []
    process.patPhotons.userData.userCands.src   = []
    process.patPhotons.userData.userClasses.src = []

    # Make a sequence
    process.patMETSequence = cms.Sequence( process.patMETs )


def changeToMiniAOD(process):

    process.patTriggerFull = cms.EDProducer("PATTriggerObjectStandAloneUnpacker",
        patTriggerObjectsStandAlone = cms.InputTag('slimmedPatTrigger'),
        triggerResults              = cms.InputTag('TriggerResults::HLT'),
        unpackFilterLabels          = cms.bool(True)
    )
    process.load('VertexCompositeAnalysis.VertexCompositeProducer.unpackedTracksAndVertices_cfi')
    process.eventFilter_HM.insert(0, process.unpackedTracksAndVertices)

    if hasattr(process, "patMuons"):
        process.load('VertexCompositeAnalysis.VertexCompositeProducer.unpackedMuons_cfi')
        process.patMuons = process.unpackedMuons.clone()

    if hasattr(process, "primaryVertexFilter"):
        process.primaryVertexFilter.cut = "!isFake && abs(z) <= 25 && position.Rho <= 2"

    from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
    process = MassReplaceInputTag(process,"offlinePrimaryVertices","unpackedTracksAndVertices")
    process = MassReplaceInputTag(process,"generalTracks","unpackedTracksAndVertices")
    for dedx in ['dedxStripLikelihood','dedxPixelLikelihood','dedxAllLikelihood', 'dedxPixelHarmonic2']:
        process = MassReplaceInputTag(process,dedx,"unpackedTracksAndVertices:"+dedx)
    process = MassReplaceInputTag(process,"particleFlow","packedPFCandidates")
    process = MassReplaceInputTag(process,"genParticles","prunedGenParticles")
