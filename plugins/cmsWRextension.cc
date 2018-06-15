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
#include "ExoAnalysis/cmsWRextensions/interface/eventHistos.h"
#include "ExoAnalysis/cmsWRextensions/interface/eventBits.h"
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"

// system include files
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>

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

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"


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
  m_highMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("highMuons"))),
  m_highElectronToken (consumes<std::vector<pat::Electron>> (iConfig.getParameter<edm::InputTag>("highElectrons"))),
  m_regMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("regMuons"))),
  m_recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("recoJets"))),
  m_AK8recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoJets"))),
  m_offlineVerticesToken (consumes<std::vector<reco::Vertex>> (iConfig.getParameter<edm::InputTag>("vertices"))),
  m_metToken (consumes<std::vector<pat::MET>> (iConfig.getParameter<edm::InputTag>("met"))),
  m_wantHardProcessMuons (iConfig.getUntrackedParameter<bool>("wantHardProcessMuons",true)),
  m_doGen (iConfig.getUntrackedParameter<bool>("doGen",false)),
  m_doReco (iConfig.getUntrackedParameter<bool>("doReco",true)),
  m_isMC (iConfig.getUntrackedParameter<bool>("isMC",true)),
  m_doTrig (iConfig.getUntrackedParameter<bool>("doTrig",false)),
  m_MCL (iConfig.getUntrackedParameter<double>("MCL", 400)),
  m_MCU (iConfig.getUntrackedParameter<double>("MCU", 8000)),
  m_flavorSideband (iConfig.getUntrackedParameter<bool>("flavorSideband", false))  //SOON TO BE DEPRECATED

{
   //now do what ever initialization is needed
  std::cout << "CONSTRUCTION" << std::endl;
  //usesResource("TFileService");
  if (m_doTrig) {
    std::cout << "CREATING TRIGGER TOKENS" << std::endl;
    m_trigResultsToken = consumes<edm::TriggerResults> (iConfig.getParameter<edm::InputTag>("trigResults"));
    m_trigObjsToken    = consumes<std::vector<pat::TriggerObjectStandAlone> > (iConfig.getParameter<edm::InputTag>("trigObjs"));
    m_muonPathsToPass   = iConfig.getParameter<std::vector<std::string> >("muonPathsToPass");
    m_muonFiltersToPass = iConfig.getParameter<std::vector<std::string> >("muonFiltersToPass");
    m_electronPathsToPass   = iConfig.getParameter<std::vector<std::string> >("electronPathsToPass");
    m_electronFiltersToPass = iConfig.getParameter<std::vector<std::string> >("electronFiltersToPass");
    m_genericTriggerEventFlag = new GenericTriggerEventFlag( iConfig, consumesCollector(), *this );
  }
  if (m_isMC){
    std::cout << "CREATING GEN EVENT INFO TOKENS" << std::endl;
    m_genEventInfoToken =consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>("genInfo"));
    m_amcatnlo = iConfig.getUntrackedParameter<bool>("amcatnlo",false);
  }
  if (m_isMC && m_doGen){
    std::cout << "CREATING GEN PARTICLE TOKENS" << std::endl;
    m_genParticleToken = consumes<std::vector<reco::GenParticle>> (iConfig.getParameter<edm::InputTag>("genParticles"));
    m_genJetsToken      =consumes<std::vector<reco::GenJet>> (iConfig.getParameter<edm::InputTag>("genJets"));
    m_AK8genJetsToken   =consumes<std::vector<reco::GenJet>> (iConfig.getParameter<edm::InputTag>("AK8genJets"));
  }
}


cmsWRextension::~cmsWRextension() {
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
   if(m_doTrig)
     delete m_genericTriggerEventFlag;

}


//
// member functions
//
// ------------ method called for each event  ------------
void cmsWRextension::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::cout << "Beginning event analysis:" << std::endl;
  eventBits myEvent;
  eventBits myRECOevent;
  myEvent.flavorSideband = m_flavorSideband;
  myRECOevent.flavorSideband = m_flavorSideband;

  bool pass2016 = false;
  bool ZMASS = false;
  bool addMuons = false;

  setEventWeight(iEvent, myEvent);
  setEventWeight(iEvent, myRECOevent);

  myRECOevent.cutProgress = 0;
  if(m_doTrig) passTrig(iEvent, myRECOevent);

  if(m_isMC && m_doGen) {
    genCounter(iEvent, myEvent);
    genCounter(iEvent, myRECOevent);
  }
   
  if (m_doGen && m_isMC && !m_flavorSideband) {
    if(preSelectGen(iEvent, myEvent)) {
      std::cout << "plotting all events" << std::endl;
      std::cout << "analyzing wr2016" << std::endl;
      pass2016 = passWR2016GEN(iEvent, myEvent);
      if(pass2016) m_eventsPassingWR2016.fill(myEvent);
      std::cout << "analyzing extension" << std::endl;
      if(passExtensionGEN(iEvent, myEvent)) m_eventsPassingExtension.fill(myEvent);
    }
  }
  if (m_doReco || !m_isMC) {
    std::cout<<"running preselection reco"<<std::endl;
    if(preSelectReco(iEvent, myRECOevent)) {
      myRECOevent.cutProgress++;
      if(passExtensionRECO(iEvent, myRECOevent)) { 
        myRECOevent.cutProgress++;
        METcuts(iEvent, myRECOevent);
        if(!pass2016) {
          myRECOevent.cutProgress++;
          ZMASS = subLeadingMuonZMass(iEvent, myRECOevent);
          addMuons = additionalMuons(iEvent, myRECOevent, false);
          if(!addMuons) {
            m_eventsPassingExtensionRECO2016VETOSINGLEMUON.fill(myRECOevent);
          }
          if(ZMASS) {
            m_eventsPassingExtensionRECO2016VETOZMASS.fill(myRECOevent);          
          } else if ((m_isMC || m_flavorSideband) && addMuons){
            myRECOevent.cutProgress++;
            std::cout << "HERE WE FILL THE GOOD STUFF" << std::endl;
            m_eventsPassingExtensionRECO2016VETO.fill(myRECOevent);
            if(massCut(iEvent, myRECOevent)) {
              m_eventsPassingExtensionRECO2016VETOMASSCUT.fill(myRECOevent);
              myRECOevent.cutProgress++;
              if(metCuts(iEvent, myRECOevent))
                m_eventsPassingExtensionRECO2016VETOMASSMETCUT.fill(myRECOevent);
            }
          }
        }
        if (m_isMC || m_flavorSideband) m_eventsPassingExtensionRECO.fill(myRECOevent);
        //std::cout <<"rECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;
        std::cout << "PASSED RECO EXTENSION, FILLING" << std::endl;
      }
    }
    if(passFlavorSideband(iEvent, myRECOevent)) m_eventsPassingFlavorSidebandRECO.fill(myRECOevent);
    //if(passWR2016Reco(iEvent,myRECOevent)) m_eventsPassingWR2016RECO.fill(myRECOevent);
  }
  m_allEvents.fill(myRECOevent);
}
void cmsWRextension::setEventWeight(const edm::Event& iEvent, eventBits& myEvent) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);
      if(!m_amcatnlo) {
        myEvent.weight = eventInfo->weight();
        myEvent.count = 1;
      }
      else {
        myEvent.weight = eventInfo->weight()/fabs(eventInfo->weight());
        myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      }
  } else {
      myEvent.weight = 1;
      myEvent.count = 1;
  }


}
bool cmsWRextension::passTrig(const edm::Event& iEvent, eventBits& myRECOevent) {
  std::cout <<"checking trigger paths "<<std::endl;
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByToken(m_trigResultsToken, triggerResults);
  std::cout <<"grabbing trigger names"<<std::endl;
  const edm::TriggerNames& trigNames = iEvent.triggerNames(*triggerResults); //SEGFAULT
  std::cout <<"grabbing trigger results "<<std::endl;
  std::cout <<"looping over paths to pass"<<std::endl;
  for(auto& pathName : m_muonPathsToPass){
    //we need to figure out which path index the pathName corresponds too
    size_t pathIndx = trigNames.triggerIndex(pathName);
    if(pathIndx>=trigNames.size()) std::cout <<" path "<<pathName<<" not found in menu"<<std::endl;
    else{
      if(triggerResults->accept(pathIndx)) {
        std::cout <<" path "<<pathName<<" passed"<<std::endl;
      } else {
        std::cout <<" path "<<pathName<<" failed"<<std::endl;
        return false;   //ALL PATHS MUST PASS
      }
    }
  }

//  std::cout <<"checking eles "<<std::endl;
//  edm::Handle<std::vector<pat::Electron>> highLeptons;
//  iEvent.getByToken(m_highLeptonToken, highLeptons);
//
//  edm::Handle<std::vector<pat::TriggerObjectStandAlone> > trigObjsHandle;
//  iEvent.getByToken(m_trigObjsToken, trigObjsHandle);
//
//  for(std::vector<pat::Electron>::const_iterator iLep = highLeptons->begin(); iLep != highLeptons->end(); iLep++) {
//    ::wrTools::checkFilters(iLep->superCluster()->eta(),iLep->superCluster()->phi(),*trigObjsHandle,m_filtersToPass);
//  }
//
  return true;


}
bool cmsWRextension::passFlavorSideband(const edm::Event& iEvent, eventBits& myRECOevent) {

  std::cout<< "BEGINNING SELECTION ON FLAVOR SIDEBAND" << std::endl;

  if( myRECOevent.myJetCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myJetCands.size()<<" JETS FOUND." << std::endl;
    return false;
  }

  if( myRECOevent.myElectronCandsHighPt.size() < 1 ){
    std::cout<< "EVENTS FAILS, NO ELECTRONS OVER 200 GEV WITHIN ACCEPTANCE. " << myRECOevent.myElectronCandsHighPt.size()<< " ELECTRONS FOUND." << std::endl;
    return false;
  }

  additionalMuons(iEvent, myRECOevent, true);

  if( myRECOevent.myMuonCands.size() < 1){
    std::cout<< "EVENTS FAILS, NO MUONS OVER 10 GEV WITHIN ACCEPTANCE. " << myRECOevent.myMuonCands.size()<< " MUONS FOUND." << std::endl;
    return false;
  }
  //BUILD PAIRS OF AK8 JETS WITH THE LEAD ELECTRON
  std::vector<std::pair<const pat::Jet*, const pat::Electron*>> electronJetPairs;
  for(std::vector<const pat::Jet*>::const_iterator iJet = myRECOevent.myJetCandsHighPt.begin(); iJet != myRECOevent.myJetCandsHighPt.end(); iJet++) {
     if(fabs(reco::deltaPhi((*iJet)->phi(), myRECOevent.myElectronCand->phi())) < 2.0 ) continue;
     electronJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
  }
  if( electronJetPairs.size() < 1 ) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET ELECTRON PAIRS" <<std::endl;
    return false;
  }
  return true;
}

bool cmsWRextension::preSelectReco(const edm::Event& iEvent, eventBits& myRECOevent) {
  METselection(iEvent, myRECOevent);
  muonSelection(iEvent, myRECOevent);
  electronSelection(iEvent, myRECOevent);
  jetSelection(iEvent, myRECOevent); 

  if( myRECOevent.myJetCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myJetCands.size()<<" JETS FOUND." << std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  if( myRECOevent.myMuonCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO MUONS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myMuonCandsHighPt.size()<<" MUONS FOUND." << std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  //BUILD PAIRS OF AK8 JETS WITH THE LEAD MUON
  std::vector<std::pair<const pat::Jet*, const pat::Muon*>> muonJetPairs; 
  for(std::vector<const pat::Jet*>::const_iterator iJet = myRECOevent.myJetCandsHighPt.begin(); iJet != myRECOevent.myJetCandsHighPt.end(); iJet++) {
    //if( ((*iJet)->p4() + (*iMuon)->p4()).mass() < 400) continue;
    //if (sqrt(deltaR2(*(*iJet),*(*iMuon)))<2.0) continue;
    if(fabs(reco::deltaPhi((*iJet)->phi(), myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  if( muonJetPairs.size() < 1 ) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET LEPTON PAIRS" <<std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  std::cout<<muonJetPairs.size()<<" Pairing CANDIDATES Selected from "<<myRECOevent.myJetCandsHighPt.size()<<" jets"<<std::endl;
  myRECOevent.myMuonJetPairs = muonJetPairs;
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
  
  std::cout << "Checking if event passes WR2016" << std::endl;
  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);

  edm::Handle<std::vector<pat::Jet>> recoJets;
  iEvent.getByToken(m_recoJetsToken, recoJets);

  if(highMuons->size() < 2 || recoJets->size() < 2) {
    std::cout << "EVENT FAILS WR2016, NOT ENOUGH TO RECONSTRUCT " << highMuons->size()<<" muons "<<  recoJets->size()<<" jets"<< std::endl;
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

  for (std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {
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

  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);

  for (std::vector<pat::Muon>::const_iterator iParticle = highMuons->begin(); iParticle != highMuons->end(); iParticle++) {
    std::cout<<iParticle->pt()<<std::endl;
    //NEEDS UPDATING
  }


  return false;
}
bool cmsWRextension::METselection(const edm::Event& iEvent, eventBits& myEvent) {
  edm::Handle<std::vector<pat::MET>> mets;
  iEvent.getByToken(m_metToken, mets);
  if (mets->size() < 1) return false;
  myEvent.myMET = &(mets->at(0));
  return true; 



}
bool cmsWRextension::subLeadingMuonZMass(const edm::Event& iEvent, eventBits& myEvent) {  //THIS SELECTION IS A SIDEBAND BASED OF THE MUON FLAVOR SELECTION ONLY
  //CHECK IF WE HAVE A SUBLEADING MUON
  if(myEvent.mySubleadMuon == 0) return false;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const pat::Jet*  selJet      = myEvent.myMuonJetPairs[0].first;

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi()));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi())); 
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();

  if(myEvent.subleadMuon_selMuonMass < 200) return true;
  return false;
}
bool cmsWRextension::METcuts(const edm::Event& iEvent, eventBits& myEvent) {
  const pat::MET*  met         = myEvent.myMET;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const pat::Jet*  selJet      = myEvent.myMuonJetPairs[0].first;

  myEvent.MET             = met->et();
  myEvent.MET_selJetdPhi  = fabs(reco::deltaPhi(met->phi(),selJet->phi()));
  myEvent.MET_selMuondPhi = fabs(reco::deltaPhi(met->phi(),selMuon->phi()));
  myEvent.MET_selJetMass  = (met->p4()+selJet->p4()).mass();
  myEvent.MET_selMuonMass = (met->p4()+selMuon->p4()).mass();
  myEvent.MET_selJetPt    = (met->p4()+selJet->p4()).pt();
  myEvent.MET_selMuonPt   = (met->p4()+selMuon->p4()).pt();

  myEvent.selectedJetTransMET = met->et() * sin(fabs(reco::deltaPhi(selJet->phi(),met->phi())));
  return true;
}
//CHECK ADDITIONAL MUONS

bool cmsWRextension::additionalMuons(const edm::Event& iEvent, eventBits& myEvent, bool flavorSideband) {
  std::cout << "Round-up of the rag-tag muons" << std::endl;
  edm::Handle<std::vector<pat::Muon>> regMuons;
  iEvent.getByToken(m_regMuonToken, regMuons);
  std::vector<const pat::Muon*> allMuons;

  for(std::vector<pat::Muon>::const_iterator iMuon = regMuons->begin(); iMuon != regMuons->end(); iMuon++) {
    if ( iMuon->pt() < 10 || fabs(iMuon->eta()) > 2.4) continue;  //10 GeV is designed to capture slow muons from Z->MUMU
    allMuons.push_back(&(*iMuon));
  }
  myEvent.muons10 = allMuons.size();
  if (myEvent.muons10 > 1) {
    std::sort(allMuons.begin(),allMuons.end(),::wrTools::compareEtCandidatePointer); 
  }
  if (myEvent.muons10 > 0) {
    myEvent.myMuonCands = allMuons; 
  }
  if(myEvent.muons10 < 2) return false;  //The leading muon should also pass these cuts, so an additional muon would mean 2 or more

  if(flavorSideband==true) {
    myEvent.mySubleadMuon = allMuons.at(0);
  }else{
  //IF WE HAVE ADDITION MUONS, WE SHOULD SEE WHICH IS THE LEADING MUON WHICH ISN'T THE MAIN CANDIDATE
    for(std::vector<const pat::Muon*>::iterator iMuon = myEvent.myMuonCands.begin(); iMuon != myEvent.myMuonCands.end(); iMuon++) {
      if(fabs(reco::deltaPhi((*iMuon)->phi(), myEvent.myMuonCand->phi())) > 0.01) {
        myEvent.mySubleadMuon = *iMuon;
        break;
      }
    }
  }
  if(myEvent.mySubleadMuon == 0) return false;  //THIS SHOULD BE IMPOSSIBLE

  return true;
}

bool cmsWRextension::electronSelection(const edm::Event& iEvent, eventBits& myEvent) {  //Flavor sideband
  std::cout<<"STARTING ELECTRON SELECTION"<<std::endl;
  std::vector<const pat::Electron*> highPTelectrons;

  edm::Handle<std::vector<pat::TriggerObjectStandAlone> > trigObjsHandle;
  if(m_doTrig) {
    iEvent.getByToken(m_trigObjsToken, trigObjsHandle);
  }


  std::cout << "Looking for a few good electrons" << std::endl;
  edm::Handle<std::vector<pat::Electron>> highElectrons;
  iEvent.getByToken(m_highElectronToken, highElectrons);

  for(std::vector<pat::Electron>::const_iterator iElec = highElectrons->begin(); iElec != highElectrons->end(); iElec++) {
    //PHASE SPACE CUTS
    if( iElec->pt() < m_highPTleptonCut || fabs(iElec->eta()) > m_leptonEtaCut ) continue;
    //TRIGGER CUTS
    if(m_doTrig)
      if (! ::wrTools::checkFilters(iElec->superCluster()->eta(),iElec->superCluster()->phi(),*trigObjsHandle,m_electronFiltersToPass) ) continue;
    //HEEP CUTS
    //this will be null if its not present
    const vid::CutFlowResult* vidResult =  iElec->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");
    if(vidResult == NULL) {
      std::cout << "ERROR CANNOT FIND ELECTRON VID RESULTS" << std::endl;
      return false;
    }
    //how to check if everything passed:
    const bool heepIDVID = vidResult->cutFlowPassed();

    //IT PASSED EVERYTHING
    highPTelectrons.push_back(&(*iElec));
    std::cout<<"ELECTRON CAND WITH PT,ETA,PHI: "<<iElec->pt()<<","<<iElec->eta()<<","<<iElec->phi()<<std::endl;
  }
      //if( iLep->pt() > 200 ) highPTMuons.push_back(&(*iLep));
  //COLLECT MUONS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  myEvent.electronCands = highPTelectrons.size();
  if (myEvent.electronCands > 1) {
    std::sort(highPTelectrons.begin(),highPTelectrons.end(),::wrTools::compareEtCandidatePointer); 
  }
  if (myEvent.electronCands < 1) {
    return false;
  } else {
    myEvent.myElectronCandsHighPt = highPTelectrons;
    //We select the lead muon in the event
    myEvent.myElectronCand = highPTelectrons[0];
  }
  return true;
}
bool cmsWRextension::muonSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout<<"STARTING MUON SELECTION"<<std::endl;
  std::vector<const pat::Muon*> highPTMuons;

  edm::Handle<std::vector<pat::TriggerObjectStandAlone> > trigObjsHandle;
  if(m_doTrig) {
    iEvent.getByToken(m_trigObjsToken, trigObjsHandle);
  }
  
  std::cout << "Looking for a few good muons" << std::endl;
  std::cout << "Muon Handle" << std::endl;
  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);
  std::cout << "Vertex Handle" << std::endl;

  edm::Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(m_offlineVerticesToken, vertices);
  
  for(std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {
    std::cout << "Looping through muons" << std::endl;
    if( iMuon->pt() < 40 || fabs(iMuon->eta()) > 2.4 ) continue; //preliminary pt cut to speed the loop, and the eta cut
    if(m_doTrig) {
      std::cout << "Checking Filter" << std::endl;
      if (! ::wrTools::checkFilters(iMuon->eta(),iMuon->phi(),*trigObjsHandle,m_muonFiltersToPass) ) continue;
      
    }
    if(( iMuon->isHighPtMuon(vertices->at(0)) && iMuon->tunePMuonBestTrack()->pt() > m_highPTleptonCut) && (iMuon->isolationR03().sumPt/iMuon->pt() <= .05)) {
      std::cout<<"LEPTON CAND WITH PT,ETA,PHI: "<<iMuon->pt()<<","<<iMuon->eta()<<","<<iMuon->phi()<<std::endl;
     
      highPTMuons.push_back(&(*iMuon));
    }
    //if( iMuon->pt() > 200 ) highPTMuons.push_back(&(*iMuon));

  }

  myEvent.muonCands = highPTMuons.size();
  //COLLECT MUONS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  if (myEvent.muonCands > 1) {
    
    std::sort(highPTMuons.begin(),highPTMuons.end(),::wrTools::compareEtCandidatePointer); 
  }
  if (myEvent.muonCands < 1) {
    return false;
  } else {
    myEvent.myMuonCandsHighPt = highPTMuons;
    //We select the lead muon in the event
    myEvent.myMuonCand = highPTMuons[0];
  }
  return true;

}
bool cmsWRextension::jetSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout<<"STARTING JET SELECTION"<<std::endl;
  edm::Handle<std::vector<pat::Jet>> recoJetsAK8;
  iEvent.getByToken(m_AK8recoJetsToken, recoJetsAK8);
  std::vector<const pat::Jet*> highPTJets;
  std::vector<const pat::Jet*> allJets;
  //COLLECT JetS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  for(std::vector<pat::Jet>::const_iterator iJet = recoJetsAK8->begin(); iJet != recoJetsAK8->end(); iJet++) {
    std::cout<<"looping over jets"<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
    //GETS ALL THE RELEVANT JET ID QUANTITIES
    double NHF  =                iJet->neutralHadronEnergyFraction();
    double NEMF =                iJet->neutralEmEnergyFraction();
    double CHF  =                iJet->chargedHadronEnergyFraction();
    double MUF  =                iJet->muonEnergyFraction();
    double CEMF =                iJet->chargedEmEnergyFraction();
    double NumConst =            iJet->chargedMultiplicity()+iJet->neutralMultiplicity();
    double NumNeutralParticles = iJet->neutralMultiplicity();
    double CHM      =            iJet->chargedMultiplicity(); 
    //APPLYING ETA AND PT CUT
    if ( iJet->pt() < 40 ) continue;
    if ( fabs(iJet->eta()) > 2.4) continue;
    //APPLYING TIGHT AND LEPTON VETO QUALITY CUTS
    if (NHF > .9) continue;
    if (NEMF > .9) continue;
    if (NumConst <= 1) continue;
    if (MUF > .8 ) continue;
    //ADDITIONAL CUTS BECAUSE OF TIGHT ETA CUT
    if (CHF == 0) continue;
    if (CHM == 0) continue;
    if (CEMF > .9) continue;
    //ANALYSIS SPECIFIC CUTS
   // if (MUF <= .05) continue;
    //JETS PASSING CUTS
    allJets.push_back(&(*iJet));
    //JETS PASSING WITH VERY HIGH PT
    if( iJet->pt() > 200 ){ 
      highPTJets.push_back(&(*iJet));
      std::cout<<"AK8JET CAND WITH PT,ETA,PHI: "<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
    }
  } 
  if (allJets.size() == 0) return false;
  std::sort(highPTJets.begin(),highPTJets.end(),::wrTools::compareEtCandidatePointer); 
  std::sort(allJets.begin(),allJets.end(),::wrTools::compareEtCandidatePointer); 

  myEvent.myJetCandsHighPt = highPTJets;
  myEvent.myJetCands = allJets;

  myEvent.ak8jetCands = highPTJets.size();
  myEvent.ak8jets40 = myEvent.myJetCands.size();


  return true;
}
bool cmsWRextension::genCounter(const edm::Event& iEvent, eventBits& myEvent)
{
  edm::Handle<std::vector<reco::GenParticle>> genParticles;
  iEvent.getByToken(m_genParticleToken, genParticles);

  //GENERIC EVENT CHARACTERIZER
  int nLeptons         = 0;
  int nMuons           = 0;
  int nTaus            = 0;
  int nElectrons       = 0; 
  int nLightPartons    = 0;
  int nTops            = 0;
  int nBs              = 0;
  int nPartons         = 0;
  int flavor           = 0;

  for(std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
    if(!iParticle->isHardProcess())   continue;
    if(iParticle->status() == 21)     continue;
    if(abs(iParticle->pdgId()) == 13) nMuons++;
    if(abs(iParticle->pdgId()) == 11) nElectrons++;
    if(abs(iParticle->pdgId()) == 15) nTaus++;
    if(abs(iParticle->pdgId())  <= 4) nLightPartons++;
    if(abs(iParticle->pdgId())  == 5) nBs++;
    if(abs(iParticle->pdgId())  == 6) nTops++;
  }
  nLeptons = nMuons + nTaus + nElectrons;
  nPartons = nTops + nBs + nLightPartons;

  myEvent.mynLeptons         = nLeptons     ;
  myEvent.mynMuons           = nMuons       ;
  myEvent.mynTaus            = nTaus        ;
  myEvent.mynElectrons       = nElectrons   ; 
  myEvent.mynLightPartons    = nLightPartons;
  myEvent.mynTops            = nTops        ;
  myEvent.mynBs              = nBs          ;
  myEvent.mynPartons         = nPartons     ;

//HERE IS THE CHARACTERIZATION DECODER TABLE

  if (nMuons == 2 && nPartons == 2)   flavor = 1;     
  if (nTaus >= 1 && nTops >= 1)       flavor = 2;       
  else if (nTaus >= 1 && nTops == 0)       flavor = 3;
  //else if (mynMuons == 1 && mynElectrons >= 1) flavor = 4;
  else flavor = 9;
  
  myEvent.myEventFlavor = flavor;

  return true;

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




  std::vector<const reco::GenParticle*> myGenParticles;


  //LOOP OVER GEN PARTICLES
  for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
    if(iParticle->isHardProcess() && iParticle->status() != 21) myGenParticles.push_back(&(*iParticle));
    if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
    if(iParticle->mother()) { if(::wrTools::particleIsFromABS(&(*iParticle),24)) continue; }//no W-SM mothered particles
    if((iParticle->isHardProcess() && iParticle->status() == 22) && abs(iParticle->pdgId()) == 6) myGenPartons.push_back(&(*iParticle)); //KEEP TOPS, NOT Qs FROM TOPS
    if((iParticle->isHardProcess() && iParticle->status() == 23) && (iParticle->pdgId() <= 6) && (iParticle->pdgId() >= -6) && !(::wrTools::particleIsFromABS(&(*iParticle),6))) myGenPartons.push_back(&(*iParticle));
    if(iParticle->fromHardProcessFinalState() && abs(iParticle->pdgId()) == 13) myGenMuons.push_back(&(*iParticle));
  }
  //GET THE CHARACTERIZATION INTEGER
  ::wrTools::characterizeEvent(myGenParticles);

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
  myRECOevent.leadAK8JetMuonPhiVal  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs[0].first->phi(), myRECOevent.myMuonJetPairs[0].second->phi())));  
  myRECOevent.selectedMuonEt  = myRECOevent.myMuonJetPairs[0].second->et(); 
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs[0].second->phi(); 
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs[0].second->eta();
  myRECOevent.selectedJetEt   = myRECOevent.myMuonJetPairs[0].first->et(); 
  myRECOevent.selectedJetPhi  = myRECOevent.myMuonJetPairs[0].first->phi(); 
  myRECOevent.selectedJetEta  = myRECOevent.myMuonJetPairs[0].first->eta(); 
  
//  countOffAxisJets(iEvent, myRECOevent);

  if(myRECOevent.myMuonJetPairs[0].first->isPFJet()) {
    myRECOevent.leadAK8JetMuonJetMuonEnergyFraction = myRECOevent.myMuonJetPairs[0].first->muonEnergyFraction();
    std::cout << "CAND PAIR WITH CALOJET" <<std::endl;
  }
  if(myRECOevent.myMuonJetPairs[0].first->isJPTJet()) std::cout << "CAND PAIR WITH JPTJET" <<std::endl;
  if(myRECOevent.myMuonJetPairs[0].first->isPFJet()) std::cout << "CAND PAIR WITH PFJET" <<std::endl;
  if(myRECOevent.myMuonJetPairs[0].first->isBasicJet()) std::cout << "CAND PAIR WITH BASICJET" <<std::endl;
  
  std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;

  myRECOevent.myEventMass = myRECOevent.leadAK8JetMuonMassVal;


  return true;
}
bool cmsWRextension::metCuts(const edm::Event& iEvent, eventBits& myEvent) {
  //if( (myEvent.subleadMuon_selMuondPhi >= 0) && myEvent.subleadMuon_selMuondPhi < 2) return false;
  if( (myEvent.MET_selMuondPhi >= 0 ) && (myEvent.MET_selMuondPhi < 2) ) return false;
  return true;

}
bool cmsWRextension::massCut(const edm::Event& iEvent, eventBits& myEvent) {
  if (myEvent.myEventMass > m_MCL && myEvent.myEventMass < m_MCU) return true;
  return false;
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
// ------------ method called at the beginning of each run -----------------------------
void cmsWRextension::beginRun(const edm::Run& run, const edm::EventSetup& setup )
{
  std::cout << "INITIALIZING RUN SPECIFIC STUFF" << std::endl;
  if(m_doTrig) {
    if ( m_genericTriggerEventFlag->on() ) m_genericTriggerEventFlag->initRun( run, setup );
  }


}

// ------------ method called once each job just before starting event loop  ------------
void 
cmsWRextension::beginJob()
{
  std::cout << "BOOKING PLOTS" << std::endl;
  edm::Service<TFileService> fs; 
  //THESE CASES ARE EXCLUSIVE OF EACH OTHER
  if (m_doGen && m_doReco) {
    std::cout << "BOOKING PLOTS FLAVOR 3" << std::endl;
  //flavor 3

    m_allEvents.book((fs->mkdir("allEvents")), 3);          
    m_eventsPassingWR2016.book((fs->mkdir("eventsPassingWR2016")), 3); 
    m_eventsPassingWR2016RECO.book((fs->mkdir("eventsPassingWR2016RECO")), 3);
    m_eventsPassingExtension.book((fs->mkdir("eventsPassingExtension")), 3);
    m_eventsPassingExtensionRECO.book((fs->mkdir("eventsPassingExtensionRECO")), 3);
    m_eventsPassingExtensionRECO2016VETO.book((fs->mkdir("eventsPassingExtensionRECO2016VETO")), 3);
    m_eventsPassingExtensionRECO2016VETOMASSMETCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSMETCUT"), 3);
    m_eventsPassingExtensionRECO2016VETOMASSCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSCUT"), 3);
    m_eventsPassingExtensionRECO2016VETOZMASS.book((fs->mkdir("eventsPassingExtensionRECO2016VETOZMASS")), 3);
    m_eventsPassingExtensionRECO2016VETOSINGLEMUON.book((fs->mkdir("eventsPassingExtensionRECO2016VETOSINGLEMUON")), 3);
    m_eventsPassingFlavorSidebandRECO.book((fs->mkdir("eventsPassingFlavorSidebandRECO")), 3);
  }
  if (m_doGen && !m_doReco) {
    std::cout << "BOOKING PLOTS FLAVOR 1" << std::endl;
  //flavor 1
    m_allEvents.book((fs->mkdir("allEvents")), 1);          
    m_eventsPassingWR2016.book((fs->mkdir("eventsPassingWR2016")), 1); 
    m_eventsPassingExtension.book((fs->mkdir("eventsPassingExtension")), 1);

  }
  if (!m_doGen && m_doReco) {
    std::cout << "BOOKING PLOTS FLAVOR 2" << std::endl;
  //flavor 2
    m_allEvents.book((fs->mkdir("allEvents")), 2);          
    m_eventsPassingWR2016RECO.book((fs->mkdir("eventsPassingWR2016RECO")), 2);
    m_eventsPassingExtensionRECO.book((fs->mkdir("eventsPassingExtensionRECO")), 2);
    m_eventsPassingExtensionRECO2016VETO.book((fs->mkdir("eventsPassingExtensionRECO2016VETO")), 2);
    m_eventsPassingExtensionRECO2016VETOMASSMETCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSMETCUT"), 2);
    m_eventsPassingExtensionRECO2016VETOMASSCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSCUT"), 2);
    m_eventsPassingExtensionRECO2016VETOZMASS.book((fs->mkdir("eventsPassingExtensionRECO2016VETOZMASS")), 2);
    m_eventsPassingExtensionRECO2016VETOSINGLEMUON.book((fs->mkdir("eventsPassingExtensionRECO2016VETOSINGLEMUON")), 2);
    m_eventsPassingFlavorSidebandRECO.book((fs->mkdir("eventsPassingFlavorSidebandRECO")), 2);

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
