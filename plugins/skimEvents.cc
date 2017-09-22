#ifndef skimEvents_included
#define skimEvents_included 1
// -*- C++ -*-
//
// Package:    ExoAnalysis/skimEvents
// Class:      skimEvents
// 
/**\class skimEvents skimEvents.cc ExoAnalysis/skimEvents/plugins/skimEvents.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrew Evans
//         Created:  Tue, 05 Sep 2017 15:23:17 GMT
//
//


// system include files
#include <memory>
#include <vector>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//ROOT includes
#include "TH1D.h"

//
//local includes
#include "ExoAnalysis/cmsWRextensions/interface/eventBits.h"
#include "ExoAnalysis/cmsWRextensions/interface/eventHistos.h"
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"

//
// class declaration
//

class skimEvents : public edm::stream::EDFilter<> {
  public:
    explicit skimEvents(const edm::ParameterSet&);
    ~skimEvents();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual bool filter(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

     // ----------member data ---------------------------
    eventHistos m_allEvents;
    edm::EDGetToken m_genEventInfoToken;
    edm::EDGetToken m_recoMuonToken;
    edm::EDGetToken m_AK8recoJetsToken;
    edm::EDGetToken m_metToken;
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
skimEvents::skimEvents(const edm::ParameterSet& iConfig) :
  m_genEventInfoToken (consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>("genInfo"))),
  m_recoMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("recoMuons"))),
  m_AK8recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoJets"))),
  m_metToken (consumes<std::vector<pat::MET>> (iConfig.getParameter<edm::InputTag>("met")))  
{
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  m_allEvents.book((fs->mkdir("allEvents")), 4);

}


skimEvents::~skimEvents()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
skimEvents::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  eventBits myRECOevent;

  edm::Handle<GenEventInfoProduct> eventInfo;
  iEvent.getByToken(m_genEventInfoToken, eventInfo);
  myRECOevent.weight = eventInfo->weight();
  m_allEvents.fill(myRECOevent);
  std::cout <<"THIS EVENT HAS A WEIGHT OF: "<<eventInfo->weight() <<std::endl;
  
  int muonPass = 0;
  edm::Handle<std::vector<pat::Muon>> recoMuons;
  iEvent.getByToken(m_recoMuonToken, recoMuons);
  for(std::vector<pat::Muon>::const_iterator iMuon = recoMuons->begin(); iMuon != recoMuons->end(); iMuon++) {
    if (iMuon->pt() < 150 || fabs(iMuon->eta()) > 2.0) continue;
    muonPass++;
  }

  int jetPass = 0;
  edm::Handle<std::vector<pat::Jet>> ak8recoJets;
  iEvent.getByToken(m_AK8recoJetsToken, ak8recoJets);
  for(std::vector<pat::Jet>::const_iterator iJet = ak8recoJets->begin(); iJet != ak8recoJets->end(); iJet++) {
    if (iJet->pt() < 150 || fabs(iJet->eta()) > 2.8) continue;
    jetPass++;
  }

  if (jetPass > 0 && muonPass > 0) {
    std::cout <<"PASSES"<<std::endl;
    return true; 
  }
  std::cout << "FAILS"<<std::endl;
  return false;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
skimEvents::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
skimEvents::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
skimEvents::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
skimEvents::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
skimEvents::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
skimEvents::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
skimEvents::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(skimEvents);
#endif
