import FWCore.ParameterSet.Config as cms

generalParticles = cms.EDProducer("ParticleProducer",

    pdgId = cms.int32(455),

    # particle selection
    preSelection = cms.string(""),
    postSelection = cms.string(""),
    finalSelection = cms.string(""),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(13), charge = cms.int32(-1), selection = cms.string("pt>1.0 && abs(eta)<2.4")),
        cms.PSet(pdgId = cms.int32(13), charge = cms.int32(+1), selection = cms.string("pt>1.0 && abs(eta)<2.4")),
    ]),

    # input collections
    primaryVertices = cms.InputTag('offlinePrimaryVertices'),
    tracks = cms.InputTag('generalTracks'),
    muons = cms.InputTag('patMuonsWithTrigger'),
    electrons = cms.InputTag(''),
    taus = cms.InputTag(''),
    photons = cms.InputTag(''),
    pfParticles = cms.InputTag(''),
    jets = cms.InputTag(''),
)

# D0
generalD0Candidates = cms.EDProducer("ParticleProducer",

    pdgId = cms.int32(421),

    # particle selection
    # selections below which are used in HIN-19-008 are commented. They show an example how to set parameters.
    # before fitting, cand p4 (slightly different from final fit value), pair information of daughters are ready
    preSelection = cms.string(""
       #"mass<2.01 && mass> 1.72 && pt > 1.0"
       #"&& userFloat('tkPtSum')>1.6 && userFloat('tkEtaDiff')<1.0"
       ),
    # after fitting cand, available information: dca between two daughters, initial p4, fit p4, vertex prob, decay vertex, primary vertex
    postSelection = cms.string(""
       #"userFloat('dca')<9999.99 && userFloat('vertexProb')>0.02"
       ),
    # flight of distance, 2D, 3D, pointing angle are available
    finalSelection = cms.string(""
       #"userFloat('rVtxMag') > 0.0 && userFloat('rVtxSig') > 2.0 "
       #" && userFloat('lVtxMag') > 0.0 && userFloat('lVtxSig') > 3.0"
       #"&& cos(userFloat('angle3D')) < 1.0 && cos(userFloat('angle2D)') < 1.0)" # this line does not work
       #"&& userFloat('collinearity3D') > -2.0 && userFloat('collinearity2D') > -2.0" # this line means cos(angle3D) > -2.0
       #"&& abs(userFloat('angle3D')) < 0.2 && abs(userFloat('angle2D')) < 0.2"
       #"&& abs(mass-1.86484)<0.15"
       ),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(321), charge = cms.int32(-1),
           selection = cms.string(
              "pt>1.0 && abs(eta)<2.4"
              #"&& quality('highPurity') && ptError/pt<0.1"
              #"&& (normalizedChi2/hitPattern.trackerLayersWithMeasurement)<0.18"
              #"&& numberOfValidHits >=11"
              ),
           finalSelection = cms.string(""
              #'userFloat("dzSig") < 3.0 && userFloat("dxySig") < 3.0'
              ),
           ),
        cms.PSet(pdgId = cms.int32(211), charge = cms.int32(+1), selection = cms.string("pt>1.0 && abs(eta)<2.4"),)
    ]),

    # input collections
    primaryVertices = cms.InputTag('offlinePrimaryVertices'),
    tracks = cms.InputTag('generalTracks'),
    muons = cms.InputTag(''),
    electrons = cms.InputTag(''),
    taus = cms.InputTag(''),
    photons = cms.InputTag(''),
    pfParticles = cms.InputTag(''),
    jets = cms.InputTag(''),

    #mvaTrackRecoSrc = cms.InputTag("generalTracks","MVAValues"),
    #in current versoin, no dedx input tag
)
