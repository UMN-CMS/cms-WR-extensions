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
   m_recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoJets"))),
   m_AK8recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("recoJets"))),
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
   //  preSelectReco(iEvent, myEvent);
  
}
  
bool cmsWRextension::preSelectReco(const edm::Event& iEvent, eventBits& myEvent) {


  return false;

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
