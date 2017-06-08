// -*- C++ -*-
//
// Package:    ExoAnalysis/cmsWRextension
// Class:      cmsWRextension
// 
/**\class cmsWRextension cmsWRextension.cc ExoAnalysis/cmsWRextension/plugins/cmsWRextension.cc

 Description: [one line class summary]
Accesses GenParticle collection to plot various kinematic variables associated with the event

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrew Christopher Evans
//         Created:  Thu, 30 Mar 2017 09:00:20 GMT
//
//

// system include files
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"



//ROOT includes
#include "TH1D.h"
#include "TTree.h"
//local includes
#include "eventBits.h"
#include "tools.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class cmsWRextension : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit cmsWRextension(const edm::ParameterSet&);
      ~cmsWRextension();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      // ----------member functions---------------------
      void selectMuons(const edm::Event&, eventBits&);
      bool selectGenParticles(const edm::Event&, eventBits&);
      void makeGenPlots();
      void makePlots();
      // ----------member data ---------------------------
      std::vector<eventBits> m_events;
      edm::EDGetToken m_genParticleToken;
      edm::EDGetToken m_genJetsToken;
      edm::EDGetToken m_recoMuonToken;
      bool m_wantHardProcessMuons;
      bool m_doGen;
      TTree* hardProcessKinematics;
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
cmsWRextension::cmsWRextension(const edm::ParameterSet& iConfig):
   m_genParticleToken (consumes<std::vector<reco::GenParticle>> (iConfig.getParameter<edm::InputTag>("genParticles"))),
   m_genJetsToken (consumes<std::vector<reco::GenJet>> (iConfig.getParameter<edm::InputTag>("genJets"))),
   m_recoMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("recoMuons"))),
   m_wantHardProcessMuons (iConfig.getUntrackedParameter<bool>("wantHardProcessMuons",true)),
   m_doGen (iConfig.getUntrackedParameter<bool>("doGen",false))

{
   //now do what ever initialization is needed
   usesResource("TFileService");
   
}


cmsWRextension::~cmsWRextension()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//
// ------------ method called for each event  ------------
void cmsWRextension::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   eventBits myEvent;

   selectMuons(iEvent, myEvent);
   bool genpass=false;
   if (m_doGen) {
     genpass=selectGenParticles(iEvent, myEvent);
     if (!genpass) return;
   }

   std::cout << "NOTE! SAVING EVENT DATA" << std::endl;
   myEvent.event = iEvent.id().event();
   m_events.push_back(myEvent);
}
  
bool cmsWRextension::selectGenParticles(const edm::Event& iEvent, eventBits& myEvent)
{
   using namespace edm;
  
   float partonJetMatchDR = .1;
 
   Handle<std::vector<reco::GenParticle>> genParticles;
   iEvent.getByToken(m_genParticleToken, genParticles);

   Handle<std::vector<reco::GenJet>> genJets;
   iEvent.getByToken(m_genJetsToken, genJets);

   //LOOP OVER GEN PARTICLES
   for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
     if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
     if((iParticle->isHardProcess() && iParticle->status() == 23) && iParticle->pdgId() <= 6 && iParticle->pdgId() >= -6) myEvent.outgoingPartons.push_back((*iParticle));
     if(((iParticle->isLastCopy()&&!m_wantHardProcessMuons) || iParticle->isHardProcess()) && (iParticle->pdgId() == 13 || iParticle->pdgId() == -13)) myEvent.outgoingMuons.push_back((*iParticle));
   }
   //CHECK THAT THE EVENT MAKES SENSE
   if (myEvent.outgoingPartons.size() < 2 || myEvent.outgoingMuons.size() < 2) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT FIND AT LEAST 2 PARTONS OR 2 MUONS"<< std::endl;
     return false;
   }
   //SORT GEN MUONS AND PARTONS BY ET
   std::sort(myEvent.outgoingPartons.begin(),myEvent.outgoingPartons.end(),::wrTools::compareEt);
   std::sort(myEvent.outgoingMuons.begin(),myEvent.outgoingMuons.end(),::wrTools::compareEt);

   myEvent.setHighestEtParton(0);
   myEvent.setSecondHighestEtParton(1);

   myEvent.setHighestEtMuon(0);
   myEvent.setSecondHighestEtMuon(1);

   //NOW THAT THE GEN MUONS AND PARTONS ARE SORTED OUT, WE'LL MATCH A GENJET TO EACH PARTON
   //FIRST WE'LL GET THE GENJETS THAT HAVE AT LEAST 10 GEV ET
   for (std::vector<reco::GenJet>::const_iterator iJet = genJets->begin(); iJet != genJets->end(); iJet++) {
     myEvent.genJets.push_back(*iJet);
   }
   if( myEvent.genJets.size() < 2 ) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT FIND AT LEAST 2 JETS"<< std::endl;
     return false;
   }
   //NOW WE'LL CHECK IF IT PASSES SOME BASIC GEN LEVEL CUTS
   if(!myEvent.passesGenCuts()) {
     std::cout << "ERROR! SKIPPING EVENT, LEADING PARTONS AND MUONS NOT OVER 20 GEV"<< std::endl;
     return false;
   }

   //HERE WE COMPARE OUR EVENTS
   bool foundFirst = false;
   bool foundSecond = false;
   float bestFirstJetMatch = 10.0;
   float bestSecondJetMatch = 10.0;
   int index = 0;
   for (std::vector<reco::GenJet>::const_iterator iJet = myEvent.genJets.begin(); iJet != myEvent.genJets.end(); iJet++) {
     if(bestFirstJetMatch > deltaR2(*iJet,*(myEvent.getHighestEtParton()))) {
       bestFirstJetMatch = deltaR2(*iJet,*(myEvent.getHighestEtParton()));
       if(bestFirstJetMatch <= partonJetMatchDR) {
         myEvent.setFirstPartonGenJet(index);
         foundFirst = true;
       }
     }
     if(bestSecondJetMatch > deltaR2(*iJet,*(myEvent.getSecondHighestEtParton()))) {
      bestSecondJetMatch = deltaR2(*iJet,*(myEvent.getSecondHighestEtParton()));
      if(bestSecondJetMatch <= partonJetMatchDR) {
        myEvent.setSecondPartonGenJet(index);
        foundSecond = true;
      }
     }  
     index++;
   }
   if (!foundFirst || !foundSecond) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT MATCH EITHER PARTONS WITH A JET WITHIN: "<< partonJetMatchDR<<" dR"<<std::endl;
     return false;
   }
   return true;
}

void cmsWRextension::selectMuons(const edm::Event& iEvent, eventBits& myEvent)
{
   using namespace edm;
   
   Handle<std::vector<pat::Muon>> recoMuons;
   iEvent.getByToken(m_recoMuonToken, recoMuons);
   
   for (std::vector<pat::Muon>::const_iterator iParticle = recoMuons->begin(); iParticle != recoMuons->end(); iParticle++) {
     myEvent.selectedMuons.push_back(*iParticle);
   }


}
void cmsWRextension::makePlots()
{
   if(!(m_events.size() > 0)) return;
   std::cout << "processing: " << m_events.size() <<" events"<< std::endl;
   if(m_doGen)
     makeGenPlots();
}

void cmsWRextension::makeGenPlots()
{
  edm::Service<TFileService> fs;
  TH1D* parton1Et = fs->make<TH1D>("parton1Et", "Parton 1 Et", 100, 0.0, 2000);
  TH1D* parton2Et = fs->make<TH1D>("parton2Et", "Parton 2 Et", 100, 0.0, 2000);
  TH1D* muonHighestEt = fs->make<TH1D>("muonHighestEt", "Highest Et Muon Et", 100, 0.0, 2000);
  TH1D* muonSecondHighestEt = fs->make<TH1D>("muonSecondHighestEt", "Second Highest Et Muon Et", 100, 0.0, 2000);

  TH1D* parton1Eta = fs->make<TH1D>("parton1Eta", "Parton 1 Eta", 100, -4.0, 4.0);
  TH1D* parton2Eta = fs->make<TH1D>("parton2Eta", "Parton 2 Eta", 100, -4.0, 4.0);
  TH1D* muonHighestEtEta = fs->make<TH1D>("muonHighestEtEta", "Highest Et muon eta",                 100, -4.0, 4.0);
  TH1D* muonSecondHighestEtEta = fs->make<TH1D>("muonSecondHighestEtEta", "2nd Highest Et muon eta", 100, -4.0, 4.0);

  TH1D* dRparton1parton2 = fs->make<TH1D>("dRparton1parton2", "deltaR between partons",       100, 0.0, 12.0); 
  TH1D* dRmuon1muon2 = fs->make<TH1D>("dRmuon1muon2", "deltaR between muons",                 100, 0.0, 12.0); 
  TH1D* dRparton1muon2 = fs->make<TH1D>("dRparton1muon2", "deltaR between parton1 and muon2", 100, 0.0, 12.0); 
  TH1D* dRparton1muon1 = fs->make<TH1D>("dRparton1muon1", "deltaR between parton1 and muon1", 100, 0.0, 12.0); 
  TH1D* dRparton2muon2 = fs->make<TH1D>("dRparton2muon2", "deltaR between parton2 and muon2", 100, 0.0, 12.0); 
  TH1D* dRparton2muon1 = fs->make<TH1D>("dRparton2muon1", "deltaR between parton2 and muon1", 100, 0.0, 12.0); 

  TH1D* firstPartonJetEtTotal =      fs->make<TH1D>("firstPartonJetEtTotal", "Total Jet Et for Leading Parton",            100,0.0,2000);
  TH1D* secondPartonJetEtTotal =     fs->make<TH1D>("secondPartonJetEtTotal","Total Jet Et for Subleading Parton",         100,0.0,2000);
  TH1D* firstPartonJetEtHadronic =   fs->make<TH1D>("firstPartonJetEtHadronic", "Hadronic Jet Et for Leading Parton",      100,0.0,2000);
  TH1D* secondPartonJetEtHadronic =  fs->make<TH1D>("secondPartonJetEtHadronic","Hadronic Jet Et for Subleading Parton",   100,0.0,2000);
  TH1D* firstPartonJetEtEM =         fs->make<TH1D>("firstPartonJetEtEM", "EM Jet Et for Leading Parton",                  100,0.0,2000);
  TH1D* secondPartonJetEtEM =        fs->make<TH1D>("secondPartonJetEtEM","EM Jet Et for Subleading Parton",               100,0.0,2000);
  TH1D* firstPartonJetEtInvisible =  fs->make<TH1D>("firstPartonJetEtInvisible", "Invisible Jet Et for Leading Parton",    100,0.0,2000);
  TH1D* secondPartonJetEtInvisible = fs->make<TH1D>("secondPartonJetEtInvisible","Invisible Jet Et for Subleading Parton", 100,0.0,2000);

  TH1D* leadSubleadingJetMuonMass  =    fs->make<TH1D>("leadingSubleadingJetMuonMass","Four Object Mass of the 2 leading Jets and Muons",100, 0.0,6000);
  TH1D* leadSubleadingPartonMuonMass  = fs->make<TH1D>("leadingSubleadingPartonMuonMass","Four Object Mass of the 2 leading Partons and Muons",100, 0.0,6000);
  
  //std::cout <<"looping over events now"<< std::endl;;
  for(std::vector<eventBits>::iterator ievent = m_events.begin(); ievent != m_events.end(); ievent++) {
    std::cout << "Making GEN plots for " <<m_events.size()<<" events"<< std::endl;
  //  std::cout <<"accessing event quantities..."<<std::endl;
    if(!(ievent->passesGenCuts())) {
      std::cout << "ERROR! THIS EVENT SHOULD HAVE FAILED" <<std::endl;
      continue;
    }
    parton1Et->Fill(ievent->getHighestEtParton()->et());
    parton2Et->Fill(ievent->getSecondHighestEtParton()->et());
    muonHighestEt->Fill(ievent->getHighestEtMuon()->et());
    muonSecondHighestEt->Fill(ievent->getSecondHighestEtMuon()->et());

    parton1Eta->Fill(ievent->getHighestEtParton()->eta());
    parton2Eta->Fill(ievent->getSecondHighestEtParton()->eta());
    muonHighestEtEta->Fill(ievent->getHighestEtMuon()->eta());
    muonSecondHighestEtEta->Fill(ievent->getSecondHighestEtMuon()->eta());

    dRparton1parton2->Fill(deltaR2(*(ievent->getHighestEtParton()),*(ievent->getSecondHighestEtParton())));
    dRmuon1muon2->Fill(deltaR2(*(ievent->getHighestEtMuon()),*(ievent->getSecondHighestEtMuon())));
    dRparton1muon2->Fill(deltaR2(*(ievent->getHighestEtParton()),*(ievent->getSecondHighestEtMuon())));
    dRparton1muon1->Fill(deltaR2(*(ievent->getHighestEtParton()),*(ievent->getHighestEtMuon())));
    dRparton2muon2->Fill(deltaR2(*(ievent->getSecondHighestEtParton()),*(ievent->getSecondHighestEtMuon())));
    dRparton2muon1->Fill(deltaR2(*(ievent->getSecondHighestEtParton()),*(ievent->getHighestEtMuon())));

    firstPartonJetEtTotal->Fill(ievent->getFirstPartonGenJet()->et());   
    secondPartonJetEtTotal->Fill(ievent->getSecondPartonGenJet()->et());  
    firstPartonJetEtHadronic->Fill(ievent->getFirstPartonGenJet()->hadEnergy());
    secondPartonJetEtHadronic->Fill(ievent->getSecondPartonGenJet()->hadEnergy());
    firstPartonJetEtEM->Fill(ievent->getFirstPartonGenJet()->emEnergy());      
    secondPartonJetEtEM->Fill(ievent->getSecondPartonGenJet()->emEnergy());     
    firstPartonJetEtInvisible->Fill(ievent->getFirstPartonGenJet()->invisibleEnergy());
    secondPartonJetEtInvisible->Fill(ievent->getSecondPartonGenJet()->invisibleEnergy());

    leadSubleadingPartonMuonMass->Fill((ievent->getHighestEtParton()->p4()+ievent->getSecondHighestEtParton()->p4()+ievent->getHighestEtMuon()->p4()+ievent->getSecondHighestEtMuon()->p4()).mass());
    leadSubleadingJetMuonMass->Fill((ievent->getSecondPartonGenJet()->p4()+ievent->getFirstPartonGenJet()->p4()+ievent->getHighestEtMuon()->p4()+ievent->getSecondHighestEtMuon()->p4()).mass());
  }
  //std::cout <<"DONE!"<<std::endl;
  return;  

}

// ------------ method called once each job just before starting event loop  ------------
void 
cmsWRextension::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
cmsWRextension::endJob() 
{
  makePlots();
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
cmsWRextension::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(cmsWRextension);
