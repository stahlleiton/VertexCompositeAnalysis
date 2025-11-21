# VertexCompositeAnalysis

Example of setting up and running gamma+gamma to dikaon tree

cmsrel CMSSW_15_1_0_patch3

cd CMSSW_15_1_0_patch3/src

cmsenv

git cms-merge-topic stahlleiton:ParticleAnalyzer_CMSSW_15_1_X

git cms-addpkg DataFormats/PatCandidates ; git fetch git@github.com:stahlleiton/cmssw.git ParticleAnalyzer_CMSSW_15_1_X ; git cherry-pick ce0e4ae41f60f84dc814aa94ff39e89419da18ef

scram b -j8

cd VertexCompositeAnalysis/VertexCompositeProducer/test

cmsRun VCTree_PbPb2025_UPCDiKa_UPCReco_cfg.py
