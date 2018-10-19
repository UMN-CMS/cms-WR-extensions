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
#include "ExoAnalysis/cmsWRextensions/interface/HEEP.h"
#include "ExoAnalysis/cmsWRextensions/interface/egammaEffi.h"
#include "ExoAnalysis/cmsWRextensions/interface/eventInfo.h"
#include "ExoAnalysis/cmsWRextensions/interface/Muons.h"
#include "ExoAnalysis/cmsWRextensions/interface/JetTools.h"
#include "BaconAna/DataFormats/interface/BaconAnaDefs.hh"
#include "BaconAna/DataFormats/interface/TAddJet.hh"

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
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/BasicJet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/BasicJetCollection.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfoMatching.h"

#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "JetMETCorrections/Modules/interface/JetResolution.h"

//ROOT includes
#include "TH1D.h"
#include "TTree.h"
#include "TRandom3.h"
#include <cmath>

#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/contrib/EnergyCorrelator.hh"
#include <fastjet/JetDefinition.hh>

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
  m_AK8recoCHSJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoCHSJets"))),
  m_AK8recoPUPPIJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoPUPPIJets"))),
  m_AK8recoPUPPISubJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("subJetName"))),
  m_offlineVerticesToken (consumes<std::vector<reco::Vertex>> (iConfig.getParameter<edm::InputTag>("vertices"))),
  m_metToken (consumes<std::vector<pat::MET>> (iConfig.getParameter<edm::InputTag>("met"))),
  m_PUInfoToken (consumes< std::vector<PileupSummaryInfo>> (iConfig.getParameter<edm::InputTag>("edmPileupInfo"))),
  m_rhoLabel    (consumes<double> (iConfig.getParameter<edm::InputTag>("rhoLabel"))),
  m_wantHardProcessMuons (iConfig.getUntrackedParameter<bool>("wantHardProcessMuons",true)),
  m_doGen (iConfig.getUntrackedParameter<bool>("doGen",false)),
  m_doReco (iConfig.getUntrackedParameter<bool>("doReco",true)),
  m_isMC (iConfig.getUntrackedParameter<bool>("isMC",true)),
  m_doTrig (iConfig.getUntrackedParameter<bool>("doTrig",false)),
  m_doFast (iConfig.getUntrackedParameter<bool>("doFast",false)),
  m_jettiness (iConfig.getUntrackedParameter<std::string>("jettinessPUPPI")),
  JECUName   (iConfig.getUntrackedParameter<std::string>("jecUncName","")),
  //m_MCL (iConfig.getUntrackedParameter<double>("MCL", 400)),
  //m_MCU (iConfig.getUntrackedParameter<double>("MCU", 8000)),
  m_flavorSideband (iConfig.getUntrackedParameter<bool>("flavorSideband", false)),  //SOON TO BE DEPRECATED
  m_outputTag (iConfig.getUntrackedParameter<string>("outputTag", "blah"))

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

  myHEEP.Initialize();
  myEgammaEffi.Initialize();

  loadCMSSWPath();
  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/cmsWRextensions/data/";

  resolution = JME::JetResolution(Form("%s/Summer16_25nsV1_MC_PtResolution_AK8PFPuppi.txt",jecPathname.c_str()));
  resolution_sf = JME::JetResolutionScaleFactor(Form("%s/Summer16_25nsV1_MC_SF_AK8PFPuppi.txt",jecPathname.c_str()));

  r = new TRandom3(1988);

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

  edm::Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(m_offlineVerticesToken, vertices);
  if(!vertices.isValid()) {
    throw cms::Exception("Vertex collection not valid!");
  }

  eventBits myEvent;
  eventBits myRECOevent;

  myEvent.outputTag = m_outputTag;
  myRECOevent.outputTag = m_outputTag;

  bool pass2016 = false;
  bool ZMASS = false;
  bool addMuons = false;

  setEventWeight(iEvent, myEvent, 1., 1.);

  if (!myEventInfo.PVselection(vertices)){
    return;
  }

  myRECOevent.nVtx = myEventInfo.nVtx;

  myRECOevent.PVertex = myEventInfo.PVertex;

  if(m_isMC){
    edm::Handle< std::vector<PileupSummaryInfo> > hPileupInfoProduct;
    iEvent.getByToken(m_PUInfoToken,hPileupInfoProduct);
    assert(hPileupInfoProduct.isValid());
    myRECOevent.puWeight = myEventInfo.PUweight(hPileupInfoProduct);
  }

  setEventWeight(iEvent, myRECOevent, 1., 1.);

  myRECOevent.cutProgress = 0;

  if(m_isMC && m_doGen && !m_doFast) {
//    genCounter(iEvent, myEvent);
    if (genCounter(iEvent, myRECOevent))
      vertexDiff(myRECOevent);
  }
   
  if (m_doGen && m_isMC && !m_flavorSideband && !m_doFast) {
    if(preSelectGen(iEvent, myRECOevent)) {
      std::cout << "analyzing wr2016" << std::endl;
      pass2016 = passWR2016GEN(iEvent, myRECOevent);
      if(pass2016) m_eventsPassingWR2016.fill(myRECOevent, 1);
      std::cout << "analyzing extension" << std::endl;
      if(passExtensionGEN(iEvent, myRECOevent)) m_eventsPassingExtension.fill(myRECOevent, 1);
    }
  }
  bool muonTrigPass = true;
  if ((m_doReco || !m_isMC) && m_doFast){
    if(preSelectReco_Fast(iEvent, iSetup, myRECOevent)) {
      passExtensionRECO_Fast(iEvent, myRECOevent);
      if (m_doTrig){
        muonTrigPass = passMuonTrig(iEvent, myRECOevent);
      }
      addMuons = additionalMuons(iEvent, myRECOevent, false);
      ZMASS = subLeadingMuonZMass(iEvent, myRECOevent);
      double Muon_LooseID_Weight = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta());
      double Muon_HighPtID_Weight = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta());
      setEventWeight(iEvent, myRECOevent, Muon_LooseID_Weight, Muon_HighPtID_Weight);
      //Fill histograms individually
      if(myRECOevent.myAddJetCandsHighPt.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs.size() > 0 && muonTrigPass && addMuons && !ZMASS){
        m_eventsPassingExtensionRECO.fill(myRECOevent, 1.);
      }
      if(myRECOevent.myAddJetCandsHighPt_JECUp.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JECUp.size() > 0 && muonTrigPass && addMuons && !ZMASS){
        m_eventsPassingExtensionRECO.fill(myRECOevent, 2.);
      }
      if(myRECOevent.myAddJetCandsHighPt_JECDown.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JECDown.size() > 0 && muonTrigPass && addMuons && !ZMASS){
        m_eventsPassingExtensionRECO.fill(myRECOevent, 3.);
      }
      if(myRECOevent.myAddJetCandsHighPt_JERUp.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JERUp.size() > 0 && muonTrigPass && addMuons && !ZMASS){
        m_eventsPassingExtensionRECO.fill(myRECOevent, 4.);
      }
      if(myRECOevent.myAddJetCandsHighPt_JERDown.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JERDown.size() > 0 && muonTrigPass && addMuons && !ZMASS){
        m_eventsPassingExtensionRECO.fill(myRECOevent, 5.);
      }
    }
  }
  if ((m_doReco || !m_isMC) && !m_doFast) {
    std::cout<<"running preselection reco"<<std::endl;
    if(preSelectReco(iEvent, iSetup, myRECOevent)) {
      if(passExtensionRECO(iEvent, myRECOevent)) { 
        if (m_doTrig){
          muonTrigPass = passMuonTrig(iEvent, myRECOevent);
        }
//        if(!pass2016) {
          myRECOevent.cutProgress++;
          addMuons = additionalMuons(iEvent, myRECOevent, false);
          ZMASS = subLeadingMuonZMass(iEvent, myRECOevent);
          if(!addMuons && muonTrigPass) {
            m_eventsPassingExtensionRECO2016VETOSINGLEMUON.fill(myRECOevent, 1);
          }
          if(m_isMC && addMuons) {
            double Muon_LooseID_Weight = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta());
            double Muon_HighPtID_Weight = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta());
            setEventWeight(iEvent, myRECOevent, Muon_LooseID_Weight, Muon_HighPtID_Weight);
          }
          if(addMuons && ZMASS && muonTrigPass) {
            std::cout<< "FILLING ZPeak Region" << std::endl;
            m_eventsPassingExtensionRECO2016VETOZMASS.fill(myRECOevent, 1);
          } else if ((m_isMC || m_flavorSideband) && addMuons){
            myRECOevent.cutProgress++;
	    if(!ZMASS){
              myRECOevent.cutProgress++;
              std::cout << "HERE WE FILL THE GOOD STUFF" << std::endl;
              if(muonTrigPass && m_doTrig){
                myRECOevent.cutProgress++;
	        m_eventsPassingExtensionRECO2016VETO.fill(myRECOevent, 1);
	      }
              m_eventsPassingExtensionRECO2016VETO_noTrig.fill(myRECOevent, 1);
            //if(massCut(iEvent, myRECOevent)) {
            //  m_eventsPassingExtensionRECO2016VETOMASSCUT.fill(myRECOevent);
            //  myRECOevent.cutProgress++;
            //  if(metCuts(iEvent, myRECOevent))
            //    m_eventsPassingExtensionRECO2016VETOMASSMETCUT.fill(myRECOevent);
            //}
            }
          }
//        }
        if ((m_isMC || m_flavorSideband) && !ZMASS && muonTrigPass) m_eventsPassingExtensionRECO.fill(myRECOevent, 1);
        //std::cout <<"rECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;
        std::cout << "PASSED RECO EXTENSION, FILLING" << std::endl;
      }
    }
    std::cout << "CHECKING THE FLAVOR SIDEBAND" << std::endl;
    myRECOevent.FSBcutProgress = 0;
    myRECOevent.FSBcutProgress++;

    if(passFlavorSideband(iEvent, myRECOevent)) {
      std::cout << "Inside FSB" << std::endl;
      bool ss = false;
      bool ss_noISO = false;
      if (myRECOevent.electronCands200  > 0) ss = sameSign(myRECOevent, false);
      if (myRECOevent.electronCands200_noISO  > 0) ss_noISO = sameSign(myRECOevent, true);

      std::cout << "PASSES THE FLAVOR SIDEBAND" << std::endl;
      if (m_doTrig){
        if (passElectronTrig(iEvent, myRECOevent)){
          std::cout<< "EVENT PASSES ELECTRON TRIGGERS" << std::endl;
          if (passFSBbin(myRECOevent, true, 50)) {
            m_eventsPassingFlavorSidebandRECOelePt50.fill(myRECOevent, 1);
	  }
          if (!m_doFast) {
            if (passFSBbin(myRECOevent, true, 100)) {
              m_eventsPassingFlavorSidebandRECOelePt100.fill(myRECOevent, 1);
	    }
            if (passFSBbin(myRECOevent, true, 150)) {
              m_eventsPassingFlavorSidebandRECOelePt150.fill(myRECOevent, 1);
	    }
            if (passFSBbin(myRECOevent, true, 200)) {
              m_eventsPassingFlavorSidebandRECOelePt200_all.fill(myRECOevent, 1);
              if (ss) m_eventsPassingFlavorSidebandRECOelePt200_samesign.fill(myRECOevent, 1);
              else m_eventsPassingFlavorSidebandRECOelePt200.fill(myRECOevent, 1);
            }
            if (passFSBbin(myRECOevent, false, 200)) {
              if (ss_noISO) m_eventsPassingFlavorSidebandRECOelePt200_noISO_samesign.fill(myRECOevent, 1);
              else m_eventsPassingFlavorSidebandRECOelePt200_noISO.fill(myRECOevent, 1);
            }
            if (passFSBbin(myRECOevent, false, 50)) {
              m_eventsPassingFlavorSidebandRECOelePt50_noISO.fill(myRECOevent, 1);
	    }
          }
        }
      }
      if (passFSBbin(myRECOevent, false, 200)) {
        if (ss_noISO) m_eventsPassingFlavorSidebandRECOelePt200_noISO_samesign_noTrig.fill(myRECOevent, 1);
        else m_eventsPassingFlavorSidebandRECOelePt200_noISO_noTrig.fill(myRECOevent, 1);
      }
      if (passFSBbin(myRECOevent, true, 50) && !m_doFast) {
        m_eventsPassingFlavorSidebandRECO_noTrig.fill(myRECOevent, 1);
      }
    }
    std::cout << "DONE WITH FSB" << std::endl;
  }
  std::cout << "TIME TO FILL ALL EVENTS" << std::endl;
  m_allEvents.fill(myRECOevent, 1);
}
bool cmsWRextension::passFSBbin(eventBits& myEvent, bool ISO, int ptCut) {
  //LOGIC IS REPEATED FOR ISO AND NONISO SELECTIONS
  if (ISO) {
    if (myEvent.myElectronJetPairs.size() < 1)  return false;  //NO PAIRS MEANS IT COULD NOT PASS
    if (myEvent.selectedElectronPt >= ptCut) return true;
  } else {
    if (myEvent.myElectronJetPairs_noISO.size() < 1) return false;
    if (myEvent.selectedElectron_noISO_Pt >= ptCut) return true;
  }
  std::cout << "WELCOME TO THE TWILIGHT ZONE" << std::endl;
  return false;
}
bool cmsWRextension::sameSign(eventBits& myEvent, bool noISO) {
  if (noISO) {
    if (abs(myEvent.myElectronCand_noISO->charge() + myEvent.mySubleadMuon->charge()) == 2)
      return true;
  } else {
    if (abs(myEvent.myElectronCand->charge() + myEvent.mySubleadMuon->charge()) == 2)
      return true;
  }
  return false;
}
void cmsWRextension::vertexDiff(eventBits& myEvent) {
  std::cout << "RUNNING VTX DIFF" << std::endl;
  if (myEvent.genVtx == 0) return;
  if (myEvent.PVertex == 0) return;
  


  std::cout << "RUNNING VTX DIFF X" << std::endl;
  double dx = myEvent.PVertex->x() - myEvent.genVtx->x();
  std::cout << "RUNNING VTX DIFF Y" << std::endl;
  double dy = myEvent.PVertex->y() - myEvent.genVtx->y();
  std::cout << "RUNNING VTX DIFF Z" << std::endl;
  double dz = myEvent.PVertex->z() - myEvent.genVtx->z();

  std::cout << "NEW POINT" << std::endl;
  math::XYZPoint* diff = new math::XYZPoint();

  std::cout << "SETTING POINT" << std::endl;
  diff->SetXYZ(dx,dy,dz);

  std::cout << "SETTING DIFF VTX" << std::endl;
  myEvent.myVertexDiff = diff;

  std::cout << "SETTING DIFF VTX TAN" << std::endl;
  myEvent.myVertexDiffTan = sqrt(dx*dx + dz*dz);
  std::cout << "SETTING DIFF VTX LON" << std::endl;
  myEvent.myVertexDiffLon = abs(dy);
  

}
void cmsWRextension::setEventWeight(const edm::Event& iEvent, eventBits& myEvent, double MuonLooseIDWeight, double MuonHighPtIDWeight) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);
      if(!m_amcatnlo) {
        myEvent.weight = eventInfo->weight()*myEvent.puWeight;
        myEvent.count = 1;
      }
      else {
        myEvent.weight = eventInfo->weight()*myEvent.puWeight/fabs(eventInfo->weight());
        myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      }
  } else {
      myEvent.weight = 1;
      myEvent.count = 1;
  }

  myEvent.weight = myEvent.weight*MuonHighPtIDWeight*MuonLooseIDWeight;

}
void cmsWRextension::setEventWeight_FSB(const edm::Event& iEvent, eventBits& myEvent, double MuonLooseIDWeight) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);
      if(!m_amcatnlo) {
        myEvent.FSBweight = eventInfo->weight()*myEvent.puWeight;
        myEvent.count = 1;
      }
      else {
        myEvent.FSBweight = eventInfo->weight()*myEvent.puWeight/fabs(eventInfo->weight());
        myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      }
  } else {
      myEvent.FSBweight = 1;
      myEvent.count = 1;
  }

  myEvent.FSBweight = myEvent.FSBweight*myEvent.HEEP_SF*myEvent.egamma_SF*MuonLooseIDWeight;

}
void cmsWRextension::setEventWeight_FSB_noISO(const edm::Event& iEvent, eventBits& myEvent, double MuonLooseIDWeight) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);
      if(!m_amcatnlo) {
        myEvent.FSBweight_noISO = eventInfo->weight()*myEvent.puWeight;
        myEvent.count = 1;
      }
      else {
        myEvent.FSBweight_noISO = eventInfo->weight()*myEvent.puWeight/fabs(eventInfo->weight());
        myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      }
  } else {
      myEvent.FSBweight_noISO = 1;
      myEvent.count = 1;
  }

  myEvent.FSBweight_noISO = myEvent.FSBweight_noISO*myEvent.HEEP_SF_noISO*myEvent.egamma_SF_noISO*MuonLooseIDWeight;

}
bool cmsWRextension::passElectronTrig(const edm::Event& iEvent, eventBits& myRECOevent) {
  bool passTriggers = false;

  std::cout <<"checking electron trigger paths "<<std::endl;
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByToken(m_trigResultsToken, triggerResults);
  std::cout <<"grabbing trigger names"<<std::endl;
  const edm::TriggerNames& trigNames = iEvent.triggerNames(*triggerResults); //SEGFAULT
  std::cout <<"grabbing trigger results "<<std::endl;
  std::cout <<"looping over paths to pass"<<std::endl;
  for(size_t i = 0; i < trigNames.size(); ++i) {
    const std::string &name = trigNames.triggerName(i);
    for(auto& pathName : m_electronPathsToPass){
      if((name.find(pathName) != std::string::npos )){
        if(triggerResults->accept(i)){
          std::cout <<" path "<<pathName<<" passed"<<std::endl;
          passTriggers = true;
        }else {
          std::cout <<" path "<<pathName<<" failed"<<std::endl;
        }
      }
    }
  }
  if(passTriggers) myRECOevent.electronTrigger = 1.;
  else myRECOevent.electronTrigger = 0.;

  return passTriggers;
}

bool cmsWRextension::passMuonTrig(const edm::Event& iEvent, eventBits& myRECOevent) {
  bool passTriggers = false;

  std::cout <<"checking muon trigger paths "<<std::endl;
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByToken(m_trigResultsToken, triggerResults);
  std::cout <<"grabbing trigger names"<<std::endl;
  const edm::TriggerNames& trigNames = iEvent.triggerNames(*triggerResults); //SEGFAULT
  std::cout <<"grabbing trigger results "<<std::endl;
  std::cout <<"looping over paths to pass"<<std::endl;
  for(size_t i = 0; i < trigNames.size(); ++i) {
    const std::string &name = trigNames.triggerName(i);
    for(auto& pathName : m_muonPathsToPass){
      if((name.find(pathName) != std::string::npos )){
	if(triggerResults->accept(i)){
	  std::cout <<" path "<<pathName<<" passed"<<std::endl;
	  passTriggers = true;
	}else {
	  std::cout <<" path "<<pathName<<" failed"<<std::endl;
	}
      }
    }
  }
/*  for(auto& pathName : m_muonPathsToPass){
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
  }*/

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
  if(passTriggers) myRECOevent.muonTrigger = 1.;
  else myRECOevent.muonTrigger = 0.;

  return passTriggers;
}
bool cmsWRextension::passFlavorSideband(const edm::Event& iEvent, eventBits& myRECOevent) {

  std::cout<< "BEGINNING SELECTION ON FLAVOR SIDEBAND" << std::endl;
  if( myRECOevent.myAddJetCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<< std::endl;
    return false;
  }
  myRECOevent.FSBcutProgress++;
  additionalMuons(iEvent, myRECOevent, true);
  if( myRECOevent.myMuonCands.size() < 1){
    std::cout<< "EVENTS FAILS, NO MUONS OVER 10 GEV WITHIN ACCEPTANCE. " << myRECOevent.myMuonCands.size()<< " MUONS FOUND." << std::endl;
    return false;
  }

  myRECOevent.FSBcutProgress++;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs_noISO;
  std::cout << "CHECKING FOR ELEC CANDS" << std::endl;
  if( myRECOevent.myElectronCandsHighPt50.size() > 0 ){
    

    //BUILD PAIRS OF AK8 JETS WITH THE LEAD ELECTRON
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) < 2.0 ) continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
 
       double eventMass = ( jetVec + myRECOevent.myElectronCand->p4() ).mass();

       if( eventMass < 200 ) continue;

       std::cout << "FOUND CAND DIOBJECT WITH ISO ELE" << std::endl;

       electronJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
    }
  }
  //NO ISO CHECKING
  std::cout << "CHECKING FOR NON ISO ELEC CANDS" << std::endl;
  if( myRECOevent.myElectronCandsHighPt50_noISO.size() > 0){
    std::cout << "FOUND NON ISO ELEC CANDS" << std::endl;
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
       std::cout << "LOOPING THROUGH DIOBJECT CANDS" << std::endl;
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand_noISO->phi())) < 2.0 ) continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
 
       double eventMass_noISO = ( jetVec + myRECOevent.myElectronCand_noISO->p4() ).mass();

       if( eventMass_noISO < 200 ) continue;

       std::cout << "FOUND CAND DIOBJECT WITH NON ISO ELE" << std::endl;

       electronJetPairs_noISO.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand_noISO ));
    }
  }
  if( electronJetPairs.size() > 0 ) {
    std::cout << "PROCESSING ELECTRON JET PAIRS" << std::endl;
    myRECOevent.myElectronJetPairs = electronJetPairs;
    std::sort(electronJetPairs.begin(),electronJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);

    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs[0].first->pT, electronJetPairs[0].first->eta, electronJetPairs[0].first->phi, electronJetPairs[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    myRECOevent.leadAK8JetElectronMassVal = ( jetVec + electronJetPairs[0].second->p4() ).mass();

    myRECOevent.selectedElectronPt  = electronJetPairs[0].second->pt();
    myRECOevent.selectedElectronPhi = electronJetPairs[0].second->phi();
    myRECOevent.selectedElectronEta = electronJetPairs[0].second->eta();

    //NOW WE ACCES THE VIDRESULT AGAIN FOR THE CHOSEN ELECTRON
    const vid::CutFlowResult* vidResult =  electronJetPairs[0].second->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");

    if(myRECOevent.selectedElectronEta < 1.4442) {//BARREL
      myRECOevent.selElectron_barrel_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED); 
      myRECOevent.selElectron_barrel_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
      myRECOevent.selElectron_barrel_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
      myRECOevent.selElectron_barrel_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
//  NOT SUPPORTED    myRECOevent.selElectron_barrel_E2x5vE5x5          = vidResult->getValueCutUpon(HEEPV70::TRKISO);
      myRECOevent.selElectron_barrel_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
      myRECOevent.selElectron_barrel_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
      myRECOevent.selElectron_barrel_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
      myRECOevent.selElectron_barrel_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
    }
    if(myRECOevent.selectedElectronEta > 1.566) {//ENDCAP
      myRECOevent.selElectron_endcap_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED); 
      myRECOevent.selElectron_endcap_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
      myRECOevent.selElectron_endcap_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
      myRECOevent.selElectron_endcap_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
//  NOT SUPPORTED    myRECOevent.selElectron_barrel_E2x5vE5x5          = vidResult->getValueCutUpon(HEEPV70::TRKISO);
      myRECOevent.selElectron_endcap_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
      myRECOevent.selElectron_endcap_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
      myRECOevent.selElectron_endcap_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
      myRECOevent.selElectron_endcap_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
    }
    myRECOevent.selectedJetPt   = electronJetPairs[0].first->pT;
    myRECOevent.selectedJetPhi  = electronJetPairs[0].first->phi;
    myRECOevent.selectedJetEta  = electronJetPairs[0].first->eta;
    myRECOevent.selectedJetMass = electronJetPairs[0].first->SDmass;

    if(electronJetPairs[0].first->tau1==0){
      myRECOevent.selectedJetTau21 = -9999.;
    }else{
      myRECOevent.selectedJetTau21 = (electronJetPairs[0].first->tau2)/(electronJetPairs[0].first->tau1);
    }
    if (subLeadingMuonZMass_FlavorSideband(iEvent, myRECOevent, false)){
        std::cout<< "EVENTS FAILS ELECTRON + MUON MASS" << std::endl;
        return false;
    }

  }
  if( electronJetPairs_noISO.size() > 0 ) {
    std::cout << "PROCESSING NONISO ELECTRON JET PAIRS" << std::endl;
    myRECOevent.myElectronJetPairs_noISO = electronJetPairs_noISO;
    std::sort(electronJetPairs_noISO.begin(),electronJetPairs_noISO.end(),::wrTools::comparePairMassPointerTAddJet);

    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs_noISO[0].first->pT, electronJetPairs_noISO[0].first->eta, electronJetPairs_noISO[0].first->phi, electronJetPairs_noISO[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    myRECOevent.leadAK8JetElectronMassVal_noISO = ( jetVec + electronJetPairs_noISO[0].second->p4() ).mass();

    myRECOevent.selectedElectron_noISO_Pt  = electronJetPairs_noISO[0].second->pt();
    myRECOevent.selectedElectron_noISO_Phi = electronJetPairs_noISO[0].second->phi();
    myRECOevent.selectedElectron_noISO_Eta = electronJetPairs_noISO[0].second->eta();

    //NOW WE ACCES THE VIDRESULT AGAIN FOR THE CHOSEN ELECTRON
    const vid::CutFlowResult* vidResult =  electronJetPairs_noISO[0].second->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");

    if(myRECOevent.selectedElectron_noISO_Eta < 1.4442) {//BARREL
      myRECOevent.selElectron_noISO_barrel_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED); 
      myRECOevent.selElectron_noISO_barrel_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
      myRECOevent.selElectron_noISO_barrel_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
      myRECOevent.selElectron_noISO_barrel_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
//  NOT SUPPORTED    myRECOevent.selElectron_noISO_barrel_E2x5vE5x5          = vidResult->getValueCutUpon(HEEPV70::TRKISO);
      myRECOevent.selElectron_noISO_barrel_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
      myRECOevent.selElectron_noISO_barrel_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
      myRECOevent.selElectron_noISO_barrel_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
      myRECOevent.selElectron_noISO_barrel_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
    }
    if(myRECOevent.selectedElectron_noISO_Eta > 1.566) {//ENDCAP
      myRECOevent.selElectron_noISO_endcap_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED); 
      myRECOevent.selElectron_noISO_endcap_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
      myRECOevent.selElectron_noISO_endcap_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
      myRECOevent.selElectron_noISO_endcap_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
//  NOT SUPPORTED    myRECOevent.selElectron_noISO_barrel_E2x5vE5x5          = vidResult->getValueCutUpon(HEEPV70::TRKISO);
      myRECOevent.selElectron_noISO_endcap_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
      myRECOevent.selElectron_noISO_endcap_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
      myRECOevent.selElectron_noISO_endcap_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
      myRECOevent.selElectron_noISO_endcap_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
    }
    myRECOevent.selectedJet_EleNoISO_Pt   = electronJetPairs_noISO[0].first->pT;
    myRECOevent.selectedJet_EleNoISO_Phi  = electronJetPairs_noISO[0].first->phi;
    myRECOevent.selectedJet_EleNoISO_Eta  = electronJetPairs_noISO[0].first->eta;
    myRECOevent.selectedJet_EleNoISO_Mass = electronJetPairs_noISO[0].first->SDmass;

    if(electronJetPairs_noISO[0].first->tau1==0){
      myRECOevent.selectedJet_EleNoISO_Tau21 = -9999.;
    }else{
      myRECOevent.selectedJet_EleNoISO_Tau21 = (electronJetPairs_noISO[0].first->tau2)/(electronJetPairs_noISO[0].first->tau1);
    }
    if (subLeadingMuonZMass_FlavorSideband(iEvent, myRECOevent, true)){
        std::cout<< "EVENTS FAILS ELECTRON + MUON MASS" << std::endl;
        return false;
    }
  }




  if(m_isMC && electronJetPairs.size() > 0) {
    double Muon_LooseID_Weight = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta());
    myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weight;
    if (electronJetPairs.size() > 0) { 
      double HEEP_SF = myHEEP.ScaleFactor(myRECOevent.selectedElectronEta);
      double egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.myElectronCand->superCluster()->eta(), myRECOevent.selectedElectronPt);
      if (fabs(myRECOevent.selectedElectronEta) > 1.6) myRECOevent.HEEP_SF_E = HEEP_SF;
      if (fabs(myRECOevent.selectedElectronEta) < 1.4) myRECOevent.HEEP_SF_B = HEEP_SF;
      myRECOevent.HEEP_SF = HEEP_SF;
      myRECOevent.egamma_SF = egamma_SF;
      setEventWeight_FSB(iEvent, myRECOevent, Muon_LooseID_Weight);
    }
    if (electronJetPairs_noISO.size() > 0) { 
      double HEEP_SF = myHEEP.ScaleFactor(myRECOevent.selectedElectron_noISO_Eta);
      double egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.myElectronCand_noISO->superCluster()->eta(), myRECOevent.selectedElectron_noISO_Pt);
      if (fabs(myRECOevent.selectedElectron_noISO_Eta) > 1.6) myRECOevent.HEEP_SF_E_noISO = HEEP_SF;
      if (fabs(myRECOevent.selectedElectron_noISO_Eta) < 1.4) myRECOevent.HEEP_SF_B_noISO = HEEP_SF;
      myRECOevent.HEEP_SF_noISO = HEEP_SF;
      myRECOevent.egamma_SF_noISO = egamma_SF;
      setEventWeight_FSB_noISO(iEvent, myRECOevent, Muon_LooseID_Weight);
    }
  }

  std::cout << "EVENT PASSES FLAVOR SIDEBAND" << std::endl;
  return true;
}

bool cmsWRextension::preSelectReco(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myRECOevent) {
  std::cout << "beginning preselection" << std::endl;
  muonSelection(iEvent, myRECOevent);
  electronSelection(iEvent, myRECOevent);
  jetSelection(iEvent, iSetup, myRECOevent);


  if( myRECOevent.myAddJetCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<< std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  if( myRECOevent.myMuonCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO MUONS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myMuonCandsHighPt.size()<<" MUONS FOUND." << std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  //BUILD PAIRS OF AK8 JETS WITH THE LEAD MUON
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
    //if( ((*iJet)->p4() + (*iMuon)->p4()).mass() < 400) continue;
    //if (sqrt(deltaR2(*(*iJet),*(*iMuon)))<2.0) continue;
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;

    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiM( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->SDmass );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
 
    double eventMass = ( jetVec + myRECOevent.myMuonCand->p4() ).mass();

    if( eventMass < 200 ) continue;

    muonJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));
    myRECOevent.leadAK8JetMuonMassVal = eventMass;

  }
  if( muonJetPairs.size() < 1 ) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET LEPTON PAIRS" <<std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  std::cout<<muonJetPairs.size()<<" Pairing CANDIDATES Selected from "<<myRECOevent.myAddJetCandsHighPt.size()<<" jets"<<std::endl;
  myRECOevent.myMuonJetPairs = muonJetPairs;

  return true;
}
bool cmsWRextension::preSelectReco_Fast(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myRECOevent) {
  muonSelection(iEvent, myRECOevent);
  electronSelection(iEvent, myRECOevent);
  jetSelection(iEvent, iSetup, myRECOevent);


  if( (myRECOevent.myAddJetCandsHighPt.size() < 1) && myRECOevent.myAddJetCandsHighPt_JECUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_JECDown.size() < 1 && myRECOevent.myAddJetCandsHighPt_JERUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_JERDown.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<< std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  if( myRECOevent.myMuonCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO MUONS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myMuonCandsHighPt.size()<<" MUONS FOUND." << std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  //BUILD PAIRS OF AK8 JETS WITH THE LEAD MUON
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_JECUp;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JECUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JECUp.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_JECUp.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_JECDown;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JECDown.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JECDown.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_JECDown.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_JERUp;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JERUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JERUp.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_JERUp.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_JERDown;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JERDown.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JERDown.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_JERDown.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }


  if( muonJetPairs.size() < 1 && muonJetPairs_JECUp.size() < 1 && muonJetPairs_JECDown.size() < 1 && muonJetPairs_JERUp.size() < 1 && muonJetPairs_JERDown.size() < 1) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET LEPTON PAIRS" <<std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  std::cout<<muonJetPairs.size()<<" Pairing CANDIDATES Selected from "<<myRECOevent.myAddJetCandsHighPt.size()<<" jets"<<std::endl;
  myRECOevent.myMuonJetPairs = muonJetPairs;

  return true;
}
//////////////FOR MUONS TUNEP HIGHPT MUONS ARE USED//////////
///////////FROM THE AN///////////////////
//leading lepton with pT > 60 GeV
//subleading lepton with pT > 53 GeV
//at least two jets with pT > 40 GeV, consider only the leading and subleading in the following if more than two
//all leptons and jets with |Î·| < 2.4
//dilepton mass mll > 200 GeV: to suppress DY+jets contribution âR > 0.4 between all objects in the final state (leptons and jets) Mlljj > 600 GeV
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
  const baconhep::TAddJet*  selJet      = myEvent.myMuonJetPairs[0].first;

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi())); 
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();
  myEvent.subleadMuonEta           = subleadMuon->eta();
  myEvent.subleadMuonPhi           = subleadMuon->phi();

  double dPhi = fabs(myEvent.lsfLeptonPhi - myEvent.subleadMuonPhi);

  if (dPhi > ROOT::Math::Pi()) dPhi -= ROOT::Math::Pi();

  double dRlsfLep_subleadMuon = sqrt(( pow((myEvent.lsfLeptonEta - myEvent.subleadMuonEta), 2.0) + pow( dPhi, 2.0) )); 
  if (dRlsfLep_subleadMuon > ROOT::Math::Pi()) dRlsfLep_subleadMuon -= 2*ROOT::Math::Pi();
  myEvent.mydRlsfLep_subleadMuon = dRlsfLep_subleadMuon;

  if(myEvent.subleadMuon_selMuonMass < 200) return true;
  return false;
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband(const edm::Event& iEvent, eventBits& myEvent, bool nonISO) {  //THIS SELECTION IS A SIDEBAND BASED OF THE MUON FLAVOR SELECTION ONLY
  //CHECK IF WE HAVE A SUBLEADING MUON
  std::cout << "FSB ZMASS" << std::endl;
  if(myEvent.mySubleadMuon == 0) return true;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  std::cout << "SUBLEAD MUON SET" << std::endl;
  const pat::Electron* selEl;
  const baconhep::TAddJet*  selJet;
  std::cout << "SUBLEAD MUON SET" << std::endl;

  if (!nonISO) {
    std::cout << "ISO ZMASS" << std::endl;
    selEl   = myEvent.myElectronCand;
    selJet  = myEvent.myElectronJetPairs[0].first;
    if (selEl == 0)
      std::cout << "CRAP!" << std::endl;
    myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
    myEvent.subleadMuon_selElectronPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selEl->phi()));
    myEvent.subleadMuon_selElectronMass = (subleadMuon->p4() + selEl->p4()).mass();
    myEvent.subleadMuon_selElectronPt   = (subleadMuon->p4() + selEl->p4()).pt();
  } else {
    std::cout << "NONISO ZMASS" << std::endl;
    selEl   = myEvent.myElectronCand_noISO;
    selJet  = myEvent.myElectronJetPairs_noISO[0].first;
    if (selEl == 0)
      std::cout << "CRAP!" << std::endl;
    myEvent.subleadMuon_selJetdPhi_EleNonISO      = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
    myEvent.subleadMuon_selElectronPhi_EleNonISO  = fabs(reco::deltaPhi(subleadMuon->phi(),selEl->phi()));
    myEvent.subleadMuon_selElectronMass_EleNonISO = (subleadMuon->p4() + selEl->p4()).mass();
    myEvent.subleadMuon_selElectronPt_EleNonISO   = (subleadMuon->p4() + selEl->p4()).pt();
  }

  myEvent.subleadMuonEt           = subleadMuon->et();

  if(nonISO)
    if (myEvent.subleadMuon_selElectronMass_EleNonISO < 200)
      return true;
  if(!nonISO)
    if (myEvent.subleadMuon_selElectronMass < 200) 
      return true;
  return false;
}
bool cmsWRextension::METcuts(const edm::Event& iEvent, eventBits& myEvent) {
  const pat::MET*  met         = myEvent.myMET;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const baconhep::TAddJet*  selJet      = myEvent.myMuonJetPairs[0].first;

  myEvent.MET             = met->et();
  myEvent.MET_selJetdPhi  = fabs(reco::deltaPhi(met->phi(),selJet->phi));
  myEvent.MET_selMuondPhi = fabs(reco::deltaPhi(met->phi(),selMuon->phi()));
  TLorentzVector *JetVector_temp = new TLorentzVector();
  JetVector_temp->SetPtEtaPhiM(selJet->pT,selJet->eta,selJet->phi,selJet->SDmass);

  math::XYZTLorentzVector JetVector;
  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());
  myEvent.MET_selJetMass  = (met->p4()+JetVector).mass();
  myEvent.MET_selMuonMass = (met->p4()+selMuon->p4()).mass();
  myEvent.MET_selJetPt    = (met->p4()+JetVector).pt();
  myEvent.MET_selMuonPt   = (met->p4()+selMuon->p4()).pt();

  myEvent.selectedJetTransMET = met->et() * sin(fabs(reco::deltaPhi(selJet->phi,met->phi())));
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
    if ( ! iMuon->isLooseMuon() ) continue;  //Loose MuonID
    allMuons.push_back(&(*iMuon));
  }
  myEvent.muons10 = allMuons.size();
  if (myEvent.muons10 > 1) {
    std::sort(allMuons.begin(),allMuons.end(),::wrTools::compareEtCandidatePointer); 
  }
  if (myEvent.muons10 > 0) {
    myEvent.myMuonCands = allMuons; 
  }
  if(myEvent.muons10 < 1) return false;  //The leading muon should also pass these cuts, so an additional muon would mean 2 or more

  if(flavorSideband==true) {
    myEvent.mySubleadMuon = allMuons.at(0);
    if (m_doGen) {
      if(myEvent.genSecondMuon != NULL) 
        myEvent.dRmuon2 = sqrt(deltaR2(*(myEvent.mySubleadMuon),*(myEvent.genSecondMuon)));
    }
  }else{
  //IF WE HAVE ADDITION MUONS, WE SHOULD SEE WHICH IS THE LEADING MUON WHICH ISN'T THE MAIN CANDIDATE
    for(std::vector<const pat::Muon*>::iterator iMuon = myEvent.myMuonCands.begin(); iMuon != myEvent.myMuonCands.end(); iMuon++) {
      if(fabs(reco::deltaPhi((*iMuon)->phi(), myEvent.myMuonCand->phi())) > 0.01) {
        myEvent.mySubleadMuon = *iMuon;
        if (m_doGen) {
          if(myEvent.genSecondMuon != 0) 
            myEvent.dRmuon2 = sqrt(deltaR2(*(myEvent.mySubleadMuon),*(myEvent.genSecondMuon)));
        }
        break;
      }
    }
  }

  if(flavorSideband==false && myEvent.mySubleadMuon != NULL){
    double muPhi  = myEvent.mySubleadMuon->phi();
    double muEta  = myEvent.mySubleadMuon->eta();
    double jetPhi = myEvent.selectedJetPhi;
    double jetEta = myEvent.selectedJetEta;
    double dPhi = abs( jetPhi - muPhi ); if ( dPhi > ROOT::Math::Pi() ) dPhi -= 2 * ROOT::Math::Pi();
    double dR = sqrt( ( jetEta - muEta ) * ( jetEta - muEta ) + dPhi * dPhi );
    if ( dR > ROOT::Math::Pi() ) dR -= 2 * ROOT::Math::Pi();

    myEvent.secondRECOMuonRECOjetDR = dR;

    TLorentzVector *JetVector_temp = new TLorentzVector();
    JetVector_temp->SetPtEtaPhiE(myEvent.myMuonJetPairs[0].first->pT,myEvent.myMuonJetPairs[0].first->eta,myEvent.myMuonJetPairs[0].first->phi,myEvent.myMuonJetPairs[0].first->E);

    math::XYZTLorentzVector JetVector;
    JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

    myEvent.leadAK8JetDiMuonMassVal = (JetVector + myEvent.myMuonJetPairs[0].second->p4() + myEvent.mySubleadMuon->p4()).mass();

  }
  std::cout << "check 3" << std::endl;

  if(myEvent.mySubleadMuon == 0) return false;  //THIS SHOULD BE IMPOSSIBLE

  return true;
}

bool cmsWRextension::electronSelection(const edm::Event& iEvent, eventBits& myEvent) {  //Flavor sideband
  std::cout<<"STARTING ELECTRON SELECTION"<<std::endl;
  std::vector<const pat::Electron*> highPTelectrons200;
  std::vector<const pat::Electron*> highPTelectrons150;
  std::vector<const pat::Electron*> highPTelectrons100;
  std::vector<const pat::Electron*> highPTelectrons50;

  std::vector<const pat::Electron*> highPTelectrons200_noISO;
  std::vector<const pat::Electron*> highPTelectrons50_noISO;   //MUST FAIL HEEP TRACK ISO

  edm::Handle<std::vector<pat::TriggerObjectStandAlone> > trigObjsHandle;
  if(m_doTrig) {
    iEvent.getByToken(m_trigObjsToken, trigObjsHandle);
  }


  std::cout << "Looking for a few good electrons" << std::endl;
  edm::Handle<std::vector<pat::Electron>> highElectrons;
  iEvent.getByToken(m_highElectronToken, highElectrons);

  myEvent.nAdditionalHEEP = 0.;
  for(std::vector<pat::Electron>::const_iterator iElec = highElectrons->begin(); iElec != highElectrons->end(); iElec++) {
    //PHASE SPACE CUTS
    const vid::CutFlowResult* vidResult =  iElec->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");
    if(vidResult == NULL) {
      std::cout << "ERROR CANNOT FIND ELECTRON VID RESULTS" << std::endl;
      return false;
    }
    //how to check if everything passed:
    const bool heepIDVID = vidResult->cutFlowPassed();

    //MUST PASS ALL BUT ISO
    if (!heepIDVID) {
      if ( vidResult->getCutResultByIndex(cutnrs::HEEPV70::ET           )  == true && 
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::ETA          )  == true && 
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::DETAINSEED   )  == true &&   
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::DPHIIN       )  == true &&    
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::SIGMAIETAIETA)  == true &&       
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::E2X5OVER5X5  )  == true &&   
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::HADEM        )  == true &&     
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::TRKISO       )  == false &&  //MUST FAIL TRACK ISOLATION 
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::EMHADD1ISO   )  == true &&     
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::DXY          )  == true &&  
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::MISSHITS     )  == true &&  
           vidResult->getCutResultByIndex(cutnrs::HEEPV70::ECALDRIVEN   )  == true     
        ) {
        if (iElec->pt() < 50) {
          myEvent.nAdditionalHEEP_noISO++;    
        }
        if (iElec->pt() >= 200){
          highPTelectrons200_noISO.push_back(&(*iElec));
        }
        if (iElec->pt() >= 50){
          highPTelectrons50_noISO.push_back(&(*iElec));
        }
      } 
    } else {
      //PHASE SPACE CUTS
      std::cout << "Electron pT: " << iElec->pt() << std::endl;
      if( iElec->pt() < 50){
        myEvent.nAdditionalHEEP++;
      }

      if (iElec->pt() >= 200){
        highPTelectrons200.push_back(&(*iElec));
      }
      if (iElec->pt() >= 150){
        highPTelectrons150.push_back(&(*iElec));
      }
      if (iElec->pt() >= 100){
        highPTelectrons100.push_back(&(*iElec));
      }
      if (iElec->pt() >= 50){
        highPTelectrons50.push_back(&(*iElec));
      }
    }
    //std::cout<<"ELECTRON CAND WITH PT,ETA,PHI: "<<iElec->pt()<<","<<iElec->eta()<<","<<iElec->phi()<<std::endl;
  }
      //if( iLep->pt() > 200 ) highPTMuons.push_back(&(*iLep));
  //COLLECT MUONS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  myEvent.electronCands200 = highPTelectrons200.size();
  myEvent.electronCands150 = highPTelectrons150.size();
  myEvent.electronCands100 = highPTelectrons100.size();
  myEvent.electronCands50  = highPTelectrons50.size();

  myEvent.electronCands200_noISO = highPTelectrons200_noISO.size();
  myEvent.electronCands50_noISO  = highPTelectrons50_noISO.size();
  if (myEvent.electronCands200_noISO > 0) {
    std::sort(highPTelectrons200_noISO.begin(),highPTelectrons200_noISO.end(),::wrTools::compareEtCandidatePointer);
    myEvent.myElectronCandsHighPt200_noISO = highPTelectrons200_noISO;
  }
  if (myEvent.electronCands50_noISO > 0) {
    std::sort(highPTelectrons50_noISO.begin(),highPTelectrons50_noISO.end(),::wrTools::compareEtCandidatePointer);
    myEvent.myElectronCandsHighPt50_noISO = highPTelectrons50_noISO;
    myEvent.myElectronCand_noISO = highPTelectrons50_noISO[0];
  }
  if (myEvent.electronCands200 > 0) {
    std::sort(highPTelectrons200.begin(),highPTelectrons200.end(),::wrTools::compareEtCandidatePointer); 
    myEvent.myElectronCandsHighPt200 = highPTelectrons200;
  }
  if (myEvent.electronCands150 > 0) {
    std::sort(highPTelectrons150.begin(),highPTelectrons150.end(),::wrTools::compareEtCandidatePointer); 
    myEvent.myElectronCandsHighPt150 = highPTelectrons150;
  }
  if (myEvent.electronCands100 > 0) {
    std::sort(highPTelectrons100.begin(),highPTelectrons100.end(),::wrTools::compareEtCandidatePointer); 
    myEvent.myElectronCandsHighPt100 = highPTelectrons100;
  }
  if (myEvent.electronCands50 > 0) {
    std::sort(highPTelectrons50.begin(),highPTelectrons50.end(),::wrTools::compareEtCandidatePointer); 
    myEvent.myElectronCandsHighPt50 =  highPTelectrons50;
    myEvent.myElectronCand = highPTelectrons50[0];
    return true;  //NOT CURRENTLY USED
  }
  return false;//NOT CURRENTLY USED

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

//  edm::Handle<std::vector<reco::Vertex>> vertices;
//  iEvent.getByToken(m_offlineVerticesToken, vertices);
  
  for(std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {
    std::cout << "Looping through muons" << std::endl;
    if( iMuon->pt() < 40 || fabs(iMuon->eta()) > 2.4 ) continue; //preliminary pt cut to speed the loop, and the eta cut
//    if(m_doTrig) {
//      std::cout << "Checking Filter" << std::endl;
//      if (! ::wrTools::checkFilters(iMuon->eta(),iMuon->phi(),*trigObjsHandle,m_muonFiltersToPass) ) continue;      
//    }
    if(( iMuon->isHighPtMuon(*myEvent.PVertex) && iMuon->tunePMuonBestTrack()->pt() > m_highPTleptonCut) && (iMuon->isolationR03().sumPt/iMuon->pt() <= .05)) {
      std::cout<<"LEPTON CAND WITH PT,ETA,PHI: "<<iMuon->pt()<<","<<iMuon->eta()<<","<<iMuon->phi()<<std::endl;
     
      highPTMuons.push_back(&(*iMuon));
    }

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
bool cmsWRextension::jetSelection(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myEvent) {
  std::cout<<"STARTING JET SELECTION"<<std::endl;
  edm::Handle<std::vector<pat::Jet>> recoJetsAK8;
  iEvent.getByToken(m_AK8recoPUPPIJetsToken, recoJetsAK8);
  assert(recoJetsAK8.isValid());

  edm::Handle<std::vector<pat::Jet>> recoCHSJetsAK8;
  iEvent.getByToken(m_AK8recoCHSJetsToken, recoCHSJetsAK8);
  assert(recoCHSJetsAK8.isValid());

  edm::Handle<std::vector<pat::Jet>> recoSubJets;
  iEvent.getByToken(m_AK8recoPUPPISubJetsToken, recoSubJets);
  assert(recoSubJets.isValid());

  edm::Handle<double> rhoHandle;
  iEvent.getByToken(m_rhoLabel, rhoHandle);
  double rho = *(rhoHandle.product());

  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/cmsWRextensions/data/";
  if(m_isMC){
    fJetUnc = new JetCorrectionUncertainty(Form("%s/Summer16_23Sep2016V4_MC_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
  }else{
    if(iEvent.id().run()<276812){
      fJetUnc = new JetCorrectionUncertainty(Form("%s//Summer16_23Sep2016BCDV4_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    }else if(iEvent.id().run() > 276830 && iEvent.id().run() < 278809){
      fJetUnc = new JetCorrectionUncertainty(Form("%s//Summer16_23Sep2016EFV4_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    }else if(iEvent.id().run() > 278819 && iEvent.id().run() < 280386){
      fJetUnc = new JetCorrectionUncertainty(Form("%s//Summer16_23Sep2016GV4_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    }else if(iEvent.id().run() > 280918){
      fJetUnc = new JetCorrectionUncertainty(Form("%s//Summer16_23Sep2016HV4_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    }
  }

  std::vector<const baconhep::TAddJet*> AddJets;
  std::vector<const baconhep::TAddJet*> AddJets_JECUp;
  std::vector<const baconhep::TAddJet*> AddJets_JECDown;
  std::vector<const baconhep::TAddJet*> AddJets_JERUp;
  std::vector<const baconhep::TAddJet*> AddJets_JERDown;

  //COLLECT JetS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  for(std::vector<pat::Jet>::const_iterator iJet = recoJetsAK8->begin(); iJet != recoJetsAK8->end(); iJet++) {
   const pat::Jet *subjet1=0, *subjet2=0, *subjet3=0, *subjet4=0;
   double csv1=-2, csv2=-2, csv3=-2, csv4=-2;
   for(std::vector<pat::Jet>::const_iterator iSubJet = recoSubJets->begin(); iSubJet != recoSubJets->end(); iSubJet++) {
    if(reco::deltaR(iJet->eta(),iJet->phi(),iSubJet->eta(),iSubJet->phi())>0.8) continue;

    std::cout << "iSubJet->pt(): " << iSubJet->pt() << std::endl;
    if(!subjet1 || iSubJet->pt() > subjet1->pt()) {
      subjet4 = subjet3;
      csv4    = csv3;

      subjet3 = subjet2;
      csv3    = csv2;

      subjet2 = subjet1;
      csv2    = csv1;

      subjet1 = &(*iSubJet);
      csv1    = iSubJet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    } else if(!subjet2 || iSubJet->pt() > subjet2->pt()) {
      subjet4 = subjet3;
      csv4    = csv3;

      subjet3 = subjet2;
      csv3    = csv2;

      subjet2 = &(*iSubJet);
      csv2    = iSubJet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");

    } else if(!subjet3 || iSubJet->pt() > subjet3->pt()) {
      subjet4 = subjet3;
      csv4    = csv3;

      subjet3 = &(*iSubJet);
      csv3    = iSubJet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");

    } else if(!subjet4 || iSubJet->pt() > subjet4->pt()) {
      subjet4 = &(*iSubJet);
      csv4    = iSubJet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    }
   }
    std::cout << "csv1: " << csv1 << ", csv2: " << csv2 << ", csv3: " << csv3 << ", csv4: " << csv4 << std::endl;
    std::cout << "Jet test1: " << iJet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") << std::endl;
    //Finding maximum CSV
    double maxCSV = -10;
    if(csv1 > maxCSV) maxCSV=csv1;
    if(csv2 > maxCSV) maxCSV=csv2;
    if(csv3 > maxCSV) maxCSV=csv3;
    if(csv4 > maxCSV) maxCSV=csv4;

    std::cout<<"maxCSV: " << maxCSV << std::endl;

    std::cout<<"looping over jets"<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
    //GETS ALL THE RELEVANT JET ID QUANTITIES
    double NHF  =                iJet->neutralHadronEnergyFraction();
    double NEMF =                iJet->neutralEmEnergyFraction();
    double CHF  =                iJet->chargedHadronEnergyFraction();
//    double MUF  =                iJet->muonEnergyFraction();
    double CEMF =                iJet->chargedEmEnergyFraction();
    double NumConst =            iJet->chargedMultiplicity()+iJet->neutralMultiplicity();
//    double NumNeutralParticles = iJet->neutralMultiplicity();
    double CHM      =            iJet->chargedMultiplicity(); 


    fJetUnc->setJetPt(iJet->pt());
    fJetUnc->setJetEta(iJet->eta());
    double jetUNC =              fJetUnc->getUncertainty(true);
    JME::JetParameters parameters = {{JME::Binning::JetPt, iJet->pt()}, {JME::Binning::JetEta, iJet->eta()}, {JME::Binning::Rho, TMath::Min(rho,44.30)}};
    float sigma_MC = resolution.getResolution(parameters);
    float sf = resolution_sf.getScaleFactor(parameters);
    float sfUp = resolution_sf.getScaleFactor(parameters, Variation::UP);
    float sfDown = resolution_sf.getScaleFactor(parameters, Variation::DOWN);
    double x1 = r->Gaus();
    double x2 = r->Gaus();
    double x3 = r->Gaus();
    double jetEnergySmearFactor = 1.0;
    double jetEnergySmearFactorUp = 1.0;
    double jetEnergySmearFactorDown = 1.0;
    if(m_isMC){
      jetEnergySmearFactor = 1.0 + sqrt(sf*sf - 1.0)*sigma_MC*x1;
      jetEnergySmearFactorUp = 1.0 + sqrt(sfUp*sfUp - 1.0)*sigma_MC*x2;
      jetEnergySmearFactorDown = 1.0 + sqrt(sfDown*sfDown - 1.0)*sigma_MC*x3;
    }
    double jetCorrPtSmear = iJet->pt()*jetEnergySmearFactor;
    double jetPtJESUp =            iJet->pt()*jetEnergySmearFactor*(1+jetUNC);
    double jetPtJESDown =          iJet->pt()*jetEnergySmearFactor*(1-jetUNC);
    double jetPtJERUp = iJet->pt()*jetEnergySmearFactorUp;
    double jetPtJERDown = iJet->pt()*jetEnergySmearFactorDown;

    double jetCorrESmear = iJet->energy()*jetEnergySmearFactor;
    double jetEJESUp = iJet->energy()*jetEnergySmearFactor*(1+jetUNC);
    double jetEJESDown = iJet->energy()*jetEnergySmearFactor*(1-jetUNC);
    double jetEJERUp = iJet->energy()*jetEnergySmearFactorUp;
    double jetEJERDown = iJet->energy()*jetEnergySmearFactorDown;


    //APPLYING ETA AND PT CUT
    if ( iJet->pt() < 40 ) continue;
    if ( fabs(iJet->eta()) > 2.4) continue;
    //APPLYING TIGHT QUALITY CUTS
    if (NHF > .9) continue;
    if (NEMF > .9) continue;
    if (NumConst <= 1) continue;
//    if (MUF > .8 ) continue;
    //ADDITIONAL CUTS BECAUSE OF TIGHT ETA CUT
    if (CHF == 0) continue;
    if (CHM == 0) continue;
    if (CEMF > .99) continue;
    //ANALYSIS SPECIFIC CUTS
   // if (MUF <= .05) continue;
    //JETS PASSING CUTS

    //LSF Info
    std::vector<reco::CandidatePtr> pfConstituents = iJet->getJetConstituents();
    std::vector<fastjet::PseudoJet>   lClusterParticles;
    for(unsigned int ic=0; ic<pfConstituents.size(); ic++) {
      reco::CandidatePtr pfcand = pfConstituents[ic];
      fastjet::PseudoJet   pPart(pfcand->px(),pfcand->py(),pfcand->pz(),pfcand->energy());
      lClusterParticles.emplace_back(pPart);
    }

     std::sort(lClusterParticles.begin(),lClusterParticles.end(),JetTools::orderPseudoJet);

    float lepCPt(-100), lepCEta(-100), lepCPhi(-100);
    float lepCId(0);

    if(JetTools::leptons((*iJet),3)> 0 && JetTools::leptons(*iJet,2)<0.8) {
      lepCPt = JetTools::leptons(*iJet,3);
      lepCEta = JetTools::leptons(*iJet,5);
      lepCPhi = JetTools::leptons(*iJet,6);
      lepCId = JetTools::leptons(*iJet,4);

      myEvent.lsfLeptonPt = lepCPt;
      myEvent.lsfLeptonEta = lepCEta;
      myEvent.lsfLeptonPhi = lepCPhi;
    
    }


    //JETS PASSING WITH VERY HIGH PT
    if( jetCorrPtSmear > 200 ){
      double minDR_CHS_PUPPI_Jets = 100.;
      double dR_CHS = 0.;
      double CHS_Mass = -10.;
      for(std::vector<pat::Jet>::const_iterator iCHSJet = recoCHSJetsAK8->begin(); iCHSJet != recoCHSJetsAK8->end(); iCHSJet++) {
	if(iCHSJet->pt() > 200){
	  dR_CHS = sqrt(deltaR2(*(iJet),*(iCHSJet)));
	  if (dR_CHS>ROOT::Math::Pi())dR_CHS-=2*ROOT::Math::Pi();
	  if (dR_CHS < minDR_CHS_PUPPI_Jets){
	    minDR_CHS_PUPPI_Jets=dR_CHS;
	    CHS_Mass = iCHSJet->userFloat("ak8PFJetsCHSPrunedMass");
	  }
	}
      }
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass") > 40){
      	baconhep::TAddJet* pAddJet = new baconhep::TAddJet();

      	pAddJet->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
      	pAddJet->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
      	pAddJet->pT   = jetCorrPtSmear;
      	pAddJet->E    = jetCorrESmear;
      	pAddJet->eta  = iJet->eta();
      	pAddJet->phi  = iJet->phi();
      	pAddJet->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
      	pAddJet->PrunedMass = CHS_Mass;
//      pAddJet->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*PUPPIweight(iJet->pt(), iJet->eta());

        std::vector<fastjet::PseudoJet> vSubCInc; pAddJet->lsfCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2);
        std::vector<fastjet::PseudoJet> vSubC_2;  pAddJet->lsfC_2 = JetTools::lsf(lClusterParticles, vSubC_2, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 2);
        std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);
        std::vector<fastjet::PseudoJet> vSubC_4;  pAddJet->lsfC_4 = JetTools::lsf(lClusterParticles, vSubC_4, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 4);

        if(pAddJet->lsfC_3 > 0.0){

          if(vSubC_3.size() > 0) {
            pAddJet->lsfC_3_sj1_pt = vSubC_3[0].pt();
            pAddJet->lsfC_3_sj1_eta = vSubC_3[0].eta();
            pAddJet->lsfC_3_sj1_phi = vSubC_3[0].phi();
            pAddJet->lsfC_3_sj1_m = vSubC_3[0].m();
          }
          if(vSubC_3.size() > 1) {
            pAddJet->lsfC_3_sj2_pt = vSubC_3[1].pt();
            pAddJet->lsfC_3_sj2_eta = vSubC_3[1].eta();
            pAddJet->lsfC_3_sj2_phi = vSubC_3[1].phi();
            pAddJet->lsfC_3_sj2_m = vSubC_3[1].m();
          }
          if(vSubC_3.size() > 2) {
            pAddJet->lsfC_3_sj3_pt = vSubC_3[2].pt();
            pAddJet->lsfC_3_sj3_eta = vSubC_3[2].eta();
            pAddJet->lsfC_3_sj3_phi = vSubC_3[2].phi();
            pAddJet->lsfC_3_sj3_m = vSubC_3[2].m();
          }

          pAddJet->lmdCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2, 1);
          pAddJet->lmdC_2 = JetTools::lsf(lClusterParticles, vSubC_2, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 2, 1);
          pAddJet->lmdC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3, 1);
          pAddJet->lmdC_4 = JetTools::lsf(lClusterParticles, vSubC_4, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 4, 1);
      
          AddJets.push_back(pAddJet);
      	  std::cout<<"AK8JET CAND WITH PT,ETA,PHI,MASS: "<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<","<< iJet->userFloat("ak8PFJetsPuppiSoftDropMass") << std::endl;
        }
      }
    }
    if( jetPtJESUp > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass") > 40){
        baconhep::TAddJet* pAddJet_JECUp = new baconhep::TAddJet();

        pAddJet_JECUp->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JECUp->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JECUp->pT = jetPtJESUp;
        pAddJet_JECUp->E  = jetEJESUp;
        pAddJet_JECUp->eta = iJet->eta();
        pAddJet_JECUp->phi = iJet->phi();
        pAddJet_JECUp->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        AddJets_JECUp.push_back(pAddJet_JECUp);
      }
    }

    if( jetPtJESDown > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass") > 40){
        baconhep::TAddJet* pAddJet_JECDown = new baconhep::TAddJet();

        pAddJet_JECDown->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JECDown->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JECDown->pT = jetPtJESDown;
        pAddJet_JECDown->E  = jetEJESDown;
        pAddJet_JECDown->eta = iJet->eta();
        pAddJet_JECDown->phi = iJet->phi();
        pAddJet_JECDown->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        AddJets_JECDown.push_back(pAddJet_JECDown);
      }
    }
    if( jetPtJERUp > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass") > 40){
        baconhep::TAddJet* pAddJet_JERUp = new baconhep::TAddJet();

        pAddJet_JERUp->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JERUp->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JERUp->pT = jetPtJERUp;
        pAddJet_JERUp->E  = jetEJERUp;
        pAddJet_JERUp->eta = iJet->eta();
        pAddJet_JERUp->phi = iJet->phi();
        pAddJet_JERUp->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        AddJets_JERUp.push_back(pAddJet_JERUp);
      }
    }
    if( jetPtJERDown > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass") > 40){
        baconhep::TAddJet* pAddJet_JERDown = new baconhep::TAddJet();

        pAddJet_JERDown->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JERDown->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JERDown->pT = jetPtJERDown;
        pAddJet_JERDown->E  = jetEJERDown;
        pAddJet_JERDown->eta = iJet->eta();
        pAddJet_JERDown->phi = iJet->phi();
        pAddJet_JERDown->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        AddJets_JERDown.push_back(pAddJet_JERDown);
      }
    }
  } 
  std::sort(AddJets.begin(),AddJets.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JECUp.begin(),AddJets_JECUp.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JECDown.begin(),AddJets_JECDown.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JERUp.begin(),AddJets_JERUp.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JERDown.begin(),AddJets_JERDown.end(),::wrTools::compareEtCandidatePointerTAddJet);

  myEvent.myAddJetCandsHighPt = AddJets;
  myEvent.myAddJetCandsHighPt_JECUp = AddJets_JECUp;
  myEvent.myAddJetCandsHighPt_JECDown = AddJets_JECDown;
  myEvent.myAddJetCandsHighPt_JERUp = AddJets_JERUp;
  myEvent.myAddJetCandsHighPt_JERDown = AddJets_JERDown;
  myEvent.ak8jetCands = AddJets.size();

  double dPhi = 0.;
  double dR   = 0.;
  int nDaughterContainedPtLeadJet = 0;
  int nDaughterContainedSubLeadJet = 0;
  int nDaughterContained2ndSubLeadJet = 0;
  int nDaughterContained3rdSubLeadJet = 0;
  int capturedBothDaughtersInSingleJet = -1;
  if(AddJets.size() > 0){
    for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
      dPhi=abs(AddJets[0]->phi-myEvent.genWRDaughters[i]->phi()); if (dPhi>ROOT::Math::Pi()) dPhi-=2*ROOT::Math::Pi();
      dR = sqrt((AddJets[0]->eta-myEvent.genWRDaughters[i]->eta())*(AddJets[0]->eta-myEvent.genWRDaughters[i]->eta()) + dPhi*dPhi);
      if (dR>ROOT::Math::Pi())dR-=2*ROOT::Math::Pi();
      if (dR<0.4)nDaughterContainedPtLeadJet++;
      myEvent.daughterClusterVector = myEvent.daughterClusterVector + myEvent.genWRDaughters[i]->p4();
    }
    if(AddJets.size() > 1){
      for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
        dPhi=abs(AddJets[1]->phi-myEvent.genWRDaughters[i]->phi()); if (dPhi>ROOT::Math::Pi()) dPhi-=2*ROOT::Math::Pi();
	dR = sqrt((AddJets[1]->eta-myEvent.genWRDaughters[i]->eta())*(AddJets[1]->eta-myEvent.genWRDaughters[i]->eta()) + dPhi*dPhi);
	if (dR>ROOT::Math::Pi())dR-=2*ROOT::Math::Pi();
	if (dR<0.4)nDaughterContainedSubLeadJet++;
      }
    }
    if(AddJets.size() > 2){
      for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
        dPhi=abs(AddJets[2]->phi-myEvent.genWRDaughters[i]->phi()); if (dPhi>ROOT::Math::Pi()) dPhi-=2*ROOT::Math::Pi();
        dR = sqrt((AddJets[2]->eta-myEvent.genWRDaughters[i]->eta())*(AddJets[2]->eta-myEvent.genWRDaughters[i]->eta()) + dPhi*dPhi);
        if (dR>ROOT::Math::Pi())dR-=2*ROOT::Math::Pi();
        if (dR<0.4)nDaughterContained2ndSubLeadJet++;
      }
    }
    if(AddJets.size() > 3){
      for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
        dPhi=abs(AddJets[3]->phi-myEvent.genWRDaughters[i]->phi()); if (dPhi>ROOT::Math::Pi()) dPhi-=2*ROOT::Math::Pi();
        dR = sqrt((AddJets[3]->eta-myEvent.genWRDaughters[i]->eta())*(AddJets[3]->eta-myEvent.genWRDaughters[i]->eta()) + dPhi*dPhi);
        if (dR>ROOT::Math::Pi())dR-=2*ROOT::Math::Pi();
        if (dR<0.4)nDaughterContained3rdSubLeadJet++;
      }
    }
  }
  if((nDaughterContainedPtLeadJet==1 && nDaughterContainedSubLeadJet==1) ||(nDaughterContainedPtLeadJet==1 && nDaughterContained2ndSubLeadJet==1) || (nDaughterContainedSubLeadJet==1 && nDaughterContained2ndSubLeadJet==1) || (nDaughterContainedPtLeadJet==1 && nDaughterContained3rdSubLeadJet==1) || (nDaughterContainedSubLeadJet==1 && nDaughterContained3rdSubLeadJet==1) || (nDaughterContained2ndSubLeadJet==1 && nDaughterContained3rdSubLeadJet==1)){
    capturedBothDaughtersInSingleJet=0;
  }else if(nDaughterContainedPtLeadJet==2 || nDaughterContainedSubLeadJet==2 || nDaughterContained2ndSubLeadJet==2 || nDaughterContained3rdSubLeadJet==2){
    capturedBothDaughtersInSingleJet=1;
  }

  int JetContainingBothDaughters = -1;
  if(nDaughterContainedPtLeadJet==2){
    JetContainingBothDaughters = 0;
  }else if(nDaughterContainedSubLeadJet==2){
    JetContainingBothDaughters = 1;
  }else if(nDaughterContained2ndSubLeadJet==2){
    JetContainingBothDaughters = 2;
  }else if(nDaughterContained3rdSubLeadJet==2){
    JetContainingBothDaughters = 3;
  }
  myEvent.JetContainingBothDaughters = JetContainingBothDaughters;
  myEvent.capturedBothDaughtersInSingleJet = capturedBothDaughtersInSingleJet;

  if (myEvent.genSecondMuon != 0 && myEvent.genWRDaughters.size() > 1) {
    std::cout << "DR CALC" << std::endl;
    double jetPhi = myEvent.daughterClusterVector.phi();
    double muPhi  = myEvent.genSecondMuon->phi();
    double jetEta = myEvent.daughterClusterVector.eta();
    double muEta  = myEvent.genSecondMuon->eta();
    double dPhi = 0.0;
    double dR   = 0.0;
    dPhi = abs( jetPhi - muPhi ); if ( dPhi > ROOT::Math::Pi() ) dPhi -= 2 * ROOT::Math::Pi();
    dR = sqrt( ( jetEta - muEta ) * ( jetEta - muEta ) + dPhi * dPhi );
    if ( dR > ROOT::Math::Pi() ) dR -= 2 * ROOT::Math::Pi();

    myEvent.secondMuonWRjetdR = dR;
  }

  return true;
}
double cmsWRextension::PUPPIweight(double puppipt, double puppieta){

  double genCorr=1;
  double recoCorr=1;
  double totalWeight=1;

  TF1 corrGEN = TF1("corrGEN", "[0]+[1]*pow(x*[2],-[3])", 200, 3500);
  corrGEN.SetParameter(0, 1.00626);
  corrGEN.SetParameter(1, -1.06161);
  corrGEN.SetParameter(2, 0.0799900);
  corrGEN.SetParameter(3, 1.20454);

  TF1 corrRECO_cen = TF1("corrRECO_cen", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)", 200, 3500);
  corrRECO_cen.SetParameter(0, 1.09302);
  corrRECO_cen.SetParameter(1, -0.000150068);
  corrRECO_cen.SetParameter(2, 3.44866e-07);
  corrRECO_cen.SetParameter(3, -2.68100e-10);
  corrRECO_cen.SetParameter(4, 8.67440e-14);
  corrRECO_cen.SetParameter(5, -1.00114e-17);

  TF1 corrRECO_for = TF1("corrRECO_for", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)", 200, 3500);
  corrRECO_for.SetParameter(0, 1.27212);
  corrRECO_for.SetParameter(1, -0.000571640);
  corrRECO_for.SetParameter(2, 8.37289e-07);
  corrRECO_for.SetParameter(3, -5.20433e-10);
  corrRECO_for.SetParameter(4, 1.45375e-13);
  corrRECO_for.SetParameter(5, -1.50389e-17);

  genCorr = corrGEN.Eval(puppipt);
  if (abs(puppieta) < 1.3){
    recoCorr = corrRECO_cen.Eval(puppipt);
  }else{
    recoCorr = corrRECO_for.Eval(puppipt);
    totalWeight = genCorr * recoCorr;
  }

  return totalWeight;

}
//float cmsWRextensions::genLSF(const edm::Event& iEvent, eventBits& myEvent)
//{
//  //LSF Info
//  std::vector<reco::CandidatePtr> pfConstituents = iJet->getJetConstituents();
//  std::vector<fastjet::PseudoJet>   lClusterParticles;
//  for(unsigned int ic=0; ic<pfConstituents.size(); ic++) {
//    reco::CandidatePtr pfcand = pfConstituents[ic];
//    fastjet::PseudoJet   pPart(pfcand->px(),pfcand->py(),pfcand->pz(),pfcand->energy());
//    lClusterParticles.emplace_back(pPart);
//  }
//  
//  std::sort(lClusterParticles.begin(),lClusterParticles.end(),JetTools::orderPseudoJet);
//  
//  float lepCPt(-100), lepCEta(-100), lepCPhi(-100);
//  float lepCId(0);
//  lepCPt = JetTools::leptons(*iJet,3);
//  lepCEta = JetTools::leptons(*iJet,5);
//  lepCPhi = JetTools::leptons(*iJet,6);
//  lepCId = JetTools::leptons(*iJet,4);
//  std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);
//
//
//}
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

  std::vector<const reco::GenParticle*> genLeptons;
  std::vector<const reco::GenParticle*> WRDaughters;

  int GENnDaughters = 0;

  for(std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
    if(!iParticle->isHardProcess())   continue;
    if(iParticle->status() == 21)     continue;
    if(abs(iParticle->pdgId()) == 13) {
      nMuons++;
      genLeptons.push_back(&(*iParticle));

      if(abs(iParticle->mother(0)->pdgId()) == 9900014)
        std::cout << "FOUND MUON NEUTRINO DECAY MUON" << std::endl;
        myEvent.genSecondMuon = &(*iParticle);
    }
    if(abs(iParticle->pdgId()) == 11) {
      nElectrons++;
      genLeptons.push_back(&(*iParticle));
    }
    if(abs(iParticle->pdgId()) == 15) nTaus++;
    if(abs(iParticle->pdgId())  <= 4) nLightPartons++;
    if(abs(iParticle->pdgId())  == 5) nBs++;
    if(abs(iParticle->pdgId())  == 6) nTops++;

    if(abs(iParticle->pdgId())  == 1 || abs(iParticle->pdgId())  == 2 || abs(iParticle->pdgId())  == 3 || abs(iParticle->pdgId())  == 4 || abs(iParticle->pdgId())  == 5 || abs(iParticle->pdgId())  == 6){
      std::cout << "iParticle->mother()->pdgId(): " << iParticle->mother()->pdgId() << std::endl;
      if(iParticle->mother()->pdgId() == 9900014 || iParticle->mother()->pdgId() == 9900012 || iParticle->mother()->pdgId() == 9900016){
	WRDaughters.push_back(&(*iParticle));
        GENnDaughters++;
      }
    }
  }

  double dPhi = 0.;
  double dR_Daughters = -1.0;
  if (WRDaughters.size() > 1){
    dPhi=abs(WRDaughters[0]->phi()-WRDaughters[1]->phi()); if (dPhi>ROOT::Math::Pi()) dPhi-=2*ROOT::Math::Pi();
    dR_Daughters = sqrt((WRDaughters[0]->eta()-WRDaughters[1]->eta())*(WRDaughters[0]->eta()-WRDaughters[1]->eta()) + dPhi*dPhi);
    if (dR_Daughters>ROOT::Math::Pi()) dR_Daughters-=2*ROOT::Math::Pi();
    
  }
  myEvent.dR_Daughters = dR_Daughters;

  if ( genLeptons.size() > 0 ) { 
    std::sort( genLeptons.begin(), genLeptons.end(), ::wrTools::compareEtGenParticlePointer ); 
    myEvent.myGenLeptons = genLeptons;
    double x =  myEvent.myGenLeptons[0]->vertex().x();
    double y =  myEvent.myGenLeptons[0]->vertex().y();
    double z =  myEvent.myGenLeptons[0]->vertex().z();
    std::cout << "GEN VTX: X: "<<x <<" Y: "<<y<<" Z: "<<z<<""<< std::endl;   
    myEvent.genVtx = new math::XYZPoint();
//    myEvent.genVtx->SetXYZ( x, y, z );
  } 
  std::cout << "SETTING VALS" << std::endl;
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
  myEvent.genWRDaughters     = WRDaughters  ;
  myEvent.nDaughters	     = GENnDaughters;

//HERE IS THE CHARACTERIZATION DECODER TABLE
//
//  if (nMuons == 2 && nPartons == 2)   flavor = 1;     
//  if (nTaus >= 1 && nTops >= 1)       flavor = 2;       
//  else if (nTaus >= 1 && nTops == 0)       flavor = 3;
//  //else if (mynMuons == 1 && mynElectrons >= 1) flavor = 4;
//  else flavor = 9;
//  
  myEvent.myEventFlavor = flavor;

  if (genLeptons.size() > 0)
    return true;
  else
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

      //LSF Info
      std::vector<reco::CandidatePtr> pfConstituents = iJet->getJetConstituents();
      std::vector<fastjet::PseudoJet>   lClusterParticles;
      for(unsigned int ic=0; ic<pfConstituents.size(); ic++) {
        reco::CandidatePtr pfcand = pfConstituents[ic];
        fastjet::PseudoJet   pPart(pfcand->px(),pfcand->py(),pfcand->pz(),pfcand->energy());
        lClusterParticles.emplace_back(pPart);
      }
      
      std::sort(lClusterParticles.begin(),lClusterParticles.end(),JetTools::orderPseudoJet);
      
      float lepCPt(-100), lepCEta(-100), lepCPhi(-100);
      float lepCId(0);
      lepCPt = JetTools::leptons(*iJet,3);
      lepCEta = JetTools::leptons(*iJet,5);
      lepCPhi = JetTools::leptons(*iJet,6);
      lepCId = JetTools::leptons(*iJet,4);
      double genLSF;
      std::vector<fastjet::PseudoJet> vSubC_3;
      genLSF = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);
      myEvent.myGenLSF = genLSF;
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
    myEvent.leadAK8JetMuonMassValGEN = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).mass();
    myEvent.leadAK8JetMuonPtValGEN   = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).pt();
    myEvent.leadAK8JetMuonEtaValGEN  = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4()).eta();
  }
  else{
    myEvent.leadAK8JetMuonMassValGEN = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenMuons[1]->p4()).mass();
    myEvent.leadAK8JetMuonPtValGEN   = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenMuons[1]->p4()).pt();
    myEvent.leadAK8JetMuonEtaValGEN  = (myEvent.myAK8GenJets[0]->p4() + myEvent.myGenMuons[0]->p4() + myEvent.myGenMuons[1]->p4()).eta();
  }
  return true;
}
bool cmsWRextension::passExtensionRECO(const edm::Event& iEvent, eventBits& myRECOevent) {
  //LOOP OVER JET MUON PAIRS AND TAKE THE HIGHEST MASS ONE
  std::sort(myRECOevent.myMuonJetPairs.begin(),myRECOevent.myMuonJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);
  TLorentzVector *JetVector_temp = new TLorentzVector();
  JetVector_temp->SetPtEtaPhiE(myRECOevent.myMuonJetPairs[0].first->pT,myRECOevent.myMuonJetPairs[0].first->eta,myRECOevent.myMuonJetPairs[0].first->phi,myRECOevent.myMuonJetPairs[0].first->E);

  math::XYZTLorentzVector JetVector;
  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

  myRECOevent.leadAK8JetMuonMassVal = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).mass();
  myRECOevent.leadAK8JetMuonPtVal   = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal  = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs[0].first->phi, myRECOevent.myMuonJetPairs[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs[0].second->eta();
  myRECOevent.selectedJetPt   = myRECOevent.myMuonJetPairs[0].first->pT;
  myRECOevent.selectedJetPhi  = myRECOevent.myMuonJetPairs[0].first->phi;
  myRECOevent.selectedJetEta  = myRECOevent.myMuonJetPairs[0].first->eta;
  myRECOevent.selectedJetMass = myRECOevent.myMuonJetPairs[0].first->SDmass;
  myRECOevent.selectedJetLSF3 = myRECOevent.myMuonJetPairs[0].first->lsfC_3;
  myRECOevent.selectedJetMaxSubJetCSV = myRECOevent.myMuonJetPairs[0].first->maxSubJetCSV;
  myRECOevent.selectedJetPrunedMass = myRECOevent.myMuonJetPairs[0].first->PrunedMass;

  if(myRECOevent.myMuonJetPairs[0].first->tau1==0){
    myRECOevent.selectedJetTau21 = -9999.;
  }else{
    myRECOevent.selectedJetTau21 = (myRECOevent.myMuonJetPairs[0].first->tau2)/(myRECOevent.myMuonJetPairs[0].first->tau1);
  }

  double MaxDR_genDaughter_CandJet = -1.0;
  double dPhi = 0.;
  int nDaughterInSelectedJet = 0;
  double dR = 0.;
  if(m_isMC){
    for(int i = 0; i < int(myRECOevent.genWRDaughters.size()); i++){
      std::cout << "test" << std::endl;
      dPhi=abs(myRECOevent.myMuonJetPairs[0].first->phi-myRECOevent.genWRDaughters[i]->phi()); if (dPhi>ROOT::Math::Pi()) dPhi-=2*ROOT::Math::Pi();
      if(sqrt((myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta())*(myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta()) + dPhi*dPhi) > MaxDR_genDaughter_CandJet){
	MaxDR_genDaughter_CandJet = sqrt((myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta())*(myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta()) + dPhi*dPhi);
	if (MaxDR_genDaughter_CandJet>ROOT::Math::Pi()) MaxDR_genDaughter_CandJet-=2*ROOT::Math::Pi();
      }
      if(sqrt((myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta())*(myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta()) + dPhi*dPhi) >ROOT::Math::Pi()){
	dR = sqrt((myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta())*(myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta()) + dPhi*dPhi) - 2*ROOT::Math::Pi();
      }else{
        dR = sqrt((myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta())*(myRECOevent.myMuonJetPairs[0].first->eta-myRECOevent.genWRDaughters[i]->eta()) + dPhi*dPhi);
      }
      if(dR < 0.4){
	nDaughterInSelectedJet++;
      }
    }
  }
  std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;

  int pickedCorrectJet = -1;
  if(myRECOevent.capturedBothDaughtersInSingleJet==1){
    if(nDaughterInSelectedJet==2){
      pickedCorrectJet = 1;
    }else{
      pickedCorrectJet = 0;
    }
  }

  double dPhi_LeadMuonJetWithDaughters = -10.;
  double selectedIncorrectJetMass = -10.;
  double JetWithDaughtersMass = -10;
  if(pickedCorrectJet==0){
    dPhi_LeadMuonJetWithDaughters = fabs(reco::deltaPhi(myRECOevent.myAddJetCandsHighPt[myRECOevent.JetContainingBothDaughters]->phi, myRECOevent.myMuonCand->phi()));
    selectedIncorrectJetMass = myRECOevent.myMuonJetPairs[0].first->SDmass;
    JetWithDaughtersMass = myRECOevent.myAddJetCandsHighPt[myRECOevent.JetContainingBothDaughters]->SDmass;
  }

  myRECOevent.selectedIncorrectJetMass = selectedIncorrectJetMass;
  myRECOevent.JetWithDaughtersMass = JetWithDaughtersMass;
  myRECOevent.dPhi_LeadMuonJetWithDaughters = dPhi_LeadMuonJetWithDaughters;
  myRECOevent.pickedCorrectJet = pickedCorrectJet;
  myRECOevent.myEventMass = myRECOevent.leadAK8JetMuonMassVal;
  myRECOevent.MaxDR_genDaughter_CandJet = MaxDR_genDaughter_CandJet;

/*  if(myRECOevent.genSecondMuon != 0){
    double muPhi  = myRECOevent.genSecondMuon->phi();
    double muEta  = myRECOevent.genSecondMuon->eta();
    double jetPhi = myRECOevent.selectedJetPhi;
    double jetEta = myRECOevent.selectedJetEta;
    double dPhi = abs( jetPhi - muPhi ); if ( dPhi > ROOT::Math::Pi() ) dPhi -= 2 * ROOT::Math::Pi();
    double dR = sqrt( ( jetEta - muEta ) * ( jetEta - muEta ) + dPhi * dPhi );
    if ( dR > ROOT::Math::Pi() ) dR -= 2 * ROOT::Math::Pi();

    myRECOevent.secondGENMuonRECOjetDR = dR;
  }*/

  return true;
}
void cmsWRextension::passExtensionRECO_Fast(const edm::Event& iEvent, eventBits& myRECOevent) {
  std::sort(myRECOevent.myMuonJetPairs.begin(),myRECOevent.myMuonJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);
  TLorentzVector *JetVector_temp = new TLorentzVector();
  JetVector_temp->SetPtEtaPhiM(myRECOevent.myMuonJetPairs[0].first->pT,myRECOevent.myMuonJetPairs[0].first->eta,myRECOevent.myMuonJetPairs[0].first->phi,myRECOevent.myMuonJetPairs[0].first->SDmass);

  math::XYZTLorentzVector JetVector;
  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

  myRECOevent.leadAK8JetMuonMassVal = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).mass();
  myRECOevent.leadAK8JetMuonPtVal   = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal  = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs[0].first->phi, myRECOevent.myMuonJetPairs[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs[0].second->eta();
  myRECOevent.selectedJetPt   = myRECOevent.myMuonJetPairs[0].first->pT;
  myRECOevent.selectedJetPhi  = myRECOevent.myMuonJetPairs[0].first->phi;
  myRECOevent.selectedJetEta  = myRECOevent.myMuonJetPairs[0].first->eta;
  myRECOevent.selectedJetMass = myRECOevent.myMuonJetPairs[0].first->SDmass;

  if(myRECOevent.myMuonJetPairs[0].first->tau1==0){
    myRECOevent.selectedJetTau21 = -9999.;
  }else{
    myRECOevent.selectedJetTau21 = (myRECOevent.myMuonJetPairs[0].first->tau2)/(myRECOevent.myMuonJetPairs[0].first->tau1);
  }

  std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;

  myRECOevent.myEventMass = myRECOevent.leadAK8JetMuonMassVal;

  JetVector_temp->SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JECUp[0].first->pT,myRECOevent.myMuonJetPairs_JECUp[0].first->eta,myRECOevent.myMuonJetPairs_JECUp[0].first->phi,myRECOevent.myMuonJetPairs_JECUp[0].first->SDmass);

  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

  myRECOevent.leadAK8JetMuonMassVal_JECUp = (JetVector + myRECOevent.myMuonJetPairs_JECUp[0].second->p4()).mass();
  myRECOevent.leadAK8JetMuonPtVal_JECUp   = (JetVector + myRECOevent.myMuonJetPairs_JECUp[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal_JECUp  = (JetVector + myRECOevent.myMuonJetPairs_JECUp[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal_JECUp  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JECUp[0].first->phi, myRECOevent.myMuonJetPairs_JECUp[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JECUp[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JECUp[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JECUp[0].second->eta();
  myRECOevent.selectedJetPt_JECUp   = myRECOevent.myMuonJetPairs_JECUp[0].first->pT;
  myRECOevent.selectedJetPhi_JECUp  = myRECOevent.myMuonJetPairs_JECUp[0].first->phi;
  myRECOevent.selectedJetEta_JECUp  = myRECOevent.myMuonJetPairs_JECUp[0].first->eta;
  myRECOevent.selectedJetMass_JECUp = myRECOevent.myMuonJetPairs_JECUp[0].first->SDmass;

  if(myRECOevent.myMuonJetPairs_JECUp[0].first->tau1==0){
    myRECOevent.selectedJetTau21_JECUp = -9999.;
  }else{
    myRECOevent.selectedJetTau21_JECUp = (myRECOevent.myMuonJetPairs_JECUp[0].first->tau2)/(myRECOevent.myMuonJetPairs_JECUp[0].first->tau1);
  }


  myRECOevent.myEventMass_JECUp = myRECOevent.leadAK8JetMuonMassVal_JECUp;

  JetVector_temp->SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JECDown[0].first->pT,myRECOevent.myMuonJetPairs_JECDown[0].first->eta,myRECOevent.myMuonJetPairs_JECDown[0].first->phi,myRECOevent.myMuonJetPairs_JECDown[0].first->SDmass);

  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

  myRECOevent.leadAK8JetMuonMassVal_JECDown = (JetVector + myRECOevent.myMuonJetPairs_JECDown[0].second->p4()).mass();
  myRECOevent.leadAK8JetMuonPtVal_JECDown   = (JetVector + myRECOevent.myMuonJetPairs_JECDown[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal_JECDown  = (JetVector + myRECOevent.myMuonJetPairs_JECDown[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal_JECDown  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JECDown[0].first->phi, myRECOevent.myMuonJetPairs_JECDown[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JECDown[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JECDown[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JECDown[0].second->eta();
  myRECOevent.selectedJetPt_JECDown   = myRECOevent.myMuonJetPairs_JECDown[0].first->pT;
  myRECOevent.selectedJetPhi_JECDown  = myRECOevent.myMuonJetPairs_JECDown[0].first->phi;
  myRECOevent.selectedJetEta_JECDown  = myRECOevent.myMuonJetPairs_JECDown[0].first->eta;
  myRECOevent.selectedJetMass_JECDown = myRECOevent.myMuonJetPairs_JECDown[0].first->SDmass;

  if(myRECOevent.myMuonJetPairs_JECDown[0].first->tau1==0){
    myRECOevent.selectedJetTau21_JECDown = -9999.;
  }else{
    myRECOevent.selectedJetTau21_JECDown = (myRECOevent.myMuonJetPairs_JECDown[0].first->tau2)/(myRECOevent.myMuonJetPairs_JECDown[0].first->tau1);
  }

  myRECOevent.myEventMass_JECDown = myRECOevent.leadAK8JetMuonMassVal_JECDown;

  JetVector_temp->SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JERUp[0].first->pT,myRECOevent.myMuonJetPairs_JERUp[0].first->eta,myRECOevent.myMuonJetPairs_JERUp[0].first->phi,myRECOevent.myMuonJetPairs_JERUp[0].first->SDmass);

  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

  myRECOevent.leadAK8JetMuonMassVal_JERUp = (JetVector + myRECOevent.myMuonJetPairs_JERUp[0].second->p4()).mass();
  myRECOevent.leadAK8JetMuonPtVal_JERUp   = (JetVector + myRECOevent.myMuonJetPairs_JERUp[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal_JERUp  = (JetVector + myRECOevent.myMuonJetPairs_JERUp[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal_JERUp  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JERUp[0].first->phi, myRECOevent.myMuonJetPairs_JERUp[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JERUp[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JERUp[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JERUp[0].second->eta();
  myRECOevent.selectedJetPt_JERUp   = myRECOevent.myMuonJetPairs_JERUp[0].first->pT;
  myRECOevent.selectedJetPhi_JERUp  = myRECOevent.myMuonJetPairs_JERUp[0].first->phi;
  myRECOevent.selectedJetEta_JERUp  = myRECOevent.myMuonJetPairs_JERUp[0].first->eta;
  myRECOevent.selectedJetMass_JERUp = myRECOevent.myMuonJetPairs_JERUp[0].first->SDmass;

  if(myRECOevent.myMuonJetPairs_JERUp[0].first->tau1==0){
    myRECOevent.selectedJetTau21_JERUp = -9999.;
  }else{
    myRECOevent.selectedJetTau21_JERUp = (myRECOevent.myMuonJetPairs_JERUp[0].first->tau2)/(myRECOevent.myMuonJetPairs_JERUp[0].first->tau1);
  }

  myRECOevent.myEventMass_JERUp = myRECOevent.leadAK8JetMuonMassVal_JERUp;

  JetVector_temp->SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JERDown[0].first->pT,myRECOevent.myMuonJetPairs_JERDown[0].first->eta,myRECOevent.myMuonJetPairs_JERDown[0].first->phi,myRECOevent.myMuonJetPairs_JERDown[0].first->SDmass);
  JetVector.SetXYZT(JetVector_temp->X(),JetVector_temp->Y(),JetVector_temp->Z(),JetVector_temp->T());

  myRECOevent.leadAK8JetMuonMassVal_JERDown = (JetVector + myRECOevent.myMuonJetPairs_JERDown[0].second->p4()).mass();
  myRECOevent.leadAK8JetMuonPtVal_JERDown   = (JetVector + myRECOevent.myMuonJetPairs_JERDown[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal_JERDown  = (JetVector + myRECOevent.myMuonJetPairs_JERDown[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal_JERDown  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JERDown[0].first->phi, myRECOevent.myMuonJetPairs_JERDown[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JERDown[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JERDown[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JERDown[0].second->eta();
  myRECOevent.selectedJetPt_JERDown   = myRECOevent.myMuonJetPairs_JERDown[0].first->pT;
  myRECOevent.selectedJetPhi_JERDown  = myRECOevent.myMuonJetPairs_JERDown[0].first->phi;
  myRECOevent.selectedJetEta_JERDown  = myRECOevent.myMuonJetPairs_JERDown[0].first->eta;
  myRECOevent.selectedJetMass_JERDown = myRECOevent.myMuonJetPairs_JERDown[0].first->SDmass;

  if(myRECOevent.myMuonJetPairs_JERDown[0].first->tau1==0){
    myRECOevent.selectedJetTau21_JERDown = -9999.;
  }else{
    myRECOevent.selectedJetTau21_JERDown = (myRECOevent.myMuonJetPairs_JERDown[0].first->tau2)/(myRECOevent.myMuonJetPairs_JERDown[0].first->tau1);
  }

  myRECOevent.myEventMass_JERDown = myRECOevent.leadAK8JetMuonMassVal_JERDown;

}
bool cmsWRextension::metCuts(const edm::Event& iEvent, eventBits& myEvent) {
  //if( (myEvent.subleadMuon_selMuondPhi >= 0) && myEvent.subleadMuon_selMuondPhi < 2) return false;
  if( (myEvent.MET_selMuondPhi >= 0 ) && (myEvent.MET_selMuondPhi < 2) ) return false;
  return true;

}
//bool cmsWRextension::massCut(const edm::Event& iEvent, eventBits& myEvent) {
//  if (myEvent.myEventMass > m_MCL && myEvent.myEventMass < m_MCU) return true;
//  return false;
//}
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
void cmsWRextension::loadCMSSWPath() {
    char* cmsswPathChar = getenv("CMSSW_BASE");
    if (cmsswPathChar == NULL) {
        std::cout << "Warning in JetLoader::loadCMSSWPath : CMSSW_BASE not detected." << std::endl;
        cmsswPath = "";
    }
    cmsswPath = std::string(cmsswPathChar);
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
  if (m_doGen && m_doReco && !m_doFast) {
    std::cout << "BOOKING PLOTS FLAVOR 3" << std::endl;
  //flavor 3

    m_allEvents.book((fs->mkdir("allEvents")), 3, m_outputTag, 0);
    m_eventsPassingWR2016.book((fs->mkdir("eventsPassingWR2016")), 3, m_outputTag, 0);
    m_eventsPassingWR2016RECO.book((fs->mkdir("eventsPassingWR2016RECO")), 3, m_outputTag, 0);
    m_eventsPassingExtension.book((fs->mkdir("eventsPassingExtension")), 3, m_outputTag, 0);
    m_eventsPassingExtensionRECO.book((fs->mkdir("eventsPassingExtensionRECO")), 3, m_outputTag, 0);
    m_eventsPassingExtensionRECO2016VETO.book((fs->mkdir("eventsPassingExtensionRECO2016VETO")), 3, m_outputTag, 0);
    m_eventsPassingExtensionRECO2016VETO_noTrig.book((fs->mkdir("eventsPassingExtensionRECO2016VETO_noTrig")), 3, m_outputTag, 0);
    //m_eventsPassingExtensionRECO2016VETOMASSMETCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSMETCUT"), 3, m_outputTag, false);
    //m_eventsPassingExtensionRECO2016VETOMASSCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSCUT"), 3, m_outputTag, false);
    m_eventsPassingExtensionRECO2016VETOZMASS.book((fs->mkdir("eventsPassingExtensionRECO2016VETOZMASS")), 3, m_outputTag, 0);
    m_eventsPassingExtensionRECO2016VETOSINGLEMUON.book((fs->mkdir("eventsPassingExtensionRECO2016VETOSINGLEMUON")), 3, m_outputTag, 0);
    m_eventsPassingFlavorSidebandRECOelePt50.book((fs->mkdir( "eventsPassingFlavorSidebandRECOelePt50")),  3, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt100.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt100")), 3, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt150.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt150")), 3, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECO_noTrig.book((fs->mkdir( "eventsPassingFlavorSidebandRECO_noTrig")),  3, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt50_noISO.book((fs->mkdir( "eventsPassingFlavorSidebandRECOelePt50_noISO")),  3, m_outputTag, 2);
    //ABCD
    m_eventsPassingFlavorSidebandRECOelePt200_noISO.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO")), 3, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200_noISO_samesign.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO_samesign")), 3, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200_noISO_noTrig.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO_noTrig")), 3, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200_noISO_samesign_noTrig.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO_samesign_noTrig")), 3, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200")), 3, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt200_samesign.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_samesign")), 3, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt200_all.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_all")), 3, m_outputTag, 1);
  }
  if (m_doGen && !m_doReco && !m_doFast) {
    std::cout << "BOOKING PLOTS FLAVOR 1" << std::endl;
  //flavor 1
    m_allEvents.book((fs->mkdir("allEvents")), 1, m_outputTag, 0);
    m_eventsPassingWR2016.book((fs->mkdir("eventsPassingWR2016")), 1, m_outputTag, 0);
    m_eventsPassingExtension.book((fs->mkdir("eventsPassingExtension")), 1, m_outputTag, 0);

  }
  if (!m_doGen && m_doReco && !m_doFast) {
    std::cout << "BOOKING PLOTS FLAVOR 2" << std::endl;
  //flavor 2
    m_allEvents.book((fs->mkdir("allEvents")), 2, m_outputTag, 0);
    m_eventsPassingWR2016RECO.book((fs->mkdir("eventsPassingWR2016RECO")), 2, m_outputTag, 0);
    m_eventsPassingExtensionRECO.book((fs->mkdir("eventsPassingExtensionRECO")), 2, m_outputTag, 0);
    m_eventsPassingExtensionRECO2016VETO.book((fs->mkdir("eventsPassingExtensionRECO2016VETO")), 2, m_outputTag, 0);
    m_eventsPassingExtensionRECO2016VETO_noTrig.book((fs->mkdir("eventsPassingExtensionRECO2016VETO_noTrig")), 2, m_outputTag, 0);
    //m_eventsPassingExtensionRECO2016VETOMASSMETCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSMETCUT"), 2, m_outputTag, false);
    //m_eventsPassingExtensionRECO2016VETOMASSCUT.book(fs->mkdir("eventsPassingExtensionRECO2016VETOMASSCUT"), 2, m_outputTag, false);
    m_eventsPassingExtensionRECO2016VETOZMASS.book((fs->mkdir("eventsPassingExtensionRECO2016VETOZMASS")), 2, m_outputTag, 0);
    m_eventsPassingExtensionRECO2016VETOSINGLEMUON.book((fs->mkdir("eventsPassingExtensionRECO2016VETOSINGLEMUON")), 2, m_outputTag, 0);
    m_eventsPassingFlavorSidebandRECOelePt50.book((fs->mkdir( "eventsPassingFlavorSidebandRECOelePt50")),  2, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt100.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt100")), 2, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt150.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt150")), 2, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECO_noTrig.book((fs->mkdir( "eventsPassingFlavorSidebandRECO_noTrig")),  2, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt50_noISO.book((fs->mkdir( "eventsPassingFlavorSidebandRECOelePt50_noISO")),  2, m_outputTag, 2);
    //ABCD
    m_eventsPassingFlavorSidebandRECOelePt200_noISO.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO")), 2, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200_noISO_samesign.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO_samesign")), 2, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200_noISO_noTrig.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO_noTrig")), 2, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200_noISO_samesign_noTrig.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_noISO_samesign_noTrig")), 2, m_outputTag, 2);
    m_eventsPassingFlavorSidebandRECOelePt200.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200")), 2, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt200_samesign.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_samesign")), 2, m_outputTag, 1);
    m_eventsPassingFlavorSidebandRECOelePt200_all.book((fs->mkdir("eventsPassingFlavorSidebandRECOelePt200_all")), 2, m_outputTag, 1);
  }
  if (m_doReco && m_doFast) {
    std::cout << "BOOKING PLOTS FLAVOR 5" << std::endl;
    m_allEvents.book((fs->mkdir("allEvents")), 5, m_outputTag, false);
    m_eventsPassingExtensionRECO.book((fs->mkdir("eventsPassingExtensionRECO")), 5, m_outputTag, false);
    m_eventsPassingExtensionRECO2016VETOZMASS.book((fs->mkdir("eventsPassingExtensionRECO2016VETOZMASS")), 5, m_outputTag, false);
    m_eventsPassingExtensionRECO2016VETOSINGLEMUON.book((fs->mkdir("eventsPassingExtensionRECO2016VETOSINGLEMUON")), 5, m_outputTag, false);
    m_eventsPassingFlavorSidebandRECOelePt50.book((fs->mkdir( "eventsPassingFlavorSidebandRECOelePt50")),  5, m_outputTag, true);
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
