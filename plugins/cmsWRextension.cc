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
      void makePlots();
     
      // ----------member data ---------------------------
      std::vector<eventBits> m_events;
      edm::EDGetToken m_genParticleToken;
      edm::EDGetToken m_genJetsToken;
 
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
   m_genParticleToken (consumes<std::vector<reco::GenParticle>> (edm::InputTag("genParticles"))),
   m_genJetsToken (consumes<std::vector<reco::GenJet>> (iConfig.getParameter<edm::InputTag>("genJets")))

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
void
cmsWRextension::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<std::vector<reco::GenParticle>> pIn;
   iEvent.getByToken(m_genParticleToken, pIn);

   eventBits myEvent;
  
   //LOOP OVER GEN PARTICLES
   for (std::vector<reco::GenParticle>::const_iterator iParticle = pIn->begin(); iParticle != pIn->end(); iParticle++) {
     if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
     if(iParticle->isHardProcess() && iParticle->pdgId() <= 6 && iParticle->pdgId() >= -6) myEvent.outgoingPartons.push_back((*iParticle));
     if(iParticle->isPromptFinalState() && (iParticle->pdgId() == 13 || iParticle->pdgId() == -13)) myEvent.outgoingMuons.push_back((*iParticle));
   }
   //CHECK THAT THE EVENT MAKES SENSE
   if (myEvent.outgoingPartons.size() < 2 || myEvent.outgoingMuons.size() < 2) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT FIND AT LEAST 2 PARTONS OR 2 MUONS"<< std::endl;
    return;
   }
   //SORT GEN MUONS AND PARTONS BY ET
   std::sort(myEvent.outgoingPartons.begin(),myEvent.outgoingPartons.end(),::wrTools::compareEt);
   std::sort(myEvent.outgoingMuons.begin(),myEvent.outgoingMuons.end(),::wrTools::compareEt);

   myEvent.highestEtParton = &myEvent.outgoingPartons.at(0);
   myEvent.secondHighestEtParton = &myEvent.outgoingPartons.at(1);

   myEvent.highestEtMuon = &myEvent.outgoingMuons.at(0);
   myEvent.secondHighestEtMuon = &myEvent.outgoingMuons.at(1);

   m_events.push_back(myEvent);
   //NOW THAT WE HAVE THE MUONS AND PARTONS WE WANT, WE FILL ALL OF OUR HISTOGRAMS
//   makePlots(myEvent);
}
void cmsWRextension::makePlots()
{
  if(!(m_events.size() > 0)) return;
  std::cout << "processing: " << m_events.size() <<"events"<< std::endl;
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
   
   std::cout <<"looping over events now"<< std::endl;;
   for(std::vector<eventBits>::iterator ievent = m_events.begin(); ievent != m_events.end(); ievent++) {
     std::cout <<"accessing event quantities..."<<std::endl;
     parton1Et->Fill(ievent->highestEtParton->et());
     parton2Et->Fill(ievent->secondHighestEtParton->et());
     muonHighestEt->Fill(ievent->highestEtMuon->et());
     muonSecondHighestEt->Fill(ievent->secondHighestEtMuon->et());

     parton1Eta->Fill(ievent->highestEtParton->eta());
     parton2Eta->Fill(ievent->secondHighestEtParton->eta());
     muonHighestEtEta->Fill(ievent->highestEtMuon->eta());
     muonSecondHighestEtEta->Fill(ievent->secondHighestEtMuon->eta());

     dRparton1parton2->Fill(deltaR2(*(ievent->highestEtParton),*(ievent->secondHighestEtParton)));
     dRmuon1muon2->Fill(deltaR2(*(ievent->highestEtMuon),*(ievent->secondHighestEtMuon)));
     dRparton1muon2->Fill(deltaR2(*(ievent->highestEtParton),*(ievent->secondHighestEtMuon)));
     dRparton1muon1->Fill(deltaR2(*(ievent->highestEtParton),*(ievent->highestEtMuon)));
     dRparton2muon2->Fill(deltaR2(*(ievent->secondHighestEtParton),*(ievent->secondHighestEtMuon)));
     dRparton2muon1->Fill(deltaR2(*(ievent->secondHighestEtParton),*(ievent->highestEtMuon)));
   }
   std::cout <<"DONE!"<<std::endl;
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
