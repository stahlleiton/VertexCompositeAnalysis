from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import config
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

config = config()
config.section_('General')
date = '2026_04_10'
config.General.workArea = 'crab_projects/'+date
config.General.transferOutputs = True
config.General.transferLogs = False
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../VCTree_PbPb2025_HIEmptyBX_UPCReco_cfg.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 720
config.JobType.numCores = 1
config.section_('Data')
config.Data.outLFNDirBase = f'/store/group/phys_heavyions/{REPLACE_WITH_USERNAME}/CERN/PbPb2025/ParticleAnalyzer/' + date
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

## Submit the HIEmptyBX PDs
config.Data.lumiMask = '/eos/user/c/cmsdqm/www/CAF/certification/Collisions25HI/Cert_Collisions2025_HI_399465_400426_Muon.json'
config.General.requestName = f'ParticleAnalyzer_HIEmptyBX_HIRun2025A_'+date
config.Data.inputDataset = f'/HIEmptyBX/HIRun2025A-PromptReco-v1/MINIAOD'
config.Data.outputDatasetTag = config.General.requestName
submit(config = config, dryrun=False)
