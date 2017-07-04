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
   m_AK8genJetsToken (consumes<std::vector<reco::GenJet>> (iConfig.getParameter<edm::InputTag>("AK8genJets"))),
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
     if(preSelectGen(iEvent, myEvent)) {
       std::cout << "plotting all events" << std::endl;
       m_allEvents.fill(myEvent);    
       std::cout << "analyzing WR2016" << std::endl;
       if(passWR2016(iEvent, myEvent)) m_eventsPassingWR2016.fill(myEvent);
       std::cout << "analyzing extension" << std::endl;
       if(passExtension(iEvent, myEvent)) m_eventsPassingExtension.fill(myEvent);
     }
   }

}
  
bool cmsWRextension::preSelectGen(const edm::Event& iEvent, eventBits& myEvent)
{
   using namespace edm;
  
   float partonJetMatchDR = .5;
 
   Handle<std::vector<reco::GenParticle>> genParticles;
   iEvent.getByToken(m_genParticleToken, genParticles);

   Handle<std::vector<reco::GenJet>> genJets;
   iEvent.getByToken(m_genJetsToken, genJets);

   Handle<std::vector<reco::GenJet>> AK8GenJets;
   iEvent.getByToken(m_AK8genJetsToken, AK8GenJets);

   std::vector<const reco::GenJet*> myGenJets;
   std::vector<const reco::GenJet*> myAK8GenJets;
   std::vector<const reco::GenParticle*> myGenPartons;
   std::vector<const reco::GenParticle*> myGenMuons;

   //LOOP OVER GEN PARTICLES
   for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
     if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
     if(iParticle->mother()) { if(abs(iParticle->mother()->pdgId()) == 24) continue; }//no W-SM mothered particles
     if((iParticle->isHardProcess() && iParticle->status() == 22) && abs(iParticle->pdgId()) == 6) myGenPartons.push_back(&(*iParticle)); //KEEP TOPS, NOT Qs FROM TOPS
     if((iParticle->isHardProcess() && iParticle->status() == 23) && (iParticle->pdgId() <= 6) && (iParticle->pdgId() >= -6) && (abs(iParticle->mother()->pdgId()) != 6)) myGenPartons.push_back(&(*iParticle));
     if(iParticle->fromHardProcessFinalState() && abs(iParticle->pdgId()) == 13) myGenMuons.push_back(&(*iParticle));
   }
   //LOOP OVER GEN MUONS TO FILTER OUT TOP GENERATED ONES
   std::vector<const reco::GenParticle*>::iterator badMuon;
   bool hasBadMuon = false;
   for(std::vector<const reco::GenParticle*>::iterator iMuon = myGenMuons.begin(); iMuon != myGenMuons.end(); iMuon++) {
     std::cout << "Muon mom type: "<<(*iMuon)->mother()->pdgId() << std::endl;
     if(::wrTools::particleIsFromABS((*iMuon),24)) {
       std::cout << "Found Muon from top decay" << std::endl;
       hasBadMuon = true;
       badMuon = iMuon;
     }
   }
   if(hasBadMuon) myGenMuons.erase(badMuon);
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

 //LOOK THROUGH GEN MUONS AND FIND THE ONES WITH THE WR AND NR MOTHERS
   size_t index = 0;
   for (std::vector<const reco::GenParticle*>::iterator iMuon = myGenMuons.begin(); iMuon != myGenMuons.end(); iMuon++) {
     for(size_t iMom = 0; iMom < (*iMuon)->numberOfMothers(); iMom++) {
       if((*iMuon)->mother(iMom)->pdgId() == 9900014){
         if (myEvent.secondInDecayMuon<=0) std::cout<<"ERROR: Two muons selected are seen as second in decay chain."<<std::endl;
	 myEvent.secondInDecayMuon = index;
       }
     }
     index++;
   }

   if (myEvent.secondInDecayMuon!=1){
     std::cout<<"Highest ET muon, is not first in decay"<<std::endl;
     if (myEvent.secondInDecayMuon==0) std::swap(myGenMuons[0],myGenMuons[1]);
     else std::cout<<"myEvent.secondInDecayMuon is "<<myEvent.secondInDecayMuon<<". I don't know what to do"<<std::endl;
   }

   myEvent.muonHighestEtVal = myGenMuons[0]->et();
   myEvent.muonHighestEtEtaVal = myGenMuons[0]->eta();
   myEvent.muonSecondHighestEtVal = myGenMuons[1]->et();
   myEvent.muonSecondHighestEtEtaVal = myGenMuons[1]->eta();

   myEvent.dRparton1parton2Val = deltaR2(*(myGenPartons[0]),*(myGenPartons[1]));
   myEvent.dRmuon1muon2Val   = deltaR2(*(myGenMuons[0]),*(myGenMuons[1]));
   myEvent.dRparton1muon2Val = deltaR2(*(myGenPartons[0]),*(myGenMuons[0]));
   myEvent.dRparton1muon1Val = deltaR2(*(myGenPartons[0]),*(myGenMuons[1]));
   myEvent.dRparton2muon2Val = deltaR2(*(myGenPartons[1]),*(myGenMuons[0]));
   myEvent.dRparton2muon1Val = deltaR2(*(myGenPartons[1]),*(myGenMuons[1])); 
   
   //NOW WE'LL CHECK IF IT PASSES SOME BASIC GEN LEVEL CUTS
   if(!myEvent.passesGenCuts()) {
     std::cout << "ERROR! SKIPPING EVENT, LEADING PARTONS AND MUONS NOT OVER 20 GEV"<< std::endl;
     return false;
   }
 
   //NOW THAT THE GEN MUONS AND PARTONS ARE SORTED OUT, WE'LL MATCH A GENJET TO EACH PARTON
   //FIRST WE'LL GET THE GENJETS THAT HAVE AT LEAST 10 GEV ET

   //HERE WE COMPARE OUR EVENTS
   double foundFirst = partonJetMatchDR;
   double foundSecond = partonJetMatchDR;
   const reco::GenJet* firstPartonGenJet=0; 
   const reco::GenJet* secondPartonGenJet=0; 
   const reco::GenJet* firstPartonAK8GenJet=0; 
   const reco::GenJet* secondPartonAK8GenJet=0; 

   for (std::vector<reco::GenJet>::const_iterator iJet = genJets->begin(); iJet != genJets->end(); iJet++) {
     if (iJet->et()<20.0) continue;
     float match1=deltaR2(*iJet,*(myGenPartons[0]))<partonJetMatchDR;
     float match2=deltaR2(*iJet,*(myGenPartons[1]))<partonJetMatchDR;
     if (match1<partonJetMatchDR || match2<partonJetMatchDR) myGenJets.push_back(&(*iJet));

     if ((match1<partonJetMatchDR && foundFirst<partonJetMatchDR) || (match2<partonJetMatchDR && foundSecond<partonJetMatchDR)){
       std::cout << "WARNING: multiple gen jets matched to the same parton"<< std::endl;
     }
     
     if (match1<foundFirst) {
       foundFirst=match1;
       firstPartonGenJet=&(*(iJet));
     }
     if (match2<foundSecond) {
       foundSecond=match2;
       secondPartonGenJet=&(*(iJet));
     }
   }
   if (!(foundFirst<0.5) || !(foundSecond<0.5)) {
     std::cout << "WARNING! DID NOT MATCH BOTH PARTONS WITH A JET WITHIN: "<< partonJetMatchDR<<" dR"<<std::endl;
   }
   if (firstPartonGenJet!=0){
     myEvent.firstPartonJetEtVal = firstPartonGenJet->et();
     myEvent.firstPartonJetEtHadronicVal = firstPartonGenJet->hadEnergy();
     myEvent.firstPartonJetEtEMVal = firstPartonGenJet->emEnergy();
     myEvent.firstPartonJetEtInvisibleVal = firstPartonGenJet->invisibleEnergy();
     myEvent.firstPartonJetEtaVal = firstPartonGenJet->eta();
     myEvent.firstPartonJetPhiVal = firstPartonGenJet->phi();
     myEvent.dRparton1jetVal= foundFirst;
   }
   if (secondPartonGenJet!=0 && secondPartonGenJet!=firstPartonGenJet){   
     myEvent.secondPartonJetEtVal = secondPartonGenJet->et();
     myEvent.secondPartonJetEtHadronicVal = secondPartonGenJet->hadEnergy();
     myEvent.secondPartonJetEtEMVal = secondPartonGenJet->emEnergy();
     myEvent.secondPartonJetEtInvisibleVal = secondPartonGenJet->invisibleEnergy();
     myEvent.secondPartonJetEtaVal = secondPartonGenJet->eta();
     myEvent.secondPartonJetPhiVal = secondPartonGenJet->phi();
     myEvent.dRparton2jetVal= foundSecond;
   }
   else if (secondPartonGenJet!=0){
     myEvent.dRparton2jetVal= foundSecond;
   }
   //REPEATED FOR AK8 GENJETS
   foundFirst = partonJetMatchDR;
   foundSecond = partonJetMatchDR;
   for (std::vector<reco::GenJet>::const_iterator iJet = AK8GenJets->begin(); iJet != AK8GenJets->end(); iJet++) {
     if (iJet->et()<20.0) continue;
     float match1=deltaR2(*iJet,*(myGenPartons[0]))<partonJetMatchDR;
     float match2=deltaR2(*iJet,*(myGenPartons[1]))<partonJetMatchDR;
     if (match1<partonJetMatchDR || match2<partonJetMatchDR) myAK8GenJets.push_back(&(*iJet));

     if ((match1<partonJetMatchDR && foundFirst<partonJetMatchDR) || (match2<partonJetMatchDR && foundSecond<partonJetMatchDR)){
       std::cout << "WARNING: multiple ak8 gen jets matched to the same parton"<< std::endl;
     }
     
     if (match1<foundFirst) {
       foundFirst=match1;
       firstPartonAK8GenJet=&(*(iJet));
     }
     if (match2<foundSecond) {
       foundSecond=match2;
       secondPartonAK8GenJet=&(*(iJet));
     }
   }

   if (!(foundFirst<0.5) || !(foundSecond<0.5)) {
     std::cout << "WARNING! DID NOT MATCH BOTH PARTONS WITH AN AK8 JET WITHIN: "<< partonJetMatchDR<<" dR"<<std::endl;
   }
   if (firstPartonAK8GenJet!=0){
     myEvent.firstPartonAK8JetEtVal = firstPartonAK8GenJet->et();
     myEvent.firstPartonAK8JetEtHadronicVal = firstPartonAK8GenJet->hadEnergy();
     myEvent.firstPartonAK8JetEtEMVal = firstPartonAK8GenJet->emEnergy();
     myEvent.firstPartonAK8JetEtInvisibleVal = firstPartonAK8GenJet->invisibleEnergy();
     myEvent.firstPartonAK8JetEtaVal = firstPartonAK8GenJet->eta();
     myEvent.firstPartonAK8JetPhiVal = firstPartonAK8GenJet->phi();
//     myEvent.dRparton1jetVal= foundFirst;
   }
   if (secondPartonAK8GenJet!=0 && secondPartonAK8GenJet!=firstPartonAK8GenJet){   
     myEvent.secondPartonAK8JetEtVal = secondPartonAK8GenJet->et();
     myEvent.secondPartonAK8JetEtHadronicVal = secondPartonAK8GenJet->hadEnergy();
     myEvent.secondPartonAK8JetEtEMVal = secondPartonAK8GenJet->emEnergy();
     myEvent.secondPartonAK8JetEtInvisibleVal = secondPartonAK8GenJet->invisibleEnergy();
     myEvent.secondPartonAK8JetEtaVal = secondPartonAK8GenJet->eta();
     myEvent.secondPartonAK8JetPhiVal = secondPartonAK8GenJet->phi();
//     myEvent.dRparton2jetVal= foundSecond;
   }
   else if (secondPartonAK8GenJet!=0){
  //   myEvent.dRparton2jetVal= foundSecond;
   }


   myEvent.myGenJets = myGenJets;
   myEvent.myAK8GenJets = myAK8GenJets;
   myEvent.myGenPartons = myGenPartons;
   myEvent.myGenMuons = myGenMuons;
   
   
   return true;
}
bool cmsWRextension::preSelectReco(const edm::Event& iEvent, eventBits& myEvent) {
  selectMuons(iEvent, myEvent);


  return false;

}
bool cmsWRextension::passWR2016(const edm::Event& iEvent, eventBits& myEvent) {
 // std::cout <<myEvent.myGenMuons.size() << " "<<myEvent.myGenJets.size() << std::endl;
  if(myEvent.myGenMuons.size() < 2 || myEvent.myGenJets.size() < 2) {
    std::cout << "EVENT FAILS, NOT ENOUGH TO RECONSTRUCT " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myGenJets.size()<<" jets"<< std::endl;
    return false;
  }
 // std::cout <<"SORTING JETS" <<std::endl;
  std::sort(myEvent.myGenJets.begin(),myEvent.myGenJets.end(),::wrTools::compareEtJetPointer);
//  std::cout <<"CALCULATING MASS" <<std::endl;
  myEvent.leadSubleadingJetsMuonsMassVal = (myEvent.myGenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenJets[1]->p4() + myEvent.myGenMuons[1]->p4()).mass();
 // std::cout<< "CALCULATING PT" <<std::endl;
  myEvent.leadSubleadingJetsMuonsPtVal = (myEvent.myGenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenJets[1]->p4() + myEvent.myGenMuons[1]->p4()).pt();
//  std::cout <<"CALCULATING ETA" <<std::endl;
  myEvent.leadSubleadingJetsMuonsEtaVal = (myEvent.myGenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenJets[1]->p4() + myEvent.myGenMuons[1]->p4()).eta();
//  std::cout <<"DONE!" <<std::endl;
  
  return true;
}
bool cmsWRextension::passExtension(const edm::Event& iEvent, eventBits& myEvent) {
 // std::cout <<myEvent.myGenMuons.size() << " "<<myEvent.myAK8GenJets.size() << std::endl;
  if(myEvent.myGenMuons.size() < 2 || myEvent.myAK8GenJets.size() < 1) {
    std::cout << "EVENT FAILS, NOT ENOUGH TO RECONSTRUCT " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myAK8GenJets.size()<<" ak8 jets"<< std::endl;
    return false;
  }
 // std::cout <<"SORTING JETS" <<std::endl;
  std::sort(myEvent.myAK8GenJets.begin(),myEvent.myAK8GenJets.end(),::wrTools::compareEtJetPointer);
  std::cout<<"There are "<<myEvent.myAK8GenJets.size()<<" AK8GenJets selected"<<std::endl;
  // if(myEvent.secondInDecayMuon != 0) {
    myEvent.leadAK8JetMuonMassVal = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).mass();
    myEvent.leadAK8JetMuonPtVal   = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).pt();
    myEvent.leadAK8JetMuonEtaVal  = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).eta();
  // } else  {
  //   myEvent.leadAK8JetMuonMassVal = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[1]->p4()).mass();
  //   myEvent.leadAK8JetMuonPtVal   = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[1]->p4()).pt();
  //   myEvent.leadAK8JetMuonEtaVal  = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[1]->p4()).eta();
  //   std::cout << "MUON SWITCH!" << std::endl;
  // }
  return true;
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
