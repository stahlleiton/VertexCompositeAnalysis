from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import config
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

config = config()
config.section_('General')
date = '2026_04_10'
config.General.workArea = 'crab_projects/'+date+'/MC/'
config.General.transferOutputs = True
config.General.transferLogs = False
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '../VCTree_PbPb2023_UPCDiKa_UPCReco_mc_cfg.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 720
config.JobType.numCores = 1
config.section_('Data')
config.Data.outLFNDirBase = '/store/group/phys_heavyions/anstahll/CERN/PbPb2023/ParticleAnalyzer/' + date+'/MC/'
config.Data.publication = False
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'

def submit(config, dryrun):
    try:
        crabCommand('submit', config = config, dryrun=dryrun)
    except HTTPException as hte:
        print("Failed submitting task: %s" % (hte.headers))
    except ClientException as cle:
        print("Failed submitting task: %s" % (cle))

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 50
config.Data.inputDBS = 'phys03'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 720

dataMap = {}
dataMap["coh_phi_dika_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-coh_phi_dika_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["coh_phi_dika_extnuclearpar_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-coh_phi_dika_extnuclearpar_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["coh_phi_dika_nuclearpar_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-coh_phi_dika_nuclearpar_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["coh_phi_direct_dika_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-coh_phi_direct_dika_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["coh_rho_dipi_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-coh_rho_dipi_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["coh_rho_direct_dipi_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-coh_rho_direct_dipi_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["incoh_phi_dika_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-incoh_phi_dika_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"
dataMap["incoh_rho_dipi_STARLIGHT"] = "/STARLIGHT_5p36TeV_2023Run3/anstahll-incoh_rho_dipi_STARLIGHT_5p36TeV_2023Run3_RECO_2026_04_14-5d2f75b4b1f4ab91f2b93e775a64a44a/USER"

## Submit PDs
for key, val in dataMap.items():
    config.General.requestName = f'PartAna_{key}_5p36TeV_2023Run3_'+date
    config.Data.inputDataset = val
    config.Data.outputDatasetTag = config.General.requestName
    submit(config = config, dryrun=False)
