from datetime import datetime
from CRABClient.UserUtilities import config

config = config()

## User Input ##############################################################################
pset_name       = 'PbPbSkimAndTree2024_DiMu_ParticleAnalyzer_cfg.py'
request_name    = 'UPCDiMu_UPCReco'
channel         = 'JPsitoDiMu'
request_name    += '_%s' % datetime.now().strftime('%y%m%d_%H%M%S')

#input_filelist  = '/afs/cern.ch/user/d/dyano/private/JPsi_Run2024/CMSSW_14_1_4_patch5/src/VertexCompositeAnalysis/VertexCompositeProducer/test/filelist/test.txt'

output_pd       = 'PbPb2024'
output_dir      = '/store/group/phys_heavyions/dyano/Run2024/JPsi_data/HI3/%s' %  request_name

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
#config.JobType.inputFiles = ['emap_2023_newZDC_v3.txt', 'CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run3v1302x04_offline_374289.db']
#config.JobType.numCores = 1
#config.JobType.maxMemoryMB = 4000
#config.JobType.maxJobRuntimeMin = 2000
# config.JobType.allowUndistributedCMSSW = True

## Data #####################################################################################
config.section_('Data')
#config.Data.inputDBS = 'phys03'
#* Using Dataset from DAS *******************************
# config.Data.inputDataset = input_dataset
# config.Data.splitting = 'FileBased'
# config.Data.unitsPerJob = 1
#* Using FileList ***************************************
#config.Data.userInputFiles = open(input_filelist).readlines()
config.Data.inputDataset = '/HIForward3/HIRun2024A-PromptReco-v1/AOD'
config.Data.splitting = 'LumiBased'
config.Data.lumiMask = '/eos/cms/store/group/phys_heavyions/sayan/HIN_run3_pseudo_JSON/HIPhysicsRawPrime_2024/Golden_387853_continue_L1DeadTimeCut10percent.txt'
config.Data.unitsPerJob = 20
#config.Data.totalUnits = 100
config.Data.publication = False
#********************************************************

# config.Data.outputPrimaryDataset = output_pd
# config.Data.outputDatasetTag = config.General.requestName
config.Data.outputDatasetTag = channel
config.Data.outLFNDirBase = output_dir

## Site #####################################################################################
config.section_('Site')
config.Site.whitelist = ['T2_US_Vanderbilt']
config.Site.storageSite = 'T2_CH_CERN'

#############################################################################################
print('OutputDirectory: '+config.Data.outLFNDirBase)
print(f"PSet Name: {pset_name}")
#print(f"Input Files: {config.Data.userInputFiles}")
print(f"Output Directory: {output_dir}")
