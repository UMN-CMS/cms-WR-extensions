#ifndef cmsWRextension_included
#define cmsWRextension_included 1
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
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"



//ROOT includes
#include "TH1D.h"
#include "TTree.h"
//local includes
#include "ExoAnalysis/cmsWRextensions/interface/eventBits.h"
#include "ExoAnalysis/cmsWRextensions/interface/eventHistos.h"
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"

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
      void selectMuons(const edm::Event&, eventBits&);
      bool preSelectGen (const edm::Event&, eventBits&);
      bool preSelectReco (const edm::Event&, eventBits&);
      bool passWR2016GEN (const edm::Event&, eventBits&);
      bool passExtensionGEN (const edm::Event&, eventBits&);
      bool passWR2016RECO (const edm::Event&, eventBits&);
      bool passExtensionRECO(const edm::Event&, eventBits&);
      bool selectHighPtISOMuon(const edm::Event&, eventBits&);
      bool passWR2016Reco(const edm::Event&, eventBits&);
      bool jetSelection(const edm::Event& iEvent, eventBits& myEvent);
      bool additionalMuons(const edm::Event& iEvent, eventBits& myEvent);
      bool leptonSelection(const edm::Event& iEvent, eventBits& myEvent);
      bool subLeadingMuonZMass(const edm::Event& iEvent, eventBits& myEvent);
      bool METselection(const edm::Event& iEvent, eventBits& myEvent);
      bool METcuts(const edm::Event& iEvent, eventBits& myEvent);
      bool lastCuts(const edm::Event& iEvent, eventBits& myEvent);
      bool massCut(const edm::Event& iEvent, eventBits& myEvent);
      bool genCounter(const edm::Event& iEvent, eventBits& myEvent);
      // ----------member data ---------------------------
      eventHistos m_allEvents;
      eventHistos m_eventsPassingWR2016;
      eventHistos m_eventsPassingExtension;
      eventHistos m_eventsPassingExtensionRECO;
      eventHistos m_eventsPassingExtensionRECO2016VETO;
      eventHistos m_eventsPassingExtensionRECO2016VETOMASSMETCUT;
      eventHistos m_eventsPassingExtensionRECO2016VETOMASSCUT;
      eventHistos m_eventsPassingExtensionRECO2016VETOZMASS;
      eventHistos m_eventsPassingExtensionRECO2016VETOSINGLEMUON;
      eventHistos m_eventsPassingWR2016RECO;
      edm::EDGetToken m_genParticleToken;
      edm::EDGetToken m_genJetsToken;
      edm::EDGetToken m_AK8genJetsToken;
      edm::EDGetToken m_highLeptonToken;
      edm::EDGetToken m_regMuonToken;
      edm::EDGetToken m_recoJetsToken;
      edm::EDGetToken m_AK8recoJetsToken;
      edm::EDGetToken m_offlineVerticesToken;
      edm::EDGetToken m_genEventInfoToken;
      edm::EDGetToken m_metToken;
      bool m_wantHardProcessMuons;
      bool m_doGen;
      bool m_doReco;
      bool m_isMC;
      double m_MCL;    //MASS UPPER AND LOWER CUTS
      double m_MCU;
      bool m_flavorSideband;
      TTree* hardProcessKinematics;
};

//define this as a plug-in
DEFINE_FWK_MODULE(cmsWRextension);
#endif
