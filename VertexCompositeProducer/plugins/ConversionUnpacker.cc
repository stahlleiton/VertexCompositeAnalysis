#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"

namespace pat {

  class ConversionUnpacker : public edm::global::EDProducer<> {
  public:
    explicit ConversionUnpacker(const edm::ParameterSet& iConfig)
        : compositeToken_(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter<edm::InputTag>("composites"))),
          conversionTokens_(getTokens<reco::ConversionCollection>(iConfig.getParameter<std::vector<edm::InputTag> >("conversions"))) {
      produces<reco::ConversionCollection>();
      produces<reco::TrackCollection>();
    };
    ~ConversionUnpacker() override{};

    void produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const override;

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    template <class T>
    std::vector<edm::EDGetTokenT<T> > getTokens(const std::vector<edm::InputTag>& v) {
      std::vector<edm::EDGetTokenT<T> > tokens(v.size());
      for (size_t i = 0; i < v.size(); i++)
        tokens[i] = consumes<T>(v[i]);
      return tokens;
    };

    const edm::EDGetTokenT<pat::CompositeCandidateCollection> compositeToken_;
    const std::vector<edm::EDGetTokenT<reco::ConversionCollection> > conversionTokens_;
  };

}  // namespace pat

void pat::ConversionUnpacker::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // extract input information
  const auto& composites = iEvent.get(compositeToken_);
  std::vector<edm::Handle<reco::ConversionCollection> > conversions(conversionTokens_.size());
  for (size_t i = 0; i < conversionTokens_.size(); i++)
    conversions[i] = iEvent.getHandle(conversionTokens_[i]);

  // create output track collection from composite candidates
  auto outTracks = std::make_unique<reco::TrackCollection>();
  std::map<size_t, std::array<size_t, 2>> trackMap; 
  for (size_t iC = 0; iC < composites.size(); iC++)
    for (size_t iTr = 0; iTr < 2; iTr++) {
      trackMap[iC][iTr] = outTracks->size();
      outTracks->emplace_back(*composites[iC].userData<reco::Track>(Form("track%lu", iTr)));
    }
  const auto& outTracksHandle = iEvent.put(std::move(outTracks));

  // create output conversion collection
  auto outConvs = std::make_unique<reco::ConversionCollection>();
  for (size_t iC = 0; iC < composites.size(); iC++) {
    const auto& cand = composites[iC];
    reco::Vertex vertex(cand.vertex(), {});
    vertex.reserve(trackMap[iC].size(), true);
    std::vector<reco::TrackRef> tracks;
    for (const auto& iTr : trackMap[iC]) {
      tracks.emplace_back(outTracksHandle, iTr);
      vertex.add(reco::TrackBaseRef(tracks.back()), *tracks.back(), 1.f);
    }
    const auto& flags = cand.userInt("flags");
    const auto algo = static_cast<reco::Conversion::ConversionAlgorithm>(flags/32 - (flags/256)*8);
    outConvs->emplace_back(reco::CaloClusterPtrVector(), tracks, vertex, algo);
  }
  for (const auto& cands : conversions)
    for (const auto& cand : *cands)
      outConvs->emplace_back(cand);
  iEvent.put(std::move(outConvs));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void pat::ConversionUnpacker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("composites", edm::InputTag("oniaPhotonCandidates:conversions"))->setComment("composite candidate collection");
  //desc.add<std::vector<edm::InputTag> >("conversions", {edm::InputTag("gsfTracksOpenConversions:gsfTracksOpenConversions"), edm::InputTag("reducedEgamma:reducedConversions")})->setComment("conversion collections");
  desc.add<std::vector<edm::InputTag> >("conversions", {})->setComment("conversion collections");
  descriptions.add("unpackedConversions", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
using namespace pat;
DEFINE_FWK_MODULE(ConversionUnpacker);
