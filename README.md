# VertexCompositeAnalysis

Example of setting up and running gamma+gamma to dikaon tree

cmsrel CMSSW_15_1_1

cd CMSSW_15_1_1/src

cmsenv

git cms-addpkg DataFormats/PatCandidates ; git fetch git@github.com:stahlleiton/cmssw.git ParticleAnalyzer_CMSSW_15_1_X ; git cherry-pick ce0e4ae41f60f84dc814aa94ff39e89419da18ef

git clone -b ParticleFitter_15_1_X git@github.com:stahlleiton/VertexCompositeAnalysis.git

scram b -j8

cd VertexCompositeAnalysis/VertexCompositeProducer/test

cmsRun VCTree_PbPb2025_UPCDiKa_UPCReco_cfg.py
