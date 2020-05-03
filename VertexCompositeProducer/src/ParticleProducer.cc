// -*- C++ -*-
//
// Package:    ParticleProducer
//
// Class:      ParticleProducer
// 
/**\class ParticleProducer ParticleProducer.cc VertexCompositeAnalysis/VertexCompositeProducer/src/ParticleProducer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Andre Stahl
//
//

#include "VertexCompositeAnalysis/VertexCompositeProducer/interface/ParticleProducer.h"


// constructor
ParticleProducer::ParticleProducer(const edm::ParameterSet& iConfig) :
  fitter_(iConfig, consumesCollector()),
  daughter_(iConfig, iConfig, consumesCollector()) 
{
  produces<pat::GenericParticleCollection>();
}

// dDestructor
ParticleProducer::~ParticleProducer() {
}


//
// Methods
//

// producer method
void ParticleProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  pat::GenericParticleCollection particles;
  // set primary vertex
  fitter_.setVertex(iEvent);
  // extract particles
  if (fitter_.hasNoDaughters()) {
    // consider particle as daughter
    fitter_.addParticles(daughter_, iEvent);
    particles = daughter_.particles();
  }
  else {
     // fit particles
     fitter_.fitAll(iEvent, iSetup);
     particles = fitter_.particles();
  }
  // store particles
  auto output = std::make_unique<pat::GenericParticleCollection>(particles);
  iEvent.put(std::move(output));
  // clear
  fitter_.clear();
  daughter_.clear();
}


void ParticleProducer::beginJob() {
}


void ParticleProducer::endJob() {
}

//define this as a plug-in
#include "FWCore/PluginManager/interface/ModuleDef.h"

DEFINE_FWK_MODULE(ParticleProducer);
