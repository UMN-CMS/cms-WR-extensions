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

   //CHECK THAT THE EVENT MAKES SENSE
   if (myEvent.outgoingPartons.size() != 2 || myEvent.outgoingMuons.size() != 2) {
     std::cout << "ERROR! Found more than 2 partons ("<<myEvent.outgoingPartons.size()<<") or muons("<<myEvent.outgoingMuons.size()<<")."<< std::endl;
   }

   myEvent.setHighestEtParton(0);
   myEvent.setSecondHighestEtParton(1);

   myEvent.setHighestEtMuon(0);
   myEvent.setSecondHighestEtMuon(1);

   //NOW WE'LL CHECK IF IT PASSES SOME BASIC GEN LEVEL CUTS
   if(!myEvent.passesGenCuts()) {
     std::cout << "ERROR! SKIPPING EVENT, LEADING PARTONS AND MUONS NOT OVER 20 GEV"<< std::endl;
     return false;
   }

   //NOW THAT THE GEN MUONS AND PARTONS ARE SORTED OUT, WE'LL MATCH A GENJET TO EACH PARTON
   //FIRST WE'LL GET THE GENJETS THAT HAVE AT LEAST 10 GEV ET
   for (std::vector<reco::GenJet>::const_iterator iJet = genJets->begin(); iJet != genJets->end(); iJet++) {
     myEvent.genJets.push_back(*iJet);
   }
   if( myEvent.genJets.size() < 2 ) {
     std::cout << "ERROR! SKIPPING EVENT, DID NOT FIND AT LEAST 2 JETS"<< std::endl;
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
     myEvent.selectedMuons.push_back(*iParticle);
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
     m_allEvents.book((fs->mkdir("allEvents")), 3);           //(1) GEN ONLY PLOTS FOR NOW
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
