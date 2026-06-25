from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import config
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

config = config()
config.section_('General')
date = '2026_04_10'
config.General.workArea = 'crab_projects/'+date+'/HIZeroBias/'
config.General.transferOutputs = True
config.General.transferLogs = False
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../VCTree_PbPb2024_HIZeroBias_UPCReco_cfg.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 720
config.JobType.numCores = 1
config.section_('Data')
config.Data.outLFNDirBase = '/store/group/phys_heavyions/anstahll/CERN/PbPb2024/ParticleAnalyzer/' + date
config.Data.publication = False
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
config.Site.ignoreGlobalBlacklist = True # to fix issue of missing blocks
#config.Site.blacklist = ['T2_EE_Estonia']

def submit(config, dryrun):
    try:
        crabCommand('submit', config = config, dryrun=dryrun)
    except HTTPException as hte:
        print("Failed submitting task: %s" % (hte.headers))
    except ClientException as cle:
        print("Failed submitting task: %s" % (cle))

config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 50
config.Data.inputDBS = 'global'

## Submit the HIZeroBias PDs
config.Data.lumiMask = '/eos/user/c/cmsdqm/www/CAF/certification/Collisions24HI/Cert_Collisions2024_HI_387853_388784_Muon.json'
for i in range(0, 3, 1):
    config.General.requestName = f'ParticleAnalyzer_HIZeroBias{i}_HIRun2024A_'+date
    config.Data.inputDataset = f'/HIZeroBias{i}/HIRun2024A-PromptReco-v1/MINIAOD'
    config.Data.outputDatasetTag = config.General.requestName
    submit(config = config, dryrun=False)
    config.General.requestName = f'ParticleAnalyzer_HIZeroBias{i}_HIRun2024B_v1_'+date
    config.Data.inputDataset = f'/HIZeroBias{i}/HIRun2024B-PromptReco-v1/MINIAOD'
    config.Data.outputDatasetTag = config.General.requestName
    submit(config = config, dryrun=False)
    config.General.requestName = f'ParticleAnalyzer_HIZeroBias{i}_HIRun2024B_v2_'+date
    config.Data.inputDataset = f'/HIZeroBias{i}/HIRun2024B-PromptReco-v2/MINIAOD'
    config.Data.outputDatasetTag = config.General.requestName
    submit(config = config, dryrun=False)
