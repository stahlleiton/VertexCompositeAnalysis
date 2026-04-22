import FWCore.ParameterSet.Config as cms

def doPATMuons(process, MC=False):
    # Check if sequence already ran
    if hasattr(process, 'patMuonsWithTrigger'): return

    # Make PAT Muons
    process.load("MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff")

    # Add trigger matching
    from MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff import switchOffAmbiguityResolution, addHLTL1Passthrough, useL1Stage2Candidates
    switchOffAmbiguityResolution(process) # Switch off ambiguity resolution: allow multiple reco muons to match to the same trigger muon
    # Use L1 Stage2 (Run2 2016-onwards)
    addHLTL1Passthrough(process)
    useL1Stage2Candidates(process)
    if "hltL1extraParticles" in process.patTrigger.collections:
        process.patTrigger.collections.remove("hltL1extraParticles")
    if "hltGtStage2Digis:Muon" not in process.patTrigger.collections:
        process.patTrigger.collections.append("hltGtStage2Digis:Muon")
    process.muonMatchHLTL1.matchedCuts = cms.string('coll("hltGtStage2Digis:Muon")')
    process.muonMatchHLTL1.useMB2InOverlap = cms.bool(True)
    process.muonMatchHLTL1.useStage2L1 = cms.bool(True)
    process.muonMatchHLTL1.preselection = cms.string("")
    # Modifications for L2 and L3 HLT muons in PbPb 2018s
    if "hltL2MuonCandidates" in process.patTrigger.collections:
        process.patTrigger.collections.remove("hltL2MuonCandidates")
    if "hltL2MuonCandidatesPPOnAA" not in process.patTrigger.collections:
        process.patTrigger.collections.append("hltL2MuonCandidatesPPOnAA")
    process.muonMatchHLTL2.matchedCuts = cms.string('coll("hltL2MuonCandidatesPPOnAA")')
    if "hltIterL3MuonCandidates" in process.patTrigger.collections:
        process.patTrigger.collections.remove("hltIterL3MuonCandidates")
    if "hltIterL3MuonCandidatesPPOnAA" not in process.patTrigger.collections:
        process.patTrigger.collections.append("hltIterL3MuonCandidatesPPOnAA")
    process.muonMatchHLTL3.matchedCuts = cms.string('coll("hltIterL3MuonCandidatesPPOnAA")')
    # Selection used to match online-offline muons
    process.muonL1Info.maxDeltaR = cms.double(0.3)
    process.muonL1Info.maxDeltaEta   = cms.double(0.2)
    process.muonL1Info.fallbackToME1 = cms.bool(True)
    process.muonL1Info.useStation2 = cms.bool(True)
    process.muonL1Info.cosmicPropagationHypothesis = cms.bool(False)
    process.muonL1Info.propagatorAlong = cms.ESInputTag("", "hltESPSteppingHelixPropagatorAlong")
    process.muonL1Info.propagatorAny = cms.ESInputTag("", "SteppingHelixPropagatorAny")
    process.muonL1Info.propagatorOpposite = cms.ESInputTag("", "hltESPSteppingHelixPropagatorOpposite")
    process.muonMatchHLTL1.maxDeltaR = cms.double(0.3)
    process.muonMatchHLTL1.maxDeltaEta   = cms.double(0.2)
    process.muonMatchHLTL1.fallbackToME1 = cms.bool(True)
    process.muonMatchHLTL1.useStation2 = cms.bool(True)
    process.muonMatchHLTL1.cosmicPropagationHypothesis = cms.bool(False)
    process.muonMatchHLTL1.propagatorAlong = cms.ESInputTag("", "hltESPSteppingHelixPropagatorAlong")
    process.muonMatchHLTL1.propagatorAny = cms.ESInputTag("", "SteppingHelixPropagatorAny")
    process.muonMatchHLTL1.propagatorOpposite = cms.ESInputTag("", "hltESPSteppingHelixPropagatorOpposite")
    process.muonMatchHLTL2.maxDeltaR = cms.double(0.3)
    process.muonMatchHLTL2.maxDPtRel = cms.double(10.0)
    process.muonMatchHLTL3.maxDeltaR = cms.double(0.1)
    process.muonMatchHLTL3.maxDPtRel = cms.double(10.0)

    from HLTrigger.Configuration.HLT_GRun_cff import fragment
    process.hltESPSteppingHelixPropagatorAlong = fragment.hltESPSteppingHelixPropagatorAlong
    process.hltESPSteppingHelixPropagatorOpposite = fragment.hltESPSteppingHelixPropagatorOpposite

    # Add MC gen matching
    if MC:
        # Prune generated particles to muons and their parents
        process.genMuons = cms.EDProducer("GenParticlePruner",
            src = cms.InputTag("genParticles"),
            select = cms.vstring(
                "drop  *  ",                      # this is the default
                "++keep abs(pdgId) = 13"          # keep muons and their parents
            )
        )
        from MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff import addMCinfo
        addMCinfo(process)
        # since we match inner tracks, keep the matching tight and make it one-to-one
        process.muonMatch.maxDeltaR = cms.double(0.05)
        process.muonMatch.resolveByMatchQuality = cms.bool(True)
        process.muonMatch.matched = cms.InputTag("genMuons")

    # Make a sequence
    if MC:
        process.patMuonSequence = cms.Sequence( process.genMuons + process.patMuonsWithTriggerSequence )
    else:
        process.patMuonSequence = cms.Sequence( process.patMuonsWithTriggerSequence )


def changeToMiniAOD(process):

    if hasattr(process, "patMuonsWithTrigger"):
        from MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff import useExistingPATMuons
        useExistingPATMuons(process, newPatMuonTag=cms.InputTag("unpackedMuons"), addL1Info=False)

        process.patTriggerFull = cms.EDProducer("PATTriggerObjectStandAloneUnpacker",
            patTriggerObjectsStandAlone = cms.InputTag('slimmedPatTrigger'),
            triggerResults              = cms.InputTag('TriggerResults::HLT'),
            unpackFilterLabels          = cms.bool(True)
        )
        process.load('VertexCompositeAnalysis.VertexCompositeProducer.unpackedTracksAndVertices_cfi')
        process.eventFilter_HM.insert(0, process.unpackedTracksAndVertices)
        process.load('VertexCompositeAnalysis.VertexCompositeProducer.unpackedMuons_cfi')
        process.eventFilter_HM.insert(1, process.unpackedMuons)

    if hasattr(process, "muonMatch"):
        from MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff import changeRecoMuonInput
        changeRecoMuonInput(process, recoMuonCollectionTag=cms.InputTag("unpackedMuonsWithGenMatch"), oldRecoMuonCollectionTag=cms.InputTag("unpackedMuons"))
        process.load('VertexCompositeAnalysis.VertexCompositeProducer.unpackedMuonsWithGenMatch_cfi')
        process.patMuonsWithTriggerSequence.insert(1, process.unpackedMuonsWithGenMatch)
        process.genMuons.src = "prunedGenParticles"
        process.muonMatch.src = "unpackedMuons"

    if hasattr(process, "output_HM"):
        process.output_HM.outputCommands.append('keep *Vert*_unpackedTracksAndVertices_*_*')
        process.output_HM.outputCommands.append('keep patMuons_unpackedMuons_*_*')
        process.output_HM.outputCommands.append('drop patMuons_patMuonsWith*_*_*')

    from FWCore.ParameterSet.MassReplace import massReplaceInputTag as MassReplaceInputTag
    process = MassReplaceInputTag(process,"offlinePrimaryVertices","unpackedTracksAndVertices")
    process = MassReplaceInputTag(process,"generalTracks","unpackedTracksAndVertices")
    process = MassReplaceInputTag(process,"muons","unpackedMuons")
