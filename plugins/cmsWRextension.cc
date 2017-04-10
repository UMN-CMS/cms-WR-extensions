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
     
      // ----------member data ---------------------------
      edm::EDGetToken m_genParticleToken;
 
      TTree* hardProcessKinematics;
      

      double parton1Et;
      double parton2Et;
      double muonFirstEt;
      double muonSecondEt;
      double muonHighestEt;
      double muonSecondHighestEt;
      double parton1Eta;
      double parton2Eta;
      double muonFirstEta;
      double muonSecondEta;
      double muonHighestEtEta;
      double muonSecondHighestEtEta;
      double dRparton1parton2;
      double dRmuon1muon2;
      double dRparton1muon2;
      double dRparton1muon1;
      double dRparton2muon2;
      double dRparton2muon1;         



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
   m_genParticleToken (consumes<std::vector<reco::GenParticle>> (edm::InputTag("genParticles")))

{
   //now do what ever initialization is needed
   usesResource("TFileService");
   edm::Service<TFileService> fs;
   hardProcessKinematics = fs->make<TTree>("hardProcessKinematics", "Kinematic Variables of the Hard Process");

   hardProcessKinematics->Branch("parton1Et"              ,&parton1Et              ,"parton 1 Et");
   hardProcessKinematics->Branch("parton2Et"              ,&parton2Et              ,"parton 2 Et");
   hardProcessKinematics->Branch("muonFirstEt"            ,&muonFirstEt            ,"first muon Et");
   hardProcessKinematics->Branch("muonSecondEt"           ,&muonSecondEt           ,"second muon Et");
   hardProcessKinematics->Branch("muonHighestEt"          ,&muonHighestEt          ,"highest et muon Et");
   hardProcessKinematics->Branch("muonSecondHighestEt"    ,&muonSecondHighestEt    ,"second highest et muon Et");

   hardProcessKinematics->Branch("parton1Eta"             ,&parton1Eta             ,"parton 1 eta");
   hardProcessKinematics->Branch("parton2Eta"             ,&parton2Eta             ,"parton 2 eta");
   hardProcessKinematics->Branch("muonFirstEta"           ,&muonFirstEta           ,"first muon eta");
   hardProcessKinematics->Branch("muonSecondEta"          ,&muonSecondEta          ,"second muon eta");
   hardProcessKinematics->Branch("muonHighestEtEta"       ,&muonHighestEtEta       ,"highest et muon eta");
   hardProcessKinematics->Branch("muonSecondHighestEtEta" ,&muonSecondHighestEtEta ,"second highest et muon eta");

   hardProcessKinematics->Branch("dRparton1parton2"       ,&dRparton1parton2       ,"deltaR between partons");
   hardProcessKinematics->Branch("dRmuon1muon2"           ,&dRmuon1muon2           ,"deltaR between muons");
   hardProcessKinematics->Branch("dRparton1muon2"         ,&dRparton1muon2         ,"deltaR between parton1 and muon2");
   hardProcessKinematics->Branch("dRparton1muon1"         ,&dRparton1muon1         ,"deltaR between parton1 and muon2");
   hardProcessKinematics->Branch("dRparton2muon2"         ,&dRparton2muon2         ,"deltaR between parton2 and muon2");
   hardProcessKinematics->Branch("dRparton2muon1"         ,&dRparton2muon1         ,"deltaR between parton2 and muon1");

}


cmsWRextension::~cmsWRextension()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//
//HELPER COMPARISON FUNCTION
static bool compareEt(const reco::GenParticle* particle1, const reco::GenParticle* particle2) {
  if ( particle1->et() > particle2->et() ) return true;
  return false;

}
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
     if(iParticle->isHardProcess() ) {
        std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
        if(iParticle->status() == 23 && iParticle->pdgId() <= 6 && iParticle->pdgId() >= -6) myEvent.outgoingPartons.push_back(&(*iParticle));
        if(iParticle->status() == 23 && (iParticle->pdgId() == 13 || iParticle->pdgId() == -13)) myEvent.outgoingMuons.push_back(&(*iParticle));
     }
   }
   //CHECK THAT THE EVENT MAKES SENSE
   if (myEvent.outgoingPartons.size() < 2 || myEvent.outgoingMuons.size() < 2) {
     std::cout << "ERROR! STRANGE EVENT, DID NOT FIND AT LEAST 2 PARTONS OR 2 MUONS"<< std::endl;
    return;
   }
   //SORT GEN MUONS AND PARTONS BY ET
   std::sort(myEvent.outgoingPartons.begin(),myEvent.outgoingPartons.end(),compareEt);
   std::sort(myEvent.outgoingMuons.begin(),myEvent.outgoingMuons.end(),compareEt);

   //DOES THE FIRST MUON IN THE LIST COME FROM THE WR?
   if(myEvent.outgoingMuons.at(0)->mother()->pdgId() == 9900024 || myEvent.outgoingMuons.at(0)->mother()->pdgId() == -9900024) {
     myEvent.firstMuon = myEvent.outgoingMuons.at(0);
     myEvent.secondMuon = myEvent.outgoingMuons.at(1);
   } else if(myEvent.outgoingMuons.at(1)->mother()->pdgId() == 9900024 || myEvent.outgoingMuons.at(1)->mother()->pdgId() == -9900024) {
     myEvent.firstMuon = myEvent.outgoingMuons.at(1);
     myEvent.secondMuon = myEvent.outgoingMuons.at(0);
   } else {
     std::cout << "ERROR! NO MUON HAS A WR MOTHER" << std::endl;
     return;
   }
   //NOW THAT WE HAVE THE MUONS AND PARTONS WE WANT, WE FILL ALL OF OUR HISTOGRAMS
   parton1Et = myEvent.highestEtParton->et();
   parton2Et = myEvent.secondHighestEtParton->et();
   muonFirstEt = myEvent.firstMuon->et();
   muonSecondEt = myEvent.secondMuon->et();
   muonHighestEt = myEvent.highestEtMuon->et();
   muonSecondHighestEt = myEvent.secondHighestEtMuon->et();

   parton1Eta = myEvent.highestEtParton->eta();
   parton2Eta = myEvent.secondHighestEtParton->eta();
   muonFirstEta = myEvent.firstMuon->eta();
   muonSecondEta = myEvent.secondMuon->eta();
   muonHighestEtEta = myEvent.highestEtMuon->eta();
   muonSecondHighestEtEta = myEvent.secondHighestEtMuon->eta();

   dRparton1parton2 = deltaR2(*myEvent.highestEtParton,*myEvent.secondHighestEtParton);
   dRmuon1muon2 = deltaR2(*myEvent.firstMuon,*myEvent.secondMuon);
   dRparton1muon2 = deltaR2(*myEvent.highestEtParton,*myEvent.secondMuon);
   dRparton1muon1 = deltaR2(*myEvent.highestEtParton,*myEvent.firstMuon);
   dRparton2muon2 = deltaR2(*myEvent.secondHighestEtParton,*myEvent.secondMuon);
   dRparton2muon1 = deltaR2(*myEvent.secondHighestEtParton,*myEvent.firstMuon);


   hardProcessKinematics->Fill();
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
