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
//local includes
#include "cmsWRextension.h"
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

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
   
   if (m_doGen) {
     if(preSelectGen(iEvent, myEvent));
     m_allEvents.fill(myEvent);    
   } else {
     if(preSelectReco(iEvent, myEvent));
     m_allEvents.fill(myEvent);



   }
   if(passWR2016(iEvent, myEvent)) m_eventsPassingWR2016.fill(myEvent);
   if(passExtension(iEvent, myEvent)) m_eventsPassingExtension.fill(myEvent);

   selectMuons(iEvent, myEvent);

}
  
bool cmsWRextension::preSelectGen(const edm::Event& iEvent, eventBits& myEvent)
{
   using namespace edm;
  
   float partonJetMatchDR = .5;
 
   Handle<std::vector<reco::GenParticle>> genParticles;
   iEvent.getByToken(m_genParticleToken, genParticles);

   Handle<std::vector<reco::GenJet>> genJets;
   iEvent.getByToken(m_genJetsToken, genJets);

   std::vector<const reco::GenJet*> myGenJets;
   std::vector<const reco::GenParticle*> myGenPartons;
   std::vector<const reco::GenParticle*> myGenMuons;

   //LOOP OVER GEN PARTICLES
   for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
     if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
     if((iParticle->isHardProcess() && iParticle->status() == 23) && iParticle->pdgId() <= 6 && iParticle->pdgId() >= -6) myGenPartons.push_back(&(*iParticle));
     if(((iParticle->isLastCopy()&&!m_wantHardProcessMuons) || iParticle->isHardProcess()) && (iParticle->pdgId() == 13 || iParticle->pdgId() == -13)) myGenMuons.push_back(&(*iParticle));
   }
   //CHECK THAT THE EVENT MAKES SENSE
   if (myGenPartons.size() < 2 || myGenMuons.size() < 2) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT FIND AT LEAST 2 PARTONS OR 2 MUONS"<< std::endl;
     return false;
   }
   if (myGenPartons.size() != 2 || myGenMuons.size() != 2) {
     std::cout << "ERROR! Found more than 2 partons ("<<myGenPartons.size()<<") or muons("<<myGenMuons.size()<<")."<< std::endl;
   }

  //SORT GEN MUONS AND PARTONS BY ET
   std::sort(myGenPartons.begin(),myGenPartons.end(),::wrTools::compareEtPointer);
   std::sort(myGenMuons.begin(),myGenMuons.end(),::wrTools::compareEtPointer);

   myEvent.parton1EtVal = myGenPartons[0]->et();
   myEvent.parton1EtaVal = myGenPartons[0]->eta();
   myEvent.parton2EtVal = myGenPartons[1]->et();
   myEvent.parton2EtaVal = myGenPartons[1]->eta();
   myEvent.muonHighestEtVal = myGenMuons[0]->et();
   myEvent.muonHighestEtEtaVal = myGenMuons[0]->eta();
   myEvent.muonSecondHighestEtVal = myGenMuons[1]->et();
   myEvent.muonSecondHighestEtEtaVal = myGenMuons[1]->eta();

   //NOW WE'LL CHECK IF IT PASSES SOME BASIC GEN LEVEL CUTS
   if(!myEvent.passesGenCuts()) {
     std::cout << "ERROR! SKIPPING EVENT, LEADING PARTONS AND MUONS NOT OVER 20 GEV"<< std::endl;
     return false;
   }

   //NOW THAT THE GEN MUONS AND PARTONS ARE SORTED OUT, WE'LL MATCH A GENJET TO EACH PARTON
   //FIRST WE'LL GET THE GENJETS THAT HAVE AT LEAST 10 GEV ET

   //HERE WE COMPARE OUR EVENTS
   bool foundFirst = false;
   bool foundSecond = false;
   float bestFirstJetMatch = 10.0;
   float bestSecondJetMatch = 10.0;
   reco::GenJet* firstPartonGenJet;
   reco::GenJet* secondPartonGenJet;
   for (std::vector<reco::GenJet>::const_iterator iJet = genJets->begin(); iJet != genJets->end(); iJet++) {
     // if ()

     // if(bestFirstJetMatch > deltaR2(*iJet,*(myGenPartons[0]))) {
     //   bestFirstJetMatch = deltaR2(*iJet,*(myGenPartons[0]));
     //   if(bestFirstJetMatch <= partonJetMatchDR) {
     //     firstPartonGenJet = &(*(iJet));
     //     foundFirst = true;
     //   }
     // }
     // if(bestSecondJetMatch > deltaR2(*iJet,*(myEvent.getSecondHighestEtParton()))) {
     //  bestSecondJetMatch = deltaR2(*iJet,*(myEvent.getSecondHighestEtParton()));
     //  if(bestSecondJetMatch <= partonJetMatchDR) {
     //    secondPartonGenJet = &(*(iJet));
     //    foundSecond = true;
     //  }
     // }  
     // index++;
   }
   if (!foundFirst || !foundSecond) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT MATCH EITHER PARTONS WITH A JET WITHIN: "<< partonJetMatchDR<<" dR"<<std::endl;
     return false;
   }
   myEvent.firstPartonJetEtTotalVal = firstPartonGenJet->et();
   myEvent.firstPartonJetEtHadronicVal = firstPartonGenJet->hadEnergy();
   myEvent.firstPartonJetEtEMVal = firstPartonGenJet->emEnergy();
   myEvent.firstPartonJetEtInvisibleVal = firstPartonGenJet->invisibleEnergy();

   myEvent.firstPartonJetEtTotalVal = secondPartonGenJet->et();
   myEvent.firstPartonJetEtHadronicVal = secondPartonGenJet->hadEnergy();
   myEvent.firstPartonJetEtEMVal = secondPartonGenJet->emEnergy();
   myEvent.firstPartonJetEtInvisibleVal = secondPartonGenJet->invisibleEnergy();

   return true;
}
bool cmsWRextension::preSelectReco(const edm::Event& iEvent, eventBits& myEvent) {
  selectMuons(iEvent, myEvent);


  return false;

}
bool cmsWRextension::passWR2016(const edm::Event& iEvent, eventBits& myEvent) {
  return false;
}
bool cmsWRextension::passExtension(const edm::Event& iEvent, eventBits& myEvent) {
  return false;
}

void cmsWRextension::selectMuons(const edm::Event& iEvent, eventBits& myEvent)
{
   using namespace edm;
   
   Handle<std::vector<pat::Muon>> recoMuons;
   iEvent.getByToken(m_recoMuonToken, recoMuons);
   
   for (std::vector<pat::Muon>::const_iterator iParticle = recoMuons->begin(); iParticle != recoMuons->end(); iParticle++) {
   //NEEDS UPDATING
   }


}

// ------------ method called once each job just before starting event loop  ------------
void 
cmsWRextension::beginJob()
{
   edm::Service<TFileService> fs; 
   if(m_doGen) {
     m_allEvents.book((fs->mkdir("allEvents")), 1);           //(1) GEN ONLY PLOTS FOR NOW
     m_eventsPassingWR2016.book((fs->mkdir("eventsPassingWR2016")), 1); 
     m_eventsPassingExtension.book((fs->mkdir("eventsPassingExtension")), 1);
   } else {
     m_allEvents.book((fs->mkdir("allEvents")), 3);           //(1) GEN AND RECO PLOTS
     m_eventsPassingWR2016.book((fs->mkdir("eventsPassingWR2016")), 3); 
     m_eventsPassingExtension.book((fs->mkdir("eventsPassingExtension")), 3);
   }
}

// ------------ method called once each job just after ending the event loop  ------------
void 
cmsWRextension::endJob() 
{

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
