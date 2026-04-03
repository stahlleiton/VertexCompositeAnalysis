import FWCore.ParameterSet.Config as cms

# select HF candidate energy
towersFromPF = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("particleFlow"),
    cut = cms.string("(pdgId() == 1 || pdgId() == 2) && abs(eta()) >= 3.0")
)
towersAboveThreshold = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("towersFromPF"),
    cut = cms.string("energy() > 3.0")
)

# make calotowers into candidates with threshold 4
towersAboveThresholdTh2 = towersAboveThreshold.clone(cut = cms.string("energy() >= 2.0"))
towersAboveThresholdTh4 = towersAboveThreshold.clone(cut = cms.string("energy() >= 4.0"))
towersAboveThresholdTh5 = towersAboveThreshold.clone(cut = cms.string("energy() >= 5.0"))
towersAboveThresholdTh6 = towersAboveThreshold.clone(cut = cms.string("energy() >= 6.0"))
towersAboveThresholdTh7 = towersAboveThreshold.clone(cut = cms.string("energy() >= 7.0"))
towersAboveThresholdTh8 = towersAboveThreshold.clone(cut = cms.string("energy() >= 8.0"))
towersAboveThresholdTh7p3 = towersAboveThreshold.clone(cut = cms.string("energy() >= 7.3"))
towersAboveThresholdTh7p6 = towersAboveThreshold.clone(cut = cms.string("energy() >= 7.6"))
towersAboveThresholdTh8p6 = towersAboveThreshold.clone(cut = cms.string("energy() >= 8.6"))
towersAboveThresholdTh9p2 = towersAboveThreshold.clone(cut = cms.string("energy() >= 9.2"))
towersAboveThresholdTh20 = towersAboveThreshold.clone(cut = cms.string("energy() >= 20.0"))
towersAboveThresholdTh200 = towersAboveThreshold.clone(cut = cms.string("energy() >= 200.0"))

# select HF+ towers above threshold
hfPosTowers = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("towersAboveThreshold"),
    cut = cms.string("eta() >= 3.0 && eta() <= 6.0")
)

# select HF- towers above threshold
hfNegTowers = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("towersAboveThreshold"),
    cut = cms.string("eta() >= -6.0 && eta() <= -3.0")
)

# select HF+/HF- towers above threshold 4 and 5
hfPosTowersTh2 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh2"))
hfNegTowersTh2 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh2"))
hfPosTowersTh4 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh4"))
hfNegTowersTh4 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh4"))
hfPosTowersTh5 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh5"))
hfNegTowersTh5 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh5"))
hfPosTowersTh6 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh6"))
hfNegTowersTh6 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh6"))
hfPosTowersTh7 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh7"))
hfNegTowersTh7 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh7"))
hfPosTowersTh8 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh8"))
hfNegTowersTh8 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh8"))
hfPosTowersTh7p3 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh7p3"))
hfNegTowersTh7p6 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh7p6"))
hfPosTowersTh9p2 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh9p2"))
hfNegTowersTh8p6 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh8p6"))
hfPosTowersTh20 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh20"))
hfNegTowersTh20 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh20"))
hfPosTowersTh200 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh200"))
hfNegTowersTh200 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh200"))

# require at least one HF+ tower above threshold
hfPosFilter = cms.EDFilter("CandCountFilter",
    src = cms.InputTag("hfPosTowers"),
    minNumber = cms.uint32(1)
)

# require at least one HF- tower above threshold
hfNegFilter = cms.EDFilter("CandCountFilter",
    src = cms.InputTag("hfNegTowers"),
    minNumber = cms.uint32(1)
)

# require at least one HF+/HF- tower above threshold 4
hfPosFilterTh2 =hfPosFilter.clone(src="hfPosTowersTh2")
hfNegFilterTh2 =hfNegFilter.clone(src="hfNegTowersTh2")
hfPosFilterTh4 =hfPosFilter.clone(src="hfPosTowersTh4")
hfNegFilterTh4 =hfNegFilter.clone(src="hfNegTowersTh4")
hfPosFilterTh5 =hfPosFilter.clone(src="hfPosTowersTh5")
hfNegFilterTh5 =hfNegFilter.clone(src="hfNegTowersTh5")
hfPosFilterTh6 =hfPosFilter.clone(src="hfPosTowersTh6")
hfNegFilterTh6 =hfNegFilter.clone(src="hfNegTowersTh6")
hfPosFilterTh7 =hfPosFilter.clone(src="hfPosTowersTh7")
hfNegFilterTh7 =hfNegFilter.clone(src="hfNegTowersTh7")
hfPosFilterTh8 =hfPosFilter.clone(src="hfPosTowersTh8")
hfNegFilterTh8 =hfNegFilter.clone(src="hfNegTowersTh8")
hfPosFilterTh7p3 =hfPosFilter.clone(src="hfPosTowersTh7p3")
hfNegFilterTh7p6 =hfNegFilter.clone(src="hfNegTowersTh7p6")
hfPosFilterTh9p2 =hfPosFilter.clone(src="hfPosTowersTh9p2")
hfNegFilterTh8p6 =hfNegFilter.clone(src="hfNegTowersTh8p6")
hfPosFilterTh20 =hfPosFilter.clone(src="hfPosTowersTh20")
hfNegFilterTh20 =hfNegFilter.clone(src="hfNegTowersTh20")
hfPosFilterTh200 =hfPosFilter.clone(src="hfPosTowersTh200")
hfNegFilterTh200 =hfNegFilter.clone(src="hfNegTowersTh200")

# one HF tower above threshold on each side
hfCoincFilterTh3 = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfPosTowers *
    hfNegTowers *
    hfPosFilter *
    hfNegFilter)

hfCoincFilterTh2 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilterTh2 *
    hfNegFilterTh2)

hfCoincFilterTh4 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilterTh4 *
    hfNegFilterTh4)

hfCoincFilterTh5 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilterTh5 *
    hfNegFilterTh5)

hfPosFilterNTh3_seq = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfPosTowers *
    ~hfPosFilter)

hfNegFilterNTh3_seq = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfNegTowers *
    ~hfNegFilter)

hfPosFilterNTh4_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    ~hfPosFilterTh4)

hfNegFilterNTh4_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfNegTowersTh4 *
    ~hfNegFilterTh4)

hfPosFilterNTh5_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    ~hfPosFilterTh5)

hfNegFilterNTh5_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfNegTowersTh5 *
    ~hfNegFilterTh5)

hfPosFilterNTh6_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh6 *
    hfPosTowersTh6 *
    ~hfPosFilterTh6)

hfNegFilterNTh6_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh6 *
    hfNegTowersTh6 *
    ~hfNegFilterTh6)

hfPosFilterNTh7_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh7 *
    hfPosTowersTh7 *
    ~hfPosFilterTh7)

hfNegFilterNTh7_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh7 *
    hfNegTowersTh7 *
    ~hfNegFilterTh7)

hfPosFilterTh8_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh8 *
    hfPosTowersTh8 *
    hfPosFilterTh8)

hfPosFilterNTh8_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh8 *
    hfPosTowersTh8 *
    ~hfPosFilterTh8)

hfNegFilterTh8_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh8 *
    hfNegTowersTh8 *
    hfNegFilterTh8)

hfNegFilterNTh8_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh8 *
    hfNegTowersTh8 *
    ~hfNegFilterTh8)

hfPosFilterNTh7p3_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh7p3 *
    hfPosTowersTh7p3 *
    ~hfPosFilterTh7p3)

hfNegFilterNTh7p6_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh7p6 *
    hfNegTowersTh7p6 *
    ~hfNegFilterTh7p6)

hfPosFilterNTh9p2_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh9p2 *
    hfPosTowersTh9p2 *
    ~hfPosFilterTh9p2)

hfNegFilterNTh8p6_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh8p6 *
    hfNegTowersTh8p6 *
    ~hfNegFilterTh8p6)

hfPosFilterNTh20_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh20 *
    hfPosTowersTh20 *
    ~hfPosFilterTh20)

hfNegFilterNTh20_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh20 *
    hfNegTowersTh20 *
    ~hfNegFilterTh20)

hfPosFilterNTh200_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh200 *
    hfPosTowersTh200 *
    ~hfPosFilterTh200)

hfNegFilterNTh200_seq = cms.Sequence(
    towersFromPF * towersAboveThresholdTh200 *
    hfNegTowersTh200 *
    ~hfNegFilterTh200)

# two HF towers above threshold on each side
hfPosFilter2 = hfPosFilter.clone(minNumber=cms.uint32(2))
hfNegFilter2 = hfNegFilter.clone(minNumber=cms.uint32(2))
hfPosFilter2Th2 = hfPosFilterTh2.clone(minNumber=cms.uint32(2))
hfNegFilter2Th2 = hfNegFilterTh2.clone(minNumber=cms.uint32(2))
hfPosFilter2Th4 = hfPosFilterTh4.clone(minNumber=cms.uint32(2))
hfNegFilter2Th4 = hfNegFilterTh4.clone(minNumber=cms.uint32(2))
hfPosFilter2Th5 = hfPosFilterTh5.clone(minNumber=cms.uint32(2))
hfNegFilter2Th5 = hfNegFilterTh5.clone(minNumber=cms.uint32(2))

hfCoincFilter2Th3 = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfPosTowers *
    hfNegTowers *
    hfPosFilter2 *
    hfNegFilter2)

hfCoincFilter2Th2 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter2Th2 *
    hfNegFilter2Th2)

hfCoincFilter2Th4 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter2Th4 *
    hfNegFilter2Th4)

hfCoincFilter2Th5 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter2Th5 *
    hfNegFilter2Th5)

#three HF towers above threshold on each side
hfPosFilter3 = hfPosFilter.clone(minNumber=cms.uint32(3))
hfNegFilter3 = hfNegFilter.clone(minNumber=cms.uint32(3))
hfPosFilter3Th2 = hfPosFilterTh2.clone(minNumber=cms.uint32(3))
hfNegFilter3Th2 = hfNegFilterTh2.clone(minNumber=cms.uint32(3))
hfPosFilter3Th4 = hfPosFilterTh4.clone(minNumber=cms.uint32(3))
hfNegFilter3Th4 = hfNegFilterTh4.clone(minNumber=cms.uint32(3))
hfPosFilter3Th5 = hfPosFilterTh5.clone(minNumber=cms.uint32(3))
hfNegFilter3Th5 = hfNegFilterTh5.clone(minNumber=cms.uint32(3))

hfCoincFilter3Th3 = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfPosTowers *
    hfNegTowers *
    hfPosFilter3 *
    hfNegFilter3)

hfCoincFilter3Th2 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter3Th2 *
    hfNegFilter3Th2)

hfCoincFilter3Th4 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter3Th4 *
    hfNegFilter3Th4)

hfCoincFilter3Th5 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter3Th5 *
    hfNegFilter3Th5)

#four HF towers above threshold on each side
hfPosFilter4 = hfPosFilter.clone(minNumber=cms.uint32(4))
hfNegFilter4 = hfNegFilter.clone(minNumber=cms.uint32(4))
hfPosFilter4Th2 = hfPosFilterTh2.clone(minNumber=cms.uint32(4))
hfNegFilter4Th2 = hfNegFilterTh2.clone(minNumber=cms.uint32(4))
hfPosFilter4Th4 = hfPosFilterTh4.clone(minNumber=cms.uint32(4))
hfNegFilter4Th4 = hfNegFilterTh4.clone(minNumber=cms.uint32(4))
hfPosFilter4Th5 = hfPosFilterTh5.clone(minNumber=cms.uint32(4))
hfNegFilter4Th5 = hfNegFilterTh5.clone(minNumber=cms.uint32(4))

hfCoincFilter4Th3 = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfPosTowers *
    hfNegTowers *
    hfPosFilter4 *
    hfNegFilter4)

hfCoincFilter4Th2 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter4Th2 *
    hfNegFilter4Th2)

hfCoincFilter4Th4 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter4Th4 *
    hfNegFilter4Th4)

hfCoincFilter4Th5 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter4Th5 *
    hfNegFilter4Th5)

#five hf towers above threshold on each side
hfPosFilter5 = hfPosFilter.clone(minNumber=cms.uint32(5))
hfNegFilter5 = hfNegFilter.clone(minNumber=cms.uint32(5))
hfPosFilter5Th2 = hfPosFilterTh2.clone(minNumber=cms.uint32(5))
hfNegFilter5Th2 = hfNegFilterTh2.clone(minNumber=cms.uint32(5))
hfPosFilter5Th4 = hfPosFilterTh4.clone(minNumber=cms.uint32(5))
hfNegFilter5Th4 = hfNegFilterTh4.clone(minNumber=cms.uint32(5))
hfPosFilter5Th5 = hfPosFilterTh5.clone(minNumber=cms.uint32(5))
hfNegFilter5Th5 = hfNegFilterTh5.clone(minNumber=cms.uint32(5))

hfCoincFilter5Th3 = cms.Sequence(
    towersFromPF * towersAboveThreshold *
    hfPosTowers *
    hfNegTowers *
    hfPosFilter5 *
    hfNegFilter5)

hfCoincFilter5Th2 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter5Th2 *
    hfNegFilter5Th2)

hfCoincFilter5Th4 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter5Th4 *
    hfNegFilter5Th4)

hfCoincFilter5Th5 = cms.Sequence(
    towersFromPF * towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter5Th5 *
    hfNegFilter5Th5)
