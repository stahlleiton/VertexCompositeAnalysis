from datetime import datetime
from CRABClient.UserUtilities import config

config = config()

## User Input ##############################################################################
pset_name       = 'PbPbSkimAndTree2024_DiMu_MC_ParticleAnalyzer_cfg.py'

request_name    = 'VCTree_STARlight_UPCDiMu_UPCReco_mc'
channel         = 'CohJpsiToMuMu'
request_name    += '_%s' % datetime.now().strftime('%y%m%d_%H%M%S')

input_filelist	= '/afs/cern.ch/user/d/dyano/private/CMSSW_14_1_4_patch5/src/VertexCompositeAnalysis/VertexCompositeProducer/test/filelist/OORun_mc_jpsi.txt'
# input_dataset	= ''

output_pd       = 'STARlight'
output_dir      = '/store/group/phys_heavyions/dyano/OORun/Jpsitodimu/MonteCarlo/%s' %  request_name

## General #################################################################################
config.section_('General')
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False
config.General.requestName = request_name

## JobType ##################################################################################
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = pset_name
#config.JobType.scriptExe = 'submitScript.sh'
config.JobType.inputFiles = ['emap_2023_newZDC_v3.txt', 'CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run3v1302x04_offline_374289.db']
config.JobType.numCores = 1
# config.JobType.maxMemoryMB = 4000
# config.JobType.maxJobRuntimeMin = 1000

## Data #####################################################################################
config.section_('Data')
config.Data.inputDBS = 'phys03'
#* Using Dataset from DAS *******************************
# config.Data.inputDataset = input_dataset
# config.Data.splitting = 'FileBased'
# config.Data.unitsPerJob = 1
#* Using FileList ***************************************
config.Data.userInputFiles = open(input_filelist).readlines() 
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
#********************************************************

config.Data.outputPrimaryDataset = output_pd
# config.Data.outputDatasetTag = config.General.requestName
config.Data.outputDatasetTag = channel
config.Data.outLFNDirBase = output_dir

## Site #####################################################################################
config.section_('Site')
config.Site.whitelist = ['T2_CH_CERN']
config.Site.storageSite = 'T2_CH_CERN'

#############################################################################################
print('OutputDirectory: '+config.Data.outLFNDirBase)

