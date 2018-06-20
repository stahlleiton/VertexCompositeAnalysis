// system include files
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <math.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjString.h>
#include <TCanvas.h>
#include <TVector3.h>
#include <TMatrixD.h>
#include <TRandom.h>
#include <TMath.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidateFwd.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/DeDxData.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/MuonReco/interface/MuonSegmentMatch.h"

#include <Math/Functions.h>
#include <Math/SVector.h>
#include <Math/SMatrix.h>


//
// class decleration
//

#define PI 3.1416

using namespace std;

class VertexCompositeNtupleProducer : public edm::EDAnalyzer {
public:
  explicit VertexCompositeNtupleProducer(const edm::ParameterSet&);
  ~VertexCompositeNtupleProducer();

  using MVACollection = std::vector<float>;

private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void fillRECO(const edm::Event&, const edm::EventSetup&) ;
  virtual void endJob() ;

  // ----------member data ---------------------------
    
    TTree* VertexCompositeNtuple;
    
    //options
    bool doGenMatching_;
    bool hasSwap_;
    bool decayInGen_;
    bool twoLayerDecay_;
    bool doMuon_;
    int PID_;
    int PID_dau1_;
    int PID_dau2_;
    
    //cut variables
    double multMax_;
    double multMin_;
    double deltaR_; //deltaR for Gen matching

    //tree branches
    //event info
    int Ntrkoffline;
    float bestvx;
    float bestvy;
    float bestvz;
    
    //Composite candidate info
    float mva;
    float pt;
    float eta;
    float y;
    float mass;
    float VtxProb;
    float dlos;
    float dl;
    float dlerror;
    float agl;
    float vtxChi2;
    float ndf;
    float agl_abs;
    float agl2D;
    float agl2D_abs;
    float dlos2D;
    bool isSwap;
    bool matchGEN;
    int idmom_reco;
    
    //dau candidate info
    float grand_mass;
    float grand_VtxProb;
    float grand_dlos;
    float grand_dl;
    float grand_dlerror;
    float grand_agl;
    float grand_vtxChi2;
    float grand_ndf;
    float grand_agl_abs;
    float grand_agl2D;
    float grand_agl2D_abs;
    float grand_dlos2D;


    //dau info
    float dzos1;
    float dzos2;
    float dxyos1;
    float dxyos2;
    float nhit1;
    float nhit2;
    bool trkquality1;
    bool trkquality2;
    float pt1;
    float pt2;
    float ptErr1;
    float ptErr2;
    float p1;
    float p2;
    float eta1;
    float eta2;
    float phi1;
    float phi2;
    int charge1;
    int charge2;
    float H2dedx1;
    float H2dedx2;
    float T4dedx1;
    float T4dedx2;
    float trkChi1;
    float trkChi2;
    
    //grand-dau info
    float grand_dzos1;
    float grand_dzos2;
    float grand_dxyos1;
    float grand_dxyos2;
    float grand_nhit1;
    float grand_nhit2;
    bool grand_trkquality1;
    bool grand_trkquality2;
    float grand_pt1;
    float grand_pt2;
    float grand_ptErr1;
    float grand_ptErr2;
    float grand_p1;
    float grand_p2;
    float grand_eta1;
    float grand_eta2;
    int grand_charge1;
    int grand_charge2;
    float grand_H2dedx1;
    float grand_H2dedx2;
    float grand_T4dedx1;
    float grand_T4dedx2;
    float grand_trkChi1;
    float grand_trkChi2;
    
    //dau muon info
    float nmatchedst1;
    float nmatchedch1;
    float ntrackerlayer1;
    float npixellayer1;
    float matchedenergy1;
    float nmatchedst2;
    float nmatchedch2;
    float ntrackerlayer2;
    float npixellayer2;
    float matchedenergy2;
    float dx1_seg_;
    float dy1_seg_;
    float dxSig1_seg_;
    float dySig1_seg_;
    float ddxdz1_seg_;
    float ddydz1_seg_;
    float ddxdzSig1_seg_;
    float ddydzSig1_seg_;
    float dx2_seg_;
    float dy2_seg_;
    float dxSig2_seg_;
    float dySig2_seg_;
    float ddxdz2_seg_;
    float ddydz2_seg_;
    float ddxdzSig2_seg_;
    float ddydzSig2_seg_;
    
    //vector for gen match
    vector< vector<double> > *pVect;
    vector<double> *Dvector1;
    vector<double> *Dvector2;
    vector<int> *pVectIDmom;
    
    bool useAnyMVA_;
    bool isSkimMVA_;

    //tokens
    edm::EDGetTokenT<reco::VertexCollection> tok_offlinePV_;
    edm::EDGetTokenT<reco::TrackCollection> tok_generalTrk_;
    edm::EDGetTokenT<reco::VertexCompositeCandidateCollection> recoVertexCompositeCandidateCollection_Token_;
    edm::EDGetTokenT<MVACollection> MVAValues_Token_;

    edm::EDGetTokenT<edm::ValueMap<reco::DeDxData> > Dedx_Token1_;
    edm::EDGetTokenT<edm::ValueMap<reco::DeDxData> > Dedx_Token2_;
    edm::EDGetTokenT<reco::GenParticleCollection> tok_genParticle_;
    edm::EDGetTokenT<reco::MuonCollection> tok_muon_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

VertexCompositeNtupleProducer::VertexCompositeNtupleProducer(const edm::ParameterSet& iConfig)
{
    //options
    twoLayerDecay_ = iConfig.getUntrackedParameter<bool>("twoLayerDecay");
    doGenMatching_ = iConfig.getUntrackedParameter<bool>("doGenMatching");
    hasSwap_ = iConfig.getUntrackedParameter<bool>("hasSwap");
    decayInGen_ = iConfig.getUntrackedParameter<bool>("decayInGen");
    doMuon_ = iConfig.getUntrackedParameter<bool>("doMuon");
    PID_ = iConfig.getUntrackedParameter<int>("PID");
    PID_dau1_ = iConfig.getUntrackedParameter<int>("PID_dau1");
    PID_dau2_ = iConfig.getUntrackedParameter<int>("PID_dau2");
    
    useAnyMVA_ = iConfig.getUntrackedParameter<bool>("useAnyMVA");
    isSkimMVA_ = iConfig.getUntrackedParameter<bool>("isSkimMVA"); 

    //cut variables
    multMax_ = iConfig.getUntrackedParameter<double>("multMax", 0.0);
    multMin_ = iConfig.getUntrackedParameter<double>("multMin", 999.9);
    deltaR_ = iConfig.getUntrackedParameter<double>("deltaR", 0.03);

    //input tokens
    tok_offlinePV_ = consumes<reco::VertexCollection>(iConfig.getUntrackedParameter<edm::InputTag>("VertexCollection"));
    tok_generalTrk_ = consumes<reco::TrackCollection>(iConfig.getUntrackedParameter<edm::InputTag>("TrackCollection"));
    recoVertexCompositeCandidateCollection_Token_ = consumes<reco::VertexCompositeCandidateCollection>(iConfig.getUntrackedParameter<edm::InputTag>("VertexCompositeCollection"));
    MVAValues_Token_ = consumes<MVACollection>(iConfig.getUntrackedParameter<edm::InputTag>("MVACollection"));
    tok_muon_ = consumes<reco::MuonCollection>(iConfig.getUntrackedParameter<edm::InputTag>("MuonCollection"));
    Dedx_Token1_ = consumes<edm::ValueMap<reco::DeDxData> >(edm::InputTag("dedxHarmonic2"));
    Dedx_Token2_ = consumes<edm::ValueMap<reco::DeDxData> >(edm::InputTag("dedxTruncated40"));
    
    tok_genParticle_ = consumes<reco::GenParticleCollection>(edm::InputTag(iConfig.getUntrackedParameter<edm::InputTag>("GenParticleCollection")));
}


VertexCompositeNtupleProducer::~VertexCompositeNtupleProducer()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
VertexCompositeNtupleProducer::analyze(const edm::Event& iEvent, const edm::EventSetup&
iSetup)
{
    using std::vector;
    using namespace edm;

    fillRECO(iEvent,iSetup);
}

void
VertexCompositeNtupleProducer::fillRECO(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    //get collections
    edm::Handle<reco::VertexCollection> vertices;
    iEvent.getByToken(tok_offlinePV_,vertices);
    
    edm::Handle<reco::TrackCollection> tracks;
    iEvent.getByToken(tok_generalTrk_, tracks);
    
    edm::Handle<reco::VertexCompositeCandidateCollection> v0candidates;
    iEvent.getByToken(recoVertexCompositeCandidateCollection_Token_,v0candidates);
    const reco::VertexCompositeCandidateCollection * v0candidates_ = v0candidates.product();
    
    edm::Handle<MVACollection> mvavalues;
    if(useAnyMVA_)
    {
      iEvent.getByToken(MVAValues_Token_,mvavalues);
      assert( (*mvavalues).size() == v0candidates->size() );
    }

    edm::Handle<edm::ValueMap<reco::DeDxData> > dEdxHandle1;
    iEvent.getByToken(Dedx_Token1_, dEdxHandle1);
    
    edm::Handle<edm::ValueMap<reco::DeDxData> > dEdxHandle2;
    iEvent.getByToken(Dedx_Token2_, dEdxHandle2);
    
    //best vertex
    bestvz=-999.9; bestvx=-999.9; bestvy=-999.9;
    double bestvzError=-999.9, bestvxError=-999.9, bestvyError=-999.9;
    const reco::Vertex & vtx = (*vertices)[0];
    bestvz = vtx.z(); bestvx = vtx.x(); bestvy = vtx.y();
    bestvzError = vtx.zError(); bestvxError = vtx.xError(); bestvyError = vtx.yError();
    
    //Ntrkoffline
    Ntrkoffline = 0;
    for(unsigned it=0; it<tracks->size(); ++it){
        
        const reco::Track & trk = (*tracks)[it];
        
        math::XYZPoint bestvtx(bestvx,bestvy,bestvz);
        
        double dzvtx = trk.dz(bestvtx);
        double dxyvtx = trk.dxy(bestvtx);
        double dzerror = sqrt(trk.dzError()*trk.dzError()+bestvzError*bestvzError);
        double dxyerror = sqrt(trk.d0Error()*trk.d0Error()+bestvxError*bestvyError);
        
        if(!trk.quality(reco::TrackBase::highPurity)) continue;
        if(fabs(trk.ptError())/trk.pt()>0.10) continue;
        if(fabs(dzvtx/dzerror) > 3) continue;
        if(fabs(dxyvtx/dxyerror) > 3) continue;
        
        double eta = trk.eta();
        double pt  = trk.pt();
        
        if(fabs(eta)>2.4) continue;
        if(pt<=0.4) continue;
        Ntrkoffline++;
    }

    //Gen info for matching
    if(doGenMatching_)
    {
        pVect = new vector< vector<double>>;
        pVectIDmom = new vector<int>;
        
        edm::Handle<reco::GenParticleCollection> genpars;
        iEvent.getByToken(tok_genParticle_,genpars);
        
        if(!genpars.isValid())
        {
            cout<<"Gen matching cannot be done without Gen collection!!"<<endl;
            return;
        }

        for(unsigned it=0; it<genpars->size(); ++it){
            
            const reco::GenParticle & trk = (*genpars)[it];
            
            int id = trk.pdgId();
            if(fabs(id)!=PID_) continue; //check is target
            if(decayInGen_ && trk.numberOfDaughters()!=2) continue; //check 2-pron decay if target decays in Gen
            
            int idmom_tmp = -77;
            
            if(trk.numberOfMothers()!=0)
            {
                const reco::Candidate * mom = trk.mother();
                idmom_tmp = mom->pdgId();
            }
            
            const reco::Candidate * Dd1 = trk.daughter(0);
            const reco::Candidate * Dd2 = trk.daughter(1);
            
            if(!(fabs(Dd1->pdgId())==PID_dau1_ && fabs(Dd2->pdgId())==PID_dau2_) && !(fabs(Dd2->pdgId())==PID_dau1_ && fabs(Dd1->pdgId())==PID_dau2_)) continue; //check daughter id
                
            Dvector1 = new vector<double>;
            Dvector2 = new vector<double>;
            
            Dvector1->push_back(Dd1->pt());
            Dvector1->push_back(Dd1->eta());
            Dvector1->push_back(Dd1->phi());
            Dvector1->push_back(Dd1->charge());
            Dvector1->push_back(Dd1->mass());
            
            Dvector2->push_back(Dd2->pt());
            Dvector2->push_back(Dd2->eta());
            Dvector2->push_back(Dd2->phi());
            Dvector2->push_back(Dd2->charge());
            Dvector2->push_back(Dd2->mass());
            
            pVect->push_back(*Dvector1);
            pVect->push_back(*Dvector2);
            
            pVectIDmom->push_back(idmom_tmp);
            
            delete Dvector1;
            delete Dvector2;
        }
    }

    //RECO Candidate info
    for(unsigned it=0; it<v0candidates_->size(); ++it){
        
        const reco::VertexCompositeCandidate & trk = (*v0candidates_)[it];
        
        double secvz=-999.9, secvx=-999.9, secvy=-999.9;
        secvz = trk.vz(); secvx = trk.vx(); secvy = trk.vy();

        eta = trk.eta();
        y = trk.rapidity();
        pt = trk.pt();

        if(useAnyMVA_) mva = (*mvavalues)[it];

        double px = trk.px();
        double py = trk.py();
        double pz = trk.pz();
        mass = trk.mass();
        
        const reco::Candidate * d1 = trk.daughter(0);
        const reco::Candidate * d2 = trk.daughter(1);
        
        if(doGenMatching_)
        {
            //Gen match
            matchGEN = false;
            int nGenDau = (int)pVect->size();
            isSwap = false;
            idmom_reco = -77;
            
            for(int i=0;i<nGenDau;i++)
            {
                vector<double> Dvector1 = (*pVect)[i]; //get GEN daugther vector
                if(d1->charge()!=Dvector1.at(3)) continue; //check match charge
                double deltaR = sqrt(pow(d1->eta()-Dvector1.at(1),2)+pow(d1->phi()-Dvector1.at(2),2));
                
                if(deltaR > deltaR_) continue; //check deltaR matching
                if(fabs((d1->pt()-Dvector1.at(0))/d1->pt()) > 0.5) continue; //check deltaPt matching
                double d1massGEN = Dvector1.at(4);
                double d1mass = d1->mass();
                double d2massGEN=0, d2mass=0;
                //check dau2
                if(i%2==0)
                {
                    vector<double> Dvector2 = (*pVect)[i+1]; //get GEN daugther vector for track2
                    if(d2->charge()!=Dvector2.at(3)) continue; //check match charge
                    double deltaR = sqrt(pow(d2->eta()-Dvector2.at(1),2)+pow(d2->phi()-Dvector2.at(2),2));
                    
                    if(deltaR > deltaR_) continue; //check deltaR matching
                    if(fabs((d2->pt()-Dvector2.at(0))/d2->pt()) > 0.5) continue; //check deltaPt matching
                    d2massGEN = Dvector2.at(4);
                    d2mass = d2->mass();
                    
                    matchGEN = true; //matched gen
                }
                if(i%2==1)
                {
                    vector<double> Dvector2 = (*pVect)[i-1]; //get GEN daugther vector for track2
                    if(d2->charge()!=Dvector2.at(3)) continue; //check match charge
                    double deltaR = sqrt(pow(d2->eta()-Dvector2.at(1),2)+pow(d2->phi()-Dvector2.at(2),2));
                    
                    if(deltaR > deltaR_) continue; //check deltaR matching
                    if(fabs((d2->pt()-Dvector2.at(0))/d2->pt()) > 0.5) continue; //check deltaPt matching
                    d2massGEN = Dvector2.at(4);
                    d2mass = d2->mass();
                    
                    matchGEN = true; //matched gen
                }
                
                //check swap
                if(abs(d1massGEN - d1mass)>0.01 || abs(d2massGEN - d2mass)>0.01) isSwap = true;
                
                //check prompt & record mom id
                idmom_reco = pVectIDmom->at(i/2);
            }
        }
        
        double pxd1 = d1->px();
        double pyd1 = d1->py();
        double pzd1 = d1->pz();
        double pxd2 = d2->px();
        double pyd2 = d2->py();
        double pzd2 = d2->pz();
        
        TVector3 dauvec1(pxd1,pyd1,pzd1);
        TVector3 dauvec2(pxd2,pyd2,pzd2);
        
        //pt
        pt1 = d1->pt();
        pt2 = d2->pt();
        
        //momentum
        p1 = d1->p();
        p2 = d2->p();
        
        //eta
        eta1 = d1->eta();
        eta2 = d2->eta();
        
        //phi
        phi1 = d1->phi();
        phi2 = d2->phi();
        
        //charge
        charge1 = d1->charge();
        charge2 = d2->charge();
        
        //vtxChi2
        vtxChi2 = trk.vertexChi2();
        ndf = trk.vertexNdof();
        VtxProb = TMath::Prob(vtxChi2,ndf);
        
        //PAngle
        TVector3 ptosvec(secvx-bestvx,secvy-bestvy,secvz-bestvz);
        TVector3 secvec(px,py,pz);
        
        TVector3 ptosvec2D(secvx-bestvx,secvy-bestvy,0);
        TVector3 secvec2D(px,py,0);
        
        agl = cos(secvec.Angle(ptosvec));
        agl_abs = secvec.Angle(ptosvec);
        
        agl2D = cos(secvec2D.Angle(ptosvec2D));
        agl2D_abs = secvec2D.Angle(ptosvec2D);
        
        //Decay length 3D
        typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;
        typedef ROOT::Math::SVector<double, 3> SVector3;
        typedef ROOT::Math::SVector<double, 6> SVector6;
        
        SMatrixSym3D totalCov = vtx.covariance() + trk.vertexCovariance();
        SVector3 distanceVector(secvx-bestvx,secvy-bestvy,secvz-bestvz);
        
        dl = ROOT::Math::Mag(distanceVector);
        dlerror = sqrt(ROOT::Math::Similarity(totalCov, distanceVector))/dl;
        
        dlos = dl/dlerror;
        
        //Decay length 2D
        SVector6 v1(vtx.covariance(0,0), vtx.covariance(0,1),vtx.covariance(1,1),0,0,0);
        SVector6 v2(trk.vertexCovariance(0,0), trk.vertexCovariance(0,1),trk.vertexCovariance(1,1),0,0,0);
        
        SMatrixSym3D sv1(v1);
        SMatrixSym3D sv2(v2);
        
        SMatrixSym3D totalCov2D = sv1 + sv2;
        SVector3 distanceVector2D(secvx-bestvx,secvy-bestvy,0);
        
        double dl2D = ROOT::Math::Mag(distanceVector2D);
        double dl2Derror = sqrt(ROOT::Math::Similarity(totalCov2D, distanceVector2D))/dl2D;
        
        dlos2D = dl2D/dl2Derror;

        //trk info
        if(!twoLayerDecay_)
        {
            auto dau1 = d1->get<reco::TrackRef>();
            
            //trk quality
            trkquality1 = dau1->quality(reco::TrackBase::highPurity);
            
            //trk dEdx
            H2dedx1 = -999.9;
            
            if(dEdxHandle1.isValid()){
                const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle1.product();
                H2dedx1 = dEdxTrack[dau1].dEdx();
            }
            
            T4dedx1 = -999.9;
            
            if(dEdxHandle2.isValid()){
                const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle2.product();
                T4dedx1 = dEdxTrack[dau1].dEdx();
            }
            
            //track Chi2
            trkChi1 = dau1->normalizedChi2();
            
            //track pT error
            ptErr1 = dau1->ptError();
            
            //vertexCovariance 00-xError 11-y 22-z
            secvz = trk.vz(); secvx = trk.vx(); secvy = trk.vy();
            
            //trkNHits
            nhit1 = dau1->numberOfValidHits();
            
            //DCA
            math::XYZPoint bestvtx(bestvx,bestvy,bestvz);
            
            double dzbest1 = dau1->dz(bestvtx);
            double dxybest1 = dau1->dxy(bestvtx);
            double dzerror1 = sqrt(dau1->dzError()*dau1->dzError()+bestvzError*bestvzError);
            double dxyerror1 = sqrt(dau1->d0Error()*dau1->d0Error()+bestvxError*bestvyError);
            
            dzos1 = dzbest1/dzerror1;
            dxyos1 = dxybest1/dxyerror1;
        }
        
        auto dau2 = d2->get<reco::TrackRef>();
        
        //trk quality
        trkquality2 = dau2->quality(reco::TrackBase::highPurity);
        
        //trk dEdx
        H2dedx2 = -999.9;
        
        if(dEdxHandle1.isValid()){
            const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle1.product();
            H2dedx2 = dEdxTrack[dau2].dEdx();
        }
        
        T4dedx2 = -999.9;
        
        if(dEdxHandle2.isValid()){
            const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle2.product();
            T4dedx2 = dEdxTrack[dau2].dEdx();
        }
        
        //track Chi2
        trkChi2 = dau2->normalizedChi2();
        
        //track pT error
        ptErr2 = dau2->ptError();
        
        //vertexCovariance 00-xError 11-y 22-z
        secvz = trk.vz(); secvx = trk.vx(); secvy = trk.vy();
        
        //trkNHits
        nhit2 = dau2->numberOfValidHits();
        
        //DCA
        math::XYZPoint bestvtx(bestvx,bestvy,bestvz);
        
        double dzbest2 = dau2->dz(bestvtx);
        double dxybest2 = dau2->dxy(bestvtx);
        double dzerror2 = sqrt(dau2->dzError()*dau2->dzError()+bestvzError*bestvzError);
        double dxyerror2 = sqrt(dau2->d0Error()*dau2->d0Error()+bestvxError*bestvyError);
        
        dzos2 = dzbest2/dzerror2;
        dxyos2 = dxybest2/dxyerror2;
        
        if(doMuon_)
        {
            edm::Handle<reco::MuonCollection> theMuonHandle;
            iEvent.getByToken(tok_muon_, theMuonHandle);
            
            nmatchedch1 = -1;
            nmatchedst1 = -1;
            matchedenergy1 = -1;
            nmatchedch2 = -1;
            nmatchedst2 = -1;
            matchedenergy2 = -1;
            
            double x_exp = -999.;
            double y_exp = -999.;
            double xerr_exp = -999.;
            double yerr_exp = -999.;
            double dxdz_exp = -999.;
            double dydz_exp = -999.;
            double dxdzerr_exp = -999.;
            double dydzerr_exp = -999.;
            
            double x_seg = -999.;
            double y_seg = -999.;
            double xerr_seg = -999.;
            double yerr_seg = -999.;
            double dxdz_seg = -999.;
            double dydz_seg = -999.;
            double dxdzerr_seg = -999.;
            double dydzerr_seg = -999.;
            
            double dx_seg = 999.;
            double dy_seg = 999.;
            double dxerr_seg = 999.;
            double dyerr_seg = 999.;
            double dxSig_seg = 999.;
            double dySig_seg = 999.;
            double ddxdz_seg = 999.;
            double ddydz_seg = 999.;
            double ddxdzerr_seg = 999.;
            double ddydzerr_seg = 999.;
            double ddxdzSig_seg = 999.;
            double ddydzSig_seg = 999.;
            
            for( unsigned id = 0; id < theMuonHandle->size(); id++ )
            {
                const reco::Muon& cand = (*theMuonHandle)[id];
                reco::TrackRef trackReftmp = cand.track();
                
                if(trackReftmp.isNull()) continue;
                
                if(fabs(trackReftmp->pt()-pt1)<0.0001 && fabs(trackReftmp->eta()-eta1)<0.001 && fabs(trackReftmp->phi()-phi1)<0.001
                   && trackReftmp->numberOfValidHits() == nhit1 )
                {
                    nmatchedch1 = cand.numberOfMatches();
                    nmatchedst1 = cand.numberOfMatchedStations();
                    
                    reco::MuonEnergy muenergy = cand.calEnergy();
                    matchedenergy1 = muenergy.hadMax;
                    
                    const std::vector<reco::MuonChamberMatch>& muchmatches = cand.matches();
                    
                    for(unsigned int ich=0;ich<muchmatches.size();ich++)
                    {
                        x_exp = muchmatches[ich].x;
                        y_exp = muchmatches[ich].y;
                        xerr_exp = muchmatches[ich].xErr;
                        yerr_exp = muchmatches[ich].yErr;
                        dxdz_exp = muchmatches[ich].dXdZ;
                        dydz_exp = muchmatches[ich].dYdZ;
                        dxdzerr_exp = muchmatches[ich].dXdZErr;
                        dydzerr_exp = muchmatches[ich].dYdZErr;
                        
                        std::vector<reco::MuonSegmentMatch> musegmatches = muchmatches[ich].segmentMatches;
                        //std::cout<<musegmatches.size()<<std::endl;
                        
                        if(!musegmatches.size()) continue;
                        for(unsigned int jseg=0;jseg<musegmatches.size();jseg++)
                        {
                            x_seg = musegmatches[jseg].x;
                            y_seg = musegmatches[jseg].y;
                            xerr_seg = musegmatches[jseg].xErr;
                            yerr_seg = musegmatches[jseg].yErr;
                            dxdz_seg = musegmatches[jseg].dXdZ;
                            dydz_seg = musegmatches[jseg].dYdZ;
                            dxdzerr_seg = musegmatches[jseg].dXdZErr;
                            dydzerr_seg = musegmatches[jseg].dYdZErr;
                            
                            if(sqrt((x_seg-x_exp)*(x_seg-x_exp)+(y_seg-y_exp)*(y_seg-y_exp))<sqrt(dx_seg*dx_seg+dy_seg*dy_seg))
                            {
                                dx_seg = x_seg - x_exp;
                                dy_seg = y_seg - y_exp;
                                dxerr_seg = sqrt(xerr_seg*xerr_seg+xerr_exp*xerr_exp);
                                dyerr_seg = sqrt(yerr_seg*yerr_seg+yerr_exp*yerr_exp);
                                dxSig_seg = dx_seg / dxerr_seg;
                                dySig_seg = dy_seg / dyerr_seg;
                                ddxdz_seg = dxdz_seg - dxdz_exp;
                                ddydz_seg = dydz_seg - dydz_exp;
                                ddxdzerr_seg = sqrt(dxdzerr_seg*dxdzerr_seg+dxdzerr_exp*dxdzerr_exp);
                                ddydzerr_seg = sqrt(dydzerr_seg*dydzerr_seg+dydzerr_exp*dydzerr_exp);
                                ddxdzSig_seg = ddxdz_seg / ddxdzerr_seg;
                                ddydzSig_seg = ddydz_seg / ddydzerr_seg;
                            }
                        }
                        
                        dx1_seg_=dx_seg;
                        dy1_seg_=dy_seg;
                        dxSig1_seg_=dxSig_seg;
                        dySig1_seg_=dySig_seg;
                        ddxdz1_seg_=ddxdz_seg;
                        ddydz1_seg_=ddydz_seg;
                        ddxdzSig1_seg_=ddxdzSig_seg;
                        ddydzSig1_seg_=ddydzSig_seg;
                    }
                }
                
                
                
                if(fabs(trackReftmp->pt()-pt2)<0.0001 && fabs(trackReftmp->eta()-eta2)<0.001 && fabs(trackReftmp->phi()-phi2)<0.001
                   && trackReftmp->numberOfValidHits() == nhit2 )
                {
                    nmatchedch2 = cand.numberOfMatches();
                    nmatchedst2 = cand.numberOfMatchedStations();
                    
                    reco::MuonEnergy muenergy = cand.calEnergy();
                    matchedenergy2 = muenergy.hadMax;
                    
                    const std::vector<reco::MuonChamberMatch>& muchmatches = cand.matches();
                    for(unsigned int ich=0;ich<muchmatches.size();ich++)
                        //                        for(unsigned int ich=0;ich<1;ich++)
                    {
                        x_exp = muchmatches[ich].x;
                        y_exp = muchmatches[ich].y;
                        xerr_exp = muchmatches[ich].xErr;
                        yerr_exp = muchmatches[ich].yErr;
                        dxdz_exp = muchmatches[ich].dXdZ;
                        dydz_exp = muchmatches[ich].dYdZ;
                        dxdzerr_exp = muchmatches[ich].dXdZErr;
                        dydzerr_exp = muchmatches[ich].dYdZErr;
                        
                        std::vector<reco::MuonSegmentMatch> musegmatches = muchmatches[ich].segmentMatches;
                        //std::cout<<musegmatches.size()<<std::endl;
                        
                        if(!musegmatches.size()) continue;
                        for(unsigned int jseg=0;jseg<musegmatches.size();jseg++)
                        {
                            x_seg = musegmatches[jseg].x;
                            y_seg = musegmatches[jseg].y;
                            xerr_seg = musegmatches[jseg].xErr;
                            yerr_seg = musegmatches[jseg].yErr;
                            dxdz_seg = musegmatches[jseg].dXdZ;
                            dydz_seg = musegmatches[jseg].dYdZ;
                            dxdzerr_seg = musegmatches[jseg].dXdZErr;
                            dydzerr_seg = musegmatches[jseg].dYdZErr;
                            
                            if(sqrt((x_seg-x_exp)*(x_seg-x_exp)+(y_seg-y_exp)*(y_seg-y_exp))<sqrt(dx_seg*dx_seg+dy_seg*dy_seg))
                            {
                                dx_seg = x_seg - x_exp;
                                dy_seg = y_seg - y_exp;
                                dxerr_seg = sqrt(xerr_seg*xerr_seg+xerr_exp*xerr_exp);
                                dyerr_seg = sqrt(yerr_seg*yerr_seg+yerr_exp*yerr_exp);
                                dxSig_seg = dx_seg / dxerr_seg;
                                dySig_seg = dy_seg / dyerr_seg;
                                ddxdz_seg = dxdz_seg - dxdz_exp;
                                ddydz_seg = dydz_seg - dydz_exp;
                                ddxdzerr_seg = sqrt(dxdzerr_seg*dxdzerr_seg+dxdzerr_exp*dxdzerr_exp);
                                ddydzerr_seg = sqrt(dydzerr_seg*dydzerr_seg+dydzerr_exp*dydzerr_exp);
                                ddxdzSig_seg = ddxdz_seg / ddxdzerr_seg;
                                ddydzSig_seg = ddydz_seg / ddydzerr_seg;
                            }
                        }
                        
                        dx2_seg_=dx_seg;
                        dy2_seg_=dy_seg;
                        dxSig2_seg_=dxSig_seg;
                        dySig2_seg_=dySig_seg;
                        ddxdz2_seg_=ddxdz_seg;
                        ddydz2_seg_=ddydz_seg;
                        ddxdzSig2_seg_=ddxdzSig_seg;
                        ddydzSig2_seg_=ddydzSig_seg;
                }
            }
        }
        }
        
        if(twoLayerDecay_)
        {
            grand_mass = d1->mass();
            
            const reco::Candidate * gd1 = d1->daughter(0);
            const reco::Candidate * gd2 = d1->daughter(1);
            
            double gpxd1 = gd1->px();
            double gpyd1 = gd1->py();
            double gpzd1 = gd1->pz();
            double gpxd2 = gd2->px();
            double gpyd2 = gd2->py();
            double gpzd2 = gd2->pz();
            
            TVector3 gdauvec1(gpxd1,gpyd1,gpzd1);
            TVector3 gdauvec2(gpxd2,gpyd2,gpzd2);
            
            auto gdau1 = gd1->get<reco::TrackRef>();
            auto gdau2 = gd2->get<reco::TrackRef>();
            
            //trk quality
            
            grand_trkquality1 = gdau1->quality(reco::TrackBase::highPurity);
            grand_trkquality2 = gdau2->quality(reco::TrackBase::highPurity);
            
            //trk dEdx
            grand_H2dedx1 = -999.9;
            grand_H2dedx2 = -999.9;
            
            if(dEdxHandle1.isValid()){
                const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle1.product();
                grand_H2dedx1 = dEdxTrack[gdau1].dEdx();
                grand_H2dedx2 = dEdxTrack[gdau2].dEdx();
            }
            
            grand_T4dedx1 = -999.9;
            grand_T4dedx2 = -999.9;
            
            if(dEdxHandle2.isValid()){
                const edm::ValueMap<reco::DeDxData> dEdxTrack = *dEdxHandle2.product();
                grand_T4dedx1 = dEdxTrack[gdau1].dEdx();
                grand_T4dedx2 = dEdxTrack[gdau2].dEdx();
            }
            
            //track pt
            grand_pt1 = gd1->pt();
            grand_pt2 = gd2->pt();
            
            //track momentum
            grand_p1 = gd1->p();
            grand_p2 = gd2->p();
            
            //track eta
            grand_eta1 = gd1->eta();
            grand_eta2 = gd2->eta();
            
            //track charge
            grand_charge1 = gd1->charge();
            grand_charge2 = gd2->charge();
            
            //track Chi2
            grand_trkChi1 = gdau1->normalizedChi2();
            grand_trkChi2 = gdau2->normalizedChi2();
            
            //track pT error
            grand_ptErr1 = gdau1->ptError();
            grand_ptErr2 = gdau2->ptError();
            
            //vertexCovariance 00-xError 11-y 22-z
            secvz = d1->vz(); secvx = d1->vx(); secvy = d1->vy();
            
            //trkNHits
            grand_nhit1 = gdau1->numberOfValidHits();
            grand_nhit2 = gdau2->numberOfValidHits();
            
            //DCA
            math::XYZPoint bestvtx(bestvx,bestvy,bestvz);
            
            double gdzbest1 = gdau1->dz(bestvtx);
            double gdxybest1 = gdau1->dxy(bestvtx);
            double gdzerror1 = sqrt(gdau1->dzError()*gdau1->dzError()+bestvzError*bestvzError);
            double gdxyerror1 = sqrt(gdau1->d0Error()*gdau1->d0Error()+bestvxError*bestvyError);
            
            grand_dzos1 = gdzbest1/gdzerror1;
            grand_dxyos1 = gdxybest1/gdxyerror1;
            
            double gdzbest2 = gdau2->dz(bestvtx);
            double gdxybest2 = gdau2->dxy(bestvtx);
            double gdzerror2 = sqrt(gdau2->dzError()*gdau2->dzError()+bestvzError*bestvzError);
            double gdxyerror2 = sqrt(gdau2->d0Error()*gdau2->d0Error()+bestvxError*bestvyError);
            
            grand_dzos2 = gdzbest2/gdzerror2;
            grand_dxyos2 = gdxybest2/gdxyerror2;
            
            //vtxChi2
            grand_vtxChi2 = d1->vertexChi2();
            grand_ndf = d1->vertexNdof();
            grand_VtxProb = TMath::Prob(grand_vtxChi2,grand_ndf);
            
            //PAngle
            TVector3 ptosvec(secvx-bestvx,secvy-bestvy,secvz-bestvz);
            TVector3 secvec(d1->px(),d1->py(),d1->pz());
            
            TVector3 ptosvec2D(secvx-bestvx,secvy-bestvy,0);
            TVector3 secvec2D(d1->px(),d1->py(),0);
            
            grand_agl = cos(secvec.Angle(ptosvec));
            grand_agl_abs = secvec.Angle(ptosvec);
            
            grand_agl2D = cos(secvec2D.Angle(ptosvec2D));
            grand_agl2D_abs = secvec2D.Angle(ptosvec2D);
            
            //Decay length 3D
            typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;
            typedef ROOT::Math::SVector<double, 3> SVector3;
            typedef ROOT::Math::SVector<double, 6> SVector6;
            
            SMatrixSym3D totalCov = vtx.covariance() + d1->vertexCovariance();
            SVector3 distanceVector(secvx-bestvx,secvy-bestvy,secvz-bestvz);
            
            grand_dl = ROOT::Math::Mag(distanceVector);
            grand_dlerror = sqrt(ROOT::Math::Similarity(totalCov, distanceVector))/grand_dl;
            
            grand_dlos = grand_dl/grand_dlerror;
            
            //Decay length 2D
            SVector6 v1(vtx.covariance(0,0), vtx.covariance(0,1),vtx.covariance(1,1),0,0,0);
            SVector6 v2(d1->vertexCovariance(0,0), d1->vertexCovariance(0,1),d1->vertexCovariance(1,1),0,0,0);
            
            SMatrixSym3D sv1(v1);
            SMatrixSym3D sv2(v2);
            
            SMatrixSym3D totalCov2D = sv1 + sv2;
            SVector3 distanceVector2D(secvx-bestvx,secvy-bestvy,0);
            
            double gdl2D = ROOT::Math::Mag(distanceVector2D);
            double gdl2Derror = sqrt(ROOT::Math::Similarity(totalCov2D, distanceVector2D))/gdl2D;
            
            grand_dlos2D = gdl2D/gdl2Derror;
        }
        VertexCompositeNtuple->Fill();
    }
}

// ------------ method called once each job just before starting event
//loop  ------------
void
VertexCompositeNtupleProducer::beginJob()
{
    edm::Service<TFileService> fs;
    
    TH1D::SetDefaultSumw2();
    
    if(twoLayerDecay_ && doMuon_)
    {
        cout<<"Muons cannot be coming from two layer decay!! Fix config!!"<<endl; return;
    }
    
    VertexCompositeNtuple = fs->make< TTree>("VertexCompositeNtuple","VertexCompositeNtuple");
    
    VertexCompositeNtuple->Branch("pT",&pt,"pT/F");
    VertexCompositeNtuple->Branch("eta",&eta,"eta/F");
    VertexCompositeNtuple->Branch("y",&y,"y/F");
    VertexCompositeNtuple->Branch("mass",&mass,"mass/F");
     
    if(useAnyMVA_) VertexCompositeNtuple->Branch("mva",&mva,"mva/F");

    if(!isSkimMVA_)  
    {
        //Event info
        VertexCompositeNtuple->Branch("Ntrkoffline",&Ntrkoffline,"Ntrkoffline/I");
        VertexCompositeNtuple->Branch("bestvtxX",&bestvx,"bestvtxX/F");
        VertexCompositeNtuple->Branch("bestvtxY",&bestvy,"bestvtxY/F");
        VertexCompositeNtuple->Branch("bestvtxZ",&bestvz,"bestvtxZ/F");
        
        //Composite candidate info RECO
        VertexCompositeNtuple->Branch("pT",&pt,"pT/F");
        VertexCompositeNtuple->Branch("eta",&eta,"eta/F");
        VertexCompositeNtuple->Branch("y",&y,"y/F");
        VertexCompositeNtuple->Branch("mass",&mass,"mass/F");
        VertexCompositeNtuple->Branch("VtxProb",&VtxProb,"VtxProb/F");
        VertexCompositeNtuple->Branch("VtxChi2",&vtxChi2,"VtxChi2/F");
        VertexCompositeNtuple->Branch("VtxNDF",&ndf,"VtxNDF/F");
        VertexCompositeNtuple->Branch("3DCosPointingAngle",&agl,"3DCosPointingAngleF");
        VertexCompositeNtuple->Branch("3DPointingAngle",&agl_abs,"3DPointingAngle/F");
        VertexCompositeNtuple->Branch("2DCosPointingAngle",&agl2D,"2DCosPointingAngle/F");
        VertexCompositeNtuple->Branch("2DPointingAngle",&agl2D_abs,"2DPointingAngle/F");
        VertexCompositeNtuple->Branch("3DDecayLengthSignificance",&dlos,"3DDecayLengthSignificance/F");
        VertexCompositeNtuple->Branch("3DDecayLength",&dl,"3DDecayLength/F");
        VertexCompositeNtuple->Branch("3DDecayLengthError",&dlerror,"3DDecayLengthError/F");
        VertexCompositeNtuple->Branch("2DDecayLengthSignificance",&dlos2D,"2DDecayLengthSignificance/F");
    
        if(doGenMatching_)
        {
            VertexCompositeNtuple->Branch("isSwap",&isSwap,"isSwap/O");
            VertexCompositeNtuple->Branch("idmom_reco",&idmom_reco,"idmom_reco/I");
            VertexCompositeNtuple->Branch("matchGEN",&matchGEN,"matchGEN/O");
        }
        
        //daugther & grand daugther info
        if(twoLayerDecay_)
        {
            VertexCompositeNtuple->Branch("massDaugther1",&grand_mass,"massDaugther1/F");
            VertexCompositeNtuple->Branch("pTD1",&pt1,"pTD1/F");
            VertexCompositeNtuple->Branch("EtaD1",&eta1,"EtaD1/F");
            VertexCompositeNtuple->Branch("PhiD1",&phi1,"PhiD1/F");
            VertexCompositeNtuple->Branch("VtxProbDaugther1",&grand_VtxProb,"VtxProbDaugther1/F");
            VertexCompositeNtuple->Branch("VtxChi2Daugther1",&grand_vtxChi2,"VtxChi2Daugther1/F");
            VertexCompositeNtuple->Branch("VtxNDFDaugther1",&grand_ndf,"VtxNDFDaugther1/F");
            VertexCompositeNtuple->Branch("3DCosPointingAngleDaugther1",&grand_agl,"3DCosPointingAngleDaugther1/F");
            VertexCompositeNtuple->Branch("3DPointingAngleDaugther1",&grand_agl_abs,"3DPointingAngleDaugther1/F");
            VertexCompositeNtuple->Branch("2DCosPointingAngleDaugther1",&grand_agl2D,"2DCosPointingAngleDaugther1/F");
            VertexCompositeNtuple->Branch("2DPointingAngleDaugther1",&grand_agl2D_abs,"2DPointingAngleDaugther1/F");
            VertexCompositeNtuple->Branch("3DDecayLengthSignificanceDaugther1",&grand_dlos,"3DDecayLengthSignificanceDaugther1/F");
            VertexCompositeNtuple->Branch("3DDecayLengthDaugther1",&grand_dl,"3DDecayLengthDaugther1/F");
            VertexCompositeNtuple->Branch("3DDecayLengthErrorDaugther1",&grand_dlerror,"3DDecayLengthErrorDaugther1/F");
            VertexCompositeNtuple->Branch("2DDecayLengthSignificanceDaugther1",&grand_dlos2D,"2DDecayLengthSignificanceDaugther1/F");
            VertexCompositeNtuple->Branch("zDCASignificanceDaugther2",&dzos2,"zDCASignificanceDaugther2/F");
            VertexCompositeNtuple->Branch("xyDCASignificanceDaugther2",&dxyos2,"xyDCASignificanceDaugther2/F");
            VertexCompositeNtuple->Branch("NHitD2",&nhit2,"NHitD2/F");
            VertexCompositeNtuple->Branch("HighPurityDaugther2",&trkquality2,"HighPurityDaugther2/O");
            VertexCompositeNtuple->Branch("pTD2",&pt2,"pTD2/F");
            VertexCompositeNtuple->Branch("pTerrD2",&ptErr2,"pTerrD2/F");
            VertexCompositeNtuple->Branch("pD2",&p2,"pD2/F");
            VertexCompositeNtuple->Branch("EtaD2",&eta2,"EtaD2/F");
            VertexCompositeNtuple->Branch("PhiD2",&phi2,"PhiD2/F");
            VertexCompositeNtuple->Branch("chargeD2",&charge2,"chargeD2/I");
            VertexCompositeNtuple->Branch("dedxHarmonic2D2",&H2dedx2,"dedxHarmonic2D2/F");
            VertexCompositeNtuple->Branch("dedxTruncated40Daugther2",&T4dedx2,"dedxTruncated40Daugther2/F");
            VertexCompositeNtuple->Branch("normalizedChi2Daugther2",&trkChi2,"normalizedChi2Daugther2/F");
            VertexCompositeNtuple->Branch("zDCASignificanceGrandDaugther1",&grand_dzos1,"zDCASignificanceGrandDaugther1/F");
            VertexCompositeNtuple->Branch("zDCASignificanceGrandDaugther2",&grand_dzos2,"zDCASignificanceGrandDaugther2/F");
            VertexCompositeNtuple->Branch("xyDCASignificanceGrandDaugther1",&grand_dxyos1,"xyDCASignificanceGrandDaugther1/F");
            VertexCompositeNtuple->Branch("xyDCASignificanceGrandDaugther2",&grand_dxyos2,"xyDCASignificanceGrandDaugther2/F");
            VertexCompositeNtuple->Branch("NHitGrandD1",&grand_nhit1,"NHitGrandD1/F");
            VertexCompositeNtuple->Branch("NHitGrandD2",&grand_nhit2,"NHitGrandD2/F");
            VertexCompositeNtuple->Branch("HighPurityGrandDaugther1",&grand_trkquality1,"HighPurityGrandDaugther1/O");
            VertexCompositeNtuple->Branch("HighPurityGrandDaugther2",&grand_trkquality2,"HighPurityGrandDaugther2/O");
            VertexCompositeNtuple->Branch("pTGrandD1",&grand_pt1,"pTGrandD1/F");
            VertexCompositeNtuple->Branch("pTGrandD2",&grand_pt2,"pTGrandD2/F");
            VertexCompositeNtuple->Branch("pTerrGrandD1",&grand_ptErr1,"pTerrGrandD1/F");
            VertexCompositeNtuple->Branch("pTerrGrandD2",&grand_ptErr2,"pTerrGrandD2/F");
            VertexCompositeNtuple->Branch("pGrandD1",&grand_p1,"pGrandD1/F");
            VertexCompositeNtuple->Branch("pGrandD2",&grand_p2,"pGrandD2/F");
            VertexCompositeNtuple->Branch("EtaGrandD1",&grand_eta1,"EtaGrandD1/F");
            VertexCompositeNtuple->Branch("EtaGrandD2",&grand_eta2,"EtaGrandD2/F");
            VertexCompositeNtuple->Branch("chargeGrandD1",&grand_charge1,"chargeGrandD1/I");
            VertexCompositeNtuple->Branch("chargeGrandD2",&grand_charge2,"chargeGrandD2/I");
            VertexCompositeNtuple->Branch("dedxHarmonic2GrandD1",&grand_H2dedx1,"dedxHarmonic2GrandD1/F");
            VertexCompositeNtuple->Branch("dedxHarmonic2GrandD2",&grand_H2dedx2,"dedxHarmonic2GrandD2/F");
            VertexCompositeNtuple->Branch("dedxTruncated40GrandDaugther1",&grand_T4dedx1,"dedxTruncated40GrandDaugther1/F");
            VertexCompositeNtuple->Branch("dedxTruncated40GrandDaugther2",&grand_T4dedx2,"dedxTruncated40GrandDaugther2/F");
            VertexCompositeNtuple->Branch("normalizedChi2GrandDaugther1",&grand_trkChi1,"normalizedChi2GrandDaugther1/F");
            VertexCompositeNtuple->Branch("normalizedChi2GrandDaugther2",&grand_trkChi2,"normalizedChi2GrandDaugther2/F");
        }
        else
        {
            VertexCompositeNtuple->Branch("zDCASignificanceDaugther1",&dzos1,"zDCASignificanceDaugther1/F");
            VertexCompositeNtuple->Branch("xyDCASignificanceDaugther1",&dxyos1,"xyDCASignificanceDaugther1/F");
            VertexCompositeNtuple->Branch("NHitD1",&nhit1,"NHitD1/F");
            VertexCompositeNtuple->Branch("HighPurityDaugther1",&trkquality1,"HighPurityDaugther1/O");
            VertexCompositeNtuple->Branch("pTD1",&pt1,"pTD1/F");
            VertexCompositeNtuple->Branch("pTerrD1",&ptErr1,"pTerrD1/F");
            VertexCompositeNtuple->Branch("pD1",&p1,"pD1/F");
            VertexCompositeNtuple->Branch("EtaD1",&eta1,"EtaD1/F");
            VertexCompositeNtuple->Branch("PhiD1",&eta1,"PhiD1/F");
            VertexCompositeNtuple->Branch("chargeD1",&charge1,"chargeD1/I");
            VertexCompositeNtuple->Branch("dedxHarmonic2D1",&H2dedx1,"dedxHarmonic2D1/F");
            VertexCompositeNtuple->Branch("dedxTruncated40Daugther1",&T4dedx1,"dedxTruncated40Daugther1/F");
            VertexCompositeNtuple->Branch("normalizedChi2Daugther1",&trkChi1,"normalizedChi2Daugther1/F");
            VertexCompositeNtuple->Branch("zDCASignificanceDaugther2",&dzos2,"zDCASignificanceDaugther2/F");
            VertexCompositeNtuple->Branch("xyDCASignificanceDaugther2",&dxyos2,"xyDCASignificanceDaugther2/F");
            VertexCompositeNtuple->Branch("NHitD2",&nhit2,"NHitD2/F");
            VertexCompositeNtuple->Branch("HighPurityDaugther2",&trkquality2,"HighPurityDaugther2/O");
            VertexCompositeNtuple->Branch("pTD2",&pt2,"pTD2/F");
            VertexCompositeNtuple->Branch("pTerrD2",&ptErr2,"pTerrD2/F");
            VertexCompositeNtuple->Branch("pD2",&p2,"pD2/F");
            VertexCompositeNtuple->Branch("EtaD2",&eta2,"EtaD2/F");
            VertexCompositeNtuple->Branch("PhiD2",&eta2,"PhiD2/F");
            VertexCompositeNtuple->Branch("chargeD2",&charge2,"chargeD2/I");
            VertexCompositeNtuple->Branch("dedxHarmonic2D2",&H2dedx2,"dedxHarmonic2D2/F");
            VertexCompositeNtuple->Branch("dedxTruncated40Daugther2",&T4dedx2,"dedxTruncated40Daugther2/F");
            VertexCompositeNtuple->Branch("normalizedChi2Daugther2",&trkChi2,"normalizedChi2Daugther2/F");
        }
        
        if(doMuon_)
        {
            VertexCompositeNtuple->Branch("nMatchedChamberD1",&nmatchedch1,"nMatchedChamberD1/F");
            VertexCompositeNtuple->Branch("nMatchedStationD1",&nmatchedst1,"nMatchedStationD1/F");
            VertexCompositeNtuple->Branch("EnergyDepositionD1",&nmatchedst1,"EnergyDepositionD1/F");
            VertexCompositeNtuple->Branch("nMatchedChamberD2",&nmatchedch2,"nMatchedChamberD2/F");
            VertexCompositeNtuple->Branch("nMatchedStationD2",&nmatchedst2,"nMatchedStationD2/F");
            VertexCompositeNtuple->Branch("EnergyDepositionD2",&nmatchedst2,"EnergyDepositionD2/F");
            VertexCompositeNtuple->Branch("dx1_seg",        &dx1_seg_, "dx1_seg/F");
            VertexCompositeNtuple->Branch("dy1_seg",        &dy1_seg_, "dy1_seg/F");
            VertexCompositeNtuple->Branch("dxSig1_seg",     &dxSig1_seg_, "dxSig1_seg/F");
            VertexCompositeNtuple->Branch("dySig1_seg",     &dySig1_seg_, "dySig1_seg/F");
            VertexCompositeNtuple->Branch("ddxdz1_seg",     &ddxdz1_seg_, "ddxdz1_seg/F");
            VertexCompositeNtuple->Branch("ddydz1_seg",     &ddydz1_seg_, "ddydz1_seg/F");
            VertexCompositeNtuple->Branch("ddxdzSig1_seg",  &ddxdzSig1_seg_, "ddxdzSig1_seg/F");
            VertexCompositeNtuple->Branch("ddydzSig1_seg",  &ddydzSig1_seg_, "ddydzSig1_seg/F");
            VertexCompositeNtuple->Branch("dx2_seg",        &dx2_seg_, "dx2_seg/F");
            VertexCompositeNtuple->Branch("dy2_seg",        &dy2_seg_, "dy2_seg/F");
            VertexCompositeNtuple->Branch("dxSig2_seg",     &dxSig2_seg_, "dxSig2_seg/F");
            VertexCompositeNtuple->Branch("dySig2_seg",     &dySig2_seg_, "dySig2_seg/F");
            VertexCompositeNtuple->Branch("ddxdz2_seg",     &ddxdz2_seg_, "ddxdz2_seg/F");
            VertexCompositeNtuple->Branch("ddydz2_seg",     &ddydz2_seg_, "ddydz2_seg/F");
            VertexCompositeNtuple->Branch("ddxdzSig2_seg",  &ddxdzSig2_seg_, "ddxdzSig2_seg/F");
            VertexCompositeNtuple->Branch("ddydzSig2_seg",  &ddydzSig2_seg_, "ddydzSig2_seg/F");
        }
    }
}

// ------------ method called once each job just after ending the event
//loop  ------------
void 
VertexCompositeNtupleProducer::endJob() {
    
}

//define this as a plug-in
DEFINE_FWK_MODULE(VertexCompositeNtupleProducer);






