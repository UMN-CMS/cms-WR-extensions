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


#include "TH1D.h"

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

      // ----------member data ---------------------------
      edm::EDGetToken m_genParticleToken;

      TH1D* parton1Et;
      TH1D* parton2Et;
      TH1D* muonFirstEt;
      TH1D* muonSecondEt;
      TH1D* muonHighestEt;
      TH1D* muonSecondHighestEt;

      TH1D* parton1Eta;
      TH1D* parton2Eta;
      TH1D* muonFirstEta;
      TH1D* muonSecondEta;
      TH1D* muonHighestEtEta;
      TH1D* muonSecondHighestEtEta;

      TH1D*  dRparton1parton2;
      TH1D*  dRmuon1muon2;
      TH1D*  dRparton1muon2;
      TH1D*  dRparton1muon1;
      TH1D*  dRparton2muon2;
      TH1D*  dRparton2muon1;


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
   parton1Et = fs->make<TH1D>("parton1Et", "Parton 1 Et", 100, 0.0, 2000);
   parton2Et = fs->make<TH1D>("parton2Et", "Parton 2 Et", 100, 0.0, 2000);
   muonFirstEt = fs->make<TH1D>("muonFirstEt", "First Muon Et", 100, 0.0, 2000);
   muonSecondEt = fs->make<TH1D>("muonSecondEt", "Second Muon Et", 100, 0.0, 2000);
   muonHighestEt = fs->make<TH1D>("muonHighestEt", "Highest Et Muon Et", 100, 0.0, 2000);
   muonSecondHighestEt = fs->make<TH1D>("muonSecondHighestEt", "Second Highest Et Muon Et", 100, 0.0, 2000);

   parton1Eta = fs->make<TH1D>("parton1Eta", "Parton 1 Eta", 100, -4.0, 4.0);
   parton2Eta = fs->make<TH1D>("parton2Eta", "Parton 2 Eta", 100, -4.0, 4.0);
   muonFirstEta = fs->make<TH1D>("muonFirstEta", "First muon eta", 100, -4.0, 4.0);
   muonSecondEta = fs->make<TH1D>("muonSecondEta", "Second muon eta",                           100, -4.0, 4.0);
   muonHighestEtEta = fs->make<TH1D>("muonHighestEtEta", "Highest Et muon eta",                 100, -4.0, 4.0);
   muonSecondHighestEtEta = fs->make<TH1D>("muonSecondHighestEtEta", "2nd Highest Et muon eta", 100, -4.0, 4.0);

   dRparton1parton2 = fs->make<TH1D>("dRparton1parton2", "deltaR between partons",       100, 0.0, 12.0); 
   dRmuon1muon2 = fs->make<TH1D>("dRmuon1muon2", "deltaR between muons",                 100, 0.0, 12.0); 
   dRparton1muon2 = fs->make<TH1D>("dRparton1muon2", "deltaR between parton1 and muon2", 100, 0.0, 12.0); 
   dRparton1muon1 = fs->make<TH1D>("dRparton1muon1", "deltaR between parton1 and muon1", 100, 0.0, 12.0); 
   dRparton2muon2 = fs->make<TH1D>("dRparton2muon2", "deltaR between parton2 and muon2", 100, 0.0, 12.0); 
   dRparton2muon1 = fs->make<TH1D>("dRparton2muon1", "deltaR between parton2 and muon1", 100, 0.0, 12.0); 

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
  
   std::vector<const reco::GenParticle*> outgoingPartonPair;
   std::vector<const reco::GenParticle*> outgoingMuons;

   const reco::GenParticle* highestEtMuon;
   const reco::GenParticle* secondHighestEtMuon;
   const reco::GenParticle* firstMuon;
   const reco::GenParticle* secondMuon;

   const reco::GenParticle* highestEtParton;
   const reco::GenParticle* secondHighestEtParton;
 
   //LOOP OVER GEN PARTICLES
   for (std::vector<reco::GenParticle>::const_iterator iParticle = pIn->begin(); iParticle != pIn->end(); iParticle++) {
     if(iParticle->isHardProcess() ) {
        std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
        if(iParticle->status() == 23 && iParticle->pdgId() <= 6 && iParticle->pdgId() >= -6) outgoingPartonPair.push_back(&(*iParticle));
        if(iParticle->status() == 23 && (iParticle->pdgId() == 13 || iParticle->pdgId() == -13)) outgoingMuons.push_back(&(*iParticle));
     }
   }
   //CHECK THAT THE EVENT MAKES SENSE
   if (outgoingPartonPair.size() != 2 || outgoingMuons.size() != 2) {
     std::cout << "ERROR! STRANGE EVENT, DID NOT FIND 2 PARTONS OR 2 MUONS"<< std::endl;
    return;
   }
   //SORT GENMUONS AND PARTONS
   if(outgoingPartonPair.at(0)->et() < outgoingPartonPair.at(1)->et()) {
     highestEtParton = outgoingPartonPair.at(1);
     secondHighestEtParton = outgoingPartonPair.at(0);
   } else {
     highestEtParton = outgoingPartonPair.at(0);
     secondHighestEtParton = outgoingPartonPair.at(1);
   }
 //   dRpartonPair = reco::deltaR2(outgoingPartonPair.at(0).eta(),outgoingPartonPair.at(0).phi(),outgoingPartonPair.at(1).eta(),outgoingPartonPair.at(1).phi());
   if(outgoingMuons.at(0)->et() < outgoingMuons.at(1)->et()) {
     highestEtMuon = outgoingMuons.at(1);
     secondHighestEtMuon = outgoingMuons.at(0);
   } else {
     highestEtMuon = outgoingMuons.at(0);
     secondHighestEtMuon = outgoingMuons.at(1);
   }
   //DOES THE FIRST MUON IN THE LIST COME FROM THE WR?
   if(outgoingMuons.at(0)->mother()->pdgId() == 9900024 || outgoingMuons.at(0)->mother()->pdgId() == -9900024) {
     firstMuon = outgoingMuons.at(0);
     secondMuon = outgoingMuons.at(1);
   } else if(outgoingMuons.at(1)->mother()->pdgId() == 9900024 || outgoingMuons.at(1)->mother()->pdgId() == -9900024) {
     firstMuon = outgoingMuons.at(1);
     secondMuon = outgoingMuons.at(0);
   } else {
     std::cout << "ERROR! NO MUON HAS A WR MOTHER" << std::endl;
     return;
   }
   //NOW THAT WE HAVE THE MUONS AND PARTONS WE WANT, WE FILL ALL OF OUR HISTOGRAMS
   parton1Et->Fill(highestEtParton->et());
   parton2Et->Fill(secondHighestEtParton->et());
   muonFirstEt->Fill(firstMuon->et());
   muonSecondEt->Fill(secondMuon->et());
   muonHighestEt->Fill(highestEtMuon->et());
   muonSecondHighestEt->Fill(secondHighestEtMuon->et());

   parton1Eta->Fill(highestEtParton->eta());
   parton2Eta->Fill(secondHighestEtParton->eta());
   muonFirstEta->Fill(firstMuon->eta());
   muonSecondEta->Fill(secondMuon->eta());
   muonHighestEtEta->Fill(highestEtMuon->eta());
   muonSecondHighestEtEta->Fill(secondHighestEtMuon->eta());

   dRparton1parton2->Fill(deltaR2(*highestEtParton,*secondHighestEtParton));
   dRmuon1muon2->Fill(deltaR2(*firstMuon,*secondMuon));
   dRparton1muon2->Fill(deltaR2(*highestEtParton,*secondMuon));
   dRparton1muon1->Fill(deltaR2(*highestEtParton,*firstMuon));
   dRparton2muon2->Fill(deltaR2(*secondHighestEtParton,*secondMuon));
   dRparton2muon1->Fill(deltaR2(*secondHighestEtParton,*firstMuon));


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
