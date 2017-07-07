//local includes
#include "cmsWRextension.h"
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

bool cmsWRextension::preSelectGen(const edm::Event& iEvent, eventBits& myEvent)
{
   using namespace edm;
  
   float partonJetMatchDR = .4;
   float partonAK8JetMatchDR = .8;
 
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

bool cmsWRextension::passExtension(const edm::Event& iEvent, eventBits& myEvent) {
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

bool cmsWRextension::passWR2016(const edm::Event& iEvent, eventBits& myEvent) {

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
