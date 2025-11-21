#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

namespace pat {

  class GenParticleUnpacker : public edm::global::EDProducer<> {
  public:
    explicit GenParticleUnpacker(const edm::ParameterSet& iConfig)
        : packedGenParticleToken_(consumes<pat::PackedGenParticleCollection>(iConfig.getParameter<edm::InputTag>("packedGenParticles"))) {
      produces<reco::GenParticleCollection>();
    };
    ~GenParticleUnpacker() override{};

    void produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const override;
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:

    const edm::EDGetTokenT<pat::PackedGenParticleCollection> packedGenParticleToken_;
  };

}  // namespace pat

void pat::GenParticleUnpacker::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // extract input information
  const auto& packedGenParticles = iEvent.get(packedGenParticleToken_);

  // create output gen particle collection
  auto outGenP = std::make_unique<reco::GenParticleCollection>();
  for (const auto& cand : packedGenParticles) {
    outGenP->emplace_back(cand.charge(), cand.p4(), cand.vertex(), cand.pdgId(), cand.status(), true);
    outGenP->back().statusFlags() = cand.statusFlags();
    if (cand.lastPrunedRef().isNonnull())
      outGenP->back().setCollisionId(cand.lastPrunedRef()->collisionId());
    //if (cand.motherRef().isNonnull())
    //  outGenP->back().addMother(cand.motherRef());
  }
  iEvent.put(std::move(outGenP));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void pat::GenParticleUnpacker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("packedGenParticles", edm::InputTag("packedGenParticles"))->setComment("packed gen particles collection");
  descriptions.add("unpackedGenParticles", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
using namespace pat;
DEFINE_FWK_MODULE(GenParticleUnpacker);
