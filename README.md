# VertexCompositeAnalysis

Example of setting up and running gamma+gamma to dikaon tree

cmsrel CMSSW_15_1_0_patch3

cd CMSSW_15_1_0_patch3/src

cmsenv

git cms-merge-topic stahlleiton:ParticleAnalyzer_CMSSW_15_1_X

git clone -b ParticleFitter_15_1_X git@github.com:stahlleiton/VertexCompositeAnalysis.git

scram b -j8

cd VertexCompositeAnalysis/VertexCompositeProducer/test

cmsRun VCTree_PbPb2025_UPCDiKa_UPCReco_cfg.py
