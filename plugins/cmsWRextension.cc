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
   m_recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("recoJets"))),
   m_AK8recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoJets"))),
   m_offlineVerticesToken (consumes<std::vector<reco::Vertex>> (iConfig.getParameter<edm::InputTag>("vertices"))),
   m_wantHardProcessMuons (iConfig.getUntrackedParameter<bool>("wantHardProcessMuons",true)),
   m_doGen (iConfig.getUntrackedParameter<bool>("doGen",false)),
   m_doReco (iConfig.getUntrackedParameter<bool>("doReco",true))

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
   eventBits myRECOevent;
   
   if (m_doGen) {
     if(preSelectGen(iEvent, myEvent)) {
       std::cout << "plotting all events" << std::endl;
       m_allEvents.fill(myEvent);    
       std::cout << "analyzing WR2016" << std::endl;
       if(passWR2016GEN(iEvent, myEvent)) m_eventsPassingWR2016.fill(myEvent);
       std::cout << "analyzing extension" << std::endl;
       if(passExtensionGEN(iEvent, myEvent)) m_eventsPassingExtension.fill(myEvent);
     }
   }
   if (m_doReco) {
     if(preSelectReco(iEvent, myRECOevent)) {
       if(passExtensionRECO(iEvent, myRECOevent)) { 
         m_eventsPassingExtensionRECO.fill(myRECOevent);
         std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;
         std::cout << "PASSED RECO EXTENSION, FILLING" << std::endl;
       }
     }
     if(passWR2016Reco(iEvent,myRECOevent)) m_eventsPassingWR2016RECO.fill(myRECOevent);
   }
}
  
bool cmsWRextension::preSelectReco(const edm::Event& iEvent, eventBits& myRECOevent) {

  edm::Handle<std::vector<pat::Muon>> recoMuons;
  iEvent.getByToken(m_recoMuonToken, recoMuons);

  edm::Handle<std::vector<pat::Jet>> recoJetsAK8;
  iEvent.getByToken(m_AK8recoJetsToken, recoJetsAK8);

  edm::Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(m_offlineVerticesToken, vertices);

  std::vector<const pat::Muon*> highPTMuons;
  std::vector<const pat::Muon*> allMuons;
  //COLLECT MUONS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  std::cout<<"PRESELECTING CANDS RECO"<<std::endl;
  for(std::vector<pat::Muon>::const_iterator iMuon = recoMuons->begin(); iMuon != recoMuons->end(); iMuon++) {
    if( iMuon->pt() < 20 || fabs(iMuon->eta()) > 2.4 ) continue;
    allMuons.push_back(&(*iMuon));
    if(( iMuon->isHighPtMuon(vertices->at(0)) && iMuon->tunePMuonBestTrack()->pt() > 200) && (iMuon->isolationR03().sumPt/iMuon->pt() <= .05)) {
      highPTMuons.push_back(&(*iMuon));
      std::cout<<"MUON CAND WITH PT,ETA,PHI: "<<iMuon->pt()<<","<<iMuon->eta()<<","<<iMuon->phi()<<std::endl;
    }
    //if( iMuon->pt() > 200 ) highPTMuons.push_back(&(*iMuon));

  }
  if( highPTMuons.size() < 1) {
    std::cout<< "EVENT FAILS, NO MUONS OVER 200 GEV WITHIN ACCEPTANCE. "<<allMuons.size()<<" MUONS FOUND." << std::endl;
    return false;
  }
  //COLLECT AK8JETS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  std::vector<const pat::Jet*> highPTJets;
  std::vector<const pat::Jet*> allJets;
  //COLLECT JetS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  for(std::vector<pat::Jet>::const_iterator iJet = recoJetsAK8->begin(); iJet != recoJetsAK8->end(); iJet++) {
    if( iJet->pt() < 20 || fabs(iJet->eta()) > 2.4 ) continue;
    allJets.push_back(&(*iJet));
    if( iJet->pt() > 200 ){ 
      highPTJets.push_back(&(*iJet));
      std::cout<<"AK8JET CAND WITH PT,ETA,PHI: "<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
    }
  }
  if( highPTJets.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<<allJets.size()<<" JETS FOUND." << std::endl;
    return false;
  }
  //BUILD PAIRS OF AK8 JETS AND MUONS
  std::vector<std::pair<const pat::Jet*, const pat::Muon*>> muonJetPairs; 
  for(std::vector<const pat::Jet*>::const_iterator iJet = highPTJets.begin(); iJet != highPTJets.end(); iJet++)
    for(std::vector<const pat::Muon*>::const_iterator iMuon = highPTMuons.begin(); iMuon != highPTMuons.end(); iMuon++) {
      //if( ((*iJet)->p4() + (*iMuon)->p4()).mass() < 400) continue;
      //if (sqrt(deltaR2(*(*iJet),*(*iMuon)))<2.0) continue;
      if(((*iJet)->phi() - (*iMuon)->phi()) < 2.0 ) continue;
      muonJetPairs.push_back(std::make_pair(*iJet,*iMuon));

    }
  if( muonJetPairs.size() < 1 ) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET MUON PAIRS" <<std::endl;
    return false;
  }
  std::cout<<muonJetPairs.size()<<" Pairing CANDIDATES Selected from "<< highPTMuons.size() << " muons and "<<highPTJets.size()<<" jets"<<std::endl;
  myRECOevent.myMuonJetPairs = muonJetPairs;
  myRECOevent.muonCands = highPTMuons.size();
  return true;
}
//////////////FOR MUONS TUNEP HIGHPT MUONS ARE USED//////////
///////////FROM THE AN///////////////////
//leading lepton with pT > 60 GeV
//subleading lepton with pT > 53 GeV
//at least two jets with pT > 40 GeV, consider only the leading and subleading in the following if more than two
//all leptons and jets with |η| < 2.4
//dilepton mass mll > 200 GeV: to suppress DY+jets contribution ∆R > 0.4 between all objects in the final state (leptons and jets) Mlljj > 600 GeV
bool cmsWRextension::passWR2016Reco(const edm::Event& iEvent, eventBits& myEvent) {

  edm::Handle<std::vector<pat::Muon>> recoMuons;
  iEvent.getByToken(m_recoMuonToken, recoMuons);

  edm::Handle<std::vector<pat::Jet>> recoJets;
  iEvent.getByToken(m_recoJetsToken, recoJets);

  if(recoMuons->size() < 2 || recoJets->size() < 2) {
    std::cout << "EVENT FAILS WR2016, NOT ENOUGH TO RECONSTRUCT " << recoMuons->size()<<" muons "<<  recoJets->size()<<" jets"<< std::endl;
    return false;
  }

  std::vector<const pat::Jet*> myPreSelectedJets;
  std::vector<const pat::Jet*> mySelectedJets;
  std::vector<const pat::Muon*> myPreSelectedMuons;
  std::vector<const pat::Muon*> mySelectedMuons;

  for(std::vector<pat::Jet>::const_iterator iJet = recoJets->begin(); iJet != recoJets->end(); iJet++){
    if (iJet->et()<40.0 || fabs(iJet->eta())>2.4) continue;
    myPreSelectedJets.push_back(&(*iJet));
  }

  std::sort(myPreSelectedJets.begin(),myPreSelectedJets.end(),::wrTools::compareEtCandidatePointer);

  bool foundpair=false;
  for (std::vector<const pat::Jet*>::iterator iJet = myPreSelectedJets.begin(); iJet != myPreSelectedJets.end(); iJet++) {
    if ((*iJet)->et()<40.0 || fabs((*iJet)->eta())>2.4) continue;
    for (std::vector<const pat::Jet*>::iterator iJet2 = iJet+1; iJet2 != myPreSelectedJets.end(); iJet2++) {
      if ((*iJet)->et()<40.0 || fabs((*iJet)->eta())>2.4) continue;
      if (sqrt(deltaR2(*(*iJet),*(*iJet2)))<0.4) continue;
      foundpair=true;
      mySelectedJets.push_back((*iJet));
      mySelectedJets.push_back((*iJet2));
      break;
    }
    if (foundpair) break;
  }
  if (!foundpair){
    std::cout << "Event fails WR2016, not Jet pair is found" << std::endl;
    return false;
  }
  if (mySelectedJets.size()!=2){
    std::cout << "ERROR, BUG ON WR2016 NUMBER OF JETS. I SHOULDN'T BE ABLE TO GET THIS ERROR" << std::endl;
    return false;
  }

  for (std::vector<pat::Muon>::const_iterator iMuon = recoMuons->begin(); iMuon != recoMuons->end(); iMuon++) {
    if (iMuon->et()<53 || fabs(iMuon->eta())>2.4) continue;
    if (sqrt(deltaR2((*iMuon),*mySelectedJets[0]))<0.4) continue;
    if (sqrt(deltaR2((*iMuon),*mySelectedJets[1]))<0.4) continue;
    myPreSelectedMuons.push_back(&(*iMuon));
  }
  if (myPreSelectedMuons.size()<2){
    std::cout << "Event fails WR2016, was unable to get 2 muons." << std::endl;
    return false;
  }

  std::sort(myPreSelectedMuons.begin(),myPreSelectedMuons.end(),::wrTools::compareEtCandidatePointer);

  foundpair=false;
  for (std::vector<const pat::Muon*>::iterator iMuon = myPreSelectedMuons.begin(); iMuon != myPreSelectedMuons.end(); iMuon++) {
    if ((*iMuon)->et()<60) continue;
    for (std::vector<const pat::Muon*>::iterator iMuon2 = iMuon+1; iMuon2 != myPreSelectedMuons.end(); iMuon2++) {
      if (sqrt(deltaR2(*(*iMuon),*(*iMuon2)))<0.4) continue;
      foundpair=true;
      mySelectedMuons.push_back((*iMuon));
      mySelectedMuons.push_back((*iMuon2));
      break;
    }
    if (foundpair) break;
  }
  if (!foundpair){
    std::cout << "Event fails WR2016, not Muon pair is found" << std::endl;
    return false;
  }
  if (mySelectedMuons.size()!=2){
    std::cout << "ERROR, BUG ON WR2016 NUMBER OF MUONS. I SHOULDN'T BE ABLE TO GET THIS ERROR" << std::endl;
    return false;
  }

  // myEvent.leadSubleadingJetsMuonsMassVal = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).mass();
  // myEvent.leadSubleadingJetsMuonsPtVal   = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).pt();
  //edMuons[0]->p4() + mySelectedMuons[1]->p4()).eta();
  
  return true;
}

bool cmsWRextension::selectHighPtISOMuon(const edm::Event& iEvent, eventBits& myEvent) {

   edm::Handle<std::vector<pat::Muon>> recoMuons;
   iEvent.getByToken(m_recoMuonToken, recoMuons);

   for (std::vector<pat::Muon>::const_iterator iParticle = recoMuons->begin(); iParticle != recoMuons->end(); iParticle++) {
     std::cout<<iParticle->pt()<<std::endl;
     //NEEDS UPDATING
   }


   return false;
}
bool cmsWRextension::preSelectGen(const edm::Event& iEvent, eventBits& myEvent)
{
  
   float partonJetMatchDR = .4;
   float partonAK8JetMatchDR = .8;
 
   edm::Handle<std::vector<reco::GenParticle>> genParticles;
   iEvent.getByToken(m_genParticleToken, genParticles);

   edm::Handle<std::vector<reco::GenJet>> genJets;
   iEvent.getByToken(m_genJetsToken, genJets);

   edm::Handle<std::vector<reco::GenJet>> AK8GenJets;
   iEvent.getByToken(m_AK8genJetsToken, AK8GenJets);

   std::vector<const reco::GenJet*> myGenJets;
   std::vector<const reco::GenJet*> myAK8GenJets;
   std::vector<const reco::GenParticle*> myGenPartons;
   std::vector<const reco::GenParticle*> myGenMuons;

   //LOOP OVER GEN PARTICLES
   for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
     if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
     if(iParticle->mother()) { if(::wrTools::particleIsFromABS(&(*iParticle),24)) continue; }//no W-SM mothered particles
     if((iParticle->isHardProcess() && iParticle->status() == 22) && abs(iParticle->pdgId()) == 6) myGenPartons.push_back(&(*iParticle)); //KEEP TOPS, NOT Qs FROM TOPS
     if((iParticle->isHardProcess() && iParticle->status() == 23) && (iParticle->pdgId() <= 6) && (iParticle->pdgId() >= -6) && !(::wrTools::particleIsFromABS(&(*iParticle),6))) myGenPartons.push_back(&(*iParticle));
     if(iParticle->fromHardProcessFinalState() && abs(iParticle->pdgId()) == 13) myGenMuons.push_back(&(*iParticle));
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
   std::sort(myGenPartons.begin(),myGenPartons.end(),::wrTools::compareEtGenParticlePointer);
   std::sort(myGenMuons.begin(),myGenMuons.end(),::wrTools::compareEtGenParticlePointer);

   myEvent.parton1EtVal = myGenPartons[0]->et();
   myEvent.parton1EtaVal = myGenPartons[0]->eta();
   myEvent.parton1PhiVal = myGenPartons[0]->phi();
   myEvent.parton2EtVal = myGenPartons[1]->et();
   myEvent.parton2EtaVal = myGenPartons[1]->eta();
   myEvent.parton2PhiVal = myGenPartons[1]->phi();

   //LOOK THROUGH GEN MUONS AND FIND THE ONES WITH THE WR AND NR MOTHERS
   int index = 0;
   for (std::vector<const reco::GenParticle*>::iterator iMuon = myGenMuons.begin(); iMuon != myGenMuons.end(); iMuon++) {
     if(::wrTools::particleIsFromABS((*iMuon),9900014)){
       if (myEvent.secondInDecayMuon>=0) std::cout<<"ERROR: Two muons selected are seen as second in decay chain."<<std::endl;
       myEvent.secondInDecayMuon = index;
     }
     index++;
   }

   if (myEvent.secondInDecayMuon!=1){
     std::cout<<"Highest ET muon, is not first in decay"<<std::endl;
     if (myEvent.secondInDecayMuon==0) std::swap(myGenMuons[0],myGenMuons[1]);
     else std::cout<<"myEvent.secondInDecayMuon is "<<myEvent.secondInDecayMuon<<". I don't know what to do"<<std::endl;
   }

   myEvent.muon1EtVal = myGenMuons[0]->et();
   myEvent.muon1EtaVal = myGenMuons[0]->eta();
   myEvent.muon1PhiVal = myGenMuons[0]->phi();
   myEvent.muon2EtVal = myGenMuons[1]->et();
   myEvent.muon2EtaVal = myGenMuons[1]->eta();
   myEvent.muon2PhiVal = myGenMuons[1]->phi();

   myEvent.dRparton1parton2Val = sqrt(deltaR2(*(myGenPartons[0]),*(myGenPartons[1])));
   myEvent.dRmuon1muon2Val   = sqrt(deltaR2(*(myGenMuons[0]),*(myGenMuons[1])));
   myEvent.dRparton1muon1Val = sqrt(deltaR2(*(myGenPartons[0]),*(myGenMuons[0])));
   myEvent.dRparton1muon2Val = sqrt(deltaR2(*(myGenPartons[0]),*(myGenMuons[1])));
   myEvent.dRparton2muon1Val = sqrt(deltaR2(*(myGenPartons[1]),*(myGenMuons[0])));
   myEvent.dRparton2muon2Val = sqrt(deltaR2(*(myGenPartons[1]),*(myGenMuons[1]))); 
   
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
     float match1=sqrt(deltaR2(*iJet,*(myGenPartons[0])));
     float match2=sqrt(deltaR2(*iJet,*(myGenPartons[1])));
     if (match1<partonJetMatchDR || match2<partonJetMatchDR){
       std::cout << "Pushing back jet with et: "<<iJet->et()  <<" eta: "<<iJet->eta()<<" phi: "<<iJet->phi()<< " match1: "<<match1<<" match2: "<<match2 <<  std::endl;
       myGenJets.push_back(&(*iJet));
     }
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
   if (!(foundFirst<partonJetMatchDR) || !(foundSecond<partonJetMatchDR)) {
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
   foundFirst = partonAK8JetMatchDR;
   foundSecond = partonAK8JetMatchDR;
   for (std::vector<reco::GenJet>::const_iterator iJet = AK8GenJets->begin(); iJet != AK8GenJets->end(); iJet++) {
     if (iJet->et()<20.0) continue;
     float match1=sqrt(deltaR2(*iJet,*(myGenPartons[0])));
     float match2=sqrt(deltaR2(*iJet,*(myGenPartons[1])));
     if (match1<partonAK8JetMatchDR || match2<partonAK8JetMatchDR) {
       std::cout << "Pushing back ak8 jet with et: "<<iJet->et()  <<" eta: "<<iJet->eta()<<" phi: "<<iJet->phi()<< " match1: "<<match1<<" match2: "<<match2 <<  std::endl;
       myAK8GenJets.push_back(&(*iJet));
     }
     if ((match1<partonAK8JetMatchDR && foundFirst<partonAK8JetMatchDR) || (match2<partonAK8JetMatchDR && foundSecond<partonAK8JetMatchDR)){
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

   if (!(foundFirst<partonAK8JetMatchDR) || !(foundSecond<partonAK8JetMatchDR)) {
     std::cout << "WARNING! DID NOT MATCH BOTH PARTONS WITH AN AK8 JET WITHIN: "<< partonAK8JetMatchDR<<" dR"<<std::endl;
   }
   if (firstPartonAK8GenJet!=0){
     myEvent.firstPartonAK8JetEtVal = firstPartonAK8GenJet->et();
     myEvent.firstPartonAK8JetEtHadronicVal = firstPartonAK8GenJet->hadEnergy();
     myEvent.firstPartonAK8JetEtEMVal = firstPartonAK8GenJet->emEnergy();
     myEvent.firstPartonAK8JetEtInvisibleVal = firstPartonAK8GenJet->invisibleEnergy();
     myEvent.firstPartonAK8JetEtaVal = firstPartonAK8GenJet->eta();
     myEvent.firstPartonAK8JetPhiVal = firstPartonAK8GenJet->phi();
     myEvent.dRparton1AK8jetVal= foundFirst;
   }
   if (secondPartonAK8GenJet!=0 && secondPartonAK8GenJet!=firstPartonAK8GenJet){   
     myEvent.secondPartonAK8JetEtVal = secondPartonAK8GenJet->et();
     myEvent.secondPartonAK8JetEtHadronicVal = secondPartonAK8GenJet->hadEnergy();
     myEvent.secondPartonAK8JetEtEMVal = secondPartonAK8GenJet->emEnergy();
     myEvent.secondPartonAK8JetEtInvisibleVal = secondPartonAK8GenJet->invisibleEnergy();
     myEvent.secondPartonAK8JetEtaVal = secondPartonAK8GenJet->eta();
     myEvent.secondPartonAK8JetPhiVal = secondPartonAK8GenJet->phi();
     myEvent.dRparton2AK8jetVal= foundSecond;
   }
   else if (secondPartonAK8GenJet!=0){
     myEvent.dRparton2AK8jetVal= foundSecond;
   }


   myEvent.myGenJets = myGenJets;
   myEvent.myAK8GenJets = myAK8GenJets;
   myEvent.myGenPartons = myGenPartons;
   myEvent.myGenMuons = myGenMuons;
   
   
   return true;
}

bool cmsWRextension::passExtensionGEN(const edm::Event& iEvent, eventBits& myEvent) {
 // std::cout <<myEvent.myGenMuons.size() << " "<<myEvent.myAK8GenJets.size() << std::endl;
  if(myEvent.myGenMuons.size() < 2 || myEvent.myAK8GenJets.size() < 1) {
    std::cout << "EVENT FAILS, NOT ENOUGH TO RECONSTRUCT " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myAK8GenJets.size()<<" ak8 jets"<< std::endl;
    return false;
  }
 // std::cout <<"SORTING JETS" <<std::endl;
  std::sort(myEvent.myAK8GenJets.begin(),myEvent.myAK8GenJets.end(),::wrTools::compareEtJetPointer);
  std::cout<<"There are "<<myEvent.myAK8GenJets.size()<<" AK8GenJets selected"<<std::endl;

  bool Muon2included=false;
  Muon2included = ::wrTools::particleInGenJet(myEvent.myGenMuons[1], myEvent.myAK8GenJets[0]);
  
  if(Muon2included) {
    myEvent.leadAK8JetMuonMassVal = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).mass();
    myEvent.leadAK8JetMuonPtVal   = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).pt();
    myEvent.leadAK8JetMuonEtaVal  = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).eta();
  }
  else{
    myEvent.leadAK8JetMuonMassVal = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenMuons[1]->p4()).mass();
    myEvent.leadAK8JetMuonPtVal   = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenMuons[1]->p4()).pt();
    myEvent.leadAK8JetMuonEtaVal  = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenMuons[1]->p4()).eta();
  }
  return true;
}
bool cmsWRextension::passExtensionRECO(const edm::Event& iEvent, eventBits& myRECOevent) {
  //LOOP OVER JET MUON PAIRS AND TAKE THE HIGHEST MASS ONE
  std::sort(myRECOevent.myMuonJetPairs.begin(),myRECOevent.myMuonJetPairs.end(),::wrTools::comparePairMassPointer);
  myRECOevent.leadAK8JetMuonMassVal = (myRECOevent.myMuonJetPairs[0].first->p4() + myRECOevent.myMuonJetPairs[0].second->p4()).mass(); 
  myRECOevent.leadAK8JetMuonPtVal   = (myRECOevent.myMuonJetPairs[0].first->p4() + myRECOevent.myMuonJetPairs[0].second->p4()).pt(); 
  myRECOevent.leadAK8JetMuonEtaVal  = (myRECOevent.myMuonJetPairs[0].first->p4() + myRECOevent.myMuonJetPairs[0].second->p4()).eta();  
  myRECOevent.leadAK8JetMuonPhiVal  = (fabs(myRECOevent.myMuonJetPairs[0].first->phi() - myRECOevent.myMuonJetPairs[0].second->phi()));  
  std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;
  return true;
}
bool cmsWRextension::passWR2016RECO(const edm::Event& iEvent, eventBits& myEvent) {

 // std::cout <<myEvent.myGenMuons.size() << " "<<myEvent.myGenJets.size() << std::endl;
  if(myEvent.myGenMuons.size() < 2 || myEvent.myGenJets.size() < 2) {
    std::cout << "EVENT FAILS WR2016, NOT ENOUGH TO RECONSTRUCT " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myGenJets.size()<<" jets"<< std::endl;
    return false;
  }

  std::sort(myEvent.myGenJets.begin(),myEvent.myGenJets.end(),::wrTools::compareEtJetPointer);

  std::vector<const reco::GenJet*> mySelectedJets;
  std::vector<const reco::GenParticle*> myPreSelectedMuons;
  std::vector<const reco::GenParticle*> mySelectedMuons;

  bool foundpair=false;
  for (std::vector<const reco::GenJet*>::iterator iJet = myEvent.myGenJets.begin(); iJet != myEvent.myGenJets.end(); iJet++) {
    if ((*iJet)->et()<40.0 || fabs((*iJet)->eta())>2.4) continue;
    for (std::vector<const reco::GenJet*>::iterator iJet2 = iJet+1; iJet2 != myEvent.myGenJets.end(); iJet2++) {
      if ((*iJet)->et()<40.0 || fabs((*iJet)->eta())>2.4) continue;
      if (sqrt(deltaR2(*(*iJet),*(*iJet2)))<0.4) continue;
      foundpair=true;
      mySelectedJets.push_back((*iJet));
      mySelectedJets.push_back((*iJet2));
      break;
    }
    if (foundpair) break;
  }
  if (!foundpair){
    std::cout << "Event fails WR2016, no Jet pair is found" << std::endl;
    return false;
  }
  if (mySelectedJets.size()!=2){
    std::cout << "ERROR, BUG ON WR2016 NUMBER OF JETS. I SHOULDN'T BE ABLE TO GET THIS ERROR" << std::endl;
    return false;
  }

  for (std::vector<const reco::GenParticle*>::iterator iMuon = myEvent.myGenMuons.begin(); iMuon != myEvent.myGenMuons.end(); iMuon++) {
    if ((*iMuon)->et()<53 || fabs((*iMuon)->eta())>2.4) continue;
    if (sqrt(deltaR2(*(*iMuon),*mySelectedJets[0]))<0.4) continue;
    if (sqrt(deltaR2(*(*iMuon),*mySelectedJets[1]))<0.4) continue;
    myPreSelectedMuons.push_back((*iMuon));
  }
  if (myPreSelectedMuons.size()<2){
    std::cout << "Event fails WR2016, was unable to get 2 muons." << std::endl;
    return false;
  }
  foundpair=false;
  for (std::vector<const reco::GenParticle*>::iterator iMuon = myPreSelectedMuons.begin(); iMuon != myPreSelectedMuons.end(); iMuon++) {
    if ((*iMuon)->et()<60) continue;
     for (std::vector<const reco::GenParticle*>::iterator iMuon2 = iMuon+1; iMuon2 != myPreSelectedMuons.end(); iMuon2++) {
      if (sqrt(deltaR2(*(*iMuon),*(*iMuon2)))<0.4) continue;
      foundpair=true;
      mySelectedMuons.push_back((*iMuon));
      mySelectedMuons.push_back((*iMuon2));
      break;
    }
    if (foundpair) break;
  }
  if (!foundpair){
    std::cout << "Event fails WR2016, not Muon pair is found" << std::endl;
    return false;
  }
  if (mySelectedMuons.size()!=2){
    std::cout << "ERROR, BUG ON WR2016 NUMBER OF MUONS. I SHOULDN'T BE ABLE TO GET THIS ERROR" << std::endl;
    return false;
  }

  myEvent.leadSubleadingJetsMuonsMassVal = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).mass();
  myEvent.leadSubleadingJetsMuonsPtVal   = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).pt();
  myEvent.leadSubleadingJetsMuonsEtaVal  = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).eta();
  
  return true;
}

bool cmsWRextension::passWR2016GEN(const edm::Event& iEvent, eventBits& myEvent) {

 // std::cout <<myEvent.myGenMuons.size() << " "<<myEvent.myGenJets.size() << std::endl;
  if(myEvent.myGenMuons.size() < 2 || myEvent.myGenJets.size() < 2) {
    std::cout << "EVENT FAILS WR2016, NOT ENOUGH TO RECONSTRUCT " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myGenJets.size()<<" jets"<< std::endl;
    return false;
  }

  std::sort(myEvent.myGenJets.begin(),myEvent.myGenJets.end(),::wrTools::compareEtJetPointer);

  std::vector<const reco::GenJet*> mySelectedJets;
  std::vector<const reco::GenParticle*> myPreSelectedMuons;
  std::vector<const reco::GenParticle*> mySelectedMuons;

  bool foundpair=false;
  for (std::vector<const reco::GenJet*>::iterator iJet = myEvent.myGenJets.begin(); iJet != myEvent.myGenJets.end(); iJet++) {
    if ((*iJet)->et()<40.0 || fabs((*iJet)->eta())>2.4) continue;
    for (std::vector<const reco::GenJet*>::iterator iJet2 = iJet+1; iJet2 != myEvent.myGenJets.end(); iJet2++) {
      if ((*iJet)->et()<40.0 || fabs((*iJet)->eta())>2.4) continue;
      if (sqrt(deltaR2(*(*iJet),*(*iJet2)))<0.4) continue;
      foundpair=true;
      mySelectedJets.push_back((*iJet));
      mySelectedJets.push_back((*iJet2));
      break;
    }
    if (foundpair) break;
  }
  if (!foundpair){
    std::cout << "Event fails WR2016, not Jet pair is found" << std::endl;
    return false;
  }
  if (mySelectedJets.size()!=2){
    std::cout << "ERROR, BUG ON WR2016 NUMBER OF JETS. I SHOULDN'T BE ABLE TO GET THIS ERROR" << std::endl;
    return false;
  }

  for (std::vector<const reco::GenParticle*>::iterator iMuon = myEvent.myGenMuons.begin(); iMuon != myEvent.myGenMuons.end(); iMuon++) {
    if ((*iMuon)->et()<53 || fabs((*iMuon)->eta())>2.4) continue;
    if (sqrt(deltaR2(*(*iMuon),*mySelectedJets[0]))<0.4) continue;
    if (sqrt(deltaR2(*(*iMuon),*mySelectedJets[1]))<0.4) continue;
    myPreSelectedMuons.push_back((*iMuon));
  }
  if (myPreSelectedMuons.size()<2){
    std::cout << "Event fails WR2016, was unable to get 2 muons." << std::endl;
    return false;
  }
  foundpair=false;
  for (std::vector<const reco::GenParticle*>::iterator iMuon = myPreSelectedMuons.begin(); iMuon != myPreSelectedMuons.end(); iMuon++) {
    if ((*iMuon)->et()<60) continue;
     for (std::vector<const reco::GenParticle*>::iterator iMuon2 = iMuon+1; iMuon2 != myPreSelectedMuons.end(); iMuon2++) {
      if (sqrt(deltaR2(*(*iMuon),*(*iMuon2)))<0.4) continue;
      foundpair=true;
      mySelectedMuons.push_back((*iMuon));
      mySelectedMuons.push_back((*iMuon2));
      break;
    }
    if (foundpair) break;
  }
  if (!foundpair){
    std::cout << "Event fails WR2016, not Muon pair is found" << std::endl;
    return false;
  }
  if (mySelectedMuons.size()!=2){
    std::cout << "ERROR, BUG ON WR2016 NUMBER OF MUONS. I SHOULDN'T BE ABLE TO GET THIS ERROR" << std::endl;
    return false;
  }

  myEvent.leadSubleadingJetsMuonsMassVal = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).mass();
  myEvent.leadSubleadingJetsMuonsPtVal   = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).pt();
  myEvent.leadSubleadingJetsMuonsEtaVal  = (mySelectedJets[0]->p4() + mySelectedJets[1]->p4() + mySelectedMuons[0]->p4() + mySelectedMuons[1]->p4()).eta();
  
  return true;
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
   if(m_doReco) {
     m_eventsPassingExtensionRECO.book((fs->mkdir("eventsPassingExtensionRECO")), 1);
     m_eventsPassingWR2016RECO.book((fs->mkdir("eventsPassingWR2016RECO")), 1);
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
