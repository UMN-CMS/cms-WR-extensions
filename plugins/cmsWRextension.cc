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
#include "ExoAnalysis/cmsWRextensions/interface/Zweight.h"
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
  m_electronToken2 (consumes<std::vector<pat::Electron> > (iConfig.getParameter<edm::InputTag>("electrons2"))),
  m_eleIdFullInfoMapToken (consumes<edm::ValueMap<vid::CutFlowResult>> (iConfig.getParameter<edm::InputTag>("eleFullInfoMap"))),
  m_regMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("regMuons"))),
  m_recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("recoJets"))),
  m_AK4recoCHSJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK4recoCHSJets"))),
  m_AK8recoPUPPIJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoPUPPIJets"))),
  m_AK8recoPUPPISubJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("subJetName"))),
  m_offlineVerticesToken (consumes<std::vector<reco::Vertex>> (iConfig.getParameter<edm::InputTag>("vertices"))),
  m_metToken (consumes<std::vector<pat::MET>> (iConfig.getParameter<edm::InputTag>("met"))),
  m_PUInfoToken (consumes< std::vector<PileupSummaryInfo>> (iConfig.getParameter<edm::InputTag>("edmPileupInfo"))),
  m_rhoLabel    (consumes<double> (iConfig.getParameter<edm::InputTag>("rhoLabel"))),
  LHEEventProductToken    ( consumes< LHEEventProduct > (iConfig.getUntrackedParameter<edm::InputTag>("LHEEventProduct"))),
  m_wantHardProcessMuons (iConfig.getUntrackedParameter<bool>("wantHardProcessMuons",true)),
  m_doGen (iConfig.getUntrackedParameter<bool>("doGen",false)),
  m_doReco (iConfig.getUntrackedParameter<bool>("doReco",true)),
  m_isSignal (iConfig.getUntrackedParameter<bool>("isSignal",false)),
  m_isMC (iConfig.getUntrackedParameter<bool>("isMC",true)),
  m_doTrig (iConfig.getUntrackedParameter<bool>("doTrig",false)),
  m_doFast (iConfig.getUntrackedParameter<bool>("doFast",false)),
  m_checkZ (iConfig.getUntrackedParameter<bool>("checkZ",false)),
  m_jettiness (iConfig.getUntrackedParameter<std::string>("jettinessPUPPI")),
  JECUName   (iConfig.getUntrackedParameter<std::string>("jecUncName","")),
  //m_MCL (iConfig.getUntrackedParameter<double>("MCL", 400)),
  //m_MCU (iConfig.getUntrackedParameter<double>("MCU", 8000)),
  m_flavorSideband (iConfig.getUntrackedParameter<bool>("flavorSideband", false)),  //SOON TO BE DEPRECATED
  m_outputTag (iConfig.getUntrackedParameter<string>("outputTag", "blah")),
  m_era (iConfig.getUntrackedParameter<string>("era", "2016"))

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

  std::cout << "Inside myEgammaEffi.Initialize" << std::endl;
  myEgammaEffi.Initialize(m_era);
  std::cout << "Outside myEgammaEffi.Initialize" << std::endl;

  myZweights.setup(m_era);
  m_foundZ           = false;

  loadCMSSWPath();
  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/cmsWRextensions/data/";
  std::string resPath;
  std::string resPathSF;
  std::string resPath_AK4;
  std::string resPathSF_AK4;

  if (m_era == "2016") {
    resPath = jecPathname + "2016/Summer16_25nsV1_MC_PtResolution_AK8PFPuppi.txt";
    resPathSF = jecPathname + "2016/Summer16_25nsV1_MC_SF_AK8PFPuppi.txt";

    resPath_AK4 = jecPathname + "2016/Summer16_25nsV1_MC_PtResolution_AK4PFchs.txt";
    resPathSF_AK4 = jecPathname + "2016/Summer16_25nsV1_MC_SF_AK4PFchs.txt";
  } else if (m_era == "2017") {
    resPath = jecPathname + "2017/Fall17_V3_MC_PtResolution_AK8PFPuppi.txt";
    resPathSF = jecPathname +"2017/Fall17_V3_MC_SF_AK8PFPuppi.txt";
    resPath_AK4 = jecPathname + "2017/Fall17_V3_MC_PtResolution_AK4PFchs.txt";
    resPathSF_AK4 = jecPathname + "2017/Fall17_V3_MC_SF_AK4PFchs.txt";
  } else if (m_era == "2018") {
    resPath = jecPathname + "2018/Autumn18_V7_MC_PtResolution_AK8PFPuppi.txt";
    resPathSF = jecPathname + "2018/Autumn18_V7_MC_SF_AK4PFchs.txt";
    resPath_AK4 = jecPathname + "2018/Autumn18_V7_MC_PtResolution_AK4PFchs.txt";
    resPathSF_AK4 = jecPathname + "2018/Autumn18_V7_MC_SF_AK8PFPuppi.txt";
  }

  resolution = JME::JetResolution(Form(resPath.c_str()));
  resolution_sf = JME::JetResolutionScaleFactor(Form(resPathSF.c_str()));
  resolution_AK4 = JME::JetResolution(Form(resPath_AK4.c_str()));
  resolution_sf_AK4 = JME::JetResolutionScaleFactor(Form(resPathSF_AK4.c_str()));

  ScaleIDRange_ = iConfig.getUntrackedParameter< std::vector<int> >("ScaleIDRange");
  PDFErrorIDRange_ = iConfig.getUntrackedParameter< std::vector<int> >("PDFErrorIDRange");
  PDFAlphaSIDRange_ = iConfig.getUntrackedParameter< std::vector<int> >("PDFAlphaSIDRange");
  PDFAlphaSScaleValue_ = iConfig.getUntrackedParameter< std::vector<double> >("PDFAlphaSScaleValue");

  r = new TRandom3(1988);

  prefweight_token = consumes< double >(edm::InputTag("prefiringweight:nonPrefiringProb"));
  prefweightup_token = consumes< double >(edm::InputTag("prefiringweight:nonPrefiringProbUp"));
  prefweightdown_token = consumes< double >(edm::InputTag("prefiringweight:nonPrefiringProbDown"));

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

  std::cout << "iEvent.luminosityBlock(): " << iEvent.luminosityBlock() << std::endl;
  std::cout << "iEvent.id().event(): " << iEvent.id().event() << std::endl;

/*  if(iEvent.id().event()==41607495 || iEvent.id().event()== 61099812 || iEvent.id().event()== 13209699 || iEvent.id().event()== 62496653 || iEvent.id().event()== 4604955 || iEvent.id().event()== 28598822 || iEvent.id().event()==56894814 ){
    std::cout << "FOUND EVENT: " << iEvent.id().event() << std::endl;
    std::cout << "Has Lumi: " << iEvent.luminosityBlock() << std::endl;
//    if(iEvent.luminosityBlock() == 342735){
//      std::cout << "FOUND LUMI 342735!!" << std::endl;
//    }
  }else{
    return;
  }*/
  //Get vertex collection
  edm::Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(m_offlineVerticesToken, vertices);
  if(!vertices.isValid()) {
    throw cms::Exception("Vertex collection not valid!");
  }

  //we'll use this to carry our event info around
  eventBits myRECOevent;
  //tag to label events with
  myRECOevent.outputTag = m_outputTag;
  myRECOevent.m_flavor = flavor;
  //booleans to check for 4 different selections
  bool passesResRECO = false;
  bool passesResFSBRECO = false;
  bool passesResGEN = false;  //these two track with a recreation of the past resolved 2016 analysis
//  LEPTON VETOES
  bool tooManyResElectrons = false;        //MORE THAN TWO
  bool tooManyResMuons = false;            //MORE THAN TWO
  bool tooManyResFSBLeptons = false;       //MORE THAN TWO TOTAL

  bool tooManyBoostElectrons = false;
  bool tooManyBoostMuons = false;
  bool tooManyBoostMuonsInJet = false;
  bool tooManyBoostElectronsInJet = false;
  bool tooManyBoostFSBLeptons = false;


 //////
//  bool passesFastBoostRECO = false;

//////
  bool passesBoostRECO = false;
  bool passesBoostGEN = false; //this tracks with our current analysis effort
  bool passesBoostFSBRECO = false;
//  bool passesBoostModGEN = false; //this tracks with our current analysis effort
  //various pass/fail bits
  bool passGenCounter = false;
  bool passPreSelectGen = false;
  bool passZSBGEN       = false;

  //trigger pass info
  bool muonTrigPass = true;
  bool electronTrigPass = true;
  //this bool is true if the event passes the Mmumu Zmass region cut
  bool ZMASSboost = false;
  int ZMASSres = 999;
  bool ZMASSFSBres = false;
  //is quality signal event
  bool isGoodSignal = false;
  //these are for the JEC/JER toy regions
  int ZMASS_Nom = 0;
  int ZMASS_JECUp = 0;
  int ZMASS_JECDown = 0;
  int ZMASS_JERUp = 0;
  int ZMASS_JERDown = 0;
  //the ZMASS region is re-evaluated for the e-mu case
  bool ZMASS_FSB = false;
  bool ZMASS_FSB_JECUp = false;
  bool ZMASS_FSB_JECDown = false;
  bool ZMASS_FSB_JERUp = false;
  bool ZMASS_FSB_JERDown = false;
  //this bool is true if the event has additional muons (more than the lead muon)
  bool addMuons = false;
  //these are for the JEC/JER toy regions
  bool addMuons_JECUp = false;
  bool addMuons_JECDown = false;
  bool addMuons_JERUp = false;
  bool addMuons_JERDown = false;
  bool addElectrons = false;
  bool addElectrons_JECUp = false;
  bool addElectrons_JECDown = false;
  bool addElectrons_JERUp = false;
  bool addElectrons_JERDown = false;
  if (m_doTrig){
    if(m_isSignal){
      muonTrigPass = true;
      myRECOevent.muonTrigPassBit = muonTrigPass;
    }else{
      muonTrigPass = passMuonTrig(iEvent, myRECOevent);
      myRECOevent.muonTrigPassBit = muonTrigPass;
    }
  }

  if (m_isSignal){
    LHEinfo(iEvent, myRECOevent);
  }

  myRECOevent.eventNumber = iEvent.id().event();
  myRECOevent.lumiSection = iEvent.luminosityBlock();

  myRECOevent.hasPVertex = myEventInfo.PVselection(vertices);

  if(myRECOevent.hasPVertex){
  myRECOevent.nVtx = myEventInfo.nVtx;
  myRECOevent.PVertex = myEventInfo.PVertex;
  }
  myRECOevent.isMC = m_isMC;

  if(m_isMC && myRECOevent.hasPVertex){
    edm::Handle< std::vector<PileupSummaryInfo> > hPileupInfoProduct;
    iEvent.getByToken(m_PUInfoToken,hPileupInfoProduct);
    assert(hPileupInfoProduct.isValid());
    std::vector<float> puWeights;
    if (m_era == "2016") {
      puWeights = myEventInfo.PUweight(hPileupInfoProduct, "2016", m_isSignal);
    }else if(m_era == "2017") {
      puWeights = myEventInfo.PUweight(hPileupInfoProduct, "2017", m_isSignal);
    }else if(m_era == "2018") {
      puWeights = myEventInfo.PUweight(hPileupInfoProduct, "2018", m_isSignal);
    }
    myRECOevent.puWeight = puWeights[0];
    myRECOevent.puWeight_Up = puWeights[1];
    myRECOevent.puWeight_Down = puWeights[2];
  }

  //EE prefiring corrections
  if(m_era == "2016" || m_era == "2017"){
    edm::Handle< double > theprefweight;
    iEvent.getByToken(prefweight_token, theprefweight ) ;
    myRECOevent._prefiringweight =(*theprefweight);

    edm::Handle< double > theprefweightup;
    iEvent.getByToken(prefweightup_token, theprefweightup ) ;
    myRECOevent._prefiringweightup =(*theprefweightup);

    edm::Handle< double > theprefweightdown;
    iEvent.getByToken(prefweightdown_token, theprefweightdown ) ;
    myRECOevent._prefiringweightdown =(*theprefweightdown);
  }else{
    myRECOevent._prefiringweight = 1.;
    myRECOevent._prefiringweightup = 1.;
    myRECOevent._prefiringweightdown = 1.;
  }

  std::cout << "myRECOevent._prefiringweight: " << myRECOevent._prefiringweight << std::endl;

  // this goes through the logic descend the proper eventweight depending on source (data/mc type)
  myRECOevent.Muon_HighPtID_Weight = 1;
  myRECOevent.Muon_LooseID_Weight = 1;
  std::cout << "Initializing event weight" << std::endl;
  setEventWeight(iEvent, myRECOevent);

  //here we start tracking our progress through the main cuts 
  myRECOevent.cutProgress = 1;
  myRECOevent.ResCutProgress = 1;
  myRECOevent.ResFSBCutProgress = 1;

  if (!m_doFast) {
    if(m_isMC && m_doGen) {
      
      passGenCounter = genCounter(iEvent, myRECOevent);
      myRECOevent.passGenCounter = passGenCounter;
      vertexDiff(myRECOevent);
      if(m_isSignal){
        isGoodSignal = signalGENidentifier(iEvent, myRECOevent);
      }
      passPreSelectGen = preSelectGen(iEvent, myRECOevent); 
      std::cout << "IS SIGNAL? " << m_isSignal << std::endl;
      std::cout << "IS GOOD SIGNAL? " << isGoodSignal << std::endl;
      if(m_isSignal && !isGoodSignal) return; //IGNORE EVENT
      
      myRECOevent.passPreSelectGen = passPreSelectGen;
      genJetAnalyzer(iEvent, myRECOevent);
      std::cout << "analyzing GEN objects" << std::endl;
      if(m_isSignal){
        objectCompareGEN(iEvent, myRECOevent);
        passZSBGEN        = passZsidebandCutGEN (iEvent, myRECOevent); 
        passesResGEN      = passResGEN          (iEvent, myRECOevent);
        passesBoostGEN    = passBoostGEN        (iEvent, myRECOevent);
      }
    }
    if(m_checkZ && m_isMC) {
      std::cout << "About to run ZFinder" << std::endl;
      m_foundZ = ZFinder(iEvent,myRECOevent);
      if(m_foundZ) {
        std::cout << "PASSED Z GEN" << std::endl;
        std::cout << "ZPT:   " <<   myRECOevent.genZpt << std::endl;
        std::cout << "ZMASS: " << myRECOevent.genZmass  << std::endl;
      }
    }
    if ((m_doReco || !m_isMC) && myRECOevent.hasPVertex) {
      passesResRECO    = passResRECO(iEvent , myRECOevent);
      passesResFSBRECO = passFSBResRECO(iEvent, myRECOevent);
      if(myRECOevent.NresolvedANAElectronCands > 1)  tooManyResElectrons = true;
      if(myRECOevent.NresolvedANAMuonCands > 2)      tooManyResMuons = true;
      if(myRECOevent.NresolvedANAFSBLeptonCands > 2) tooManyResFSBLeptons = true;

      std::cout << "Outside subLeadingMuonZMass" << std::endl;
      ZMASSres         = subLeadingMuonZMass(iEvent, myRECOevent, false, true);
      ZMASSFSBres      = subLeadingMuonZMass_FlavorSideband(iEvent, myRECOevent, true);
      std::cout << "myRECOevent.myResCandJets.size(): " << myRECOevent.myResCandJets.size() << " myRECOevent.resolvedANAMuons.size(): " << myRECOevent.resolvedANAMuons.size() << std::endl;
      std::cout<<"running preselection reco"<<std::endl;
      if(preSelectBoostReco(iEvent, iSetup, myRECOevent)) {

        myRECOevent.cutProgress++;
        if(myRECOevent.myMuonJetPairs.size() > 0){
          if(passBoostRECO(iEvent, myRECOevent)) {
	    std::cout << "Filling m_eventsPassJetSelection" << std::endl;
	    m_eventsPassJetSelection.fill(  myRECOevent, 1, m_isSignal);
            addMuons = additionalMuons(iEvent, myRECOevent, false, false, 0, false);
	    std::cout << "Outside subLeadingMuonZMass Boost" << std::endl;
            ZMASSboost = subLeadingMuonZMass(iEvent, myRECOevent, false, false);
            if(m_isMC && addMuons) {
              std::vector<double> Muon_LooseID_Weights;
	      if(m_era == "2016") {
                Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
              }else if(m_era == "2017"){
                Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
              }else if(m_era == "2018"){
                Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
	      }
              myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
              myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
              myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
              std::vector<double> Muon_HighPtID_Weights;
	      std::vector<double> Muon_LooseTkIso_Weights;
	      if(m_era == "2016") {
                Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
		Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
              }else if(m_era == "2017"){
                Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
                Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
              }else if(m_era == "2018"){
                Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
                Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
	      }
              myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
              myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
              myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
	      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
              myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
              myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
	      std::vector<double> Muon_Trig_Weights;
	      if(m_era == "2016") { 
	        Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016", m_isSignal);
	      }else if(m_era == "2017"){
		Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017", m_isSignal);
	      }else if(m_era == "2018"){
		Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018", m_isSignal);
	      }
	      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
              myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
              myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
              setEventWeight(iEvent, myRECOevent);
            }
            if ((m_isMC || m_flavorSideband) && addMuons){
              myRECOevent.cutProgress++;
              if(!ZMASSboost){
		std::cout << "above the ZMASS" << std::endl;
                myRECOevent.cutProgress++;
                if(muonTrigPass && m_doTrig){
		  std::cout << "triggers are passed" << std::endl;
                  myRECOevent.cutProgress++;
          	  if(myRECOevent.nHighPtMuonsOutsideJet == 1){
		      std::cout << "one high pt muon outside jet" <<std::endl;
                      myRECOevent.cutProgress++;
                      passesBoostRECO = true;             
                      
                      std::cout << "myRECOevent.weight: " << myRECOevent.weight << std::endl;
          	  }
                }
              }
            }
          }
        }
        std::cout << "Inside preselection region" << std::endl;
	std::cout << "myRECOevent myMuonJetPairs_noLSF size(): " << myRECOevent.myMuonJetPairs_noLSF.size() << std::endl;
        if(myRECOevent.myMuonJetPairs_noLSF.size() > 0){
          std::cout << "Inside ZPeak preselection" << std::endl;
          if(passExtensionRECO_ZPeak(iEvent, myRECOevent)) {
            std::cout << "Inside ZPeak passExtensionRECO_ZPeak" << std::endl;
            addMuons = additionalMuons(iEvent, myRECOevent, false, true, 0, false);
	    std::cout << "myRECOevent.resolvedANAMuons.size(): " << myRECOevent.resolvedANAMuons.size() << std::endl;
            ZMASSboost = subLeadingMuonZMass(iEvent, myRECOevent, true, false);
//            std::cout << "addMuons: " << addMuons << ", ZMASS: " << ZMASS << std::endl;
            if(m_isMC && addMuons) {
              std::vector<double> Muon_LooseID_Weights;
              if(m_era == "2016") {
                Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
              }else if(m_era == "2017"){
                Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
              }else if(m_era == "2018"){
                Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
              }
              myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
              myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
              myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
              std::vector<double> Muon_HighPtID_Weights;
	      std::vector<double> Muon_LooseTkIso_Weights;
              if(m_era == "2016") {
                Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
                Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
              }else if(m_era == "2017"){
                Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
                Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
              }else if(m_era == "2018"){
                Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
                Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
              }
              myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
              myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
              myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
              myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
              myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
              myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
              std::vector<double> Muon_Trig_Weights;
              if(m_era == "2016") {
                Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016", m_isSignal);
              }else if(m_era == "2017"){
                Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017", m_isSignal);
              }else if(m_era == "2018"){
                Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018", m_isSignal);
              }
              myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
              myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
              myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
              setEventWeight(iEvent, myRECOevent);
            }
          }
        }
      }
      std::cout << "CHECKING THE FLAVOR SIDEBAND" << std::endl;
      myRECOevent.FSBcutProgress = 0;
      myRECOevent.FSBcutProgress++;

      if(passFlavorSideband(iEvent, myRECOevent)) {
        std::cout << "PASSES THE FLAVOR SIDEBAND" << std::endl;
        if (m_doTrig){
          if (passElectronTrig(iEvent, myRECOevent)){
            std::cout<< "EVENT PASSES ELECTRON TRIGGERS" << std::endl;
            if (passFSBbin(myRECOevent, true, 60)) {
              passesBoostFSBRECO = true;
            }
          }
        }
      }
      std::cout << "DONE WITH FSB" << std::endl;
    }
  
    //BOOLEANS STUFF////////////////////////////////////////////////
    bool inTheFailBox = false;
    bool inTheOppoBox = false;
    if (passesBoostFSBRECO) {
      if (::wrTools::InTheHEMfailBox(myRECOevent.selectedJetEta, myRECOevent.selectedJetPhi, 0.8) )       inTheFailBox = true;
      if (::wrTools::InTheHEMfailBox(-1*myRECOevent.selectedJetEta, -1*myRECOevent.selectedJetPhi, 0.8) ) inTheOppoBox = true;

    }
    bool resFailBox = false;
    bool resOppoBox = false;
    if (passesBoostFSBRECO) {
      if (::wrTools::InTheHEMfailBox(myRECOevent.myResFSBCandJets[0]->eta, myRECOevent.myResFSBCandJets[0]->phi, 0.4) )       resFailBox = true;
      if (::wrTools::InTheHEMfailBox(-1*myRECOevent.myResFSBCandJets[1]->eta, -1*myRECOevent.myResFSBCandJets[1]->phi, 0.4) ) resOppoBox = true;

    }

    std::cout << "passesResFSBRECO: " << passesResFSBRECO << " muonTrigPass: " << muonTrigPass << " ZMASSFSBres: " << ZMASSFSBres << std::endl;
    bool tooManyResLeptons = tooManyResElectrons || tooManyResMuons;
    bool passesZMASSres =         (passesResRECO    && muonTrigPass &&  ZMASSres == 1    && !tooManyResLeptons);
    passesResRECO    = (passesResRECO    && muonTrigPass && ZMASSres == 2    && !tooManyResLeptons);
    passesResFSBRECO = (passesResFSBRECO && muonTrigPass && !ZMASSFSBres && !tooManyResFSBLeptons);
    std::cout << "passesResRECO: " << passesResRECO << "muonTrigPass: " << muonTrigPass << "ZMASSres: " << ZMASSres << std::endl;
    std::cout << "ZMASSres: " << ZMASSres << std::endl;
    std::cout << "passesResFSBRECO: " << passesResFSBRECO << std::endl;
    //BOOST MUON SELECTIONS
    if( myRECOevent.electronCands200     > 0)  tooManyBoostElectrons      = true;
    if( myRECOevent.muonCands            > 1)  tooManyBoostMuons          = true;
//    if( myRECOevent.muons10              > 2)  tooManyBoostMuonsInJet     = true;
    if( myRECOevent.nSecondElectronCands > 0)  tooManyBoostElectronsInJet = true;

//    bool tooManyBoostLeptons = tooManyBoostElectrons || tooManyBoostMuons || tooManyBoostMuonsInJet || tooManyBoostElectronsInJet;
    bool tooManyBoostLeptons = tooManyBoostElectrons || tooManyBoostMuons || tooManyBoostElectronsInJet;

    //BOOST FSB
    if(myRECOevent.electronCands200     > 1 ||
       myRECOevent.nMuonOutSideJetFSB   > 0 ||
//       myRECOevent.muons10              > 1 ||
       myRECOevent.nSecondElectronCands > 0
    ) {
      tooManyBoostFSBLeptons = true;
    }
    std::cout << "myRECOevent.electronCands200: " << myRECOevent.electronCands200 << " myRECOevent.nSecondElectronCands: " << myRECOevent.nSecondElectronCands << std::endl;
    ///////////////////////////////////////////////////////
    if(passesZMASSres){
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_HighPtID2nd_Weights;
      std::vector<double> Muon_LooseTkIso2nd_Weights;
      std::vector<double> Muon_Trig_Weights;
      if(m_era == "2016") {
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2016");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2016");
        Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2016");
        Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2016");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2016", m_isSignal, "Resolved");
      }else if(m_era == "2017"){
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2017");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2017");
        Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2017");
        Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2017");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2017", m_isSignal, "Resolved");
      }else if(m_era == "2018"){
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2018");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2018");
        Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2018");
        Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2018");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2018", m_isSignal, "Resolved");
      }
      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.Muon_HighPtID2nd_Weight = Muon_HighPtID2nd_Weights[0];
      myRECOevent.Muon_HighPtID2nd_WeightUp = Muon_HighPtID2nd_Weights[1];
      myRECOevent.Muon_HighPtID2nd_WeightDown = Muon_HighPtID2nd_Weights[2];
      myRECOevent.Muon_LooseTkIso2nd_Weight = Muon_LooseTkIso2nd_Weights[0];
      myRECOevent.Muon_LooseTkIso2nd_WeightUp = Muon_LooseTkIso2nd_Weights[1];
      myRECOevent.Muon_LooseTkIso2nd_WeightDown = Muon_LooseTkIso2nd_Weights[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
      setEventWeight_Resolved(iEvent, myRECOevent);

    }
    if(passesResFSBRECO){
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_Trig_Weights;
      std::vector<double> HEEP_SF;
      std::vector<double> egamma_SF;
      if(m_era == "2016") {
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2016");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2016");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2016", m_isSignal);
        egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.resFSBElec->superCluster()->eta(), myRECOevent.resFSBElec_pt, "2016");
      }else if(m_era == "2017"){
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2017");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2017");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2017", m_isSignal);
        egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.resFSBElec->superCluster()->eta(), myRECOevent.resFSBElec_pt, "2017");
      }else if(m_era == "2018"){
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2018");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2018");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, "2018", m_isSignal);
        egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.resFSBElec->superCluster()->eta(), myRECOevent.resFSBElec_pt, "2018");
      }
      HEEP_SF = myHEEP.ScaleFactor(myRECOevent.resFSBElec->et(), myRECOevent.resFSBElec_eta, m_era);
      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.HEEP_SF = HEEP_SF[0];
      myRECOevent.HEEP_SF_Up = HEEP_SF[1];
      myRECOevent.HEEP_SF_Down = HEEP_SF[2];
      myRECOevent.egamma_SF = egamma_SF[0];
      myRECOevent.egamma_SF_Up = egamma_SF[1];
      myRECOevent.egamma_SF_Down = egamma_SF[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
      setEventWeight_ResolvedFSB(iEvent, myRECOevent);
    }
    if(passesResRECO){
      std::cout << "PASSING RES EVENT" << std::endl;
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_HighPtID2nd_Weights;
      std::vector<double> Muon_LooseTkIso2nd_Weights;
      std::vector<double> Muon_Trig_Weights;
      if(m_era == "2016") {
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2016");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2016");
        Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2016");
        Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2016");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2016", m_isSignal, "Resolved");
      }else if(m_era == "2017"){
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2017");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2017");
        Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2017");
        Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2017");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2017", m_isSignal, "Resolved");
      }else if(m_era == "2018"){
        Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2018");
        Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), "2018");
        Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2018");
        Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2018");
        Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), "2018", m_isSignal, "Resolved");
      }  
      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.Muon_HighPtID2nd_Weight = Muon_HighPtID2nd_Weights[0];
      myRECOevent.Muon_HighPtID2nd_WeightUp = Muon_HighPtID2nd_Weights[1];
      myRECOevent.Muon_HighPtID2nd_WeightDown = Muon_HighPtID2nd_Weights[2];
      myRECOevent.Muon_LooseTkIso2nd_Weight = Muon_LooseTkIso2nd_Weights[0];
      myRECOevent.Muon_LooseTkIso2nd_WeightUp = Muon_LooseTkIso2nd_Weights[1];
      myRECOevent.Muon_LooseTkIso2nd_WeightDown = Muon_LooseTkIso2nd_Weights[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];

      setEventWeight_Resolved(iEvent, myRECOevent);
    }else if(passesBoostRECO){
      std::cout << "PASSING BOOST EVENT" << std::endl;
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_LooseID_Weights;
      std::vector<double> Muon_Trig_Weights;
    if(m_era == "2016") {
      Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
      Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
      Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
      Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016", m_isSignal, "Boosted");
    }else if(m_era == "2017"){
      Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
      Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
      Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
      Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017", m_isSignal, "Boosted");
    }else if(m_era == "2018"){
      Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
      Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
      Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
      Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018", m_isSignal, "Boosted");
    }
      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
      myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
      myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
      setEventWeight(iEvent, myRECOevent);
    }

    if(passesResRECO)myRECOevent.RECOcategory = 1;
    else if(passesBoostRECO) myRECOevent.RECOcategory = 2;
//////////////////////////////////////////////////////////////////FILLING FOR NOT FAST ANALYSIS/////////////////////////////////////////////////////////
//FILLS BEFORE HERE MUST BE MOVED/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "passesResRECO: " << passesResRECO << " passesBoostRECO: " << passesBoostRECO << std::endl;
//SILLY REGION
    if ( passesResRECO &&  passesBoostRECO && myRECOevent.resolvedRECOmassAbove600 && myRECOevent.boostedRECOmassAbove600)    m_eventsPassResPassBoostRECO.fill(myRECOevent, 1, m_isSignal);
//ALL FAIL
    if (!passesResRECO && !passesBoostRECO)                                                                                   m_eventsFailResFailBoostRECO.fill(myRECOevent, 1, m_isSignal);
//PASSES RES
    if ( passesResRECO && !passesBoostRECO && myRECOevent.resolvedRECOmassAbove600)                                           m_eventsPassResFailBoostRECO.fill(myRECOevent, 1, m_isSignal);
//PASSES BOOST
    if (!passesResRECO &&  passesBoostRECO && myRECOevent.boostedRECOmassAbove600 && !tooManyBoostLeptons){
       std::cout << "m_eventsFailResPassBoostRECO" << std::endl;
       m_eventsFailResPassBoostRECO.fill(myRECOevent, 1, m_isSignal);
    }
//BOOST SIDEBANDS
    bool tooManyBoostTightLeptons = tooManyBoostElectrons || tooManyBoostMuons;
    std::cout << "passesBoostFSBRECO: " << passesBoostFSBRECO << " tooManyBoostFSBLeptons: " << tooManyBoostFSBLeptons << std::endl;
    if (!passesResRECO &&  passesBoostRECO && !myRECOevent.boostedRECOmassAbove600 && !tooManyBoostTightLeptons){
       std::cout << "m_eventsPassBoostLowMassCRRECO" << std::endl;
       m_eventsPassBoostLowMassCRRECO.fill(myRECOevent, 1, m_isSignal);
    }
    if (!passesResRECO &&  passesBoostRECO && ZMASSboost && !tooManyBoostTightLeptons){
       std::cout << "m_eventsPassBoostZMASSRECO" << std::endl;
       m_eventsPassBoostZMASSRECO.fill(myRECOevent, 1, m_isSignal);
    }
    if (passesBoostFSBRECO &&  myRECOevent.boostedFSBRECOmassAbove600 && !tooManyBoostFSBLeptons){
       std::cout << "m_eventsPassBoostFSBRECO" << std::endl;
       m_eventsPassBoostFSBRECO.fill(myRECOevent, 1, m_isSignal);
    }
    if (passesBoostFSBRECO && !myRECOevent.boostedFSBRECOmassAbove600 && !tooManyBoostFSBLeptons){
       std::cout << "m_eventsPassBoostFSBLowMassCRRECO" << std::endl;
       m_eventsPassBoostFSBLowMassCRRECO.fill(myRECOevent, 1, m_isSignal);
    }
    if (passesBoostFSBRECO && !myRECOevent.boostedFSBRECOmassAbove600 && inTheFailBox) {
       std::cout << "m_eventsPassBoostFSBLowMassCRRECO_FailBox" << std::endl;
       m_eventsPassBoostFSBLowMassCRRECO_failBox.fill(myRECOevent, 1, m_isSignal);
    }
    if (passesBoostFSBRECO && !myRECOevent.boostedFSBRECOmassAbove600 && inTheOppoBox) {
       std::cout << "m_eventsPassBoostFSBLowMassCRRECO_OppoBox" << std::endl;
       m_eventsPassBoostFSBLowMassCRRECO_oppoBox.fill(myRECOevent, 1, m_isSignal);
    }
  //RES SIDEBANDS                              
    std::cout << "passesResFSBRECO: " << passesResFSBRECO << " myRECOevent.resolvedFSBRECOmassAbove600: " << myRECOevent.resolvedFSBRECOmassAbove600 << " tooManyBoostFSBLeptons: " << tooManyBoostFSBLeptons << std::endl;
    if ( passesResRECO && !passesBoostRECO && !myRECOevent.resolvedRECOmassAbove600)                                       m_eventsPassResLowMassCRRECO.fill(myRECOevent, 1, m_isSignal);
    if ( passesResFSBRECO &&  myRECOevent.resolvedFSBRECOmassAbove600)                                                     m_eventsPassResFSBRECO.fill(  myRECOevent, 1, m_isSignal);
    if ( passesResFSBRECO && !myRECOevent.resolvedFSBRECOmassAbove600)                                                     m_eventsPassResFSBLowMassCRRECO.fill(  myRECOevent, 1, m_isSignal);
    if ( passesResFSBRECO && !myRECOevent.resolvedFSBRECOmassAbove600 && resFailBox)                                       m_eventsPassResFSBLowMassCRRECO_failBox.fill(  myRECOevent, 1, m_isSignal);
    if ( passesResFSBRECO && !myRECOevent.resolvedFSBRECOmassAbove600 && resOppoBox)                                       m_eventsPassResFSBLowMassCRRECO_oppoBox.fill(  myRECOevent, 1, m_isSignal);
    if ( passesResRECO && !passesBoostRECO && passesZMASSres)                                                              m_eventsPassResZMASSRECO.fill(myRECOevent, 1, m_isSignal);
    std::cout << "Done filling RECO stuff" << std::endl;

/////////////////////////GEN STUFF////////////////////////////////////////////////////////////////////////////////////////////
    if (passPreSelectGen && passZSBGEN) {
      //nominal
      if (!passesResGEN && !passesBoostGEN)    m_eventsFailResFailBoostGEN.fill(myRECOevent, 1, m_isSignal);
      if ( passesResGEN &&  passesBoostGEN)    m_eventsPassResPassBoostGEN.fill(myRECOevent, 1, m_isSignal);
      if ( passesResGEN && !passesBoostGEN)    m_eventsPassResFailBoostGEN.fill(myRECOevent, 1, m_isSignal);
      if (!passesResGEN &&  passesBoostGEN)    m_eventsFailResPassBoostGEN.fill(myRECOevent, 1, m_isSignal);
      //res unchanged
    }
  }
  //THIS PART OF THE CODE RUNS THE ANALYSIS IN FAST MODE.  THE GOAL HERE IS TO PRODUCE ALL THE NECESSARY INFORMATION FOR HIGGS COMBINE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (m_doFast && myRECOevent.hasPVertex){
    std::cout << "Inside doFast running" << std::endl;
    if (m_isMC && m_doGen) {

      passGenCounter = genCounter(iEvent, myRECOevent);
      myRECOevent.passGenCounter = passGenCounter;
      vertexDiff(myRECOevent);
      if(m_isSignal){
        isGoodSignal = signalGENidentifier(iEvent, myRECOevent);
      }
      passPreSelectGen = preSelectGen(iEvent, myRECOevent);
      std::cout << "IS SIGNAL? " << m_isSignal << std::endl;
      std::cout << "IS GOOD SIGNAL? " << isGoodSignal << std::endl;
      if(m_isSignal && !isGoodSignal) return; //IGNORE EVENT
    }
    if(m_checkZ && m_isMC) {
      std::cout << "About to run ZFinder" << std::endl;
      m_foundZ = ZFinder(iEvent,myRECOevent);
      if(m_foundZ) {
        std::cout << "PASSED Z GEN" << std::endl;
        std::cout << "ZPT:   " <<   myRECOevent.genZpt << std::endl;
        std::cout << "ZMASS: " << myRECOevent.genZmass  << std::endl;
      }
    }
    std::vector<bool> passesResRECOAllRegions = passResRECO_Fast(iEvent , myRECOevent);
    std::vector<bool> passesResFSBRECOAllRegions = passFSBResRECO_Fast(iEvent, myRECOevent);
    ZMASSres         = subLeadingMuonZMass(iEvent, myRECOevent, false, true);
    ZMASSFSBres      = subLeadingMuonZMass_FlavorSideband(iEvent, myRECOevent, true);

    if(myRECOevent.NresolvedANAElectronCands > 0)  tooManyResElectrons = true;
    if(myRECOevent.NresolvedANAMuonCands > 2)      tooManyResMuons = true;
    if(myRECOevent.NresolvedANAFSBLeptonCands > 2) tooManyResFSBLeptons = true;

    bool tooManyResLeptons = tooManyResElectrons || tooManyResMuons;

    std::cout << "passesResRECOAllRegions[0]: " << passesResRECOAllRegions[0] << " muonTrigPass: " << muonTrigPass << "ZMASSres: " << ZMASSres << " tooManyResLeptons: " << tooManyResLeptons << " myRECOevent.hasPVertex: " << myRECOevent.hasPVertex << " myRECOevent.resolvedRECOmass: " << myRECOevent.resolvedRECOmass << std::endl;
    if((passesResRECOAllRegions[0]||passesResRECOAllRegions[1]||passesResRECOAllRegions[2]||passesResRECOAllRegions[3]||passesResRECOAllRegions[4]) && muonTrigPass && ZMASSres==1 && !tooManyResLeptons && myRECOevent.hasPVertex){
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_HighPtID2nd_Weights;
      std::vector<double> Muon_LooseTkIso2nd_Weights;
      std::vector<double> Muon_Reco_Weights;
      std::vector<double> Muon_Reco2nd_Weights;
      std::vector<double> Muon_Trig_Weights;
      Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), m_era);
      Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), m_era);
      Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), m_era);
      Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), m_era);
      Muon_Reco_Weights = myMuons.MuonRECOweight(myRECOevent.resolvedANAMuons[0]->p(), myRECOevent.resolvedANAMuons[0]->eta(), m_era);
      Muon_Reco2nd_Weights = myMuons.MuonRECOweight(myRECOevent.resolvedANAMuons[1]->p(), myRECOevent.resolvedANAMuons[1]->eta(), m_era);
      Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), m_era, m_isSignal, "Resolved");

      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.Muon_HighPtID2nd_Weight = Muon_HighPtID2nd_Weights[0];
      myRECOevent.Muon_HighPtID2nd_WeightUp = Muon_HighPtID2nd_Weights[1];
      myRECOevent.Muon_HighPtID2nd_WeightDown = Muon_HighPtID2nd_Weights[2];
      myRECOevent.Muon_LooseTkIso2nd_Weight = Muon_LooseTkIso2nd_Weights[0];
      myRECOevent.Muon_LooseTkIso2nd_WeightUp = Muon_LooseTkIso2nd_Weights[1];
      myRECOevent.Muon_LooseTkIso2nd_WeightDown = Muon_LooseTkIso2nd_Weights[2];
      myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
      myRECOevent.Muon_RECO_WeightUp = Muon_Reco_Weights[1];
      myRECOevent.Muon_RECO_WeightDown = Muon_Reco_Weights[2];
      myRECOevent.Muon_RECO2nd_Weight = Muon_Reco2nd_Weights[0];
      myRECOevent.Muon_RECO2nd_WeightUp = Muon_Reco2nd_Weights[1];
      myRECOevent.Muon_RECO2nd_WeightDown = Muon_Reco2nd_Weights[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
      std::cout<< "Setting Resolved ZPeak Weight" << std::endl;
      setEventWeight_Resolved(iEvent, myRECOevent);

      if(passesResRECOAllRegions[0]){
	std::cout << "PASSES RES ZPEAK CR with iEvent.id().event(): " << iEvent.id().event() << std::endl;
        m_eventsPassResZMASSRECO.fill(myRECOevent, 1, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 6, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 7, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 20, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 21, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 16, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 17, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 18, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 19, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 34, m_isSignal);
        m_eventsPassResZMASSRECO.fill(myRECOevent, 35, m_isSignal);
      }
      if(passesResRECOAllRegions[1]){
        m_eventsPassResZMASSRECO.fill(myRECOevent, 24, m_isSignal);
      }
      if(passesResRECOAllRegions[2]){
        m_eventsPassResZMASSRECO.fill(myRECOevent, 25, m_isSignal);
      }
      if(passesResRECOAllRegions[3]){
        m_eventsPassResZMASSRECO.fill(myRECOevent, 26, m_isSignal);
      }
      if(passesResRECOAllRegions[4]){
        m_eventsPassResZMASSRECO.fill(myRECOevent, 27, m_isSignal);
      }
    }

    if((passesResFSBRECOAllRegions[0]||passesResFSBRECOAllRegions[1]||passesResFSBRECOAllRegions[2]||passesResFSBRECOAllRegions[3]||passesResFSBRECOAllRegions[4]) && muonTrigPass && !ZMASSFSBres && !tooManyResFSBLeptons && myRECOevent.hasPVertex){
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_Reco_Weights;
      std::vector<double> Muon_Trig_Weights;
      std::vector<double> HEEP_SF;
      std::vector<double> egamma_SF;
      Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, m_era);
      std::cout << "myRECOevent.resFSBMuon_pt: " << myRECOevent.resFSBMuon_pt << " myRECOevent.resFSBMuon_eta: " << myRECOevent.resFSBMuon_eta << std::endl;
      Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, m_era);
      Muon_Reco_Weights = myMuons.MuonRECOweight(myRECOevent.resFSBMuon_p, myRECOevent.resFSBMuon_eta, m_era);
      Muon_Trig_Weights = myMuons.MuonTriggerWeight_ResFSB(myRECOevent.resFSBMuon_pt, myRECOevent.resFSBMuon_eta, m_era, m_isSignal);
      std::cout << "myRECOevent.resFSBElec->superCluster()->eta(): " << myRECOevent.resFSBElec->superCluster()->eta() << " myRECOevent.resFSBElec_pt: " << myRECOevent.resFSBElec_pt << std::endl;
      egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.resFSBElec->superCluster()->eta(), myRECOevent.resFSBElec_pt, m_era);

      HEEP_SF = myHEEP.ScaleFactor(myRECOevent.resFSBElec->et(), myRECOevent.resFSBElec_eta, m_era);
      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
      myRECOevent.Muon_RECO_WeightUp = Muon_Reco_Weights[1];
      myRECOevent.Muon_RECO_WeightDown = Muon_Reco_Weights[2];
      myRECOevent.HEEP_SF = HEEP_SF[0];
      myRECOevent.HEEP_SF_Up = HEEP_SF[1];
      myRECOevent.HEEP_SF_Down = HEEP_SF[2];
      myRECOevent.egamma_SF = egamma_SF[0];
      myRECOevent.egamma_SF_Up = egamma_SF[1];
      myRECOevent.egamma_SF_Down = egamma_SF[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
      setEventWeight_ResolvedFSB(iEvent, myRECOevent);

      if(passesResFSBRECOAllRegions[0] && myRECOevent.subleadMuon_selElectronMass > 400){
        myRECOevent.ResFSBCutProgress++;
        std::cout << "PASSING FSB" << std::endl;
        m_eventsPassResFSBRECO.fill(  myRECOevent, 1, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 6, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 7, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 28, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 29, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 12, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 13, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 16, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 17, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 18, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 19, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 34, m_isSignal);
        m_eventsPassResFSBRECO.fill(  myRECOevent, 35, m_isSignal);
/*	if(myRECOevent.subleadMuon_selElectronMass > 450){
	  std::cout << "filling m_eventsPassResFSBRECO_mll450" << std::endl;
          m_eventsPassResFSBRECO_mll450.fill(  myRECOevent, 1);
	}
        if(myRECOevent.subleadMuon_selElectronMass > 400){
          std::cout << "filling m_eventsPassResFSBRECO_mll400" << std::endl;
          m_eventsPassResFSBRECO_mll400.fill(  myRECOevent, 1);
        }
        if(myRECOevent.subleadMuon_selElectronMass > 500){
          std::cout << "filling m_eventsPassResFSBRECO_mll500" << std::endl;
          m_eventsPassResFSBRECO_mll500.fill(  myRECOevent, 1);
        }
        if(myRECOevent.subleadMuon_selElectronMass > 550){
          m_eventsPassResFSBRECO_mll550.fill(  myRECOevent, 1);
        }
        if(myRECOevent.subleadMuon_selElectronMass > 600){
          m_eventsPassResFSBRECO_mll600.fill(  myRECOevent, 1);
        }*/
      }
      if(passesResFSBRECOAllRegions[1] && myRECOevent.subleadMuon_selElectronMass > 400){
        m_eventsPassResFSBRECO.fill(  myRECOevent, 24, m_isSignal);
      }
      if(passesResFSBRECOAllRegions[2] && myRECOevent.subleadMuon_selElectronMass > 400){
        m_eventsPassResFSBRECO.fill(  myRECOevent, 25, m_isSignal);
      }
      if(passesResFSBRECOAllRegions[3] && myRECOevent.subleadMuon_selElectronMass > 400){
        m_eventsPassResFSBRECO.fill(  myRECOevent, 26, m_isSignal);
      }
      if(passesResFSBRECOAllRegions[4] && myRECOevent.subleadMuon_selElectronMass > 400){
        m_eventsPassResFSBRECO.fill(  myRECOevent, 27, m_isSignal);
      }
    }

    if((passesResRECOAllRegions[0]||passesResRECOAllRegions[1]||passesResRECOAllRegions[2]||passesResRECOAllRegions[3]||passesResRECOAllRegions[4]) && muonTrigPass && ZMASSres==2 && !tooManyResLeptons && myRECOevent.hasPVertex){
      myRECOevent.ResCutProgress++;
      std::vector<double> Muon_HighPtID_Weights;
      std::vector<double> Muon_LooseTkIso_Weights;
      std::vector<double> Muon_HighPtID2nd_Weights;
      std::vector<double> Muon_LooseTkIso2nd_Weights;
      std::vector<double> Muon_Reco_Weights;
      std::vector<double> Muon_Reco2nd_Weights;
      std::vector<double> Muon_Trig_Weights;

      Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), m_era);
      Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), m_era);
      Muon_HighPtID2nd_Weights = myMuons.MuonHighPTIDweight(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), m_era);
      Muon_LooseTkIso2nd_Weights = myMuons.MuonLooseTkIso(myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), m_era);
      Muon_Reco_Weights = myMuons.MuonRECOweight(myRECOevent.resolvedANAMuons[0]->p(), myRECOevent.resolvedANAMuons[0]->eta(), m_era);
      Muon_Reco2nd_Weights = myMuons.MuonRECOweight(myRECOevent.resolvedANAMuons[1]->p(), myRECOevent.resolvedANAMuons[1]->eta(), m_era);
      Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.resolvedANAMuons[0]->pt(), myRECOevent.resolvedANAMuons[0]->eta(), myRECOevent.resolvedANAMuons[1]->pt(), myRECOevent.resolvedANAMuons[1]->eta(), m_era, m_isSignal, "Resolved");

      myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
      myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
      myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
      myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
      myRECOevent.Muon_HighPtID2nd_Weight = Muon_HighPtID2nd_Weights[0];
      myRECOevent.Muon_HighPtID2nd_WeightUp = Muon_HighPtID2nd_Weights[1];
      myRECOevent.Muon_HighPtID2nd_WeightDown = Muon_HighPtID2nd_Weights[2];
      myRECOevent.Muon_LooseTkIso2nd_Weight = Muon_LooseTkIso2nd_Weights[0];
      myRECOevent.Muon_LooseTkIso2nd_WeightUp = Muon_LooseTkIso2nd_Weights[1];
      myRECOevent.Muon_LooseTkIso2nd_WeightDown = Muon_LooseTkIso2nd_Weights[2];
      myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
      myRECOevent.Muon_RECO_WeightUp = Muon_Reco_Weights[1];
      myRECOevent.Muon_RECO_WeightDown = Muon_Reco_Weights[2];
      myRECOevent.Muon_RECO2nd_Weight = Muon_Reco2nd_Weights[0];
      myRECOevent.Muon_RECO2nd_WeightUp = Muon_Reco2nd_Weights[1];
      myRECOevent.Muon_RECO2nd_WeightDown = Muon_Reco2nd_Weights[2];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
      myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
      myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];

      std::cout << "Setting Resolved SR Weight" << std::endl;
      setEventWeight_Resolved(iEvent, myRECOevent);

      if(passesResRECOAllRegions[0] && myRECOevent.subleadMuon_selMuonMass > 400){
        std::cout << "PASSING SR" << std::endl;
        myRECOevent.ResCutProgress++;
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 1, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 6, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 7, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 20, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 21, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 16, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 17, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 18, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 19, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 34, m_isSignal);
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 35, m_isSignal);
/*	if(myRECOevent.subleadMuon_selMuonMass > 450){
	  m_eventsPassResFailBoostRECO_mll450.fill(myRECOevent, 1);
	}
	if(myRECOevent.subleadMuon_selMuonMass > 400){
          m_eventsPassResFailBoostRECO_mll400.fill(myRECOevent, 1);
        }
        if(myRECOevent.subleadMuon_selMuonMass > 500){
          m_eventsPassResFailBoostRECO_mll500.fill(myRECOevent, 1);
        }
        if(myRECOevent.subleadMuon_selMuonMass > 550){
          m_eventsPassResFailBoostRECO_mll550.fill(myRECOevent, 1);
        }
        if(myRECOevent.subleadMuon_selMuonMass > 600){
          m_eventsPassResFailBoostRECO_mll600.fill(myRECOevent, 1);
        }*/
      }
      if(passesResRECOAllRegions[1] && myRECOevent.subleadMuon_selMuonMass > 400){
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 24, m_isSignal);
      }
      if(passesResRECOAllRegions[2] && myRECOevent.subleadMuon_selMuonMass > 400){
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 25, m_isSignal);
      }
      if(passesResRECOAllRegions[3] && myRECOevent.subleadMuon_selMuonMass > 400){
	std::cout << "Filling Res JERUp" << std::endl;
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 26, m_isSignal);
      }
      if(passesResRECOAllRegions[4] && myRECOevent.subleadMuon_selMuonMass > 400){
        m_eventsPassResFailBoostRECO.fill(myRECOevent, 27, m_isSignal);
      }
    }

  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ((m_doReco || !m_isMC) && m_doFast && myRECOevent.hasPVertex){
    std::cout << "RUNNING CONDENSED ANALYSIS FOR HIGGS COMBINE" << std::endl;
    if(preSelectReco_Fast(iEvent, iSetup, myRECOevent)) {
      std::cout<< "starting passExtensionRECO_Fast" << std::endl;
      passExtensionRECO_Fast(iEvent, myRECOevent);
      if (m_doTrig){
        if(m_isSignal){
         muonTrigPass = true;
	}else{
        muonTrigPass = passMuonTrig(iEvent, myRECOevent);
	}
      }
      if(myRECOevent.myMuonJetPairs.size() > 0){
        addMuons = additionalMuons(iEvent, myRECOevent, false, false, 0, false);
	ZMASS_Nom = subLeadingMuonZMass_Nominal(iEvent, myRECOevent, false);
        addElectrons = additionalElectrons(iEvent, myRECOevent, false, 0);
      }
      if(myRECOevent.myMuonJetPairs_JECUp.size() > 0){
        addMuons_JECUp = additionalMuons(iEvent, myRECOevent, false, false, 1, false);
        ZMASS_JECUp = subLeadingMuonZMass_JECUp(iEvent, myRECOevent, false);
        addElectrons_JECUp = additionalElectrons(iEvent, myRECOevent, false, 1);
      }
      if(myRECOevent.myMuonJetPairs_JECDown.size() > 0){
        addMuons_JECDown = additionalMuons(iEvent, myRECOevent, false, false, 2, false);
        ZMASS_JECDown = subLeadingMuonZMass_JECDown(iEvent, myRECOevent, false);
        addElectrons_JECDown = additionalElectrons(iEvent, myRECOevent, false, 2);
      }
      if(myRECOevent.myMuonJetPairs_JERUp.size() > 0){
        addMuons_JERUp = additionalMuons(iEvent, myRECOevent, false, false, 3, false);
        ZMASS_JERUp = subLeadingMuonZMass_JERUp(iEvent, myRECOevent, false);
        addElectrons_JERUp = additionalElectrons(iEvent, myRECOevent, false, 3);
      }
      if(myRECOevent.myMuonJetPairs_JERDown.size() > 0){
        addMuons_JERDown = additionalMuons(iEvent, myRECOevent, false, false, 4, false);
        ZMASS_JERDown = subLeadingMuonZMass_JERDown(iEvent, myRECOevent, false);
        addElectrons_JECDown = additionalElectrons(iEvent, myRECOevent, false, 4);
      }
      if(m_isMC && (addMuons || addMuons_JECUp || addMuons_JECDown || addMuons_JERUp || addMuons_JERDown)) {
	std::vector<double> Muon_LooseID_Weights;
        if(m_era == "2016") {
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
        }else if(m_era == "2017"){
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
        }else if(m_era == "2018"){
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
        }
        myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
        myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
        myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
	std::vector<double> Muon_HighPtID_Weights;
	std::vector<double> Muon_LooseTkIso_Weights;
        std::vector<double> Muon_Reco_Weights;
        if(m_era == "2016") {
          Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
          Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
        }else if(m_era == "2017"){
          Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
          Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
        }else if(m_era == "2018"){
          Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
          Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
        }
        Muon_Reco_Weights = myMuons.MuonRECOweight(myRECOevent.myMuonCand->p(), myRECOevent.myMuonCand->eta(), m_era);
	myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
        myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
        myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
        myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
        myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
        myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
        myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
        myRECOevent.Muon_RECO_WeightUp = Muon_Reco_Weights[1];
        myRECOevent.Muon_RECO_WeightDown = Muon_Reco_Weights[2];
        std::vector<double> Muon_Trig_Weights;
        if(m_era == "2016") {
          Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016", m_isSignal, "Boosted");
        }else if(m_era == "2017"){
          Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016", m_isSignal, "Boosted");
        }else if(m_era == "2018"){
          Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016", m_isSignal, "Boosted");
        }
        myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
        myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
        myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
	std::cout << "Setting event weight for boosted SR" << std::endl;
        setEventWeight(iEvent, myRECOevent);
      }
      //Fill histograms individually
      bool onShell = false;
      if(m_isSignal){
        onShell = WRresonanceStudy(iEvent, myRECOevent);
      }
      
      std::cout << "myRECOevent.myAddJetCandsHighPt.size(): " << myRECOevent.myAddJetCandsHighPt.size() << "myRECOevent.myMuonCandsHighPt.size(): " << myRECOevent.myMuonCandsHighPt.size() << "myRECOevent.myMuonJetPairs.size(): " << myRECOevent.myMuonJetPairs.size() << "muonTrigPass: " << muonTrigPass << "addMuons: " << addMuons << "ZMASS_Nom: " << ZMASS_Nom << "myRECOevent.nHighPtMuonsOutsideJet: " << myRECOevent.nHighPtMuonsOutsideJet << std::endl;
      if(myRECOevent.myAddJetCandsHighPt.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs.size() > 0 && muonTrigPass && myRECOevent.subleadMuon_selMuonMass > 150 && myRECOevent.nHighPtMuonsOutsideJet == 1 && myRECOevent.electronCands200 == 0 && !addElectrons && addMuons && ZMASS_Nom==2){// && myRECOevent.leadAK8JetMuonMassVal > 1800){
//        passesFastBoostRECO = true;
	myRECOevent.cutProgress++;
	std::cout << "doFast myRECOevent.weight: " << myRECOevent.weight << std::endl;
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 1., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 6., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 7., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 8., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 9., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 16., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 17., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 18., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 19., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 34., m_isSignal);
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 35., m_isSignal);
        if ( onShell){
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 1 ,m_isSignal);
	  m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 6 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 7 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 8 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 9 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 16 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 17 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 18 ,m_isSignal);
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 19 ,m_isSignal);
	}else if(!onShell){ 
	  m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 1 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 6 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 7 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 8 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 9 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 16 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 17 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 18 ,m_isSignal);
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 19 ,m_isSignal);
        }

      }
      if(myRECOevent.myAddJetCandsHighPt_JECUp.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JECUp.size() > 0 && muonTrigPass && addMuons_JECUp && ZMASS_JECUp==2 && myRECOevent.nHighPtMuonsOutsideJet_JECUp == 1 && !addElectrons_JECUp && myRECOevent.electronCands200 == 0){
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 2., m_isSignal);
        if ( onShell){
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 2 ,m_isSignal);
        }else if(!onShell){
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 2 ,m_isSignal);
        }
      }
      if(myRECOevent.myAddJetCandsHighPt_JECDown.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JECDown.size() > 0 && muonTrigPass && addMuons_JECDown && ZMASS_JECDown==2 && myRECOevent.nHighPtMuonsOutsideJet_JECDown == 1 && !addElectrons_JECDown && myRECOevent.electronCands200 == 0){
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 3., m_isSignal);
        if ( onShell){
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 3 ,m_isSignal);
        }else if(!onShell){
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 3 ,m_isSignal);
        }

      }
      if(myRECOevent.myAddJetCandsHighPt_JERUp.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JERUp.size() > 0 && muonTrigPass && addMuons_JERUp && ZMASS_JERUp==2 && myRECOevent.nHighPtMuonsOutsideJet_JERUp == 1 && !addElectrons_JERUp && myRECOevent.electronCands200 == 0){
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 4., m_isSignal);
        if ( onShell){
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 4 ,m_isSignal);
        }else if(!onShell){
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 4 ,m_isSignal);
        }
      }
      if(myRECOevent.myAddJetCandsHighPt_JERDown.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_JERDown.size() > 0 && muonTrigPass && addMuons_JERDown && ZMASS_JERDown==2 && myRECOevent.nHighPtMuonsOutsideJet_JERDown == 1 && !addElectrons_JERDown && myRECOevent.electronCands200 == 0){
        m_eventsFailResPassBoostRECO.fill(myRECOevent, 5., m_isSignal);
        if ( onShell){
          m_eventsFailResPassBoostRECO_onShell.fill( myRECOevent, 5 ,m_isSignal);
        }else if(!onShell){
          m_eventsFailResPassBoostRECO_offShell.fill(myRECOevent, 5 ,m_isSignal);
        }
      }
      ZMASS_Nom = 0;
      ZMASS_JECUp = 0;
      ZMASS_JECDown = 0;
      ZMASS_JERUp = 0;
      ZMASS_JERDown = 0;
      addMuons = false;
      addMuons_JECUp = false;
      addMuons_JECDown = false;
      addMuons_JERUp = false;
      addMuons_JERDown = false;
      std::cout << "BEGINNING ZPEAK FILLING" << std::endl;
      if(myRECOevent.myMuonJetPairs_noLSF.size() > 0){
        addMuons = additionalMuons(iEvent, myRECOevent, false, true, 0, false);
        ZMASS_Nom = subLeadingMuonZMass_Nominal(iEvent, myRECOevent, true);
      }
      if(myRECOevent.myMuonJetPairs_noLSF_JECUp.size() > 0){
        addMuons_JECUp = additionalMuons(iEvent, myRECOevent, false, true, 1, false);
        ZMASS_JECUp = subLeadingMuonZMass_JECUp(iEvent, myRECOevent, true);
      }
      if(myRECOevent.myMuonJetPairs_noLSF_JECDown.size() > 0){
        addMuons_JECDown = additionalMuons(iEvent, myRECOevent, false, true, 2, false);
        ZMASS_JECDown = subLeadingMuonZMass_JECDown(iEvent, myRECOevent, true);
      }
      if(myRECOevent.myMuonJetPairs_noLSF_JERUp.size() > 0){
        addMuons_JERUp = additionalMuons(iEvent, myRECOevent, false, true, 3, false);
        ZMASS_JERUp = subLeadingMuonZMass_JERUp(iEvent, myRECOevent, true);
      }
      if(myRECOevent.myMuonJetPairs_noLSF_JERDown.size() > 0){
        addMuons_JERDown = additionalMuons(iEvent, myRECOevent, false, true, 4, false);
        ZMASS_JERDown = subLeadingMuonZMass_JERDown(iEvent, myRECOevent, true);
      }
      if(m_isMC && (addMuons || addMuons_JECUp || addMuons_JECDown || addMuons_JERUp || addMuons_JERDown)) {
        std::vector<double> Muon_LooseID_Weights;
        if(m_era == "2016") {
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
        }else if(m_era == "2017"){
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
        }else if(m_era == "2018"){
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
        }
        myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
        myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
        myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
        std::vector<double> Muon_HighPtID_Weights;
	std::vector<double> Muon_LooseTkIso_Weights;
        std::vector<double> Muon_Reco_Weights;
        if(m_era == "2016") {
          Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
          Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2016");
        }else if(m_era == "2017"){
          Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
          Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2017");
        }else if(m_era == "2018"){
          Muon_HighPtID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
          Muon_LooseTkIso_Weights = myMuons.MuonLooseTkIso(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), "2018");
        }
        Muon_Reco_Weights = myMuons.MuonRECOweight(myRECOevent.myMuonCand->p(), myRECOevent.myMuonCand->eta(), m_era);
        myRECOevent.Muon_HighPtID_Weight = Muon_HighPtID_Weights[0];
        myRECOevent.Muon_HighPtID_WeightUp = Muon_HighPtID_Weights[1];
        myRECOevent.Muon_HighPtID_WeightDown = Muon_HighPtID_Weights[2];
        myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
        myRECOevent.Muon_LooseTkIso_WeightUp = Muon_LooseTkIso_Weights[1];
        myRECOevent.Muon_LooseTkIso_WeightDown = Muon_LooseTkIso_Weights[2];
        myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
        myRECOevent.Muon_RECO_WeightUp = Muon_Reco_Weights[1];
        myRECOevent.Muon_RECO_WeightDown = Muon_Reco_Weights[2];
        std::vector<double> Muon_Trig_Weights;
        if(m_era == "2016") {
          Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016", m_isSignal, "Boosted");
        }else if(m_era == "2017"){
          Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(), myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017", m_isSignal, "Boosted");
        }else if(m_era == "2018"){
          Muon_Trig_Weights = myMuons.MuonTriggerWeight(myRECOevent.myMuonCand->pt(), myRECOevent.myMuonCand->eta(),myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018", m_isSignal, "Boosted");
        }
        myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];
        myRECOevent.Muon_Trig_WeightUp = Muon_Trig_Weights[1];
        myRECOevent.Muon_Trig_WeightDown = Muon_Trig_Weights[2];
        std::cout << "Setting Boosted ZPeak Weight" << std::endl;
        setEventWeight(iEvent, myRECOevent);
      }
      std::cout << "myRECOevent.myAddJetCandsHighPt_noLSF.size(): " << myRECOevent.myAddJetCandsHighPt_noLSF.size() << " myRECOevent.myMuonCandsHighPt.size(): " << myRECOevent.myMuonCandsHighPt.size() << " myRECOevent.myMuonJetPairs_noLSF.size(): " << myRECOevent.myMuonJetPairs_noLSF.size() << " muonTrigPass: " << muonTrigPass << " addMuons: " << addMuons << " ZMASS_Nom: " << ZMASS_Nom << " myRECOevent.electronCands200: " << myRECOevent.electronCands200 << " myRECOevent.leadAK8JetMuonMassVal_noLSF: " << myRECOevent.leadAK8JetMuonMassVal_noLSF << std::endl;
      if(myRECOevent.myAddJetCandsHighPt_noLSF.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_noLSF.size() > 0 && muonTrigPass && addMuons && ZMASS_Nom==1 && myRECOevent.electronCands200 == 0){
        std::cout << "PASSES BOOST ZPEAK CR with iEvent.id().event(): " << iEvent.id().event() << std::endl;
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 1, m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 6., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 7., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 8., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 9., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 16., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 17., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 18., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 19., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 34., m_isSignal);
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 35., m_isSignal);
      }
      if(myRECOevent.myAddJetCandsHighPt_noLSF_JECUp.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_noLSF_JECUp.size() > 0 && muonTrigPass && addMuons_JECUp && ZMASS_JECUp==1 && myRECOevent.electronCands200 == 0){
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 2., m_isSignal);
      }
      if(myRECOevent.myAddJetCandsHighPt_noLSF_JECDown.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_noLSF_JECDown.size() > 0 && muonTrigPass && addMuons_JECDown && ZMASS_JECDown==1 && myRECOevent.electronCands200 == 0){
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 3., m_isSignal);
      }
      if(myRECOevent.myAddJetCandsHighPt_noLSF_JERUp.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_noLSF_JERUp.size() > 0 && muonTrigPass && addMuons_JERUp && ZMASS_JERUp==1 && myRECOevent.electronCands200 == 0){
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 4., m_isSignal);
      }
      if(myRECOevent.myAddJetCandsHighPt_noLSF_JERDown.size() > 0 && myRECOevent.myMuonCandsHighPt.size() > 0 && myRECOevent.myMuonJetPairs_noLSF_JERDown.size() > 0 && muonTrigPass && addMuons_JERDown && ZMASS_JERDown==1 && myRECOevent.electronCands200 == 0){
        m_eventsPassBoostZMASSRECO.fill(myRECOevent, 5., m_isSignal);
      }
    }

    myRECOevent.FSBcutProgress = 0;
    myRECOevent.FSBcutProgress++;
    addMuons = false;
    addMuons_JECUp = false;
    addMuons_JECDown = false;
    addMuons_JERUp = false;
    addMuons_JERDown = false;
    addElectrons = false;
    addElectrons_JECUp = false;
    addElectrons_JECDown = false;
    addElectrons_JERUp = false;
    addElectrons_JERDown = false;

    if(passFlavorSideband_Fast(iEvent, myRECOevent)) {
      std::cout << "Inside passFlavorSideband_Fast" << std::endl;
      electronTrigPass = passElectronTrig(iEvent, myRECOevent);
      std::cout << "myRECOevent.myElectronJetPairs.size(): " << myRECOevent.myElectronJetPairs.size() << std::endl;
      if(myRECOevent.myElectronJetPairs.size() > 0){
        myRECOevent.FSBcutProgress++;
	if(additionalMuons(iEvent, myRECOevent, true, false, 0, true)){
	  std::cout << "Passed additionalMuons" << std::endl;
	  addMuons = true;
          ZMASS_FSB = subLeadingMuonZMass_FlavorSideband_Nominal(iEvent, myRECOevent);
          addElectrons = additionalElectrons(iEvent, myRECOevent, true, 0);
	}
      }
      if(myRECOevent.myElectronJetPairs_JECUp.size() > 0){
        if(additionalMuons(iEvent, myRECOevent, true, false, 1, true)){
          addMuons_JECUp = true;
          ZMASS_FSB_JECUp = subLeadingMuonZMass_FlavorSideband_JECUp(iEvent, myRECOevent);
          addElectrons_JECUp = additionalElectrons(iEvent, myRECOevent, true, 1);
	}
      }
      if(myRECOevent.myElectronJetPairs_JECDown.size() > 0){
        if(additionalMuons(iEvent, myRECOevent, true, false, 2, true)){
	  addMuons_JECDown = true;
          ZMASS_FSB_JECDown = subLeadingMuonZMass_FlavorSideband_JECDown(iEvent, myRECOevent);
          addElectrons_JECDown = additionalElectrons(iEvent, myRECOevent, true, 2);
	}
      }
      if(myRECOevent.myElectronJetPairs_JERUp.size() > 0){
        if(additionalMuons(iEvent, myRECOevent, true, false, 3, true)){
	  addMuons_JERUp = true;
          ZMASS_FSB_JERUp = subLeadingMuonZMass_FlavorSideband_JERUp(iEvent, myRECOevent);
          addElectrons_JERUp = additionalElectrons(iEvent, myRECOevent, true, 3);
	}
      }
      if(myRECOevent.myElectronJetPairs_JERDown.size() > 0){
        if(additionalMuons(iEvent, myRECOevent, true, false, 4, true)){
          addMuons_JERDown = true;
          ZMASS_FSB_JERDown = subLeadingMuonZMass_FlavorSideband_JERDown(iEvent, myRECOevent);
          addElectrons_JERDown = additionalElectrons(iEvent, myRECOevent, true, 4);
	}
      }
      std::cout << "myRECOevent.myElectronJetPairs.size(): " << myRECOevent.myElectronJetPairs.size() << " addMuons: " << addMuons << std::endl;
      if(m_isMC && (myRECOevent.myElectronJetPairs.size() > 0 || myRECOevent.myElectronJetPairs_JECUp.size() > 0 || myRECOevent.myElectronJetPairs_JECDown.size() > 0 || myRECOevent.myElectronJetPairs_JERUp.size() > 0 || myRECOevent.myElectronJetPairs_JERDown.size() > 0) && (addMuons || addMuons_JECUp || addMuons_JECDown || addMuons_JERUp || addMuons_JERDown) ){
        std::vector<double> Muon_LooseID_Weights;
        if(m_era == "2016") {
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
        }else if(m_era == "2017"){
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
        }else if(m_era == "2018"){
          Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
        }
        myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
        myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
        myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
        std::vector<double> HEEP_SF = myHEEP.ScaleFactor(myRECOevent.myElectronCand->et(), myRECOevent.selectedElectronEta, m_era);
        std::vector<double> egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.myElectronCand->superCluster()->eta(), myRECOevent.selectedElectronPt, m_era);
        std::vector<double> egamma_SF_HLT = myEgammaEffi.HLT_ScaleFactor(myRECOevent.myElectronCand->superCluster()->eta(), myRECOevent.selectedElectronPt, m_era);
        myRECOevent.HEEP_SF = HEEP_SF[0];
	myRECOevent.HEEP_SF_Up = HEEP_SF[1];
        myRECOevent.HEEP_SF_Down = HEEP_SF[2];
        myRECOevent.egamma_SF = egamma_SF[0];
        myRECOevent.egamma_SF_Up = egamma_SF[1];
        myRECOevent.egamma_SF_Down = egamma_SF[2];
        myRECOevent.egamma_SF_HLT = egamma_SF_HLT[0];
        myRECOevent.egamma_SF_HLT_Up = egamma_SF_HLT[1];
        myRECOevent.egamma_SF_HLT_Down = egamma_SF_HLT[2];
        setEventWeight_FSB(iEvent, myRECOevent);
      }
      std::cout << "electronTrigPass: " << electronTrigPass << " ZMASS_FSB: " << ZMASS_FSB << " addElectrons: " << addElectrons << " myRECOevent.leadAK8JetElectronMassVal: " << myRECOevent.leadAK8JetElectronMassVal << std::endl;
      if(myRECOevent.myElectronJetPairs.size() > 0 && electronTrigPass && !ZMASS_FSB && addMuons && !addElectrons && myRECOevent.nMuonOutSideJetFSB == 0){
        std::cout << "doFast FILLING m_eventsPassBoostFSBRECO_all" << std::endl;
	m_eventsPassBoostFSBRECO.fill(myRECOevent, 1, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 6, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 7, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 10, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 11, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 12, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 13, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 32, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 33, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 34, m_isSignal);
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 35, m_isSignal);
      }
      if(myRECOevent.myElectronJetPairs_JECUp.size() > 0 && electronTrigPass && !ZMASS_FSB_JECUp && addMuons_JECUp && !addElectrons_JECUp){
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 2, m_isSignal);
      }
      if(myRECOevent.myElectronJetPairs_JECDown.size() > 0 && electronTrigPass && !ZMASS_FSB_JECDown && addMuons_JECDown && !addElectrons_JECDown){
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 3, m_isSignal);
      }
      if(myRECOevent.myElectronJetPairs_JERUp.size() > 0 && electronTrigPass && !ZMASS_FSB_JERUp && addMuons_JERUp && !addElectrons_JERUp){
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 4, m_isSignal);
      }
      if(myRECOevent.myElectronJetPairs_JERDown.size() > 0 && electronTrigPass && !ZMASS_FSB_JERDown && addMuons_JERDown && !addElectrons_JERDown){
        m_eventsPassBoostFSBRECO.fill(myRECOevent, 5, m_isSignal);
      }
    }
  }
  bool onShell = false;
  if(m_isSignal){
    onShell = WRresonanceStudy(iEvent, myRECOevent);
  }
  
  if (onShell) m_allOnShellEvents.fill(myRECOevent, 1, m_isSignal);
  if(!onShell) m_allOffShellEvents.fill(myRECOevent, 1, m_isSignal);

  std::cout << "TIME TO FILL ALL EVENTS" << std::endl;
  setEventWeight_ResolvedFSB(iEvent, myRECOevent);
  setEventWeight_Resolved(iEvent, myRECOevent);
  m_allEvents.fill(myRECOevent, 1, m_isSignal);
  std::cout << "Done with event" << std::endl;
  
}
// A B
// C D
// DON'T CALL THIS UNLESS YOU PASS THE NOISO ELECTRON EVENT SELECTION
bool cmsWRextension::passABCD(eventBits& myEvent, bool AvB /*versus AvC */) {
  const vid::CutFlowResult* vidResult =  myEvent.myElectronCand_noISO->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");
  
  if(AvB) { 
    return vidResult->getCutResultByIndex(cutnrs::HEEPV70::SIGMAIETAIETA);
  }
  if(!AvB) {
    return vidResult->getCutResultByIndex(cutnrs::HEEPV70::TRKISO);
  }
  return false;            
}                                                                               
bool cmsWRextension::passFSBbin(eventBits& myEvent, bool ISO, int ptCut) {
  std::cout << "inside passFSBbin" << std::endl;
  //LOGIC IS REPEATED FOR ISO AND NONISO SELECTIONS
  if (ISO) {
    std::cout << "myEvent.myElectronJetPairs.size(): " << myEvent.myElectronJetPairs.size() << std::endl;
    std::cout << "myEvent.selectedElectronPt: " << myEvent.selectedElectronPt << std::endl;
    if (myEvent.myElectronJetPairs.size() < 1)  return false;  //NO PAIRS MEANS IT COULD NOT PASS
    if (myEvent.selectedElectronPt >= ptCut) return true;
  } else {
    std::cout << "myEvent.myElectronJetPairs_noISO.size(): " << myEvent.myElectronJetPairs_noISO.size() << std::endl;
    std::cout << "myEvent.selectedElectron_noISO_Pt: " << myEvent.selectedElectron_noISO_Pt << std::endl;
    if (myEvent.myElectronJetPairs_noISO.size() < 1) return false;
    if (myEvent.selectedElectron_noISO_Pt >= ptCut) return true;
  }
  //std::cout << "WELCOME TO THE TWILIGHT ZONE" << std::endl;
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
  if (myEvent.genVtx == NULL) {
	std::cout << "myEvent.genVtx is not defined!" << std::endl;
    return;
  }
  if (myEvent.PVertex == NULL) {
    std::cout << "myEvent.PVertex is not defined!" << std::endl;
    return;
  }

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
void cmsWRextension::setEventWeight_Resolved(const edm::Event& iEvent, eventBits& myEvent) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);

      myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      myEvent.weight = myEvent.count;
      myEvent.weight = myEvent.weight*myEvent.Muon_HighPtID_Weight*myEvent.Muon_LooseTkIso_Weight*myEvent.Muon_HighPtID2nd_Weight*myEvent.Muon_LooseTkIso2nd_Weight*myEvent.Muon_Trig_Weight*myEvent._prefiringweight*myEvent.Muon_RECO_Weight*myEvent.Muon_RECO2nd_Weight*myEvent.puWeight;
      if(m_foundZ){
	std::vector<double> Zweight = getZweight(iEvent, myEvent);
	myEvent.Zweight = Zweight[0];
	myEvent.Zweight_Up = Zweight[1];
        myEvent.Zweight_Down = Zweight[2];
	myEvent.weight = myEvent.weight * Zweight[0];
      }
  } else {
      myEvent.weight = 1;
      myEvent.count = 1;
  }

}
void cmsWRextension::setEventWeight(const edm::Event& iEvent, eventBits& myEvent) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);
      
      myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      myEvent.weight = myEvent.count;
	myEvent.weight = myEvent.weight*myEvent.Muon_HighPtID_Weight*myEvent.Muon_LooseID_Weight*myEvent.Muon_LooseTkIso_Weight*myEvent.Muon_Trig_Weight*myEvent._prefiringweight*myEvent.Muon_RECO_Weight*myEvent.puWeight;
	std::cout << "myEvent.Muon_HighPtID_Weight: " << myEvent.Muon_HighPtID_Weight << "myEvent.Muon_LooseID_Weight: " << myEvent.Muon_LooseID_Weight << "myEvent.Muon_LooseTkIso_Weight: " << myEvent.Muon_LooseTkIso_Weight << "myEvent.Muon_Trig_Weight: " << myEvent.Muon_Trig_Weight << "myEvent._prefiringweight: " << myEvent._prefiringweight << "myEvent.weight: " << myEvent.weight << " myEvent.Muon_RECO_Weight: " << myEvent.Muon_RECO_Weight << std::endl;
      }
      if(m_foundZ){
        std::vector<double> Zweight = getZweight(iEvent, myEvent);
        myEvent.Zweight = Zweight[0];
        myEvent.Zweight_Up = Zweight[1];
        myEvent.Zweight_Down = Zweight[2];
        myEvent.weight = myEvent.weight * Zweight[0];
      }
  } else {
      myEvent.weight = 1;
      myEvent.count = 1;
  }

}
void cmsWRextension::setEventWeight_ResolvedFSB(const edm::Event& iEvent, eventBits& myEvent){
  std::cout << "Setting Resolved FSB weight" << std::endl;
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);

      myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      myEvent.FSBweight = myEvent.count;
      myEvent.FSBweight = myEvent.FSBweight*myEvent.HEEP_SF*myEvent.egamma_SF*myEvent.Muon_HighPtID_Weight*myEvent.Muon_LooseTkIso_Weight*myEvent._prefiringweight*myEvent.Muon_Trig_Weight*myEvent.Muon_RECO_Weight*myEvent.puWeight;      
      if(m_foundZ){
        std::vector<double> Zweight = getZweight(iEvent, myEvent);
        myEvent.Zweight = Zweight[0];
        myEvent.Zweight_Up = Zweight[1];
        myEvent.Zweight_Down = Zweight[2];
        myEvent.FSBweight = myEvent.FSBweight * Zweight[0];
      }
  } else {
      myEvent.FSBweight = 1;
      myEvent.count = 1;
  }

}
void cmsWRextension::setEventWeight_FSB(const edm::Event& iEvent, eventBits& myEvent) {
  if(m_isMC) {
      edm::Handle<GenEventInfoProduct> eventInfo;
      iEvent.getByToken(m_genEventInfoToken, eventInfo);

      myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
      myEvent.FSBweight = myEvent.count;
      myEvent.FSBweight = myEvent.FSBweight*myEvent.HEEP_SF*myEvent.egamma_SF*myEvent.egamma_SF_HLT*myEvent.Muon_LooseID_Weight*myEvent._prefiringweight*myEvent.puWeight;
      }
      if(m_foundZ){
        std::vector<double> Zweight = getZweight(iEvent, myEvent);
        myEvent.Zweight = Zweight[0];
        myEvent.Zweight_Up = Zweight[1];
        myEvent.Zweight_Down = Zweight[2];
        myEvent.FSBweight = myEvent.FSBweight * Zweight[0];
      }
  } else {
      myEvent.FSBweight = 1;
      myEvent.count = 1;
  }

}
std::vector<double> cmsWRextension::getZweight(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "Running getZweight" << std::endl;
  std::vector<double> Zweights;
  double zpt = -1;
  double zm  = -1;
  zpt = myEvent.genZpt;
  zm = myEvent.genZmass;
//  myEvent.genZpt   = zpt;
//  myEvent.genZmass = zm;

  if(zpt > 0 && zm > 0) {
    return myZweights.getZweight(zm, zpt);
  }
  Zweights.push_back(1.0);
  Zweights.push_back(1.0);
  Zweights.push_back(1.0);
  return Zweights;
}
//void cmsWRextension::setEventWeight_FSB_noISO(const edm::Event& iEvent, eventBits& myEvent) {
//  if(m_isMC) {
//      edm::Handle<GenEventInfoProduct> eventInfo;
//      iEvent.getByToken(m_genEventInfoToken, eventInfo);
//      if(!m_amcatnlo) {
//        myEvent.FSBweight_noISO = eventInfo->weight()*myEvent.puWeight;
//        myEvent.count = 1;
//	myEvent.FSBweight_noISO = myEvent.FSBweight_noISO*myEvent.HEEP_SF_noISO*myEvent.egamma_SF_noISO*myEvent.Muon_LooseID_Weight*myEvent._prefiringweight;
//      }
//      else {
//        myEvent.FSBweight_noISO = eventInfo->weight()*myEvent.puWeight/fabs(eventInfo->weight());
//        myEvent.count = eventInfo->weight()/fabs(eventInfo->weight());
//	myEvent.FSBweight_noISO = myEvent.FSBweight_noISO*myEvent.HEEP_SF_noISO*myEvent.egamma_SF_noISO*myEvent.Muon_LooseID_Weight*myEvent._prefiringweight;
//      }
//  } else {
//      myEvent.FSBweight_noISO = 1;
//      myEvent.count = 1;
//  }
//
//}
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
bool cmsWRextension::passFlavorSideband_Fast(const edm::Event& iEvent, eventBits& myRECOevent) {

  /*if (myRECOevent.myResCandJets.size() == 1 && myRECOevent.resolvedANAMuons.size() > 0){
    double dR_jet1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[0]->eta(),myRECOevent.myResCandJets[0]->eta,myRECOevent.resolvedANAMuons[0]->phi(),myRECOevent.myResCandJets[0]->phi));
    std::cout << "dR_jet1_muon2: " << dR_jet1_muon2 << std::endl;
    if (dR_jet1_muon2 > 0.4) return false;
  }else*/ if(myRECOevent.myResFSBCandJets.size() > 1 && myRECOevent.nResFSBMuons > 0){
    double dR_jet1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resFSBMuon->eta(),myRECOevent.myResFSBCandJets[0]->eta,myRECOevent.resFSBMuon->phi(),myRECOevent.myResFSBCandJets[0]->phi));
    double dR_jet2_muon2 = sqrt(::wrTools::dR2(myRECOevent.resFSBMuon->eta(),myRECOevent.myResFSBCandJets[1]->eta,myRECOevent.resFSBMuon->phi(),myRECOevent.myResFSBCandJets[1]->phi));
    std::cout << "dR_jet1_muon2: " << dR_jet1_muon2 << std::endl;
    std::cout << "dR_jet2_muon2: " << dR_jet2_muon2 << std::endl;
    if (dR_jet1_muon2 > 0.4 && dR_jet2_muon2 > 0.4) return false;
  }
  myRECOevent.FSBcutProgress++;
  jetSelection(iEvent, myRECOevent);
  electronSelection(iEvent, myRECOevent);
  std::cout << "done with electronSelection" <<std::endl;


  if( (myRECOevent.myAddJetCandsHighPt.size() < 1) && myRECOevent.myAddJetCandsHighPt_JECUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_JECDown.size() < 1 && myRECOevent.myAddJetCandsHighPt_JERUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_JERDown.size() < 1) {
//  if( (myRECOevent.myAddJetCandsHighPt_noLSF.size() < 1) && myRECOevent.myAddJetCandsHighPt_noLSF_JECUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF_JECDown.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF_JERUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF_JERDown.size() < 1) {
    return false;
  }

//Removed for cutFlow study
/*  if(myRECOevent.muonCands > 0){
    std::cout << "TOO MANY BOOSTED MUONS IN FSB EVENT" << std::endl;
    return false;
  }*/

  if(myRECOevent.myElectronCandsHighPt200.size() == 0) return false;

  if(myRECOevent.myElectronCand->pt() < 60) return false;

  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs_JECUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs_JECDown;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs_JERUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs_JERDown;
  if( myRECOevent.myElectronCandsHighPt200.size() > 0 ){
    if(myRECOevent.myAddJetCandsHighPt.size() > 0){
//    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
       const baconhep::TAddJet* iJet = myRECOevent.myAddJetCandsHighPt.front();    
       if(fabs(reco::deltaPhi((iJet)->phi, myRECOevent.myElectronCand->phi())) > 2.0 ){// continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (iJet)->pT, (iJet)->eta, (iJet)->phi, (iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
       electronJetPairs.push_back(std::make_pair( (iJet) , myRECOevent.myElectronCand ));
       }
    }
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JECUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JECUp.end(); iJet++) {
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) < 2.0 ) continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
       electronJetPairs_JECUp.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
    }
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JECDown.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JECDown.end(); iJet++) {
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) < 2.0 ) continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
       electronJetPairs_JECDown.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
    }
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JERUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JERUp.end(); iJet++) {
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) < 2.0 ) continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
       electronJetPairs_JERUp.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
    }
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JERDown.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JERDown.end(); iJet++) {
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) < 2.0 ) continue;

       TLorentzVector* jetVec_temp = new TLorentzVector();
       jetVec_temp->SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());
       electronJetPairs_JERDown.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
    }
  }
  std::sort(electronJetPairs.begin(),electronJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);
  myRECOevent.myElectronJetPairs = electronJetPairs;

  std::sort(electronJetPairs_JECUp.begin(),electronJetPairs_JECUp.end(),::wrTools::comparePairMassPointerTAddJet);
  myRECOevent.myElectronJetPairs_JECUp = electronJetPairs_JECUp;

  std::sort(electronJetPairs_JECDown.begin(),electronJetPairs_JECDown.end(),::wrTools::comparePairMassPointerTAddJet);
  myRECOevent.myElectronJetPairs_JECDown = electronJetPairs_JECDown;

  std::sort(electronJetPairs_JERUp.begin(),electronJetPairs_JERUp.end(),::wrTools::comparePairMassPointerTAddJet);
  myRECOevent.myElectronJetPairs_JERUp = electronJetPairs_JERUp;

  std::sort(electronJetPairs_JERDown.begin(),electronJetPairs_JERDown.end(),::wrTools::comparePairMassPointerTAddJet);
  myRECOevent.myElectronJetPairs_JERDown = electronJetPairs_JERDown;

  if(electronJetPairs.size() > 0){
    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs[0].first->pT, electronJetPairs[0].first->eta, electronJetPairs[0].first->phi, electronJetPairs[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    auto corrP4  = electronJetPairs[0].second->p4();
/*    if(m_isMC && m_era == "2016"){
      auto corrP4  = electronJetPairs[0].second->p4()*electronJetPairs[0].second->userFloat("ecalTrkEnergyPostCorr") / electronJetPairs[0].second->energy();
      myRECOevent.leadAK8JetElectronMassVal = ( jetVec + corrP4 ).mass();
    }*/

    myRECOevent.leadAK8JetElectronMassVal = ( jetVec + corrP4 ).mass();
    myRECOevent.selectedElectronPt  = electronJetPairs[0].second->pt();
    myRECOevent.selectedElectronEta = electronJetPairs[0].second->eta();
    std::cout << "myRECOevent.selectedElectronPt: " << myRECOevent.selectedElectronPt << std::endl;
  }

  if(electronJetPairs_JECUp.size() > 0){
    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs_JECUp[0].first->pT, electronJetPairs_JECUp[0].first->eta, electronJetPairs_JECUp[0].first->phi, electronJetPairs_JECUp[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    auto corrP4  = electronJetPairs_JECUp[0].second->p4();
/*    if(m_isMC && m_era == "2016"){
      auto corrP4  = electronJetPairs_JECUp[0].second->p4()*electronJetPairs_JECUp[0].second->userFloat("ecalTrkEnergyPostCorr") / electronJetPairs_JECUp[0].second->energy();
      myRECOevent.leadAK8JetElectronMassVal_JECUp = ( jetVec + corrP4 ).mass();
    }*/

    myRECOevent.leadAK8JetElectronMassVal_JECUp = ( jetVec + corrP4 ).mass();
    myRECOevent.selectedElectronPt  = electronJetPairs_JECUp[0].second->pt();
    myRECOevent.selectedElectronEta = electronJetPairs_JECUp[0].second->eta();

  }
  if(electronJetPairs_JECDown.size() > 0){
    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs_JECDown[0].first->pT, electronJetPairs_JECDown[0].first->eta, electronJetPairs_JECDown[0].first->phi, electronJetPairs_JECDown[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    auto corrP4  = electronJetPairs_JECDown[0].second->p4();
/*    if(m_isMC && m_era == "2016"){
      auto corrP4  = electronJetPairs_JECDown[0].second->p4()*electronJetPairs_JECDown[0].second->userFloat("ecalTrkEnergyPostCorr") / electronJetPairs_JECDown[0].second->energy();
      myRECOevent.leadAK8JetElectronMassVal_JECDown = ( jetVec + corrP4 ).mass();
    }*/

    myRECOevent.leadAK8JetElectronMassVal_JECDown = ( jetVec + corrP4 ).mass();
    myRECOevent.selectedElectronPt  = electronJetPairs_JECDown[0].second->pt();
    myRECOevent.selectedElectronEta = electronJetPairs_JECDown[0].second->eta();

  }
  if(electronJetPairs_JERUp.size() > 0){
    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs_JERUp[0].first->pT, electronJetPairs_JERUp[0].first->eta, electronJetPairs_JERUp[0].first->phi, electronJetPairs_JERUp[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    auto corrP4  = electronJetPairs_JERUp[0].second->p4();
/*    if(m_isMC && m_era == "2016"){
      auto corrP4  = electronJetPairs_JERUp[0].second->p4()*electronJetPairs_JERUp[0].second->userFloat("ecalTrkEnergyPostCorr") / electronJetPairs_JERUp[0].second->energy();
      myRECOevent.leadAK8JetElectronMassVal_JERUp = ( jetVec + corrP4 ).mass();
    }*/

    myRECOevent.leadAK8JetElectronMassVal_JERUp = ( jetVec + corrP4 ).mass();
   myRECOevent.selectedElectronPt  = electronJetPairs_JERUp[0].second->pt();
   myRECOevent.selectedElectronEta = electronJetPairs_JERUp[0].second->eta();

  }
  if(electronJetPairs_JERDown.size() > 0){
    TLorentzVector* jetVec_temp = new TLorentzVector();
    jetVec_temp->SetPtEtaPhiE( electronJetPairs_JERDown[0].first->pT, electronJetPairs_JERDown[0].first->eta, electronJetPairs_JERDown[0].first->phi, electronJetPairs_JERDown[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp->X(),jetVec_temp->Y(),jetVec_temp->Z(),jetVec_temp->T());

    auto corrP4  = electronJetPairs_JERDown[0].second->p4();
/*    if(m_isMC && m_era == "2016"){
      auto corrP4  = electronJetPairs_JERDown[0].second->p4()*electronJetPairs_JERDown[0].second->userFloat("ecalTrkEnergyPostCorr") / electronJetPairs_JERDown[0].second->energy();
      myRECOevent.leadAK8JetElectronMassVal_JERDown = ( jetVec + corrP4 ).mass();
    }*/

    myRECOevent.leadAK8JetElectronMassVal_JERDown = ( jetVec + corrP4 ).mass();
    myRECOevent.selectedElectronPt  = electronJetPairs_JERDown[0].second->pt();
    myRECOevent.selectedElectronEta = electronJetPairs_JERDown[0].second->eta();

  }

  return true;

}
bool cmsWRextension::passFlavorSideband(const edm::Event& iEvent, eventBits& myRECOevent) {

  if (myRECOevent.myElectronCand == NULL || \
      myRECOevent.myElectronCand_noISO == NULL)
     { return false; }

  std::cout<< "BEGINNING SELECTION ON FLAVOR SIDEBAND" << std::endl;
  if( myRECOevent.myAddJetCandsHighPt_noLSF.size() < 1 and myRECOevent.myAddJetCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<< std::endl;
    return false;
  }

  myRECOevent.FSBcutProgress++;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> electronJetPairs_noISO;
  std::cout << "CHECKING FOR ELEC CANDS" << std::endl;
  std::cout << "myRECOevent.myElectronCandsHighPt200.size(): " << myRECOevent.myElectronCandsHighPt200.size() << std::endl;
  TLorentzVector jetVec_temp = TLorentzVector();
  if( myRECOevent.myElectronCandsHighPt200.size() > 0 ){

    //BUILD PAIRS OF AK8 JETS WITH THE LEAD ELECTRON
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
       std::cout << "Looping over Jets" << std::endl;
       std::cout << "fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())): " << fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) << std::endl;
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand->phi())) < 2.0 ) continue;

       jetVec_temp.SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp.X(),jetVec_temp.Y(),jetVec_temp.Z(),jetVec_temp.T());
 
       double eventMass = ( jetVec + myRECOevent.myElectronCand->p4() ).mass();
       std::cout << "eventMass: " << eventMass << std::endl;

       if( eventMass < 800 ) {myRECOevent.boostedFSBRECOmassAbove600 = false;}
       else {myRECOevent.boostedFSBRECOmassAbove600 = true;}

       std::cout << "FOUND CAND DIOBJECT WITH ISO ELE" << std::endl;

       electronJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand ));
    }
  }
  //NO ISO CHECKING
  std::cout << "CHECKING FOR NON ISO ELEC CANDS" << std::endl;
  if( myRECOevent.myElectronCandsHighPt50_noISO.size() > 0){
    std::cout << "FOUND NON ISO ELEC CANDS" << std::endl;
    for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF.end(); iJet++) {
       std::cout << "LOOPING THROUGH DIOBJECT CANDS" << std::endl;
       if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myElectronCand_noISO->phi())) < 2.0 ) continue;

       jetVec_temp.SetPtEtaPhiE( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->E );

       math::XYZTLorentzVector jetVec;
       jetVec.SetXYZT(jetVec_temp.X(),jetVec_temp.Y(),jetVec_temp.Z(),jetVec_temp.T());
 
       double eventMass_noISO = ( jetVec + myRECOevent.myElectronCand_noISO->p4() ).mass();

       if( eventMass_noISO < 800 ) continue;

       std::cout << "FOUND CAND DIOBJECT WITH NON ISO ELE" << std::endl;

       electronJetPairs_noISO.push_back(std::make_pair( (*iJet) , myRECOevent.myElectronCand_noISO ));
    }
  }

  if(electronJetPairs.size() < 1 && electronJetPairs_noISO.size() < 1){
    std::cout<< "EVENTS FAILS, NO ELECTRON JET PAIRS." << std::endl;
    return false;
  }

  if( electronJetPairs.size() > 0 ) {
    std::cout << "PROCESSING ELECTRON JET PAIRS" << std::endl;
    myRECOevent.myElectronJetPairs = electronJetPairs;
    std::sort(electronJetPairs.begin(),electronJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);

    jetVec_temp.SetPtEtaPhiE( electronJetPairs[0].first->pT, electronJetPairs[0].first->eta, electronJetPairs[0].first->phi, electronJetPairs[0].first->E );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp.X(),jetVec_temp.Y(),jetVec_temp.Z(),jetVec_temp.T());

    auto corrP4  = electronJetPairs[0].second->p4();
/*    if(m_isMC && m_era == "2016"){
      auto corrP4  = electronJetPairs[0].second->p4()*electronJetPairs[0].second->userFloat("ecalTrkEnergyPostCorr") / electronJetPairs[0].second->energy();
      myRECOevent.leadAK8JetElectronMassVal = ( jetVec + corrP4 ).mass();
    }*/

    myRECOevent.leadAK8JetElectronMassVal = ( jetVec + corrP4 ).mass();

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
    myRECOevent.selectedJetMass = electronJetPairs[0].first->SDmass*electronJetPairs[0].first->SDmassCorr;

    if(electronJetPairs[0].first->tau1==0){
      myRECOevent.selectedJetTau21 = -9999.;
    }else{
      myRECOevent.selectedJetTau21 = (electronJetPairs[0].first->tau2)/(electronJetPairs[0].first->tau1);
    }

    if(!additionalMuons(iEvent, myRECOevent, true, false,0, true)){
      std::cout<< "EVENTS FAILS, NO MUONS OVER 10 GEV WITHIN ACCEPTANCE. " << myRECOevent.myMuonCands.size()<< " MUONS FOUND." << std::endl;
      return false;
    }
    std::cout << "before subLeadingMuonZMass_FlavorSideband" << std::endl;
    if (subLeadingMuonZMass_FlavorSideband(iEvent, myRECOevent, false)){
        std::cout<< "EVENTS FAILS ELECTRON + MUON MASS" << std::endl;
        return false;
    }
  }
  if( electronJetPairs_noISO.size() > 0 ) {
    std::cout << "##NOT PROCESSING NONISO ELECTRON JET PAIRS### WE DON'T CARE ANY MORE" << std::endl;
  //  myRECOevent.myElectronJetPairs_noISO = electronJetPairs_noISO;
  //  std::sort(electronJetPairs_noISO.begin(),electronJetPairs_noISO.end(),::wrTools::comparePairMassPointerTAddJet);

  //  jetVec_temp.SetPtEtaPhiE( electronJetPairs_noISO[0].first->pT, electronJetPairs_noISO[0].first->eta, electronJetPairs_noISO[0].first->phi, electronJetPairs_noISO[0].first->E );

  //  math::XYZTLorentzVector jetVec;
  //  jetVec.SetXYZT(jetVec_temp.X(),jetVec_temp.Y(),jetVec_temp.Z(),jetVec_temp.T());

  //  myRECOevent.leadAK8JetElectronMassVal_noISO = ( jetVec + electronJetPairs_noISO[0].second->p4() ).mass();

  //  myRECOevent.selectedElectron_noISO_Pt  = electronJetPairs_noISO[0].second->pt();
  //  myRECOevent.selectedElectron_noISO_Phi = electronJetPairs_noISO[0].second->phi();
  //  myRECOevent.selectedElectron_noISO_Eta = electronJetPairs_noISO[0].second->eta();

  //  //NOW WE ACCES THE VIDRESULT AGAIN FOR THE CHOSEN ELECTRON
  //  const vid::CutFlowResult* vidResult =  electronJetPairs_noISO[0].second->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");

  //  if(myRECOevent.selectedElectron_noISO_Eta < 1.4442) {//BARREL
  //    myRECOevent.selElectron_noISO_barrel_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED); 
  //    myRECOevent.selElectron_noISO_barrel_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
  //    myRECOevent.selElectron_noISO_barrel_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
  //    myRECOevent.selElectron_noISO_barrel_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
////  NOT SUPPORTED    myRECOevent.selElectron_noISO_barrel_E2x5vE5x5          = vidResult->getValueCutUpon(HEEPV70::TRKISO);
  //    myRECOevent.selElectron_noISO_barrel_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
  //    myRECOevent.selElectron_noISO_barrel_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
  //    myRECOevent.selElectron_noISO_barrel_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
  //    myRECOevent.selElectron_noISO_barrel_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
  //  }
  //  if(myRECOevent.selectedElectron_noISO_Eta > 1.566) {//ENDCAP
  //    myRECOevent.selElectron_noISO_endcap_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED); 
  //    myRECOevent.selElectron_noISO_endcap_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
  //    myRECOevent.selElectron_noISO_endcap_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
  //    myRECOevent.selElectron_noISO_endcap_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
////  NOT SUPPORTED    myRECOevent.selElectron_noISO_barrel_E2x5vE5x5          = vidResult->getValueCutUpon(HEEPV70::TRKISO);
  //    myRECOevent.selElectron_noISO_endcap_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
  //    myRECOevent.selElectron_noISO_endcap_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
  //    myRECOevent.selElectron_noISO_endcap_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
  //    myRECOevent.selElectron_noISO_endcap_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
  //  }
  //  myRECOevent.selectedJet_EleNoISO_Pt   = electronJetPairs_noISO[0].first->pT;
  //  myRECOevent.selectedJet_EleNoISO_Phi  = electronJetPairs_noISO[0].first->phi;
  //  myRECOevent.selectedJet_EleNoISO_Eta  = electronJetPairs_noISO[0].first->eta;
  //  myRECOevent.selectedJet_EleNoISO_Mass = electronJetPairs_noISO[0].first->SDmass;

  //  if(electronJetPairs_noISO[0].first->tau1==0){
  //    myRECOevent.selectedJet_EleNoISO_Tau21 = -9999.;
  //  }else{
  //    myRECOevent.selectedJet_EleNoISO_Tau21 = (electronJetPairs_noISO[0].first->tau2)/(electronJetPairs_noISO[0].first->tau1);
  //  }
  //  additionalMuons(iEvent, myRECOevent, true, false, 0, false);
  //  if( myRECOevent.myMuonCands.size() < 1){
  //    std::cout<< "EVENTS FAILS, NO MUONS OVER 10 GEV WITHIN ACCEPTANCE. " << myRECOevent.myMuonCands.size()<< " MUONS FOUND." << std::endl;
  //    return false;
  //  }
  //  if (subLeadingMuonZMass_FlavorSideband(iEvent, myRECOevent, true)){
  //      std::cout<< "EVENTS FAILS ELECTRON + MUON MASS" << std::endl;
  //      return false;
  //  }
  }

  if(m_isMC && electronJetPairs.size() > 0) {
    std::vector<double> Muon_LooseID_Weights;
    if(m_era == "2016") {
      Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
    }else if(m_era == "2017"){
      Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
    }else if(m_era == "2018"){
      Muon_LooseID_Weights = myMuons.MuonLooseIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
    }
    myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
    myRECOevent.Muon_LooseID_WeightUp = Muon_LooseID_Weights[1];
    myRECOevent.Muon_LooseID_WeightDown = Muon_LooseID_Weights[2];
    if (electronJetPairs.size() > 0) { 
      std::vector<double> HEEP_SF = myHEEP.ScaleFactor(myRECOevent.myElectronCand->et(), myRECOevent.selectedElectronEta, m_era);
      std::vector<double> egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.myElectronCand->superCluster()->eta(), myRECOevent.selectedElectronPt, m_era);
      std::vector<double> egamma_SF_HLT = myEgammaEffi.HLT_ScaleFactor(myRECOevent.myElectronCand->superCluster()->eta(), myRECOevent.selectedElectronPt, m_era);
      if (fabs(myRECOevent.selectedElectronEta) > 1.6) myRECOevent.HEEP_SF_E = HEEP_SF[0];
      if (fabs(myRECOevent.selectedElectronEta) < 1.4) myRECOevent.HEEP_SF_B = HEEP_SF[0];
      myRECOevent.HEEP_SF = HEEP_SF[0];
      myRECOevent.egamma_SF = egamma_SF[0];
      myRECOevent.egamma_SF_HLT = egamma_SF_HLT[0];
      setEventWeight_FSB(iEvent, myRECOevent);
    }
//    if (electronJetPairs_noISO.size() > 0) { 
//      std::vector<double> HEEP_SF = myHEEP.ScaleFactor(myRECOevent.myElectronCand->et(), myRECOevent.selectedElectron_noISO_Eta, m_era);
//      std::vector<double> egamma_SF = myEgammaEffi.ScaleFactor(myRECOevent.myElectronCand_noISO->superCluster()->eta(), myRECOevent.selectedElectron_noISO_Pt, m_era);
//      if (fabs(myRECOevent.selectedElectron_noISO_Eta) > 1.6) myRECOevent.HEEP_SF_E_noISO = HEEP_SF[0];
//      if (fabs(myRECOevent.selectedElectron_noISO_Eta) < 1.4) myRECOevent.HEEP_SF_B_noISO = HEEP_SF[0];
//      myRECOevent.HEEP_SF_noISO = HEEP_SF[0];
//      myRECOevent.egamma_SF_noISO = egamma_SF[0];
//      setEventWeight_FSB_noISO(iEvent, myRECOevent);
//    }
  }

  std::cout << "EVENT PASSES FLAVOR SIDEBAND" << std::endl;
  return true;
}

bool cmsWRextension::preSelectBoostReco(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myRECOevent) {
  std::cout << "checking dR for orthogonality" << std::endl;
  if (myRECOevent.myResCandJets.size() == 1 && myRECOevent.resolvedANAMuons.size() > 1){
    double dR_jet1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myResCandJets[0]->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myResCandJets[0]->phi));
    std::cout << "dR_jet1_muon2: " << dR_jet1_muon2 << std::endl;
    if (dR_jet1_muon2 > 0.4) return false;
  }else if(myRECOevent.myResCandJets.size() > 1 && myRECOevent.resolvedANAMuons.size() > 1){
    double dR_jet1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myResCandJets[0]->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myResCandJets[0]->phi));
    double dR_jet2_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myResCandJets[1]->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myResCandJets[1]->phi));
    std::cout << "dR_jet1_muon2: " << dR_jet1_muon2 << std::endl;
    std::cout << "dR_jet2_muon2: " << dR_jet2_muon2 << std::endl;
    if (dR_jet1_muon2 > 0.4 && dR_jet2_muon2 > 0.4) return false;
  }/*else if(myRECOevent.resolvedANAMuons.size() < 2){
    std::cout << "myRECOevent.resolvedANAMuons.size(): " << myRECOevent.resolvedANAMuons.size() << std::endl;
    std::cout << "don't have 2 muons" <<std::endl;
    return false;
  }*/

  std::cout << "beginning preselection" << std::endl;
  muonSelection(iEvent, myRECOevent);
  electronSelection(iEvent, myRECOevent);
  jetSelection(iEvent, myRECOevent);
//  additionalElectrons(iEvent, myRECOevent, true, 0);

  if (myRECOevent.myMuonCand == NULL) { return false; }

  if( myRECOevent.myAddJetCandsHighPt.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<< std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  if( myRECOevent.myMuonCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO MUONS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myMuonCandsHighPt.size()<<" MUONS FOUND." << std::endl;
    return false;
  }
  if(myRECOevent.myMuonCandsHighPt[0]->tunePMuonBestTrack()->pt() < 60) return false;
  myRECOevent.cutProgress++;
  //BUILD PAIRS OF AK8 JETS WITH THE LEAD MUON
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs;
  TLorentzVector jetVec_temp = TLorentzVector();
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
    //if( ((*iJet)->p4() + (*iMuon)->p4()).mass() < 400) continue;
    //if (sqrt(deltaR2(*(*iJet),*(*iMuon)))<2.0) continue;
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;

    jetVec_temp.SetPtEtaPhiM( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->SDmass*(*iJet)->SDmassCorr );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp.X(),jetVec_temp.Y(),jetVec_temp.Z(),jetVec_temp.T());
 
//    double eventMass = ( jetVec + myRECOevent.myMuonCand->p4()*myRECOevent.leadBoostMuonScale[0] ).mass();

//    if( eventMass < 200 ) continue;

    muonJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }

  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_noLSF;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;

    jetVec_temp.SetPtEtaPhiM( (*iJet)->pT, (*iJet)->eta, (*iJet)->phi, (*iJet)->SDmass*(*iJet)->SDmassCorr );

    math::XYZTLorentzVector jetVec;
    jetVec.SetXYZT(jetVec_temp.X(),jetVec_temp.Y(),jetVec_temp.Z(),jetVec_temp.T());

    double eventMass = ( jetVec + myRECOevent.myMuonCand->p4() ).mass();

    muonJetPairs_noLSF.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }

  if( muonJetPairs.size() < 1 && muonJetPairs_noLSF.size() < 1) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET LEPTON PAIRS" <<std::endl;
    return false;
  }
  myRECOevent.cutProgress++;
  std::cout<<muonJetPairs.size()<<" Pairing CANDIDATES Selected from "<<myRECOevent.myAddJetCandsHighPt.size()<<" jets"<<std::endl;
  myRECOevent.myMuonJetPairs = muonJetPairs;
  myRECOevent.myMuonJetPairs_noLSF = muonJetPairs_noLSF;

  return true;
}
bool cmsWRextension::preSelectReco_Fast(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myRECOevent) {
  std::cout << "starting preSelectReco_Fast" << std::endl;
  if(myRECOevent.myResCandJets.size() > 0){
    std::cout << "WE HAVE ENOUGH RES CAND JETS" << std::endl;
  }
  if(myRECOevent.resolvedANAMuons.size() > 1){
    std::cout << "WE JAVE ENOUGH resolved Muons" << std::endl;
  }
  /*if (myRECOevent.myResCandJets.size() == 1 && myRECOevent.resolvedANAMuons.size() > 1){
    double dR_jet1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myResCandJets[0]->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myResCandJets[0]->phi));
    std::cout << "RES MUON1 with pT, eta, phi: " << myRECOevent.resolvedANAMuons[0]->pt() << "," << myRECOevent.resolvedANAMuons[0]->eta() << "," << myRECOevent.resolvedANAMuons[0]->phi() << std::endl;
    std::cout << "RES MUON2 with pT, eta, phi: " << myRECOevent.resolvedANAMuons[1]->pt() << "," << myRECOevent.resolvedANAMuons[1]->eta() << "," << myRECOevent.resolvedANAMuons[1]->phi() << std::endl;
    std::cout << "RES JET1 with pT, eta, phi: " << myRECOevent.myResCandJets[0]->pT << "," << myRECOevent.myResCandJets[0]->eta << "," << myRECOevent.myResCandJets[0]->phi << std::endl;    
    std::cout << "dR_jet1_muon2: " << dR_jet1_muon2 << std::endl;
    if (dR_jet1_muon2 > 0.4) return false;
  }else*/ if(myRECOevent.myResCandJets.size() > 1 && myRECOevent.resolvedANAMuons.size() > 1){
    double dR_jet1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myResCandJets[0]->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myResCandJets[0]->phi));
    double dR_jet2_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myResCandJets[1]->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myResCandJets[1]->phi));
    double dR_jet1_muon1 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[0]->eta(),myRECOevent.myResCandJets[0]->eta,myRECOevent.resolvedANAMuons[0]->phi(),myRECOevent.myResCandJets[0]->phi));
    double dR_jet2_muon1 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[0]->eta(),myRECOevent.myResCandJets[1]->eta,myRECOevent.resolvedANAMuons[0]->phi(),myRECOevent.myResCandJets[1]->phi));
    double dR_jet1_jet2 = sqrt(::wrTools::dR2(myRECOevent.myResCandJets[0]->eta,myRECOevent.myResCandJets[1]->eta,myRECOevent.myResCandJets[0]->phi,myRECOevent.myResCandJets[1]->phi));
    double dR_muon1_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[0]->eta(),myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.resolvedANAMuons[0]->phi(),myRECOevent.resolvedANAMuons[1]->phi()));
    std::cout << "dR_jet1_muon2: " << dR_jet1_muon2 << std::endl;
    std::cout << "dR_jet2_muon2: " << dR_jet2_muon2 << std::endl;
    if (dR_jet1_muon2 > 0.4 && dR_jet2_muon2 > 0.4 && dR_jet1_muon1 > 0.4 && dR_jet2_muon1 > 0.4 && dR_jet1_jet2 > 0.4 && dR_muon1_muon2 > 0.4) return false;
  }
  myRECOevent.cutProgress++;

  muonSelection(iEvent, myRECOevent);
  electronSelection(iEvent, myRECOevent);
  jetSelection(iEvent, myRECOevent);


  if( (myRECOevent.myAddJetCandsHighPt.size() < 1) && myRECOevent.myAddJetCandsHighPt_JECUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_JECDown.size() < 1 && myRECOevent.myAddJetCandsHighPt_JERUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_JERDown.size() < 1 && (myRECOevent.myAddJetCandsHighPt_noLSF.size() < 1) && myRECOevent.myAddJetCandsHighPt_noLSF_JECUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF_JECDown.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF_JERUp.size() < 1 && myRECOevent.myAddJetCandsHighPt_noLSF_JERDown.size() < 1) {
    std::cout<< "EVENT FAILS, NO JETS OVER 200 GEV WITHIN ACCEPTANCE. "<< std::endl;
    return false;
  }
//  myRECOevent.cutProgress++;
  if( myRECOevent.myMuonCandsHighPt.size() < 1) {
    std::cout<< "EVENT FAILS, NO MUONS OVER 200 GEV WITHIN ACCEPTANCE. "<<myRECOevent.myMuonCandsHighPt.size()<<" MUONS FOUND." << std::endl;
    return false;
  }
  if(myRECOevent.myMuonCandsHighPt[0]->tunePMuonBestTrack()->pt() < 60) return false;
//  myRECOevent.cutProgress++;
  //BUILD PAIRS OF AK8 JETS WITH THE LEAD MUON
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt.begin(); iJet != myRECOevent.myAddJetCandsHighPt.end(); iJet++) {
    std::cout << "Nominal: (*iJet)->phi: " << (*iJet)->phi << std::endl;
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_JECUp;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_JECUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_JECUp.end(); iJet++) {
    std::cout << "JECUp: (*iJet)->phi: " << (*iJet)->phi << std::endl;
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
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_noLSF;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF.end(); iJet++) {
    std::cout << "Nominal: (*iJet)->phi: " << (*iJet)->phi << std::endl;
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_noLSF.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_noLSF_JECUp;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF_JECUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF_JECUp.end(); iJet++) {
    std::cout << "JECUp: (*iJet)->phi: " << (*iJet)->phi << std::endl;
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_noLSF_JECUp.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_noLSF_JECDown;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF_JECDown.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF_JECDown.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_noLSF_JECDown.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_noLSF_JERUp;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF_JERUp.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF_JERUp.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_noLSF_JERUp.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>> muonJetPairs_noLSF_JERDown;
  for(std::vector<const baconhep::TAddJet*>::const_iterator iJet = myRECOevent.myAddJetCandsHighPt_noLSF_JERDown.begin(); iJet != myRECOevent.myAddJetCandsHighPt_noLSF_JERDown.end(); iJet++) {
    if(fabs(reco::deltaPhi((*iJet)->phi, myRECOevent.myMuonCand->phi())) < 2.0 ) continue;
    muonJetPairs_noLSF_JERDown.push_back(std::make_pair( (*iJet) , myRECOevent.myMuonCand ));

  }


  if( muonJetPairs.size() < 1 && muonJetPairs_JECUp.size() < 1 && muonJetPairs_JECDown.size() < 1 && muonJetPairs_JERUp.size() < 1 && muonJetPairs_JERDown.size() < 1 && muonJetPairs_noLSF.size() < 1 && muonJetPairs_noLSF_JECUp.size() < 1 && muonJetPairs_noLSF_JECDown.size() < 1 && muonJetPairs_noLSF_JERUp.size() < 1 && muonJetPairs_noLSF_JERDown.size() < 1) {
    std::cout<< "EVENT FAILS, NO CANDIDATE JET LEPTON PAIRS" <<std::endl;
    return false;
  }
//  myRECOevent.cutProgress++;
  std::cout<<muonJetPairs.size()<<" Pairing CANDIDATES Selected from "<<myRECOevent.myAddJetCandsHighPt.size()<<" jets"<<std::endl;
  myRECOevent.myMuonJetPairs = muonJetPairs;
  myRECOevent.myMuonJetPairs_JECUp = muonJetPairs_JECUp;
  myRECOevent.myMuonJetPairs_JECDown = muonJetPairs_JECDown;
  myRECOevent.myMuonJetPairs_JERUp = muonJetPairs_JERUp;
  myRECOevent.myMuonJetPairs_JERDown = muonJetPairs_JERDown;  
  myRECOevent.myMuonJetPairs_noLSF = muonJetPairs_noLSF;
  myRECOevent.myMuonJetPairs_noLSF_JECUp = muonJetPairs_noLSF_JECUp;
  myRECOevent.myMuonJetPairs_noLSF_JECDown = muonJetPairs_noLSF_JECDown;
  myRECOevent.myMuonJetPairs_noLSF_JERUp = muonJetPairs_noLSF_JERUp;
  myRECOevent.myMuonJetPairs_noLSF_JERDown = muonJetPairs_noLSF_JERDown;

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
int cmsWRextension::subLeadingMuonZMass(const edm::Event& iEvent, eventBits& myEvent, bool ZPeak, bool useResMu) {  //THIS SELECTION IS A SIDEBAND BASED OF THE MUON FLAVOR SELECTION ONLY
  //CHECK IF WE HAVE A SUBLEADING MUON
  std::cout << "Inside subLeadingMuonZMass" << std::endl;
//  const pat::Muon* subleadMuon = myEvent.resolvedANAMuons[1];
//  std::cout << "Have 2nd muon" << std::endl;
  const pat::Muon* subleadMuon;
  const pat::Muon* selMuon;
  double leadMuScale = 0.;
  double secondMuScale = 0.;
  if (useResMu) {
    if(myEvent.resolvedANAMuons.size() < 2) return false;
    if(myEvent.resolvedANAMuons[0] == NULL) return false;
    if(myEvent.resolvedANAMuons[1] == NULL) return false;
    subleadMuon = myEvent.resolvedANAMuons[1];
    selMuon     = myEvent.resolvedANAMuons[0];
    secondMuScale = myEvent.secondResMuonScale[0];
    leadMuScale   = myEvent.leadResMuonScale[0];
  } else { 
    if(myEvent.mySubleadMuon == NULL) return false;
    if(myEvent.myMuonCand    == NULL) return false;
    subleadMuon = myEvent.mySubleadMuon;
    selMuon     = myEvent.myMuonCand;
    secondMuScale = myEvent.secondBoostMuonScale[0];
    leadMuScale   = myEvent.leadBoostMuonScale[0];
  }
  if (!useResMu) {
    const baconhep::TAddJet*  selJet;
    if(ZPeak){
      selJet  = myEvent.myMuonJetPairs_noLSF[0].first;
    }else{
      selJet  = myEvent.myMuonJetPairs[0].first;
    }

    myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  }
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi()));
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4()*secondMuScale + selMuon->p4()*leadMuScale).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonPt		  = subleadMuon->pt();
  myEvent.subleadMuonEt           = subleadMuon->et();
  myEvent.subleadMuonEta           = subleadMuon->eta();
  myEvent.subleadMuonPhi           = subleadMuon->phi();

  if (!useResMu) {
    //double dPhi = fabs(::wrTools::dPhi(myEvent.lsfLeptonPhi, myEvent.subleadMuonPhi));

    double dRlsfLep_subleadMuon = sqrt( ::wrTools::dR2(myEvent.lsfLeptonEta, myEvent.subleadMuonEta, myEvent.lsfLeptonPhi, myEvent.subleadMuonPhi ));
//    if (dRlsfLep_subleadMuon > 2*ROOT::Math::Pi()) dRlsfLep_subleadMuon -= 2*ROOT::Math::Pi();
    myEvent.mydRlsfLep_subleadMuon = dRlsfLep_subleadMuon;
  }
//  subleadMuon_selMuondR
//  double dPhi;
//  dPhi = fabs(::wrTools::dPhi(subleadMuon->phi(), myEvent.myMuonCand->phi()));

  double subleadMuon_selMuondR = sqrt( ::wrTools::dR2(selMuon->eta(), myEvent.subleadMuonEta, selMuon->phi(), myEvent.subleadMuonPhi ));
 // if (subleadMuon_selMuondR > 2*ROOT::Math::Pi()) subleadMuon_selMuondR -= 2*ROOT::Math::Pi();
  myEvent.subleadMuon_selMuondR = subleadMuon_selMuondR;

  if(useResMu){
    if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){
       return 1;
    }else if(myEvent.subleadMuon_selMuonMass > 200){
      return 2;
    }else{
      return 0;
    }
  }else{
    if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){
      return 1;
    }else if(myEvent.subleadMuon_selMuonMass > 200){
      return 2;
    }else{
      return 0;
    }
  }
}
int cmsWRextension::subLeadingMuonZMass_Nominal(const edm::Event& iEvent, eventBits& myEvent, bool ZPeak) {  //THIS SELECTION IS A SIDEBAND BASED OF THE MUON FLAVOR SELECTION ONLY
  //CHECK IF WE HAVE A SUBLEADING MUON
  if(myEvent.mySubleadMuon == 0) return 0;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const baconhep::TAddJet*  selJet;
  if(ZPeak){
    selJet  = myEvent.myMuonJetPairs_noLSF[0].first;
  }else{
    selJet  = myEvent.myMuonJetPairs[0].first;
  }

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi())); 
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();
  myEvent.subleadMuonEta           = subleadMuon->eta();
  myEvent.subleadMuonPhi           = subleadMuon->phi();

  double dPhi = fabs(::wrTools::dPhi(myEvent.lsfLeptonPhi, myEvent.subleadMuonPhi));

  double dRlsfLep_subleadMuon = sqrt(( pow((myEvent.lsfLeptonEta - myEvent.subleadMuonEta), 2.0) + pow( dPhi, 2.0) )); 
//  if (dRlsfLep_subleadMuon > 2*ROOT::Math::Pi()) dRlsfLep_subleadMuon -= 2*ROOT::Math::Pi();
  myEvent.mydRlsfLep_subleadMuon = dRlsfLep_subleadMuon;

//  subleadMuon_selMuondR
  
  dPhi = fabs(::wrTools::dPhi(subleadMuon->phi(), myEvent.myMuonCand->phi()));

  double subleadMuon_selMuondR = sqrt(( pow((myEvent.subleadMuonEta - myEvent.myMuonCand->eta()), 2.0) + pow( dPhi, 2.0) )); 
 // if (subleadMuon_selMuondR > 2*ROOT::Math::Pi()) subleadMuon_selMuondR -= 2*ROOT::Math::Pi();
  myEvent.subleadMuon_selMuondR = subleadMuon_selMuondR;

  if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){ 
    return 1;
  }else if(myEvent.subleadMuon_selMuonMass > 200){
    return 2;
  }else{
    return 0;
  }
}
int cmsWRextension::subLeadingMuonZMass_JECUp(const edm::Event& iEvent, eventBits& myEvent, bool ZPeak) {
  if(myEvent.mySubleadMuon == 0) return 0;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const baconhep::TAddJet*  selJet;
  if(ZPeak){
    selJet  = myEvent.myMuonJetPairs_noLSF_JECUp[0].first;
  }else{
    selJet  = myEvent.myMuonJetPairs_JECUp[0].first;
  }

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi()));
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();

  if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){
    return 1;
  }else if(myEvent.subleadMuon_selMuonMass > 200){
    return 2;
  }else{
    return 0;
  }
}
int cmsWRextension::subLeadingMuonZMass_JECDown(const edm::Event& iEvent, eventBits& myEvent, bool ZPeak) {
  if(myEvent.mySubleadMuon == 0) return 0;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const baconhep::TAddJet*  selJet;
  if(ZPeak){
    selJet  = myEvent.myMuonJetPairs_noLSF_JECDown[0].first;
  }else{
    selJet  = myEvent.myMuonJetPairs_JECDown[0].first;
  }

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi()));
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();

  if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){
    return 1;
  }else if(myEvent.subleadMuon_selMuonMass > 200){
    return 2;
  }else{
    return 0;
  }
}
int cmsWRextension::subLeadingMuonZMass_JERUp(const edm::Event& iEvent, eventBits& myEvent, bool ZPeak) {
  if(myEvent.mySubleadMuon == 0) return 0;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const baconhep::TAddJet*  selJet;
  if(ZPeak){
    selJet  = myEvent.myMuonJetPairs_noLSF_JERUp[0].first;
  }else{
    selJet  = myEvent.myMuonJetPairs_JERUp[0].first;
  }

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi()));
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();

  if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){
    return 1;
  }else if(myEvent.subleadMuon_selMuonMass > 200 && myEvent.subleadMuon_selMuonMass > 50){
    return 2;
  }else{
    return 0;
  }
}
int cmsWRextension::subLeadingMuonZMass_JERDown(const edm::Event& iEvent, eventBits& myEvent, bool ZPeak) {
  if(myEvent.mySubleadMuon == 0) return 0;

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Muon* selMuon     = myEvent.myMuonCand;
  const baconhep::TAddJet*  selJet;
  if(ZPeak){
    selJet  = myEvent.myMuonJetPairs_noLSF_JERDown[0].first;
  }else{
    selJet  = myEvent.myMuonJetPairs_JERDown[0].first;
  }

  myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
  myEvent.subleadMuon_selMuondPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selMuon->phi()));
  myEvent.subleadMuon_selMuonMass = (subleadMuon->p4() + selMuon->p4()).mass();
  myEvent.subleadMuon_selMuonPt   = (subleadMuon->p4() + selMuon->p4()).pt();
  myEvent.subleadMuonEt           = subleadMuon->et();

  if(myEvent.subleadMuon_selMuonMass < 150 && myEvent.subleadMuon_selMuonMass > 60){
    return 1;
  }else if(myEvent.subleadMuon_selMuonMass > 200){
    return 2;
  }else{
    return 0;
  }
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband(const edm::Event& iEvent, eventBits& myEvent, bool useResLeps) {
  std::cout << "Inside subLeadingMuonZMass_FlavorSideband" << std::endl;
  const pat::Muon* subleadMuon = NULL;
  const pat::Electron* selEl   = NULL;
  double leadMuScale = 0.;
  if (useResLeps) {
    if ( myEvent.resFSBMuon == NULL ) return false;
    if ( myEvent.resFSBElec == NULL ) return false;
    subleadMuon = myEvent.resFSBMuon;
    selEl       = myEvent.resFSBElec;
    leadMuScale   = myEvent.leadFSBMuonScale[0];
  } else {
    if ( myEvent.myElectronCand == NULL ) return false;
    if ( myEvent.mySubleadMuon  == NULL ) return false;
    selEl = myEvent.myElectronCand;
    subleadMuon       = myEvent.mySubleadMuon;
    leadMuScale   = myEvent.secondBoostMuonScale[0];
  }
  if ( subleadMuon == NULL ) return false;
  if ( selEl       == NULL ) return false;

  auto corrP4  = selEl->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = selEl->p4()*selEl->userFloat("ecalTrkEnergyPostCorr") / selEl->energy();  
    myEvent.subleadMuon_selElectronMass = (subleadMuon->p4()*leadMuScale + corrP4).mass();
  }*/

  myEvent.subleadMuon_selElectronMass = (subleadMuon->p4()*leadMuScale + corrP4).mass();

  if(myEvent.subleadMuon_selElectronMass < 200)  return true;


  return false;
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband_Nominal(const edm::Event& iEvent, eventBits& myEvent) {

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Electron* selEl;

  selEl   = myEvent.myElectronCand;

  auto corrP4  = selEl->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = selEl->p4()*selEl->userFloat("ecalTrkEnergyPostCorr") / selEl->energy();
    double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();
  }*/


  double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();

  if(subleadMuon_selElectronMass < 200){
    return true;
  }else{
    return false;
  }
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband_JECUp(const edm::Event& iEvent, eventBits& myEvent) {

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Electron* selEl;

  selEl   = myEvent.myElectronCand;

  auto corrP4  = selEl->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = selEl->p4()*selEl->userFloat("ecalTrkEnergyPostCorr") / selEl->energy();
    double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();
  }*/

  double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();

  if(subleadMuon_selElectronMass < 200){
    return true;
  }else{
    return false;
  }
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband_JECDown(const edm::Event& iEvent, eventBits& myEvent) {

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Electron* selEl;

  selEl   = myEvent.myElectronCand;

  auto corrP4  = selEl->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = selEl->p4()*selEl->userFloat("ecalTrkEnergyPostCorr") / selEl->energy();
    double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();
  }*/

  double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();

  if(subleadMuon_selElectronMass < 200){
    return true;
  }else{
    return false;
  }
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband_JERUp(const edm::Event& iEvent, eventBits& myEvent) {

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Electron* selEl;

  selEl   = myEvent.myElectronCand;

  auto corrP4  = selEl->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = selEl->p4()*selEl->userFloat("ecalTrkEnergyPostCorr") / selEl->energy();
    double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();
  }*/

  double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();

  if(subleadMuon_selElectronMass < 200){
    return true;
  }else{
    return false;
  }
}
bool cmsWRextension::subLeadingMuonZMass_FlavorSideband_JERDown(const edm::Event& iEvent, eventBits& myEvent) {

  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
  const pat::Electron* selEl;

  selEl   = myEvent.myElectronCand;

  auto corrP4  = selEl->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = selEl->p4()*selEl->userFloat("ecalTrkEnergyPostCorr") / selEl->energy();
    double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();
  }*/

  double subleadMuon_selElectronMass = (subleadMuon->p4()*myEvent.secondBoostMuonScale[0] + corrP4).mass();

  if(subleadMuon_selElectronMass < 200){
    return true;
  }else{
    return false;
  }
}
//bool cmsWRextension::subLeadingMuonZMass_FlavorSideband(const edm::Event& iEvent, eventBits& myEvent, bool nonISO) {  //THIS SELECTION IS A SIDEBAND BASED OF THE MUON FLAVOR SELECTION ONLY
//  //CHECK IF WE HAVE A SUBLEADING MUON
//  std::cout << "FSB ZMASS" << std::endl;
//  if(myEvent.mySubleadMuon == 0) return true;
//
//  const pat::Muon* subleadMuon = myEvent.mySubleadMuon;
//  std::cout << "SUBLEAD MUON SET" << std::endl;
//  const pat::Electron* selEl;
//  const baconhep::TAddJet*  selJet;
//  std::cout << "SUBLEAD MUON SET" << std::endl;
//
//  if (!nonISO) {
//    std::cout << "ISO ZMASS" << std::endl;
//    selEl   = myEvent.myElectronCand;
//    selJet  = myEvent.myElectronJetPairs[0].first;
//    if (selEl == 0)
//      std::cout << "CRAP!" << std::endl;
//    myEvent.subleadMuon_selJetdPhi  = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
//    myEvent.subleadMuon_selElectronPhi = fabs(reco::deltaPhi(subleadMuon->phi(),selEl->phi()));
//    myEvent.subleadMuon_selElectronMass = (subleadMuon->p4() + selEl->p4()).mass();
//    myEvent.subleadMuon_selElectronPt   = (subleadMuon->p4() + selEl->p4()).pt();
//  } else {
//    std::cout << "NONISO ZMASS" << std::endl;
//    selEl   = myEvent.myElectronCand_noISO;
//    selJet  = myEvent.myElectronJetPairs_noISO[0].first;
//    if (selEl == 0)
//      std::cout << "CRAP!" << std::endl;
//    myEvent.subleadMuon_selJetdPhi_EleNonISO      = fabs(reco::deltaPhi(subleadMuon->phi(),selJet->phi));
//    myEvent.subleadMuon_selElectronPhi_EleNonISO  = fabs(reco::deltaPhi(subleadMuon->phi(),selEl->phi()));
//    myEvent.subleadMuon_selElectronMass_EleNonISO = (subleadMuon->p4() + selEl->p4()).mass();
//    myEvent.subleadMuon_selElectronPt_EleNonISO   = (subleadMuon->p4() + selEl->p4()).pt();
//  }
//
//  myEvent.subleadMuonEt           = subleadMuon->et();
//
//  if(nonISO)
//    if (myEvent.subleadMuon_selElectronMass_EleNonISO < 200 && myEvent.subleadMuon_selMuonMass > 50)
//      return true;
//  if(!nonISO)
//    if (myEvent.subleadMuon_selElectronMass < 200 && myEvent.subleadMuon_selMuonMass > 50) 
//      return true;
//  return false;
//}
//CHECK ADDITIONAL MUONS
bool cmsWRextension::additionalMuons(const edm::Event& iEvent, eventBits& myEvent, bool flavorSideband, bool ZPeak, int JetCorrectionRegion, bool ISO) {
  std::cout << "Round-up of the rag-tag muons" << std::endl;
  edm::Handle<std::vector<pat::Muon>> regMuons;
  iEvent.getByToken(m_regMuonToken, regMuons);
  std::vector<const pat::Muon*> allMuons;

  double jetPhi = 0;
  double jetEta = 0;

  for(std::vector<pat::Muon>::const_iterator iMuon = regMuons->begin(); iMuon != regMuons->end(); iMuon++) {
    if ( iMuon->pt() < 53 || fabs(iMuon->eta()) > 2.4) continue;  //10 GeV is designed to capture slow muons from Z->MUMU
    if ( ! iMuon->isLooseMuon() ) continue;  //Loose MuonID
    std::cout << "Additional Muon passes Loose ID" << std::endl;
    std::cout << "additional Muon pT: " << iMuon->pt() << std::endl;
    if(flavorSideband==true){
      double muPhi  = iMuon->phi();
      double muEta  = iMuon->eta();
//        double jetPhi = 0;
//        double jetEta = 0;
        if(JetCorrectionRegion == 0){
	  if(ISO){
            jetPhi = myEvent.myElectronJetPairs[0].first->phi;
            jetEta = myEvent.myElectronJetPairs[0].first->eta;
	  }else{
            jetPhi = myEvent.myElectronJetPairs_noISO[0].first->phi;
            jetEta = myEvent.myElectronJetPairs_noISO[0].first->eta;
	  }
        }else if(JetCorrectionRegion == 1){
            jetPhi = myEvent.myElectronJetPairs_JECUp[0].first->phi;
            jetEta = myEvent.myElectronJetPairs_JECUp[0].first->eta;
        }else if(JetCorrectionRegion == 2){
            jetPhi = myEvent.myElectronJetPairs_JECDown[0].first->phi;
            jetEta = myEvent.myElectronJetPairs_JECDown[0].first->eta;
        }else if(JetCorrectionRegion == 3){
            jetPhi = myEvent.myElectronJetPairs_JERUp[0].first->phi;
            jetEta = myEvent.myElectronJetPairs_JERUp[0].first->eta;
        }else if(JetCorrectionRegion == 4){
            jetPhi = myEvent.myElectronJetPairs_JERDown[0].first->phi;
            jetEta = myEvent.myElectronJetPairs_JERDown[0].first->eta;
        }
        //double dPhi = fabs(::wrTools::dPhi(jetPhi, muPhi));
        double dR = sqrt(::wrTools::dR2(jetEta, muEta, jetPhi, muPhi));
        std::cout << "abs(dR): " << abs(dR) << std::endl;

        if(abs(dR) > 0.8) continue;
    }else{
      if(ZPeak==true){
	std::cout << "checking muon-jet dR in ZPeak" << std::endl;
      	double muPhi  = iMuon->phi();
      	double muEta  = iMuon->eta();
//        double jetPhi = 0;
//        double jetEta = 0;
        if(JetCorrectionRegion == 0){
      	  jetPhi = myEvent.myMuonJetPairs_noLSF[0].first->phi;
      	  jetEta = myEvent.myMuonJetPairs_noLSF[0].first->eta;
	}else if(JetCorrectionRegion == 1){
          jetPhi = myEvent.myMuonJetPairs_noLSF_JECUp[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_noLSF_JECUp[0].first->eta;
        }else if(JetCorrectionRegion == 2){
          jetPhi = myEvent.myMuonJetPairs_noLSF_JECDown[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_noLSF_JECDown[0].first->eta;
        }else if(JetCorrectionRegion == 3){
          jetPhi = myEvent.myMuonJetPairs_noLSF_JERUp[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_noLSF_JERUp[0].first->eta;
        }else if(JetCorrectionRegion == 4){
          jetPhi = myEvent.myMuonJetPairs_noLSF_JERDown[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_noLSF_JERDown[0].first->eta;
        }
        //double dPhi = fabs(::wrTools::dPhi(jetPhi, muPhi));
        double dR = sqrt(::wrTools::dR2(jetEta, muEta, jetPhi, muPhi));
	std::cout << "abs(dR): " << abs(dR) << std::endl;

      	//if(abs(dR) > 0.8) continue;
      }
      else{
        double muPhi  = iMuon->phi();
        double muEta  = iMuon->eta();
//        double jetPhi = 0;
//        double jetEta = 0;
        if(JetCorrectionRegion == 0){
          jetPhi = myEvent.myMuonJetPairs[0].first->phi;
          jetEta = myEvent.myMuonJetPairs[0].first->eta;
	}else if(JetCorrectionRegion == 1){
          jetPhi = myEvent.myMuonJetPairs_JECUp[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_JECUp[0].first->eta;
        }else if(JetCorrectionRegion == 2){
          jetPhi = myEvent.myMuonJetPairs_JECDown[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_JECDown[0].first->eta;
	}else if(JetCorrectionRegion == 3){
          jetPhi = myEvent.myMuonJetPairs_JERUp[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_JERUp[0].first->eta;
        }else if(JetCorrectionRegion == 4){
          jetPhi = myEvent.myMuonJetPairs_JERDown[0].first->phi;
          jetEta = myEvent.myMuonJetPairs_JERDown[0].first->eta;
        }
        //double dPhi = fabs(::wrTools::dPhi(jetPhi, muPhi));
        double dR = sqrt(::wrTools::dR2(jetEta, muEta, jetPhi, muPhi));

        if(abs(dR) > 0.8){
	  const pat::Muon* selMuon     = myEvent.myMuonCand;
          double subleadMuon_selMuonMass = (iMuon->p4() + selMuon->p4()).mass();
          if(subleadMuon_selMuonMass < 150 && subleadMuon_selMuonMass > 60)return false;
	  continue;
	}

      }
    }
    allMuons.push_back(&(*iMuon));
  }
  myEvent.muons10 = allMuons.size();
  std::cout << "Number of possible additional muons: " << myEvent.muons10 << std::endl;
  if (myEvent.muons10 > 1) {
    std::sort(allMuons.begin(),allMuons.end(),::wrTools::compareEtCandidatePointer); 
  }
  if (myEvent.muons10 > 0) {
    std::cout << "filling myMuonCands" << std::endl;
    myEvent.myMuonCands = allMuons; 
  }
  if(myEvent.muons10 < 1) return false;  //The leading muon should also pass these cuts, so an additional muon would mean 2 or more
//  if(myEvent.muons10 > 2) return false;  //too many muons!

  if(flavorSideband==true) {
    int nMuonOutSideJet = 0;
    myEvent.mySubleadMuon = allMuons.at(0);
    for(std::vector<const pat::Muon*>::iterator iMuon = myEvent.myMuonCandsHighPt.begin(); iMuon != myEvent.myMuonCandsHighPt.end(); iMuon++){
        double dR = sqrt(::wrTools::dR2(jetEta, (*iMuon)->eta(), jetPhi, (*iMuon)->phi()));
	if(dR > 0.8) nMuonOutSideJet++;
    }
    myEvent.nMuonOutSideJetFSB = nMuonOutSideJet;
    if (m_doGen) {
      if(myEvent.genSecondMuon != NULL) 
        myEvent.dRmuon2 = sqrt(deltaR2(*(myEvent.mySubleadMuon),*(myEvent.genSecondMuon)));
    }
  }else{
  //IF WE HAVE ADDITION MUONS, WE SHOULD SEE WHICH IS THE LEADING MUON WHICH ISN'T THE MAIN CANDIDATE
   if(ZPeak==false){
    for(std::vector<const pat::Muon*>::iterator iMuon = myEvent.myMuonCands.begin(); iMuon != myEvent.myMuonCands.end(); iMuon++) {
      if(sqrt(::wrTools::dR2((*iMuon)->eta(),(myEvent.myMuonCand)->eta(),(*iMuon)->phi(),(myEvent.myMuonCand)->phi())) > 0.02) {
        myEvent.mySubleadMuon = *iMuon;
	if(ZPeak==false){
    	  double muPhi  = myEvent.mySubleadMuon->phi();
    	  double muEta  = myEvent.mySubleadMuon->eta();
          double jetPhi = 0;
          double jetEta = 0;
          if(JetCorrectionRegion == 0){
            jetPhi = myEvent.myMuonJetPairs[0].first->phi;
            jetEta = myEvent.myMuonJetPairs[0].first->eta;
          }else if(JetCorrectionRegion == 1){
            jetPhi = myEvent.myMuonJetPairs_JECUp[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JECUp[0].first->eta;
          }else if(JetCorrectionRegion == 2){
            jetPhi = myEvent.myMuonJetPairs_JECDown[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JECDown[0].first->eta;
          }else if(JetCorrectionRegion == 3){
            jetPhi = myEvent.myMuonJetPairs_JERUp[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JERUp[0].first->eta;
          }else if(JetCorrectionRegion == 4){
            jetPhi = myEvent.myMuonJetPairs_JERDown[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JERDown[0].first->eta;
          }
          //double dPhi = fabs(::wrTools::dPhi(jetPhi, muPhi));
          double dR = sqrt( ::wrTools::dR2( jetEta, muEta, jetPhi, muPhi));

          myEvent.secondRECOMuonRECOjetDR = dR;
	}
        if (m_doGen) {
          if(myEvent.genSecondMuon != NULL) 
            myEvent.dRmuon2 = sqrt(deltaR2(*(myEvent.mySubleadMuon),*(myEvent.genSecondMuon)));
        }
        break;
      }
    }
   }else{
    for(std::vector<const pat::Muon*>::iterator iMuon = myEvent.myMuonCands.begin(); iMuon != myEvent.myMuonCands.end(); iMuon++) {
      if(sqrt(::wrTools::dR2((*iMuon)->eta(),(myEvent.myMuonCand)->eta(),(*iMuon)->phi(),(myEvent.myMuonCand)->phi())) > 0.02) {
        const pat::Muon* selMuon     = myEvent.myMuonCand;
        double subleadMuon_selMuonMass = ((*iMuon)->p4() + selMuon->p4()).mass();
	if(subleadMuon_selMuonMass > 150 || subleadMuon_selMuonMass < 60)continue;
        myEvent.mySubleadMuon = *iMuon;
        if(ZPeak==false){
          double muPhi  = myEvent.mySubleadMuon->phi();
          double muEta  = myEvent.mySubleadMuon->eta();
          double jetPhi = 0;
          double jetEta = 0;
          if(JetCorrectionRegion == 0){
            jetPhi = myEvent.myMuonJetPairs[0].first->phi;
            jetEta = myEvent.myMuonJetPairs[0].first->eta;
          }else if(JetCorrectionRegion == 1){
            jetPhi = myEvent.myMuonJetPairs_JECUp[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JECUp[0].first->eta;
          }else if(JetCorrectionRegion == 2){
            jetPhi = myEvent.myMuonJetPairs_JECDown[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JECDown[0].first->eta;
          }else if(JetCorrectionRegion == 3){
            jetPhi = myEvent.myMuonJetPairs_JERUp[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JERUp[0].first->eta;
          }else if(JetCorrectionRegion == 4){
            jetPhi = myEvent.myMuonJetPairs_JERDown[0].first->phi;
            jetEta = myEvent.myMuonJetPairs_JERDown[0].first->eta;
          }
          double dR = sqrt( ::wrTools::dR2( jetEta, muEta, jetPhi, muPhi));

          myEvent.secondRECOMuonRECOjetDR = dR;
        }
        if (m_doGen) {
          if(myEvent.genSecondMuon != NULL)
            myEvent.dRmuon2 = sqrt(deltaR2(*(myEvent.mySubleadMuon),*(myEvent.genSecondMuon)));
        }
        break;
      }
    }
   }
  }

//Study of 3-object invariant mass
/*  if(flavorSideband==false && myEvent.mySubleadMuon != NULL){
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

  }*/

  if(myEvent.mySubleadMuon !=0){
    if(m_isMC){
      myEvent.secondBoostMuonScale = myMuons.RochesterMethod_MCSmear(myEvent.mySubleadMuon, m_era);
    }
    else{
      myEvent.secondBoostMuonScale = myMuons.RochesterMethod_DataScale(myEvent.mySubleadMuon, m_era);
    }

  }

  if(myEvent.mySubleadMuon == 0)return false;

  return true;
}
//CHECK ADDITIONAL ELECTRONS
//   0                                              MinPtCut_0    0        769.976297        1
//   1                              GsfEleSCEtaMultiRangeCut_0    0        0.783060          1
//   2                                   GsfEleDEtaInSeedCut_0    0        0.000538          1
//   3                                       GsfEleDPhiInCut_0    0        0.001986          1
//   4                         GsfEleFull5x5SigmaIEtaIEtaCut_0    0        0.008978          1
//   5                               GsfEleHadronicOverEMCut_0    0        0.053140          1
//   6                        GsfEleEInverseMinusPInverseCut_0    0        0.000155          1
//   7                                 GsfEleEffAreaPFIsoCut_0    0        0.738612          0
//   8                               GsfEleConversionVetoCut_0    0        1.000000          1
//   9                                  GsfEleMissingHitsCut_0    0        1.000000          1

bool cmsWRextension::additionalElectrons(const edm::Event& iEvent, eventBits& myEvent, bool isFSB, int JetCorrectionRegion) {
  std::cout << "Sorting non-lead electrons within and without selected jet" << std::endl;

  const baconhep::TAddJet* selJet;

  if(!isFSB){
    if(JetCorrectionRegion == 0){
      selJet = myEvent.myMuonJetPairs[0].first;
    }else if(JetCorrectionRegion == 1){
      selJet = myEvent.myMuonJetPairs_JECUp[0].first;
    }else if(JetCorrectionRegion == 2){
      selJet = myEvent.myMuonJetPairs_JECDown[0].first;
    }else if(JetCorrectionRegion == 3){
      selJet = myEvent.myMuonJetPairs_JERUp[0].first;
    }else if(JetCorrectionRegion == 4){
      selJet = myEvent.myMuonJetPairs_JERDown[0].first;
    }
  }else{
    if(JetCorrectionRegion == 0){
      selJet = myEvent.myElectronJetPairs[0].first;
    }else if(JetCorrectionRegion == 1){
      selJet = myEvent.myElectronJetPairs_JECUp[0].first;
    }else if(JetCorrectionRegion == 2){
      selJet = myEvent.myElectronJetPairs_JECDown[0].first;
    }else if(JetCorrectionRegion == 3){
      selJet = myEvent.myElectronJetPairs_JERUp[0].first;
    }else if(JetCorrectionRegion == 4){
      selJet = myEvent.myElectronJetPairs_JERDown[0].first;
    }
  }

  edm::Handle<std::vector<pat::Electron>> electrons;
  iEvent.getByToken(m_electronToken2, electrons);
  const pat::ElectronCollection *eleCol = electrons.product();

  std::vector<const pat::Electron*> secondElectronCands;

  edm::Handle<edm::ValueMap<vid::CutFlowResult> > ele_id_cutflow_data;
  iEvent.getByToken(m_eleIdFullInfoMapToken,ele_id_cutflow_data);

  std::cout << ele_id_cutflow_data->size() << std::endl;
  std::cout << electrons->size() << std::endl;
  for(std::vector<pat::Electron>::const_iterator iElec = eleCol->begin(); iElec != eleCol->end(); iElec++) {
    edm::RefToBase<reco::GsfElectron> eleBaseRef( edm::Ref<pat::ElectronCollection>(electrons, iElec - eleCol->begin()) );

    std::cout << "iElec->pt(): " << iElec->pt() << std::endl;
    if(iElec->pt() < 53) continue;    
    if(fabs(iElec->eta()) > 2.4) continue;

    vid::CutFlowResult fullCutFlowData = (*ele_id_cutflow_data)[eleBaseRef];
    vid::CutFlowResult maskedCutFlowData = fullCutFlowData.getCutFlowResultMasking(7); //we want all but ISO
    ::wrTools::printCutFlowResult(maskedCutFlowData);
  
    if (! maskedCutFlowData.cutFlowPassed()) continue;   

    if (sqrt(::wrTools::dR2(selJet->eta, iElec->eta(), selJet->phi, iElec->phi())) < 0.8 ) {  //within jet 
      std::cout << "found second electron cand" << std::endl;
      secondElectronCands.push_back(&(*(iElec))); 
    }
  }
  myEvent.nSecondElectronCands = secondElectronCands.size();

  if (secondElectronCands.size() < 1) return false;
  std::sort(secondElectronCands.begin(), secondElectronCands.end(), ::wrTools::compareEtCandidatePointer);
  myEvent.secondElectronCand = secondElectronCands[0];
  myEvent.secondElecJetDR = sqrt(::wrTools::dR2(selJet->eta, secondElectronCands[0]->eta(), selJet->phi, secondElectronCands[0]->phi()));
  myEvent.secondElecPt = secondElectronCands[0]->pt();
  
  return true;
}
bool cmsWRextension::resolvedFSBleptonSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES FSB LEPTON SELECTION" << std::endl;
  std::vector<const pat::Electron*> resElecs;
  std::vector<const pat::Muon*> resMus;

  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);

  edm::Handle<std::vector<pat::Electron>> highElectrons;
  iEvent.getByToken(m_highElectronToken, highElectrons);

  //LOOK FOR MUONS
  for(std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {

   if( fabs(iMuon->eta()) > 2.4) continue;
   if(( iMuon->isHighPtMuon(*myEvent.PVertex) && iMuon->tunePMuonBestTrack()->pt() > 53)  && (iMuon->isolationR03().sumPt/iMuon->pt() < 0.1)) {      //2017
      std::cout<<"RES FSB MUON CAND WITH PT,ETA,PHI: "<<iMuon->pt()<<","<<iMuon->eta()<<","<<iMuon->phi()<<std::endl;
      std::cout<<"RES FSB MUON CAND WITH Iso: " << iMuon->isolationR03().sumPt/iMuon->pt() << std::endl;
     
      resMus.push_back(&(*iMuon));
    }
  }
  for(std::vector<pat::Electron>::const_iterator iElec = highElectrons->begin(); iElec != highElectrons->end(); iElec++) {
    //PHASE SPACE CUTS
    if( fabs(iElec->eta()) > 2.4) continue; 
    if( iElec->pt() < 53) continue;
    //HEEP ID
    const vid::CutFlowResult* vidResult =  iElec->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");
    if(vidResult == NULL) {
      std::cout << "ERROR CANNOT FIND ELECTRON VID RESULTS" << std::endl;
      return false;
    }
    
    //how to check if everything passed:
    const bool heepIDVID = vidResult->cutFlowPassed();

    if (!heepIDVID) continue;

    resElecs.push_back(&(*iElec));
  }
  if (resElecs.size() < 1) return false;
  if (resMus.size()   < 1) return false;

  if (resElecs.size() > 1) std::sort(resElecs.begin(), resElecs.end(), ::wrTools::compareEtCandidatePointer);
  if (resMus.size()   > 1) std::sort(resMus.begin(),   resMus.end(),   ::wrTools::compareEtCandidatePointer); 

  if(m_isMC){
    myEvent.leadFSBMuonScale = myMuons.RochesterMethod_MCSmear(resMus[0], m_era);
  }
  else{
    myEvent.leadFSBMuonScale = myMuons.RochesterMethod_DataScale(resMus[0], m_era);
  }  

  //NOW WE TAKE THE LEAD ELECTRON AND MUON AS OUR LEADS
  const pat::Electron* leadElec = resElecs[0];
  const pat::Muon*     leadMuon = resMus[0];

  myEvent.resFSBElec_pt  = leadElec->pt();
  myEvent.resFSBElec_phi = leadElec->phi();
  myEvent.resFSBElec_eta = leadElec->eta();

  myEvent.resFSBMuon_pt  = leadMuon->tunePMuonBestTrack()->pt();
  myEvent.resFSBMuon_p  = leadMuon->tunePMuonBestTrack()->p();
  myEvent.resFSBMuon_phi = leadMuon->phi();
  myEvent.resFSBMuon_eta = leadMuon->eta();

  myEvent.NresolvedANAFSBLeptonCands = resElecs.size() + resMus.size();

  myEvent.nResFSBMuons = resMus.size();
  //CHECK THAT AT LEAST ONE IS ABOVE 60 GEV
  
  if( (leadElec->pt() < 60) && (leadMuon->tunePMuonBestTrack()->pt()*myEvent.leadFSBMuonScale[0] < 60) ) return false;

  myEvent.resFSBMuon = leadMuon;
  myEvent.resFSBElec = leadElec;

  std::cout << "RES FSB MUON WITH PT,ETA,PHI: " << myEvent.resFSBMuon->pt() << "," << myEvent.resFSBMuon->eta() << "," << myEvent.resFSBMuon->phi() << std::endl;
  std::cout << "RES FSB ELECTRON WITH PT,ETA,PHI: " << myEvent.resFSBElec->pt() << "," << myEvent.resFSBElec->eta() << "," << myEvent.resFSBElec->phi() << std::endl;
   
  return true;
} 
bool cmsWRextension::resolvedMuonSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES MUON SELECTION" << std::endl;
  std::vector<const pat::Muon*> resolvedANAMuons;
  std::vector<const pat::Muon*> resolvedLooseMuons;

  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);

  edm::Handle<std::vector<pat::Muon>> regMuons;
  iEvent.getByToken(m_regMuonToken, regMuons);

//  for(std::vector<pat::Muon>::const_iterator iMuon = regMuons->begin(); iMuon != regMuons->end(); iMuon++) {
  for(std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {

   if( fabs(iMuon->eta()) > 2.4) continue;
   if(( iMuon->isHighPtMuon(*myEvent.PVertex) && iMuon->tunePMuonBestTrack()->pt() > 53) && (iMuon->isolationR03().sumPt/iMuon->pt() < 0.1)) {      //2017
      std::cout<<"RES LEPTON CAND WITH PT,ETA,PHI: "<<iMuon->pt()<<","<<iMuon->eta()<<","<<iMuon->phi()<<std::endl;
      std::cout<<"RES LEPTON CAND WITH Iso: " << iMuon->isolationR03().sumPt/iMuon->pt() << std::endl;
     
      resolvedANAMuons.push_back(&(*iMuon));
//      resolvedHighPtMuons.push_back(&(*iMuon));
    }
  }

  if (resolvedANAMuons.size() < 2) return false;
  myEvent.ResCutProgress++;

  std::sort(resolvedANAMuons.begin(), resolvedANAMuons.end(), ::wrTools::compareEtCandidatePointer);

  if(m_isMC){
    myEvent.leadResMuonScale = myMuons.RochesterMethod_MCSmear(resolvedANAMuons[0], m_era);
    myEvent.secondResMuonScale = myMuons.RochesterMethod_MCSmear(resolvedANAMuons[1], m_era);
  }
  else{
    myEvent.leadResMuonScale = myMuons.RochesterMethod_DataScale(resolvedANAMuons[0], m_era);
    myEvent.secondResMuonScale = myMuons.RochesterMethod_DataScale(resolvedANAMuons[1], m_era);
  }

  std::cout << "high pT lead muon" << std::endl;
  //if (resolvedANAMuons[0]->pt() <= 52) return false;  //korea
  //if (!resolvedANAMuons[0]->isHighPtMuon(*myEvent.PVertex)) return false;
  std::cout << "60 GeV lead muon" << std::endl;
  if (resolvedANAMuons[0]->tunePMuonBestTrack()->pt()*myEvent.leadResMuonScale[0] <= 60) return false;
  myEvent.ResCutProgress++;
  std::cout << "isolation of lead muon" << std::endl;
  if (resolvedANAMuons[0]->isolationR03().sumPt/resolvedANAMuons[0]->pt() > 0.1) return false;
  myEvent.ResCutProgress++;

  myEvent.NresolvedANAMuonCands=0;
  for(uint i=0; i<resolvedANAMuons.size(); i++){
    if(resolvedANAMuons[i]->isolationR03().sumPt/resolvedANAMuons[i]->pt() < 0.1) myEvent.NresolvedANAMuonCands++;
  }

  myEvent.resolvedANAMuons = resolvedANAMuons;


  myEvent.resolvedSubleadMuPt = resolvedANAMuons[1]->pt();
   
  return true;
}
bool cmsWRextension::resolvedElectronSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES ELECTRON SELECTION" << std::endl;
  std::vector<const pat::Electron*> resolvedANAElectrons;

  edm::Handle<std::vector<pat::Electron>> highElectrons;
  iEvent.getByToken(m_highElectronToken, highElectrons);

  for(std::vector<pat::Electron>::const_iterator iElectron = highElectrons->begin(); iElectron != highElectrons->end(); iElectron++) {
   if( fabs(iElectron->eta()) > 2.4) continue;
   if( iElectron->pt() < 53 ) continue;
   const vid::CutFlowResult* vidResult =  iElectron->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");
   if(vidResult == NULL) {
     std::cout << "ERROR CANNOT FIND ELECTRON VID RESULTS" << std::endl;
     return false;
   }
   const bool heepIDVID = vidResult->cutFlowPassed();
   if (!heepIDVID) continue;

   resolvedANAElectrons.push_back(&(*iElectron));
  }

  myEvent.NresolvedANAElectronCands = resolvedANAElectrons.size();
  if (myEvent.NresolvedANAElectronCands < 2) return false;
  //HERE IS THE VETO PARAMETER//
  //myEvent.NresolvedANAElectronCands
  // RIGHT HERE /\     //
  myEvent.ResCutProgress++;

  std::sort(resolvedANAElectrons.begin(), resolvedANAElectrons.end(), ::wrTools::compareEtCandidatePointer);

  if(m_isMC){
  }
  else{
  }

  std::cout << "high pT lead electron" << std::endl;
  std::cout << "60 GeV lead electron" << std::endl;
  if (resolvedANAElectrons[0]->pt()/**myEvent.leadResElectronScale[0]*/ <= 60) return false;
  myEvent.ResElecCutProgress++;

  myEvent.resolvedANAElectrons = resolvedANAElectrons;

  myEvent.resolvedSubleadMuPt = resolvedANAElectrons[1]->pt();
   
  return true;
}
void cmsWRextension::LHEinfo(const edm::Event& iEvent, eventBits& myEvent){
  std::cout << "Inside LHEinfo" << std::endl;
  edm::Handle<LHEEventProduct> LHEInfo;
  iEvent.getByToken(LHEEventProductToken, LHEInfo);
  if(!LHEInfo.isValid()) return;

  std::cout << "Found LHEEventProduct" << std::endl;

  int nWeight = (int)LHEInfo->weights().size();

  map<int,double> map_id_to_weight;
  for(int i=0; i<nWeight; i++){
    int this_id = stoi(LHEInfo->weights()[i].id.c_str());
    map_id_to_weight[this_id] = LHEInfo->weights()[i].wgt;
    std::cout << "[SKFlatMaker::fillLHEInfo] map_id_to_weight["<<this_id<<"] = " << map_id_to_weight[this_id] << std::endl;
  }

  int central = ScaleIDRange_.at(0);
  std::cout << "[SKFlatMaker::fillLHEInfo] central = " << central << std::endl;
  std::cout << "[SKFlatMaker::fillLHEInfo] map_id_to_weight[central] = " << map_id_to_weight[central] << std::endl;

  //=============================
  //==== 1) QCD Scale variation
  //=============================

  for(int i=ScaleIDRange_.at(0);i<=ScaleIDRange_.at(1);i++){
    std::cout << "[SKFlatMaker::fillLHEInfo] Scale Varation; adding id = " << i << " map_id_to_weight[i]: " << map_id_to_weight[i] << " map_id_to_weight[ScaleIDRange_.at(0)]: " << map_id_to_weight[ScaleIDRange_.at(0)] << std::endl;
    myEvent.PDFWeights_Scale.push_back( map_id_to_weight[i]/map_id_to_weight[ScaleIDRange_.at(0)] );
  }

  //==============================
  //==== 2) PDF Error and AlphaS
  //==============================
  int N_ErrorSet = PDFErrorIDRange_.at(1)-PDFErrorIDRange_.at(0)+1;

  for(int i=PDFErrorIDRange_.at(0);i<=PDFErrorIDRange_.at(1);i++){
    double this_reweight = map_id_to_weight[i] / map_id_to_weight[central];
    myEvent.PDFWeights_Error.push_back( this_reweight );
    std::cout << "[SKFlatMaker::fillLHEInfo] Error set; adding id = " << i << ", reweight = " << this_reweight << "map_id_to_weight[central]: " << map_id_to_weight[central] << std::endl;
  }

  //==== AlphaS

  double this_as_dn = (map_id_to_weight[PDFAlphaSIDRange_.at(0)] - map_id_to_weight[central]) / map_id_to_weight[central] * PDFAlphaSScaleValue_.at(0);
  double this_as_up = (map_id_to_weight[PDFAlphaSIDRange_.at(1)] - map_id_to_weight[central]) / map_id_to_weight[central] * PDFAlphaSScaleValue_.at(1);

  std::cout << "[SKFlatMaker::fillLHEInfo] AlphaS; dn id = " << PDFAlphaSIDRange_.at(0) << std::endl;
  std::cout << "[SKFlatMaker::fillLHEInfo] AlphaS; up id = " << PDFAlphaSIDRange_.at(1) << std::endl;

  myEvent.PDFWeights_AlphaS.push_back(1.+this_as_dn);
  myEvent.PDFWeights_AlphaS.push_back(1.+this_as_up);

  std::cout << "[SKFlatMaker::fillLHEInfo] LHEInfo->originalXWGTUP() = " << LHEInfo->originalXWGTUP() << std::endl;
  std::cout << "[SKFlatMaker::fillLHEInfo] [myEvent.PDFWeights_Scale]" << std::endl;
//  for(unsigned int i=0;i<myEvent.PDFWeights_Scale.size();i++) std::cout << "[SKFlatMaker::fillLHEInfo] " << myEvent.PDFWeights_Scale.at(i) << std::endl;
//  std::cout << "[SKFlatMaker::fillLHEInfo] [myEvent.PDFWeights_Error]" << std::endl;
//  for(unsigned int i=0;i<myEvent.PDFWeights_Error.size();i++) std::cout << "[SKFlatMaker::fillLHEInfo] " << myEvent.PDFWeights_Error.at(i) << std::endl;
//  std::cout << "[SKFlatMaker::fillLHEInfo] [myEvent.PDFWeights_AlphaS]" << std::endl;
//  for(unsigned int i=0;i<myEvent.PDFWeights_AlphaS.size();i++) std::cout << "[SKFlatMaker::fillLHEInfo] " << myEvent.PDFWeights_AlphaS.at(i) << std::endl;



}
bool cmsWRextension::electronSelection(const edm::Event& iEvent, eventBits& myEvent) {  //Flavor sideband
  std::cout<<"STARTING ELECTRON SELECTION"<<std::endl;
  std::vector<const pat::Electron*> highPTelectrons200;

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
    if ( vidResult->getCutResultByIndex(cutnrs::HEEPV70::ET           )  == true && 
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::ETA          )  == true && 
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::DETAINSEED   )  == true &&   
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::DPHIIN       )  == true &&    
        // vidResult->getCutResultByIndex(cutnrs::HEEPV70::SIGMAIETAIETA)  == false && we don't require ISO, we select for it later       
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::E2X5OVER5X5  )  == true &&   
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::HADEM        )  == true &&     
        // vidResult->getCutResultByIndex(cutnrs::HEEPV70::TRKISO       )  == false &&  //MUST FAIL TRACK ISOLATION 
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::EMHADD1ISO   )  == true &&     
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::DXY          )  == true &&  
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::MISSHITS     )  == true &&  
         vidResult->getCutResultByIndex(cutnrs::HEEPV70::ECALDRIVEN   )  == true     
    ) {
        if (iElec->pt() < 200) {
          myEvent.nAdditionalHEEP_noISO++;    
        }
    }
    if (heepIDVID) {
      //PHASE SPACE CUTS
      std::cout << "Electron pT: " << iElec->pt() << std::endl;
      if( iElec->pt() < 200){
        myEvent.nAdditionalHEEP++;
      }

      if (iElec->pt() >= 53){
	std::cout << "iElec->pt(): " << iElec->pt() << std::endl;
        highPTelectrons200.push_back(&(*iElec));
      }
    }
    //std::cout<<"ELECTRON CAND WITH PT,ETA,PHI: "<<iElec->pt()<<","<<iElec->eta()<<","<<iElec->phi()<<std::endl;
  }
      //if( iLep->pt() > 200 ) highPTMuons.push_back(&(*iLep));
  //COLLECT MUONS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  //USE THIS FOR LEPTON VETOES
  myEvent.electronCands200 = highPTelectrons200.size();
  std::cout << "check #1" << std::endl;
  //UP HERE /\ //
  if (myEvent.electronCands200 > 0) {
    std::sort(highPTelectrons200.begin(),highPTelectrons200.end(),::wrTools::compareEtCandidatePointer); 
    myEvent.myElectronCandsHighPt200 = highPTelectrons200;
    myEvent.myElectronCand = highPTelectrons200[0];
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
//  std::cout << "Vertex Handle" << std::endl;

//  edm::Handle<std::vector<reco::Vertex>> vertices;
//  iEvent.getByToken(m_offlineVerticesToken, vertices);
  
  for(std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {
    std::cout << "Looping through muons" << std::endl;
    std::cout << "Boost Muon cand pT: " << iMuon->tunePMuonBestTrack()->pt() << " eta: " << iMuon->eta() << " iMuon->isHighPtMuon(*myEvent.PVertex): " << iMuon->isHighPtMuon(*myEvent.PVertex) << " iMuon->isolationR03().sumPt/iMuon->pt(): " << iMuon->isolationR03().sumPt/iMuon->pt() << std::endl;
    if( iMuon->tunePMuonBestTrack()->pt() < 53 || fabs(iMuon->eta()) > 2.4 ) continue; //preliminary pt cut to speed the loop, and the eta cut
//    if(m_doTrig) {
//      std::cout << "Checking Filter" << std::endl;
//      if (! ::wrTools::checkFilters(iMuon->eta(),iMuon->phi(),*trigObjsHandle,m_muonFiltersToPass) ) continue;      
//    }
    if(( iMuon->isHighPtMuon(*myEvent.PVertex) && iMuon->tunePMuonBestTrack()->pt() > m_highPTleptonCut) && (iMuon->isolationR03().sumPt/iMuon->pt() < .1)) {
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
    if(m_isMC){
      if (highPTMuons[0]->pt() < 200){
        myEvent.leadBoostMuonScale = myMuons.RochesterMethod_MCSmear(highPTMuons[0], m_era);
      }else{
        myEvent.leadBoostMuonScale = myMuons.GeneralizedEndpointMethod(highPTMuons[0]);
      }
    }
    else{
      if (highPTMuons[0]->pt() < 200){
        myEvent.leadBoostMuonScale = myMuons.RochesterMethod_DataScale(highPTMuons[0], m_era);
      }else{
        myEvent.leadBoostMuonScale = myMuons.GeneralizedEndpointMethod_Data(highPTMuons[0]);
      }
    }

    std::cout << "myEvent.leadBoostMuonScale.size(): " << myEvent.leadBoostMuonScale.size() << std::endl;
    myEvent.myMuonCandsHighPt = highPTMuons;
    //We select the lead muon in the event
    myEvent.myMuonCand = highPTMuons[0];
    if(myEvent.muonCands > 1){
      myEvent.my2ndHighPtMuonCand = highPTMuons[1];
    }
  }
  return true;

}
bool cmsWRextension::resolvedJetSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "STARTING JET SELECTION FOR RESOLVED ANALYSIS" << std::endl;
  edm::Handle<std::vector<pat::Jet>> recoJetsAK4;
  iEvent.getByToken(m_AK4recoCHSJetsToken, recoJetsAK4);
  assert(recoJetsAK4.isValid());
//  std::vector<const pat::Jet*> resCandJets;

  std::vector<const baconhep::TAddJet*> resCandJets;
  std::vector<const baconhep::TAddJet*> resCandJets_JECUp;
  std::vector<const baconhep::TAddJet*> resCandJets_JECDown;
  std::vector<const baconhep::TAddJet*> resCandJets_JERUp;
  std::vector<const baconhep::TAddJet*> resCandJets_JERDown;

  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/cmsWRextensions/data/";

  edm::Handle<double> rhoHandle;
  iEvent.getByToken(m_rhoLabel, rhoHandle);
  double rho = *(rhoHandle.product());


  if(m_isMC){
	if(m_era == "2016"){
	  fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017_V11_MC_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	}else if(m_era == "2017"){
          fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	}else if(m_era == "2018"){
          fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_V19_MC_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	}
  }else{
	if (m_era == "2016") {
          if(iEvent.id().run()<276812){
	    fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017BCD_V11_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  }else if(iEvent.id().run() > 276830 && iEvent.id().run() < 278809){
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017EF_V11_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  }else if(iEvent.id().run() > 278819){
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017GH_V11_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  }
	}else if(m_era == "2017"){
          if (iEvent.id().run() < 299330) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017B_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  } else if (iEvent.id().run() > 299367 && iEvent.id().run() < 302030) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017C_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  } else if (iEvent.id().run() > 302029 && iEvent.id().run() < 304798) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  } else if (iEvent.id().run() > 305039 && iEvent.id().run() < 306461) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017F_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
	  }
	}else if(m_era == "2018"){
          if (iEvent.id().run() < 316996) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunA_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 317079 && iEvent.id().run() < 319311) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunB_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 319336 && iEvent.id().run() < 320066) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunC_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 320672 && iEvent.id().run() < 325176) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunD_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          }
	}
  }

/*  if (myEvent.resolvedANAMuons.size() < 2) {
    std::cout << "NOT ENOUGH MUONS, EXITING RESOLVED JET SELECTION" << std::endl;
    return false;
  }*/
  double NHF  =                0.;
  double NEMF =                0.;
  double CHF  =                0.;
  double CEMF =                0.;
  double NumConst =            0.;
  double MUF      =            0.;
  double CHM      =            0.;
  double jetUNC =              0.;
  float sigma_MC = 0.;
  float sf = 0.;
  float sfUp = 0.;
  float sfDown = 0.;
  double x1 = 0.;
  double x2 = 0.;
  double x3 = 0.;
  double jetEnergySmearFactor = 1.0;
  double jetEnergySmearFactorUp = 1.0;
  double jetEnergySmearFactorDown = 1.0;
  double jetCorrPtSmear = 0.;
  double jetPtJESUp =            0.;
  double jetPtJESDown =          0.;
  double jetPtJERUp = 0.;
  double jetPtJERDown = 0.;
  double jetCorrESmear = 0.;
  double jetEJESUp = 0.;
  double jetEJESDown = 0.;
  double jetEJERUp = 0.;
  double jetEJERDown = 0.;

  for(std::vector<pat::Jet>::const_iterator iJet = recoJetsAK4->begin(); iJet != recoJetsAK4->end(); iJet++) {
    if ( fabs(iJet->eta()) > 2.4) continue;
    std::cout<<"POSSIBLE JET CAND WITH PT,ETA,PHI: "<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
    std::cout << "iJet->muonEnergyFraction(): " << iJet->muonEnergyFraction() << " iJet->chargedEmEnergyFraction(): " << iJet->chargedEmEnergyFraction() << std::endl;
    NHF  =                iJet->neutralHadronEnergyFraction();
    NEMF =                iJet->neutralEmEnergyFraction();
    CHF  =                iJet->chargedHadronEnergyFraction();
    CEMF =                iJet->chargedEmEnergyFraction();
    NumConst =            iJet->chargedMultiplicity()+iJet->neutralMultiplicity();
    MUF      =            iJet->muonEnergyFraction();
    CHM      =            iJet->chargedMultiplicity(); 
    //APPLYING TIGHT QUALITY CUTS
    if (NHF > .9) continue;
    if (NEMF > .9) continue;
    if (NumConst <= 1) continue;
    if (MUF >= .8) continue; //MAKE SURE THE AREN'T MUONS
    //ADDITIONAL CUTS BECAUSE OF TIGHT ETA CUT
    if (CHF == 0) continue;
    if (CHM == 0) continue;
    //if (CEMF > .99) continue;
    if (CEMF > .90) continue; 

    fJetUnc->setJetEta(iJet->eta());
    fJetUnc->setJetPt(iJet->pt());
    jetUNC =              fJetUnc->getUncertainty(true);
    JME::JetParameters parameters = {{JME::Binning::JetPt, iJet->pt()}, {JME::Binning::JetEta, iJet->eta()}, {JME::Binning::Rho, TMath::Min(rho,44.30)}};
    sigma_MC = resolution_AK4.getResolution(parameters);
    sf = resolution_sf_AK4.getScaleFactor(parameters);
    sfUp = resolution_sf_AK4.getScaleFactor(parameters, Variation::UP);
    sfDown = resolution_sf_AK4.getScaleFactor(parameters, Variation::DOWN);
    x1 = r->Gaus();
    x2 = r->Gaus();
    x3 = r->Gaus();
    jetEnergySmearFactor = 1.0;
    jetEnergySmearFactorUp = 1.0;
    jetEnergySmearFactorDown = 1.0;
    if(m_isMC){
      jetEnergySmearFactor = 1.0 + sqrt(sf*sf - 1.0)*sigma_MC*x1;
      jetEnergySmearFactorUp = 1.0 + sqrt(sfUp*sfUp - 1.0)*sigma_MC*x2;
      jetEnergySmearFactorDown = 1.0 + sqrt(sfDown*sfDown - 1.0)*sigma_MC*x3;
    }
    jetCorrPtSmear = iJet->pt()*jetEnergySmearFactor;
    jetPtJESUp =            iJet->pt()*jetEnergySmearFactor*(1+jetUNC);
    jetPtJESDown =          iJet->pt()*jetEnergySmearFactor*(1-jetUNC);
    jetPtJERUp = iJet->pt()*jetEnergySmearFactorUp;
    jetPtJERDown = iJet->pt()*jetEnergySmearFactorDown;

    jetCorrESmear = iJet->energy()*jetEnergySmearFactor;
    jetEJESUp = iJet->energy()*jetEnergySmearFactor*(1+jetUNC);
    jetEJESDown = iJet->energy()*jetEnergySmearFactor*(1-jetUNC);
    jetEJERUp = iJet->energy()*jetEnergySmearFactorUp;
    jetEJERDown = iJet->energy()*jetEnergySmearFactorDown;


    if(jetCorrPtSmear > 40){
      baconhep::TAddJet* pAddJet = new baconhep::TAddJet();

      pAddJet->pT   = jetCorrPtSmear;
      pAddJet->E    = jetCorrESmear;
      pAddJet->eta  = iJet->eta();
      pAddJet->phi  = iJet->phi();

      resCandJets.push_back(pAddJet);
      std::cout<<"RES JET CAND WITH PT,ETA,PHI: "<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;

    }
    if(jetPtJESUp > 40){
      baconhep::TAddJet* pAddJet_JECUp = new baconhep::TAddJet();

      pAddJet_JECUp->pT = jetPtJESUp;
      pAddJet_JECUp->E  = jetEJESUp;
      pAddJet_JECUp->eta = iJet->eta();
      pAddJet_JECUp->phi = iJet->phi();

      resCandJets_JECUp.push_back(pAddJet_JECUp);
    }
    if(jetPtJESDown > 40){
      baconhep::TAddJet* pAddJet_JECDown = new baconhep::TAddJet();

      pAddJet_JECDown->pT = jetPtJESDown;
      pAddJet_JECDown->E  = jetEJESDown;
      pAddJet_JECDown->eta = iJet->eta();
      pAddJet_JECDown->phi = iJet->phi();

      resCandJets_JECDown.push_back(pAddJet_JECDown);
    }
    if(jetPtJERUp > 40){
      baconhep::TAddJet* pAddJet_JERUp = new baconhep::TAddJet();

      pAddJet_JERUp->pT = jetPtJERUp;
      pAddJet_JERUp->E  = jetEJERUp;
      pAddJet_JERUp->eta = iJet->eta();
      pAddJet_JERUp->phi = iJet->phi();

      resCandJets_JERUp.push_back(pAddJet_JERUp);
    }
    if(jetPtJERDown > 40){
      baconhep::TAddJet* pAddJet_JERDown = new baconhep::TAddJet();

      pAddJet_JERDown->pT = jetPtJERDown;
      pAddJet_JERDown->E  = jetEJERDown;
      pAddJet_JERDown->eta = iJet->eta();
      pAddJet_JERDown->phi = iJet->phi();

      resCandJets_JERDown.push_back(pAddJet_JERDown);
    }
  }

  myEvent.myResCandJets = resCandJets;
  myEvent.myResCandJets_JECUp = resCandJets_JECUp;
  myEvent.myResCandJets_JECDown = resCandJets_JECDown;
  myEvent.myResCandJets_JERUp = resCandJets_JERUp;
  myEvent.myResCandJets_JERDown = resCandJets_JERDown;

  delete fJetUnc;
  return true;

}
bool cmsWRextension::resolvedFSBJetSelection(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "STARTING JET SELECTION FOR RESOLVED ANALYSIS" << std::endl;
  edm::Handle<std::vector<pat::Jet>> recoJetsAK4;
  iEvent.getByToken(m_AK4recoCHSJetsToken, recoJetsAK4);
  assert(recoJetsAK4.isValid());

  std::vector<const baconhep::TAddJet*> resCandJets;
  std::vector<const baconhep::TAddJet*> resCandJets_JECUp;
  std::vector<const baconhep::TAddJet*> resCandJets_JECDown;
  std::vector<const baconhep::TAddJet*> resCandJets_JERUp;
  std::vector<const baconhep::TAddJet*> resCandJets_JERDown;

  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/cmsWRextensions/data/";

  edm::Handle<double> rhoHandle;
  iEvent.getByToken(m_rhoLabel, rhoHandle);
  double rho = *(rhoHandle.product());

  //CHECK FOR THE FSB LEPTONS
  if (myEvent.resFSBMuon == NULL || myEvent.resFSBElec == NULL ) {
    std::cout << "NOT ENOUGH LEPTONS, EXITING RESOLVED FSB JET SELECTION" << std::endl;
    return false;
  }

  if(m_isMC){
        if(m_era == "2016"){
          fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017_V11_MC_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
        }else if(m_era == "2017"){
          fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
        }else if(m_era == "2018"){
          fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_V19_MC_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
        }
  }else{
        if (m_era == "2016") {
          if(iEvent.id().run()<276812){
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017BCD_V11_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          }else if(iEvent.id().run() > 276830 && iEvent.id().run() < 278809){
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017EF_V11_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          }else if(iEvent.id().run() > 278819){
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017GH_V11_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          }
        }else if(m_era == "2017"){
          if (iEvent.id().run() < 299330) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017B_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 299367 && iEvent.id().run() < 302030) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017C_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 302029 && iEvent.id().run() < 304798) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 305039 && iEvent.id().run() < 306461) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017F_V32_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          }
        }else if(m_era == "2018"){
          if (iEvent.id().run() < 316996) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunA_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 317079 && iEvent.id().run() < 319311) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunB_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 319336 && iEvent.id().run() < 320066) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunC_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 320672 && iEvent.id().run() < 325176) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunD_V19_DATA_Uncertainty_AK4PFchs.txt",jecPathname.c_str()));
          }
        }
  }


  for(std::vector<pat::Jet>::const_iterator iJet = recoJetsAK4->begin(); iJet != recoJetsAK4->end(); iJet++) {
    if ( fabs(iJet->eta()) > 2.4) continue;
    std::cout<<"POSSIBLE JET CAND WITH PT,ETA,PHI: "<<iJet->pt()<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
    double NHF  =                iJet->neutralHadronEnergyFraction();
    double NEMF =                iJet->neutralEmEnergyFraction();
    double CHF  =                iJet->chargedHadronEnergyFraction();
    double CEMF =                iJet->chargedEmEnergyFraction();
    double NumConst =            iJet->chargedMultiplicity()+iJet->neutralMultiplicity();
    double MUF      =            iJet->muonEnergyFraction();
    double CHM      =            iJet->chargedMultiplicity(); 
    //APPLYING TIGHT QUALITY CUTS
    if (NHF > .9) continue;
    if (NEMF > .9) continue;
    if (NumConst <= 1) continue;
    if (MUF >= .8) continue; //MAKE SURE THE AREN'T MUONS
    //ADDITIONAL CUTS BECAUSE OF TIGHT ETA CUT
    if (CHF == 0) continue;
    if (CHM == 0) continue;
    //if (CEMF > .99) continue;
    if (CEMF > .90) continue; 
    //MUST BE AWAY FROM LEPTONS

    fJetUnc->setJetPt(iJet->pt());
    fJetUnc->setJetEta(iJet->eta());
    double jetUNC =              fJetUnc->getUncertainty(true);
    JME::JetParameters parameters = {{JME::Binning::JetPt, iJet->pt()}, {JME::Binning::JetEta, iJet->eta()}, {JME::Binning::Rho, TMath::Min(rho,44.30)}};
    float sigma_MC = resolution_AK4.getResolution(parameters);
    float sf = resolution_sf_AK4.getScaleFactor(parameters);
    float sfUp = resolution_sf_AK4.getScaleFactor(parameters, Variation::UP);
    float sfDown = resolution_sf_AK4.getScaleFactor(parameters, Variation::DOWN);
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


    if(jetCorrPtSmear > 40){
      baconhep::TAddJet* pAddJet = new baconhep::TAddJet();

      pAddJet->pT   = jetCorrPtSmear;
      pAddJet->E    = jetCorrESmear;
      pAddJet->eta  = iJet->eta();
      pAddJet->phi  = iJet->phi();

      resCandJets.push_back(pAddJet);
      std::cout<<"RES JET CAND WITH PT,ETA,PHI: "<<jetCorrPtSmear<<","<<iJet->eta()<<","<<iJet->phi()<<std::endl;
      std::cout <<"jetEnergySmearFactor: " << jetEnergySmearFactor << std::endl;
      std::cout <<"sf: " << sf << " sigma_MC: " << sigma_MC << " x1: " << x1 << std::endl;

    }
    if(jetPtJESUp > 40){
      baconhep::TAddJet* pAddJet_JECUp = new baconhep::TAddJet();

      pAddJet_JECUp->pT = jetPtJESUp;
      pAddJet_JECUp->E  = jetEJESUp;
      pAddJet_JECUp->eta = iJet->eta();
      pAddJet_JECUp->phi = iJet->phi();

      resCandJets_JECUp.push_back(pAddJet_JECUp);
    }
    if(jetPtJESDown > 40){
      baconhep::TAddJet* pAddJet_JECDown = new baconhep::TAddJet();

      pAddJet_JECDown->pT = jetPtJESDown;
      pAddJet_JECDown->E  = jetEJESDown;
      pAddJet_JECDown->eta = iJet->eta();
      pAddJet_JECDown->phi = iJet->phi();

      resCandJets_JECDown.push_back(pAddJet_JECDown);
    }
    if(jetPtJERUp > 40){
      baconhep::TAddJet* pAddJet_JERUp = new baconhep::TAddJet();

      pAddJet_JERUp->pT = jetPtJERUp;
      pAddJet_JERUp->E  = jetEJERUp;
      pAddJet_JERUp->eta = iJet->eta();
      pAddJet_JERUp->phi = iJet->phi();

      resCandJets_JERUp.push_back(pAddJet_JERUp);
    }
    if(jetPtJERDown > 40){
      baconhep::TAddJet* pAddJet_JERDown = new baconhep::TAddJet();

      pAddJet_JERDown->pT = jetPtJERDown;
      pAddJet_JERDown->E  = jetEJERDown;
      pAddJet_JERDown->eta = iJet->eta();
      pAddJet_JERDown->phi = iJet->phi();

      resCandJets_JERDown.push_back(pAddJet_JERDown);
    }    
  }

  myEvent.myResFSBCandJets = resCandJets;
  myEvent.myResFSBCandJets_JECUp = resCandJets_JECUp;
  myEvent.myResFSBCandJets_JECDown = resCandJets_JECDown;
  myEvent.myResFSBCandJets_JERUp = resCandJets_JERUp;
  myEvent.myResFSBCandJets_JERDown = resCandJets_JERDown;

  delete fJetUnc;
  return true;

}
bool cmsWRextension::jetSelection(const edm::Event& iEvent,  eventBits& myEvent) {
   
//  if (myEvent.genSecondMuon == NULL) { return false; }

  std::cout<<"STARTING JET SELECTION"<<std::endl;
  edm::Handle<std::vector<pat::Jet>> recoJetsAK8;
  iEvent.getByToken(m_AK8recoPUPPIJetsToken, recoJetsAK8);
  assert(recoJetsAK8.isValid());

  edm::Handle<std::vector<pat::Jet>> recoSubJets;
  iEvent.getByToken(m_AK8recoPUPPISubJetsToken, recoSubJets);
  assert(recoSubJets.isValid());

  edm::Handle<double> rhoHandle;
  iEvent.getByToken(m_rhoLabel, rhoHandle);
  double rho = *(rhoHandle.product());

  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/cmsWRextensions/data/";
  if(m_isMC){
	if (m_era == "2016") {
      fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017_V11_MC_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    } else if (m_era == "2017") {
      fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017_V32_MC_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    } else if (m_era == "2018") {
      fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_V19_MC_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
    }
  }else{
	if (m_era == "2016") {
      if(iEvent.id().run()<276812){
        fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017BCD_V11_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      }else if(iEvent.id().run() > 276830 && iEvent.id().run() < 278809){
        fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017EF_V11_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      }else if(iEvent.id().run() > 278819){
        fJetUnc = new JetCorrectionUncertainty(Form("%s/2016/Summer16_07Aug2017GH_V11_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      }
	} else if (m_era == "2017") {
	  if (iEvent.id().run() < 299330) {
        fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017B_V32_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      } else if (iEvent.id().run() > 299367 && iEvent.id().run() < 302030) {
        fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017C_V32_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      } else if (iEvent.id().run() > 302029 && iEvent.id().run() < 304798) {
         fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017DE_V32_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      } else if (iEvent.id().run() > 305039 && iEvent.id().run() < 306461) {
        fJetUnc = new JetCorrectionUncertainty(Form("%s/2017/Fall17_17Nov2017F_V32_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
      }
	} else if (m_era == "2018") {
          if (iEvent.id().run() < 316996) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunA_V19_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 317079 && iEvent.id().run() < 319311) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunB_V19_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 319336 && iEvent.id().run() < 320066) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunC_V19_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
          } else if (iEvent.id().run() > 320672 && iEvent.id().run() < 325176) {
            fJetUnc = new JetCorrectionUncertainty(Form("%s/2018/Autumn18_RunD_V19_DATA_Uncertainty_AK8PFPuppi.txt",jecPathname.c_str()));
          }
	}
  }

  std::vector<const baconhep::TAddJet*> AddJets;
  std::vector<const baconhep::TAddJet*> AddJets_JECUp;
  std::vector<const baconhep::TAddJet*> AddJets_JECDown;
  std::vector<const baconhep::TAddJet*> AddJets_JERUp;
  std::vector<const baconhep::TAddJet*> AddJets_JERDown;

  std::vector<const baconhep::TAddJet*> AddJets_noLSF;
  std::vector<const baconhep::TAddJet*> AddJets_noLSF_JECUp;
  std::vector<const baconhep::TAddJet*> AddJets_noLSF_JECDown;
  std::vector<const baconhep::TAddJet*> AddJets_noLSF_JERUp;
  std::vector<const baconhep::TAddJet*> AddJets_noLSF_JERDown;

  //COLLECT JetS INTO HIGHPT AND ALLPT WITHIN ACCEPTANCE
  for(std::vector<pat::Jet>::const_iterator iJet = recoJetsAK8->begin(); iJet != recoJetsAK8->end(); iJet++) {
   const pat::Jet *subjet1=0, *subjet2=0, *subjet3=0, *subjet4=0;
   double csv1=-2, csv2=-2, csv3=-2, csv4=-2;
   for(std::vector<pat::Jet>::const_iterator iSubJet = recoSubJets->begin(); iSubJet != recoSubJets->end(); iSubJet++) {
    if(reco::deltaR(iJet->eta(),iJet->phi(),iSubJet->eta(),iSubJet->phi())>0.8) continue;

//    std::cout << "iSubJet->pt(): " << iSubJet->pt() << std::endl;
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
    //Finding maximum CSV
    double maxCSV = -10;
    if(csv1 > maxCSV) maxCSV=csv1;
    if(csv2 > maxCSV) maxCSV=csv2;
    if(csv3 > maxCSV) maxCSV=csv3;
    if(csv4 > maxCSV) maxCSV=csv4;

    std::cout << " iJet->pt(): " <<  iJet->pt() << std::endl;
    if ( iJet->pt() < 180 ) continue;
    //GETS ALL THE RELEVANT JET ID QUANTITIES
    double NHF  =                iJet->neutralHadronEnergyFraction();
    double NEMF =                iJet->neutralEmEnergyFraction();
    double CHF  =                iJet->chargedHadronEnergyFraction();
//    double MUF  =                iJet->muonEnergyFraction();
    double CEMF =                iJet->chargedEmEnergyFraction();
    double NumConst =            iJet->chargedMultiplicity()+iJet->neutralMultiplicity();
//    double NumNeutralParticles = iJet->neutralMultiplicity();
    double CHM      =            iJet->chargedMultiplicity(); 


//    fJetUnc->setJetPt(iJet->pt());
//    fJetUnc->setJetEta(iJet->eta());
//    double jetUNC =              fJetUnc->getUncertainty(true);
    JME::JetParameters parameters = {{JME::Binning::JetPt, iJet->pt()}, {JME::Binning::JetEta, iJet->eta()}, {JME::Binning::Rho, rho}};
    float jet_resolution = resolution.getResolution(parameters);
    float sf = resolution_sf.getScaleFactor(parameters);
    float sfUp = resolution_sf.getScaleFactor(parameters, Variation::UP);
    float sfDown = resolution_sf.getScaleFactor(parameters, Variation::DOWN);
//    double x1 = r->Gaus(0., sigma_MC);
//    double x2 = r->Gaus(0., sigma_MC);
//    double x3 = r->Gaus(0., sigma_MC);
    double x1;
    double x2;
    double x3;
    double jetEnergySmearFactor = 1.0;
    double jetEnergySmearFactorUp = 1.0;
    double jetEnergySmearFactorDown = 1.0;
    double sigma = 1.0;
    double sigmaUp = 1.0;
    double sigmaDown = 1.0;
    if(m_isMC){
      std::cout << "sf: " << sf << " sfUp: " << sfUp << " sfDown: " << sfDown << std::endl;
      sigma = jet_resolution * sqrt(TMath::Max(sf*sf - 1.0,0.));
      sigmaUp = jet_resolution * sqrt(TMath::Max(sfUp*sfUp - 1.0,0.));
      sigmaDown = jet_resolution * sqrt(TMath::Max(sfDown*sfDown - 1.0,0.));
      x1 = r->Gaus(0., sigma);
      x2 = r->Gaus(0., sigmaUp);
      x3 = r->Gaus(0., sigmaDown);
      jetEnergySmearFactor = 1.0+x1;
      jetEnergySmearFactorUp = 1.0+x2;
      jetEnergySmearFactorDown = 1.0+x3;
//      jetEnergySmearFactor = 1.0 + sqrt(TMath::Max(sf*sf - 1.0,0.))*x1;
//      jetEnergySmearFactorUp = 1.0 + sqrt(TMath::Max(sfUp*sfUp - 1.0,0.))*x2;
//      jetEnergySmearFactorDown = 1.0 + sqrt(TMath::Max(sfDown*sfDown - 1.0,0.))*x3;
      std::cout << "jetEnergySmearFactor: " << jetEnergySmearFactor << " jetEnergySmearFactorUp: " << jetEnergySmearFactorUp << " jetEnergySmearFactorDown: " << jetEnergySmearFactorDown << std::endl;
//      std::cout << "jetEnergySmearFactorDown: " << jetEnergySmearFactorDown << " sfDown: " << sfDown << " x3: " << x3 << std::endl;
    }
//    fJetUnc->setJetPt(iJet->pt());
    fJetUnc->setJetPt(iJet->pt()*jetEnergySmearFactor);
    fJetUnc->setJetEta(iJet->eta());
    double jetUNC =              fJetUnc->getUncertainty(true);
/*    double jetCorrPtSmear = iJet->pt();
    double jetPtJESUp =            iJet->pt()*(1+jetUNC);
    double jetPtJESDown =          iJet->pt()*(1-jetUNC);
    double jetPtJERUp = iJet->pt()*jetEnergySmearFactorUp;
    double jetPtJERDown = iJet->pt()*jetEnergySmearFactorDown;*/
    double jetCorrPtSmear = iJet->pt()*jetEnergySmearFactor;
    double jetPtJESUp =            iJet->pt()*jetEnergySmearFactor*(1+jetUNC);
    double jetPtJESDown =          iJet->pt()*jetEnergySmearFactor*(1-jetUNC);
    double jetPtJERUp = iJet->pt()*jetEnergySmearFactorUp;
    double jetPtJERDown = iJet->pt()*jetEnergySmearFactorDown;

/*    double jetCorrESmear = iJet->energy();
    double jetEJESUp = iJet->energy()*(1+jetUNC);
    double jetEJESDown = iJet->energy()*(1-jetUNC);
    double jetEJERUp = iJet->energy()*jetEnergySmearFactorUp;
    double jetEJERDown = iJet->energy()*jetEnergySmearFactorDown;*/
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

    if(JetTools::leptons((*iJet),3)> 0 && JetTools::leptons(*iJet,7)<0.8) {
      lepCPt = JetTools::leptons(*iJet,3);
      lepCEta = JetTools::leptons(*iJet,5);
      lepCPhi = JetTools::leptons(*iJet,6);
      lepCId = JetTools::leptons(*iJet,4);

      myEvent.lsfLeptonPt = lepCPt;
      myEvent.lsfLeptonEta = lepCEta;
      myEvent.lsfLeptonPhi = lepCPhi;
    
    }

    std::cout << "iJet->pt(): " << iJet->pt() << std::endl;
    std::cout << iJet->userFloat("ak8PFJetsPuppiSoftDropMass") << std::endl;
    std::cout<<"POSSIBLE AK8JET CAND WITH PT,ETA,PHI,MASS: "<<jetCorrPtSmear<<","<<iJet->eta()<<","<<iJet->phi()<<","<< iJet->userFloat("ak8PFJetsPuppiSoftDropMass") << std::endl;

    std::cout << "jetCorrPtSmear: " << jetCorrPtSmear << " jetPtJESUp: " << jetPtJESUp << " jetPtJESDown: " << jetPtJESDown << " jetPtJERUp: " << jetPtJERUp << " jetPtJERDown: " << jetPtJERDown << std::endl;

    //JETS PASSING WITH VERY HIGH PT
    if( jetCorrPtSmear > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*PUPPIweight(jetCorrPtSmear,iJet->eta(),m_isMC) > 40){
      	baconhep::TAddJet* pAddJet = new baconhep::TAddJet();

      	pAddJet->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
      	pAddJet->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
      	pAddJet->pT   = jetCorrPtSmear;
      	pAddJet->E    = jetCorrESmear;
	pAddJet->UncorrectedE = iJet->energy();
      	pAddJet->eta  = iJet->eta();
      	pAddJet->phi  = iJet->phi();
      	pAddJet->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        pAddJet->SDmassCorr = PUPPIweight(pAddJet->pT, pAddJet->eta, m_isMC);
	std::cout << "pAddJet->SDmassCorr: " << pAddJet->SDmassCorr << std::endl;

        std::vector<fastjet::PseudoJet> vSubCInc; pAddJet->lsfCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2);
        std::vector<fastjet::PseudoJet> vSubC_2;  pAddJet->lsfC_2 = JetTools::lsf(lClusterParticles, vSubC_2, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 2);
        std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);
        std::vector<fastjet::PseudoJet> vSubC_4;  pAddJet->lsfC_4 = JetTools::lsf(lClusterParticles, vSubC_4, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 4);

	std::cout << "pAddJet->lsfC_3: " << pAddJet->lsfC_3 << std::endl;

//        if(pAddJet->lsfC_3 > 0.5){
        if(pAddJet->lsfC_3 > 0.75){

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
      	  std::cout<<"AK8JET CAND WITH PT,ETA,PHI,MASS,LSF: "<<jetCorrPtSmear<<","<<iJet->eta()<<","<<iJet->phi()<<","<< iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*pAddJet->SDmassCorr << "," << pAddJet->lsfC_3 << std::endl;
        }
	AddJets_noLSF.push_back(pAddJet);
      }
    }
    if( jetPtJESUp > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*PUPPIweight(jetCorrPtSmear,iJet->eta(),m_isMC) > 40){
        baconhep::TAddJet* pAddJet_JECUp = new baconhep::TAddJet();

        pAddJet_JECUp->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JECUp->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JECUp->pT = jetPtJESUp;
        pAddJet_JECUp->E  = jetEJESUp;
        pAddJet_JECUp->eta = iJet->eta();
        pAddJet_JECUp->phi = iJet->phi();
        pAddJet_JECUp->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        pAddJet_JECUp->SDmassCorr = PUPPIweight(pAddJet_JECUp->pT, pAddJet_JECUp->eta,m_isMC);

        std::vector<fastjet::PseudoJet> vSubCInc; pAddJet_JECUp->lsfCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2);
        std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet_JECUp->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);

        if(pAddJet_JECUp->lsfC_3 > 0.75){

          if(vSubC_3.size() > 0) {
            pAddJet_JECUp->lsfC_3_sj1_pt = vSubC_3[0].pt();
            pAddJet_JECUp->lsfC_3_sj1_eta = vSubC_3[0].eta();
            pAddJet_JECUp->lsfC_3_sj1_phi = vSubC_3[0].phi();
            pAddJet_JECUp->lsfC_3_sj1_m = vSubC_3[0].m();
          }

          AddJets_JECUp.push_back(pAddJet_JECUp);
	}
	AddJets_noLSF_JECUp.push_back(pAddJet_JECUp);
      }
    }

    if( jetPtJESDown > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*PUPPIweight(jetCorrPtSmear,iJet->eta(), m_isMC) > 40){
        baconhep::TAddJet* pAddJet_JECDown = new baconhep::TAddJet();

        pAddJet_JECDown->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JECDown->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JECDown->pT = jetPtJESDown;
        pAddJet_JECDown->E  = jetEJESDown;
        pAddJet_JECDown->eta = iJet->eta();
        pAddJet_JECDown->phi = iJet->phi();
        pAddJet_JECDown->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        pAddJet_JECDown->SDmassCorr = PUPPIweight(pAddJet_JECDown->pT, pAddJet_JECDown->eta, m_isMC);

        std::vector<fastjet::PseudoJet> vSubCInc; pAddJet_JECDown->lsfCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2);
        std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet_JECDown->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);

        if(pAddJet_JECDown->lsfC_3 > 0.75){

          if(vSubC_3.size() > 0) {
            pAddJet_JECDown->lsfC_3_sj1_pt = vSubC_3[0].pt();
            pAddJet_JECDown->lsfC_3_sj1_eta = vSubC_3[0].eta();
            pAddJet_JECDown->lsfC_3_sj1_phi = vSubC_3[0].phi();
            pAddJet_JECDown->lsfC_3_sj1_m = vSubC_3[0].m();
          }

          AddJets_JECDown.push_back(pAddJet_JECDown);
        }
	AddJets_noLSF_JECDown.push_back(pAddJet_JECDown);
      }
    }
    if( jetPtJERUp > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*PUPPIweight(jetCorrPtSmear,iJet->eta(),m_isMC) > 40){
        baconhep::TAddJet* pAddJet_JERUp = new baconhep::TAddJet();

        pAddJet_JERUp->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JERUp->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JERUp->pT = jetPtJERUp;
        pAddJet_JERUp->E  = jetEJERUp;
        pAddJet_JERUp->eta = iJet->eta();
        pAddJet_JERUp->phi = iJet->phi();
        pAddJet_JERUp->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        pAddJet_JERUp->SDmassCorr = PUPPIweight(pAddJet_JERUp->pT, pAddJet_JERUp->eta,m_isMC);

        std::vector<fastjet::PseudoJet> vSubCInc; pAddJet_JERUp->lsfCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2);
        std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet_JERUp->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);

        if(pAddJet_JERUp->lsfC_3 > 0.75){

          if(vSubC_3.size() > 0) {
            pAddJet_JERUp->lsfC_3_sj1_pt = vSubC_3[0].pt();
            pAddJet_JERUp->lsfC_3_sj1_eta = vSubC_3[0].eta();
            pAddJet_JERUp->lsfC_3_sj1_phi = vSubC_3[0].phi();
            pAddJet_JERUp->lsfC_3_sj1_m = vSubC_3[0].m();
          }

          AddJets_JERUp.push_back(pAddJet_JERUp);
        }
	AddJets_noLSF_JERUp.push_back(pAddJet_JERUp);
      }
    }
    if( jetPtJERDown > 200 ){
      if(iJet->userFloat("ak8PFJetsPuppiSoftDropMass")*PUPPIweight(jetCorrPtSmear,iJet->eta(),m_isMC) > 40){
        baconhep::TAddJet* pAddJet_JERDown = new baconhep::TAddJet();

        pAddJet_JERDown->tau1 = iJet->userFloat(m_jettiness + std::string(":tau1"));
        pAddJet_JERDown->tau2 = iJet->userFloat(m_jettiness + std::string(":tau2"));
        pAddJet_JERDown->pT = jetPtJERDown;
        pAddJet_JERDown->E  = jetEJERDown;
        pAddJet_JERDown->eta = iJet->eta();
        pAddJet_JERDown->phi = iJet->phi();
        pAddJet_JERDown->SDmass = iJet->userFloat("ak8PFJetsPuppiSoftDropMass");
        pAddJet_JERDown->SDmassCorr = PUPPIweight(pAddJet_JERDown->pT, pAddJet_JERDown->eta,m_isMC);

        std::vector<fastjet::PseudoJet> vSubCInc; pAddJet_JERDown->lsfCInc = JetTools::lsf(lClusterParticles, vSubCInc, lepCPt, lepCEta, lepCPhi, lepCId, 0.2, 2);
        std::vector<fastjet::PseudoJet> vSubC_3;  pAddJet_JERDown->lsfC_3 = JetTools::lsf(lClusterParticles, vSubC_3, lepCPt, lepCEta, lepCPhi, lepCId, 2.0, 3);

        if(pAddJet_JERDown->lsfC_3 > 0.75){

          if(vSubC_3.size() > 0) {
            pAddJet_JERDown->lsfC_3_sj1_pt = vSubC_3[0].pt();
            pAddJet_JERDown->lsfC_3_sj1_eta = vSubC_3[0].eta();
            pAddJet_JERDown->lsfC_3_sj1_phi = vSubC_3[0].phi();
            pAddJet_JERDown->lsfC_3_sj1_m = vSubC_3[0].m();
          }

          AddJets_JERDown.push_back(pAddJet_JERDown);
        }
	AddJets_noLSF_JERDown.push_back(pAddJet_JERDown);
      }
    }
  } 
  std::sort(AddJets.begin(),AddJets.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JECUp.begin(),AddJets_JECUp.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JECDown.begin(),AddJets_JECDown.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JERUp.begin(),AddJets_JERUp.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_JERDown.begin(),AddJets_JERDown.end(),::wrTools::compareEtCandidatePointerTAddJet);

  std::sort(AddJets_noLSF.begin(),AddJets_noLSF.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_noLSF_JECUp.begin(),AddJets_noLSF_JECUp.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_noLSF_JECDown.begin(),AddJets_noLSF_JECDown.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_noLSF_JERUp.begin(),AddJets_noLSF_JERUp.end(),::wrTools::compareEtCandidatePointerTAddJet);
  std::sort(AddJets_noLSF_JERDown.begin(),AddJets_noLSF_JERDown.end(),::wrTools::compareEtCandidatePointerTAddJet);

  myEvent.myAddJetCandsHighPt = AddJets;
  myEvent.myAddJetCandsHighPt_JECUp = AddJets_JECUp;
  myEvent.myAddJetCandsHighPt_JECDown = AddJets_JECDown;
  myEvent.myAddJetCandsHighPt_JERUp = AddJets_JERUp;
  myEvent.myAddJetCandsHighPt_JERDown = AddJets_JERDown;

  myEvent.myAddJetCandsHighPt_noLSF = AddJets_noLSF;
  myEvent.myAddJetCandsHighPt_noLSF_JECUp = AddJets_noLSF_JECUp;
  myEvent.myAddJetCandsHighPt_noLSF_JECDown = AddJets_noLSF_JECDown;
  myEvent.myAddJetCandsHighPt_noLSF_JERUp = AddJets_noLSF_JERUp;
  myEvent.myAddJetCandsHighPt_noLSF_JERDown = AddJets_noLSF_JERDown;

  myEvent.ak8jetCands = AddJets.size();

  std::cout << "Number of Candidate Jets with LSF: " << AddJets.size() << std::endl;
  std::cout << "Number of Candidate Jets without LSF: " << AddJets_noLSF.size() << std::endl;

  //double dPhi = 0.;
  double dR   = 0.;
  int nDaughterContainedPtLeadJet = 0;
  int nDaughterContainedSubLeadJet = 0;
  int nDaughterContained2ndSubLeadJet = 0;
  int nDaughterContained3rdSubLeadJet = 0;
  int capturedBothDaughtersInSingleJet = -1;
  if(AddJets.size() > 0){
    for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
    //  dPhi=fabs(::wrTools::dPhi(AddJets[0]->phi, myEvent.genWRDaughters[i]->phi()));
      dR = sqrt(::wrTools::dR2(AddJets[0]->eta, myEvent.genWRDaughters[i]->eta(), AddJets[0]->phi, myEvent.genWRDaughters[i]->phi()));
      if (dR<0.4)nDaughterContainedPtLeadJet++;
      myEvent.daughterClusterVector = myEvent.daughterClusterVector + myEvent.genWRDaughters[i]->p4();
    }
    if(AddJets.size() > 1){
      for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
     //   dPhi=fabs(::wrTools::dPhi(AddJets[1]->phi, myEvent.genWRDaughters[i]->phi()));
        dR = sqrt(::wrTools::dR2(AddJets[1]->eta, myEvent.genWRDaughters[i]->eta(), AddJets[1]->phi, myEvent.genWRDaughters[i]->phi()));
	if (dR<0.4)nDaughterContainedSubLeadJet++;
      }
    }
    if(AddJets.size() > 2){
      for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
    //    dPhi=fabs(::wrTools::dPhi(AddJets[2]->phi, myEvent.genWRDaughters[i]->phi()));
        dR = sqrt(::wrTools::dR2(AddJets[2]->eta, myEvent.genWRDaughters[i]->eta(), AddJets[2]->phi, myEvent.genWRDaughters[i]->phi()));
        if (dR<0.4)nDaughterContained2ndSubLeadJet++;
      }
    }
    if(AddJets.size() > 3){
      for(int i = 0; i < int(myEvent.genWRDaughters.size()); i++){
   //     dPhi=fabs(::wrTools::dPhi(AddJets[3]->phi, myEvent.genWRDaughters[i]->phi()));
        dR = sqrt(::wrTools::dR2(AddJets[3]->eta, myEvent.genWRDaughters[i]->eta(), AddJets[3]->phi, myEvent.genWRDaughters[i]->phi()));
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

  if (myEvent.genSecondMuon != NULL && myEvent.genWRDaughters.size() > 1) {
    std::cout << "DR CALC" << std::endl;
    double jetPhi = myEvent.daughterClusterVector.phi();
    double muPhi  = myEvent.genSecondMuon->phi();
    double jetEta = myEvent.daughterClusterVector.eta();
    double muEta  = myEvent.genSecondMuon->eta();
 //   double dPhi = 0.0;
    double dR   = 0.0;
 //   dPhi = fabs(::wrTools::dPhi(jetPhi, muPhi));
    dR = sqrt( ::wrTools::dR2(jetEta, muEta, jetPhi, muPhi));

    myEvent.secondMuonWRjetdR = dR;
  }

  delete fJetUnc;
  return true;
}
double cmsWRextension::PUPPIweight(double puppipt, double puppieta, bool isMC){

  double totalWeight=1;

  if(isMC){
    double genCorr=1;
    double recoCorr=1;

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
    }
    totalWeight = genCorr * recoCorr;
  }else{
    totalWeight = 1.0;
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
bool cmsWRextension::ZFinder(const edm::Event& iEvent, eventBits& myEvent)
{
  std::cout << "LOOKING FOR A Z BOSON" << std::endl;
  edm::Handle<std::vector<reco::GenParticle>> genParticles;
  iEvent.getByToken(m_genParticleToken, genParticles);
  const reco::GenParticle* genZcand = NULL;
  std::vector<const reco::GenParticle*> genZdaughters;
  int nFound = 0;
  for(std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
    if(iParticle->pdgId() == 23) {
      std::cout << "FOUND A Z BOSON IN STATE: " << iParticle->status()<< std::endl;
    }
    if(!iParticle->isHardProcess()) continue;
    if(iParticle->status() == 21) continue;
    if(iParticle->pdgId() != 23 && iParticle->mother()->pdgId() != 23) continue;
    if(iParticle->pdgId() == 23) {
      std::cout << "FOUND A Z BOSON" << std::endl;
      std::cout << "STATUS: " << iParticle->status() << std::endl;
      nFound++;
      genZcand = &(*iParticle);
      continue;
    }
    std::cout << "FOUND Z DAUGHTER" << std::endl; 
    genZdaughters.push_back(&(*(iParticle)));
  }
  std::cout << "FOUND Zs:" << nFound << std::endl;
  if(nFound != 1) {
    std::cout << "NO Z FOUND:" << std::endl;
    for(std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
      if(iParticle->pdgId() == 23)  std::cout << "BAD Z FOUND" << std::endl;
      if(iParticle->status() == 22) std::cout << "EXCHANGE PARTICLE: " << iParticle->pdgId() << std::endl;
      if(!iParticle->isHardProcess()) continue;
      std::cout << "PDGID: " << iParticle->pdgId() << " STATUS: " << iParticle->status() << " MOTHER: " << iParticle->mother()->pdgId() << std::endl;
      if(abs(iParticle->pdgId()) == 11 || abs(iParticle->pdgId()) == 13 || abs(iParticle->pdgId()) == 15) {
        genZdaughters.push_back(&(*iParticle));
      }
    }
    
  } else {
    myEvent.myZ = genZcand;
  }
    //AS A CROSS-CHECK WE NOW GET THE Z DAUGHTERS
  std::cout << "FOUND NZ DAUGHTERS: " << genZdaughters.size() << std::endl;
  if(genZdaughters.size() != 2) return false; 

  double ZdaughterPt   = (genZdaughters[0]->p4() + genZdaughters[1]->p4()).pt();
  double ZdaughterMass = (genZdaughters[0]->p4() + genZdaughters[1]->p4()).mass();

  if(nFound == 1) {
    double ZptDiff   = genZcand->pt()   - ZdaughterPt;
    double ZmassDiff = genZcand->mass() - ZdaughterMass;

    std::cout << "Z kinematic diff (pt):   " << ZptDiff   << std::endl;
    std::cout << "Z kinematic diff (mass): " << ZmassDiff << std::endl;
  }

  myEvent.genZmass = ZdaughterMass;
  myEvent.genZpt   = ZdaughterPt;
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
//  int flavor           = 0;

  std::vector<const reco::GenParticle*> genLeptons;
  std::vector<const reco::GenParticle*> WRDaughters;

  int GENnDaughters = 0;

  for(std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
    if(!iParticle->isHardProcess()) continue;
    if(iParticle->status() == 21) continue;

    if(abs(iParticle->pdgId()) == 13) {
      nMuons++;
      genLeptons.push_back(&(*iParticle));

      if(abs(iParticle->mother(0)->pdgId()) == 9900014){
        std::cout << "FOUND MUON NEUTRINO DECAY MUON" << std::endl;
        myEvent.genSecondMuon = &(*iParticle);
      }
    }
    if(abs(iParticle->pdgId()) == 9900014 || iParticle->pdgId() == 9900012 || iParticle->pdgId() == 9900016){
      std::cout << "iParticle->E: " << iParticle->energy() << std::endl;
      myEvent.NRenergy = iParticle->energy();
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

  //double dPhi = 0.;
  double dR_Daughters = -1.0;
  if (WRDaughters.size() > 1){
    //dPhi=fabs(::wrTools::dPhi(WRDaughters[0]->phi(), WRDaughters[1]->phi()));
    dR_Daughters = sqrt(::wrTools::dR2(WRDaughters[0]->eta(), WRDaughters[1]->eta(), WRDaughters[0]->phi(), WRDaughters[1]->phi()));
    
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
    myEvent.genVtx->SetXYZ( x, y, z );
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
//  myEvent.myEventFlavor = flavor;

  if (genLeptons.size() > 0)
    return true;
  else
    return false;

}
bool cmsWRextension::preSelectGen(const edm::Event& iEvent, eventBits& myEvent)
{
  if(myEvent.myGenMuons.size() < 2 || myEvent.myGenPartons.size() < 2) {
    std::cout << "EVENT FAILS, HARD PROCESS NOT FOUND " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myGenJets.size()<<" partons"<< std::endl;
    return false;
  }
  //CHECK THE INDIVIDUAL OBJECT KINEMATICS PASS
  const reco::GenParticle* mu1 =  myEvent.firstMuon;
  const reco::GenParticle* mu2 =  myEvent.secondMuon;
  const reco::GenParticle*  parton1 = myEvent.myGenPartons[0];
  const reco::GenParticle*  parton2 = myEvent.myGenPartons[1];

  if(fabs(mu1->eta()) > 2.4 ) return false;
  if(fabs(mu2->eta()) > 2.4 ) return false;
  if(fabs(parton1->eta()) > 2.4 ) return false;
  if(fabs(parton2->eta()) > 2.4 ) return false;

  if( mu1->pt() < 40 ) return false;
  if( mu2->pt() < 10 ) return false;
  if( parton1->pt() < 40 ) return false;
  if( parton2->pt() < 40 ) return false;

  return true;

}
bool cmsWRextension::signalGENidentifier(const edm::Event& iEvent, eventBits& myEvent)
{
  std::cout << "Starting signalGENidentifier" << std::endl; 
  edm::Handle<std::vector<reco::GenParticle>> genParticles;
  iEvent.getByToken(m_genParticleToken, genParticles);

  std::vector<const reco::GenParticle*> myGenPartons;
  std::vector<const reco::GenParticle*> myGenMuons;
  std::vector<const reco::GenParticle*> myGenElectrons;

  std::vector<const reco::GenParticle*> myGenParticles;

  myEvent.neutrinoDecays = 0;

  //LOOP OVER GEN PARTICLES
  //9900024 WR 9900014 NRu 9900012 NRe 9900016 NRt
  for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
//    if(iParticle->isHardProcess() && iParticle->status() != 21) myGenParticles.push_back(&(*iParticle));
//    if(iParticle->isHardProcess()) std::cout << "Particle of type: "<<iParticle->pdgId() <<" isHardProcess and has status: "<<iParticle->status()<<std::endl;
//    if(iParticle->mother()) { if(::wrTools::particleIsFromABS(&(*iParticle),24)) continue; }//no W-SM mothered particles
//    if((iParticle->isHardProcess() && iParticle->status() == 22) && abs(iParticle->pdgId()) == 6) myGenPartons.push_back(&(*iParticle)); //KEEP TOPS, NOT Qs FROM TOPS
//    if((iParticle->isHardProcess() && iParticle->status() == 23) && (iParticle->pdgId() <= 6) && (iParticle->pdgId() >= -6) && !(::wrTools::particleIsFromABS(&(*iParticle),6))) myGenPartons.push_back(&(*iParticle));
//    if(iParticle->fromHardProcessFinalState() && abs(iParticle->pdgId()) == 13) myGenMuons.push_back(&(*iParticle));
    if( ! iParticle->isHardProcess() ) continue;  //ONLY HARD PROCESS AND NOT INCOMING
    if( iParticle->status() == 21 )    continue;
    if(iParticle->pdgId() == 13) {
        std::cout << "MUON HAS MOTHER WITH ID: " << iParticle->mother()->pdgId() << std::endl;
    }


    if( iParticle->mother() ) {

      if ( abs(iParticle->mother()->pdgId()) == 9900024 || abs(iParticle->mother()->pdgId()) == 34) {
        const reco::Candidate* WR = iParticle->mother();
        myEvent.WR = WR;
        std::cout << "WR DAUGHTERS: "<<iParticle->pdgId()<<std::endl;
        if ( abs(iParticle->pdgId()) == 13 ) { //it's the first muon
          std::cout << "FIRST MUON" << std::endl;
          //const reco::GenParticle* firstMuon = ::wrTools::evolveParticle(&(*(iParticle)));
          const reco::GenParticle* firstMuon = &(*(iParticle));
          myEvent.firstMuon = firstMuon;
          myGenMuons.push_back(firstMuon);
        }
        if ( abs(iParticle->pdgId()) == 11 ) { //it's the first electron
          std::cout << "FIRST ELECTRON" << std::endl;
          const reco::GenParticle* firstElectron = &(*(iParticle));
          myEvent.firstElectron = firstElectron;
          myGenElectrons.push_back(firstElectron);
        }
      }
      if ( abs(iParticle->pdgId()) == 9900024 ||  abs(iParticle->mother()->pdgId()) == 34) {
        std::cout << "WR with mass: "<< iParticle->mass() << std::endl;
      }
      if ( abs(iParticle->pdgId()) == 9900012 || abs(iParticle->pdgId()) == 9900014 || abs(iParticle->pdgId()) == 9900016) {
        std::cout << "NR CAND: " << iParticle->pdgId() << " WITH MOTHER: " << iParticle->mother()->pdgId() << std::endl;
        if ( abs(iParticle->mother()->pdgId()) == 9900012 || abs(iParticle->mother()->pdgId()) == 9900014 || abs(iParticle->mother()->pdgId()) == 9900016) {
          std::cout << "NEUTRINO DECAY EVENT" << std::endl;
          myEvent.neutrinoDecays = 1;
        }
      }
      if ( abs(iParticle->mother()->pdgId()) == 9900012 || abs(iParticle->mother()->pdgId()) == 9900014 || abs(iParticle->mother()->pdgId()) == 9900016) {
        const reco::Candidate* NR = iParticle->mother();
        myEvent.NR = NR;
        std::cout << "NR DAUGHTER: " << abs(iParticle->pdgId()) << std::endl;        
        if ( abs(iParticle->pdgId()) == 13 ) {
          std::cout << "SECOND MUON" << std::endl;
     //     const reco::GenParticle* secondMu = ::wrTools::evolveParticle(&(*(iParticle)));
          const reco::GenParticle* secondMu = &(*(iParticle));
          myEvent.secondMuon = secondMu; 
          myGenMuons.push_back(secondMu);
        }
        if ( abs(iParticle->pdgId()) == 11 ) {
          std::cout << "SECOND ELECTRON" << std::endl;
          const reco::GenParticle* secondEl = &(*(iParticle));
          myEvent.secondElectron = secondEl;
          myGenElectrons.push_back(secondEl);
        }
        if ( abs(iParticle->pdgId()) <= 5 ) { //it's a quark
          std::cout << "VIRTUAL WR DAUGHTERS: QUARKS"<<std::endl;
//          const reco::GenParticle* parton = ::wrTools::evolveParticle(&(*(iParticle)));
          const reco::GenParticle* parton = &(*(iParticle));
          myGenPartons.push_back(parton);
        }
        if ( abs(iParticle->pdgId()) == 6 ) {
          std::cout << "VIRTUAL WR DAUGHTERS: TOP QUARKS"<<std::endl;
          myGenPartons.push_back(&(*(iParticle)));
        }
      }
      
    }
  }
  std::cout << "FOUND N GEN PARTONS: " << myGenPartons.size() << endl;
  //GET THE CHARACTERIZATION INTEGER
//////////////////  ::wrTools::characterizeEvent(myGenParticles);

  //CHECK THAT THE EVENT MAKES SENSE
  if (myGenPartons.size() == 2 && (myGenMuons.size() == 2)) {
    std::cout << "KINEMATIC CLOSURE CHECK" << std::endl;
    std::cout << "NR MASS:" << myEvent.NR->mass() << std::endl;
    //SORT GEN MUONS AND PARTONS BY ET
    std::sort(myGenPartons.begin(),myGenPartons.end(),::wrTools::compareEtGenParticlePointer);
    std::sort(myGenMuons.begin(),myGenMuons.end(),::wrTools::compareEtGenParticlePointer);

    myEvent.parton1EtVal = myGenPartons[0]->et();
    myEvent.parton1EtaVal = myGenPartons[0]->eta();
    myEvent.parton1PhiVal = myGenPartons[0]->phi();
    myEvent.parton2EtVal = myGenPartons[1]->et();
    myEvent.parton2EtaVal = myGenPartons[1]->eta();
    myEvent.parton2PhiVal = myGenPartons[1]->phi();

    if(myGenMuons.size() == 2){
      myEvent.muon1EtVal  = myEvent.firstMuon->et();
      myEvent.muon1EtaVal = myEvent.firstMuon->eta();
      myEvent.muon1PhiVal = myEvent.firstMuon->phi();
      myEvent.muon2EtVal  = myEvent.secondMuon->et();
      myEvent.muon2EtaVal = myEvent.secondMuon->eta();
      myEvent.muon2PhiVal = myEvent.secondMuon->phi();
    }

  }

  myEvent.myGenPartons = myGenPartons;
  myEvent.myGenMuons = myGenMuons;

  if (myEvent.neutrinoDecays != 0) return false;

  return true;
}  
//  //NOW WE'LL CHECK IF IT PASSES SOME BASIC GEN LEVEL CUTS
//  if(!myEvent.passesGenCuts()) {
//    std::cout << "ERROR! SKIPPING EVENT, LEADING PARTONS AND MUONS NOT OVER 20 GEV"<< std::endl;
//    return false;
//  }
bool cmsWRextension::genJetAnalyzer(const edm::Event& iEvent, eventBits& myEvent) {
  //NOW THAT THE GEN MUONS AND PARTONS ARE SORTED OUT, WE'LL MATCH A GENJET TO EACH PARTON
  //FIRST WE'LL GET THE GENJETS THAT HAVE AT LEAST 10 GEV ET
  
  if (myEvent.myGenPartons.size() < 2) {
    std::cout << "UH OH CAN'T ANALYZE JETS, NEVER FOUND PARTONS" << std::endl;
    return false;
  }
  edm::Handle<std::vector<reco::GenJet>> genJets;
  iEvent.getByToken(m_genJetsToken, genJets);

  edm::Handle<std::vector<reco::GenJet>> AK8GenJets;
  iEvent.getByToken(m_AK8genJetsToken, AK8GenJets);

  std::vector<const reco::GenJet*> myGenJets;
  std::vector<const reco::GenJet*> myAK8GenJets;

  //HERE WE COMPARE OUR EVENTS
  float partonJetMatchDR = .4;
  float partonAK8JetMatchDR = .8;
  double foundFirst = partonJetMatchDR;
  double foundSecond = partonJetMatchDR;
  const reco::GenJet* firstPartonGenJet=0; 
  const reco::GenJet* secondPartonGenJet=0; 
  const reco::GenJet* firstPartonAK8GenJet=0; 
  const reco::GenJet* secondPartonAK8GenJet=0; 

  for (std::vector<reco::GenJet>::const_iterator iJet = genJets->begin(); iJet != genJets->end(); iJet++) {
    if (iJet->et()<20.0) continue;
    float match1=sqrt(deltaR2(*iJet,*(myEvent.myGenPartons[0])));
    float match2=sqrt(deltaR2(*iJet,*(myEvent.myGenPartons[1])));
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
    float match1=sqrt(deltaR2(*iJet,*(myEvent.myGenPartons[0])));
    float match2=sqrt(deltaR2(*iJet,*(myEvent.myGenPartons[1])));
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
  
  
  return true;
}

bool cmsWRextension::passExtensionRECO_ZPeak(const edm::Event& iEvent, eventBits& myRECOevent) {

  std::sort(myRECOevent.myMuonJetPairs_noLSF.begin(),myRECOevent.myMuonJetPairs_noLSF.end(),::wrTools::comparePairMassPointerTAddJet);
  TLorentzVector JetVector_temp = TLorentzVector();
  JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_noLSF[0].first->pT,myRECOevent.myMuonJetPairs_noLSF[0].first->eta,myRECOevent.myMuonJetPairs_noLSF[0].first->phi,myRECOevent.myMuonJetPairs_noLSF[0].first->SDmass*myRECOevent.myMuonJetPairs_noLSF[0].first->SDmassCorr);

  math::XYZTLorentzVector JetVector;
  JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

  myRECOevent.leadAK8JetMuonMassVal_noLSF = (JetVector + myRECOevent.myMuonJetPairs_noLSF[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();

  return true;
}
bool cmsWRextension::passBoostRECO(const edm::Event& iEvent, eventBits& myRECOevent) { //LOOP OVE. JET MUON PAIRS AND TAKE THE HIGHEST MASS ONE

  std::sort(myRECOevent.myMuonJetPairs.begin(),myRECOevent.myMuonJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);

//  double dR_AK8jet_muon2 = sqrt(::wrTools::dR2(myRECOevent.resolvedANAMuons[1]->eta(),myRECOevent.myMuonJetPairs[0].first->eta,myRECOevent.resolvedANAMuons[1]->phi(),myRECOevent.myMuonJetPairs[0].first->phi));
//  std::cout << "dR_AK8jet_muon2: " << dR_AK8jet_muon2 << std::endl;
//  if (dR_AK8jet_muon2 > 0.4) return false;

  TLorentzVector JetVector_temp = TLorentzVector();
  JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs[0].first->pT,myRECOevent.myMuonJetPairs[0].first->eta,myRECOevent.myMuonJetPairs[0].first->phi,myRECOevent.myMuonJetPairs[0].first->SDmass*myRECOevent.myMuonJetPairs[0].first->SDmassCorr);

  math::XYZTLorentzVector JetVector;
  JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

  myRECOevent.leadAK8JetMuonMassVal = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  myRECOevent.leadAK8JetMuonPtVal   = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).pt();
  myRECOevent.leadAK8JetMuonEtaVal  = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).eta();
  myRECOevent.leadAK8JetMuonPhiVal  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs[0].first->phi, myRECOevent.myMuonJetPairs[0].second->phi())));
  myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs[0].second->pt();
  myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs[0].second->phi();
  myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs[0].second->eta();
  myRECOevent.selectedJetPt   = myRECOevent.myMuonJetPairs[0].first->pT;
  myRECOevent.selectedJetPhi  = myRECOevent.myMuonJetPairs[0].first->phi;
  myRECOevent.selectedJetEta  = myRECOevent.myMuonJetPairs[0].first->eta;
  myRECOevent.selectedJetMass = myRECOevent.myMuonJetPairs[0].first->SDmass*myRECOevent.myMuonJetPairs[0].first->SDmassCorr;
  myRECOevent.selectedJetLSF3 = myRECOevent.myMuonJetPairs[0].first->lsfC_3;
  myRECOevent.selectedJetMaxSubJetCSV = myRECOevent.myMuonJetPairs[0].first->maxSubJetCSV;
  myRECOevent.selectedJetEnergy =  myRECOevent.myMuonJetPairs[0].first->E;
  myRECOevent.selectedJetEnergyUncorr =  myRECOevent.myMuonJetPairs[0].first->UncorrectedE;

  int nHighPtMuonsOutsideJet = 0;
  if(myRECOevent.muonCands > 1){
    myRECOevent.secondHighPtMuonPt = myRECOevent.my2ndHighPtMuonCand->pt();
    myRECOevent.secondHighPtMuonPhi = myRECOevent.my2ndHighPtMuonCand->phi();
    myRECOevent.secondHighPtMuonEta = myRECOevent.my2ndHighPtMuonCand->eta();
    //double dPhi = fabs( ::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.secondHighPtMuonPhi));
    double dR = sqrt(::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.secondHighPtMuonEta, myRECOevent.selectedJetPhi, myRECOevent.secondHighPtMuonPhi));
    myRECOevent.selectedJetSecondHighPtMuonDR = dR;
  }
  for(int j=0; j<myRECOevent.muonCands; j++){
    //double dPhi = fabs( ::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
    double dR = sqrt(::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.myMuonCandsHighPt[j]->eta(), myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
    if(j==0)myRECOevent.leadAK8JetMuonDR = dR;
    if ( abs(dR) > 0.8) nHighPtMuonsOutsideJet++;
  }
  myRECOevent.nHighPtMuonsOutsideJet = nHighPtMuonsOutsideJet;

 // myRECOevent.secondGENMuon_selMuondR
  if (myRECOevent.genSecondMuon != NULL) {
    //double dPhi = fabs( ::wrTools::dPhi(myRECOevent.genSecondMuon->phi(), myRECOevent.selectedMuonPhi));

    double secondGENMuon_selMuondR = sqrt(::wrTools::dR2(myRECOevent.genSecondMuon->eta(), myRECOevent.selectedMuonEta, myRECOevent.genSecondMuon->phi(), myRECOevent.selectedMuonPhi));
    myRECOevent.secondGENMuon_selMuondR = secondGENMuon_selMuondR;
  }

  if(myRECOevent.myMuonJetPairs[0].first->tau1==0){
    myRECOevent.selectedJetTau21 = -9999.;
  }else{
    myRECOevent.selectedJetTau21 = (myRECOevent.myMuonJetPairs[0].first->tau2)/(myRECOevent.myMuonJetPairs[0].first->tau1);
  }

  double MaxDR_genDaughter_CandJet = -1.0;
  //double dPhi = 0.;
  int nDaughterInSelectedJet = 0;
  double dR = 0.;
  if(m_isMC){
    for(int i = 0; i < int(myRECOevent.genWRDaughters.size()); i++){
    //  dPhi=fabs(::wrTools::dPhi(myRECOevent.myMuonJetPairs[0].first->phi, myRECOevent.genWRDaughters[i]->phi()));
      dR = sqrt(::wrTools::dR2(myRECOevent.myMuonJetPairs[0].first->eta, myRECOevent.myMuonJetPairs[0].first->eta, myRECOevent.myMuonJetPairs[0].first->phi, myRECOevent.genWRDaughters[i]->phi()));
      if( dR > MaxDR_genDaughter_CandJet){
	MaxDR_genDaughter_CandJet = dR;
      }
      if(dR < 0.4){
	nDaughterInSelectedJet++;
      }
    }
  }
  std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;

  if( myRECOevent.leadAK8JetMuonMassVal < 800 ) {myRECOevent.boostedRECOmassAbove600 = false; }
  else { myRECOevent.boostedRECOmassAbove600 = true; }


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
    selectedIncorrectJetMass = myRECOevent.myMuonJetPairs[0].first->SDmass*myRECOevent.myMuonJetPairs[0].first->SDmassCorr;
    JetWithDaughtersMass = myRECOevent.myAddJetCandsHighPt[myRECOevent.JetContainingBothDaughters]->SDmass*myRECOevent.myAddJetCandsHighPt[myRECOevent.JetContainingBothDaughters]->SDmassCorr;
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

  int nHighPtMuonsOutsideJet = 0;
  TLorentzVector JetVector_temp = TLorentzVector();
  if(myRECOevent.myMuonJetPairs.size() > 0){
	myRECOevent.cutProgress++;
  	std::sort(myRECOevent.myMuonJetPairs.begin(),myRECOevent.myMuonJetPairs.end(),::wrTools::comparePairMassPointerTAddJet);
  	JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs[0].first->pT,myRECOevent.myMuonJetPairs[0].first->eta,myRECOevent.myMuonJetPairs[0].first->phi,myRECOevent.myMuonJetPairs[0].first->SDmass*myRECOevent.myMuonJetPairs[0].first->SDmassCorr);

  	math::XYZTLorentzVector JetVector;
  	JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

	std::cout << "myRECOevent.leadBoostMuonScale[0]: " << myRECOevent.leadBoostMuonScale[0] << std::endl;
	TLorentzVector MuonVector = TLorentzVector();
        MuonVector.SetPtEtaPhiM(myRECOevent.myMuonJetPairs[0].second->pt()*myRECOevent.leadBoostMuonScale[0],myRECOevent.myMuonJetPairs[0].second->eta(),myRECOevent.myMuonJetPairs[0].second->phi(),myRECOevent.myMuonJetPairs[0].second->mass());

  	myRECOevent.leadAK8JetMuonMassVal = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
	if(m_isMC){
          myRECOevent.leadAK8JetMuonMassVal_MuResolUp = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()*myRECOevent.leadBoostMuonScale[1]).mass();
          myRECOevent.leadAK8JetMuonMassVal_MuResolDown = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()*myRECOevent.leadBoostMuonScale[2]).mass();
        }
	std::cout << "myRECOevent.leadAK8JetMuonMassVal: " << myRECOevent.leadAK8JetMuonMassVal << std::endl;
	std::cout << "AK8 Jet Vector(pT,eta,phi,m): (" << myRECOevent.myMuonJetPairs[0].first->pT << "," << myRECOevent.myMuonJetPairs[0].first->eta << "," << myRECOevent.myMuonJetPairs[0].first->phi << "," << JetVector_temp.M() << ")" << std::endl;
	std::cout << "Muon Vector(pT,eta,phi,m): (" << myRECOevent.myMuonJetPairs[0].second->p4()*myRECOevent.leadBoostMuonScale[0] << ")" << std::endl;
        std::cout << "MuonVector.p4(): " << MuonVector.X() << "," << MuonVector.Y() << "," << MuonVector.Z() << "," << MuonVector.T() << std::endl;
        std::cout << "Muon Vector(pT,eta,phi,m): (" << myRECOevent.myMuonJetPairs[0].second->pt() << "," << myRECOevent.myMuonJetPairs[0].second->eta() << "," << myRECOevent.myMuonJetPairs[0].second->phi()<< "," << myRECOevent.myMuonJetPairs[0].second->mass() << ")" << std::endl;
        std::cout << "MuonVector(pT,eta.phi,m): " << MuonVector.Pt() << "," << MuonVector.Eta() << "," << MuonVector.Phi() << "," << MuonVector.M() << std::endl;

  	myRECOevent.leadAK8JetMuonPtVal   = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).pt();
  	myRECOevent.leadAK8JetMuonEtaVal  = (JetVector + myRECOevent.myMuonJetPairs[0].second->p4()).eta();
  	myRECOevent.leadAK8JetMuonPhiVal  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs[0].first->phi, myRECOevent.myMuonJetPairs[0].second->phi())));
  	myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs[0].second->pt();
  	myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs[0].second->phi();
  	myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs[0].second->eta();
  	myRECOevent.selectedJetPt   = myRECOevent.myMuonJetPairs[0].first->pT;
  	myRECOevent.selectedJetPhi  = myRECOevent.myMuonJetPairs[0].first->phi;
  	myRECOevent.selectedJetEta  = myRECOevent.myMuonJetPairs[0].first->eta;
	myRECOevent.selectedJetLSF3 = myRECOevent.myMuonJetPairs[0].first->lsfC_3;
  	myRECOevent.selectedJetMass = myRECOevent.myMuonJetPairs[0].first->SDmass*myRECOevent.myMuonJetPairs[0].first->SDmassCorr;

  	if(myRECOevent.myMuonJetPairs[0].first->tau1==0){
    	  myRECOevent.selectedJetTau21 = -9999.;
  	}else{
    	  myRECOevent.selectedJetTau21 = (myRECOevent.myMuonJetPairs[0].first->tau2)/(myRECOevent.myMuonJetPairs[0].first->tau1);
  	}

  	std::cout <<"RECO OBJECT MASS: "<<myRECOevent.leadAK8JetMuonMassVal << std::endl;

  	myRECOevent.myEventMass = myRECOevent.leadAK8JetMuonMassVal;

        for(int j=0; j<myRECOevent.muonCands; j++){
           //double dPhi = fabs(::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           double dR = sqrt( ::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.myMuonCandsHighPt[j]->eta(), myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
         
           if(j==0)myRECOevent.leadAK8JetMuonDR = dR;
           if ( abs(dR) > 0.8) nHighPtMuonsOutsideJet++;
        }
        myRECOevent.nHighPtMuonsOutsideJet = nHighPtMuonsOutsideJet;
	
  }

  nHighPtMuonsOutsideJet = 0;
  if(myRECOevent.myMuonJetPairs_JECUp.size() > 0){
  	JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JECUp[0].first->pT,myRECOevent.myMuonJetPairs_JECUp[0].first->eta,myRECOevent.myMuonJetPairs_JECUp[0].first->phi,myRECOevent.myMuonJetPairs_JECUp[0].first->SDmass*myRECOevent.myMuonJetPairs_JECUp[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
  	JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

  	myRECOevent.leadAK8JetMuonMassVal_JECUp = (JetVector + myRECOevent.myMuonJetPairs_JECUp[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  	myRECOevent.leadAK8JetMuonPtVal_JECUp   = (JetVector + myRECOevent.myMuonJetPairs_JECUp[0].second->p4()).pt();
  	myRECOevent.leadAK8JetMuonEtaVal_JECUp  = (JetVector + myRECOevent.myMuonJetPairs_JECUp[0].second->p4()).eta();
  	myRECOevent.leadAK8JetMuonPhiVal_JECUp  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JECUp[0].first->phi, myRECOevent.myMuonJetPairs_JECUp[0].second->phi())));
  	myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JECUp[0].second->pt();
  	myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JECUp[0].second->phi();
  	myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JECUp[0].second->eta();
  	myRECOevent.selectedJetPt_JECUp   = myRECOevent.myMuonJetPairs_JECUp[0].first->pT;
  	myRECOevent.selectedJetPhi_JECUp  = myRECOevent.myMuonJetPairs_JECUp[0].first->phi;
  	myRECOevent.selectedJetEta_JECUp  = myRECOevent.myMuonJetPairs_JECUp[0].first->eta;
  	myRECOevent.selectedJetMass_JECUp = myRECOevent.myMuonJetPairs_JECUp[0].first->SDmass*myRECOevent.myMuonJetPairs_JECUp[0].first->SDmassCorr;

  	if(myRECOevent.myMuonJetPairs_JECUp[0].first->tau1==0){
    	  myRECOevent.selectedJetTau21_JECUp = -9999.;
  	}else{
    	  myRECOevent.selectedJetTau21_JECUp = (myRECOevent.myMuonJetPairs_JECUp[0].first->tau2)/(myRECOevent.myMuonJetPairs_JECUp[0].first->tau1);
  	}

  	myRECOevent.myEventMass_JECUp = myRECOevent.leadAK8JetMuonMassVal_JECUp;
        for(int j=0; j<myRECOevent.muonCands; j++){
           //double dPhi = fabs(::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           double dR = sqrt( ::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.myMuonCandsHighPt[j]->eta(), myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           if(j==0)myRECOevent.leadAK8JetMuonDR = dR;
           if ( abs(dR) > 0.8) nHighPtMuonsOutsideJet++;
        }
        myRECOevent.nHighPtMuonsOutsideJet_JECUp = nHighPtMuonsOutsideJet;
  }

  nHighPtMuonsOutsideJet = 0;
  if(myRECOevent.myMuonJetPairs_JECDown.size() > 0){
  	JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JECDown[0].first->pT,myRECOevent.myMuonJetPairs_JECDown[0].first->eta,myRECOevent.myMuonJetPairs_JECDown[0].first->phi,myRECOevent.myMuonJetPairs_JECDown[0].first->SDmass*myRECOevent.myMuonJetPairs_JECDown[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
  	JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

  	myRECOevent.leadAK8JetMuonMassVal_JECDown = (JetVector + myRECOevent.myMuonJetPairs_JECDown[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  	myRECOevent.leadAK8JetMuonPtVal_JECDown   = (JetVector + myRECOevent.myMuonJetPairs_JECDown[0].second->p4()).pt();
  	myRECOevent.leadAK8JetMuonEtaVal_JECDown  = (JetVector + myRECOevent.myMuonJetPairs_JECDown[0].second->p4()).eta();
  	myRECOevent.leadAK8JetMuonPhiVal_JECDown  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JECDown[0].first->phi, myRECOevent.myMuonJetPairs_JECDown[0].second->phi())));
  	myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JECDown[0].second->pt();
  	myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JECDown[0].second->phi();
  	myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JECDown[0].second->eta();
  	myRECOevent.selectedJetPt_JECDown   = myRECOevent.myMuonJetPairs_JECDown[0].first->pT;
  	myRECOevent.selectedJetPhi_JECDown  = myRECOevent.myMuonJetPairs_JECDown[0].first->phi;
  	myRECOevent.selectedJetEta_JECDown  = myRECOevent.myMuonJetPairs_JECDown[0].first->eta;
  	myRECOevent.selectedJetMass_JECDown = myRECOevent.myMuonJetPairs_JECDown[0].first->SDmass*myRECOevent.myMuonJetPairs_JECDown[0].first->SDmassCorr;

  	if(myRECOevent.myMuonJetPairs_JECDown[0].first->tau1==0){
    	  myRECOevent.selectedJetTau21_JECDown = -9999.;
  	}else{
    	  myRECOevent.selectedJetTau21_JECDown = (myRECOevent.myMuonJetPairs_JECDown[0].first->tau2)/(myRECOevent.myMuonJetPairs_JECDown[0].first->tau1);
  	}

  	myRECOevent.myEventMass_JECDown = myRECOevent.leadAK8JetMuonMassVal_JECDown;
        for(int j=0; j<myRECOevent.muonCands; j++){
           //double dPhi = fabs(::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           double dR = sqrt( ::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.myMuonCandsHighPt[j]->eta(), myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           if(j==0)myRECOevent.leadAK8JetMuonDR = dR;
           if ( abs(dR) > 0.8) nHighPtMuonsOutsideJet++;
        }
        myRECOevent.nHighPtMuonsOutsideJet_JECDown = nHighPtMuonsOutsideJet;
  }

  nHighPtMuonsOutsideJet = 0;
  if(myRECOevent.myMuonJetPairs_JERUp.size() > 0){
  	JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JERUp[0].first->pT,myRECOevent.myMuonJetPairs_JERUp[0].first->eta,myRECOevent.myMuonJetPairs_JERUp[0].first->phi,myRECOevent.myMuonJetPairs_JERUp[0].first->SDmass*myRECOevent.myMuonJetPairs_JERUp[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
  	JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

  	myRECOevent.leadAK8JetMuonMassVal_JERUp = (JetVector + myRECOevent.myMuonJetPairs_JERUp[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  	myRECOevent.leadAK8JetMuonPtVal_JERUp   = (JetVector + myRECOevent.myMuonJetPairs_JERUp[0].second->p4()).pt();
  	myRECOevent.leadAK8JetMuonEtaVal_JERUp  = (JetVector + myRECOevent.myMuonJetPairs_JERUp[0].second->p4()).eta();
  	myRECOevent.leadAK8JetMuonPhiVal_JERUp  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JERUp[0].first->phi, myRECOevent.myMuonJetPairs_JERUp[0].second->phi())));
  	myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JERUp[0].second->pt();
  	myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JERUp[0].second->phi();
  	myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JERUp[0].second->eta();
  	myRECOevent.selectedJetPt_JERUp   = myRECOevent.myMuonJetPairs_JERUp[0].first->pT;
  	myRECOevent.selectedJetPhi_JERUp  = myRECOevent.myMuonJetPairs_JERUp[0].first->phi;
  	myRECOevent.selectedJetEta_JERUp  = myRECOevent.myMuonJetPairs_JERUp[0].first->eta;
  	myRECOevent.selectedJetMass_JERUp = myRECOevent.myMuonJetPairs_JERUp[0].first->SDmass*myRECOevent.myMuonJetPairs_JERUp[0].first->SDmassCorr;

  	if(myRECOevent.myMuonJetPairs_JERUp[0].first->tau1==0){
    	  myRECOevent.selectedJetTau21_JERUp = -9999.;
  	}else{
    	  myRECOevent.selectedJetTau21_JERUp = (myRECOevent.myMuonJetPairs_JERUp[0].first->tau2)/(myRECOevent.myMuonJetPairs_JERUp[0].first->tau1);
  	}

  	myRECOevent.myEventMass_JERUp = myRECOevent.leadAK8JetMuonMassVal_JERUp;
        for(int j=0; j<myRECOevent.muonCands; j++){
           //double dPhi = fabs(::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           double dR = sqrt( ::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.myMuonCandsHighPt[j]->eta(), myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           if(j==0)myRECOevent.leadAK8JetMuonDR = dR;
           if ( abs(dR) > 0.8) nHighPtMuonsOutsideJet++;
        }
        myRECOevent.nHighPtMuonsOutsideJet_JERUp = nHighPtMuonsOutsideJet;
  }

  nHighPtMuonsOutsideJet = 0;
  if(myRECOevent.myMuonJetPairs_JERDown.size() > 0){
  	JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_JERDown[0].first->pT,myRECOevent.myMuonJetPairs_JERDown[0].first->eta,myRECOevent.myMuonJetPairs_JERDown[0].first->phi,myRECOevent.myMuonJetPairs_JERDown[0].first->SDmass*myRECOevent.myMuonJetPairs_JERDown[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
  	JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

  	myRECOevent.leadAK8JetMuonMassVal_JERDown = (JetVector + myRECOevent.myMuonJetPairs_JERDown[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  	myRECOevent.leadAK8JetMuonPtVal_JERDown   = (JetVector + myRECOevent.myMuonJetPairs_JERDown[0].second->p4()).pt();
  	myRECOevent.leadAK8JetMuonEtaVal_JERDown  = (JetVector + myRECOevent.myMuonJetPairs_JERDown[0].second->p4()).eta();
  	myRECOevent.leadAK8JetMuonPhiVal_JERDown  = (fabs(reco::deltaPhi(myRECOevent.myMuonJetPairs_JERDown[0].first->phi, myRECOevent.myMuonJetPairs_JERDown[0].second->phi())));
  	myRECOevent.selectedMuonPt  = myRECOevent.myMuonJetPairs_JERDown[0].second->pt();
  	myRECOevent.selectedMuonPhi = myRECOevent.myMuonJetPairs_JERDown[0].second->phi();
  	myRECOevent.selectedMuonEta = myRECOevent.myMuonJetPairs_JERDown[0].second->eta();
  	myRECOevent.selectedJetPt_JERDown   = myRECOevent.myMuonJetPairs_JERDown[0].first->pT;
  	myRECOevent.selectedJetPhi_JERDown  = myRECOevent.myMuonJetPairs_JERDown[0].first->phi;
  	myRECOevent.selectedJetEta_JERDown  = myRECOevent.myMuonJetPairs_JERDown[0].first->eta;
  	myRECOevent.selectedJetMass_JERDown = myRECOevent.myMuonJetPairs_JERDown[0].first->SDmass*myRECOevent.myMuonJetPairs_JERDown[0].first->SDmassCorr;

  	if(myRECOevent.myMuonJetPairs_JERDown[0].first->tau1==0){
    	  myRECOevent.selectedJetTau21_JERDown = -9999.;
  	}else{
          myRECOevent.selectedJetTau21_JERDown = (myRECOevent.myMuonJetPairs_JERDown[0].first->tau2)/(myRECOevent.myMuonJetPairs_JERDown[0].first->tau1);
  	}

  	myRECOevent.myEventMass_JERDown = myRECOevent.leadAK8JetMuonMassVal_JERDown;
        for(int j=0; j<myRECOevent.muonCands; j++){
           //double dPhi = fabs(::wrTools::dPhi(myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           double dR = sqrt( ::wrTools::dR2(myRECOevent.selectedJetEta, myRECOevent.myMuonCandsHighPt[j]->eta(), myRECOevent.selectedJetPhi, myRECOevent.myMuonCandsHighPt[j]->phi()));
           if(j==0)myRECOevent.leadAK8JetMuonDR = dR;
           if ( abs(dR) > 0.8) nHighPtMuonsOutsideJet++;
        }
        myRECOevent.nHighPtMuonsOutsideJet_JERDown = nHighPtMuonsOutsideJet;
  }

  if(myRECOevent.myMuonJetPairs_noLSF.size() > 0){
        std::sort(myRECOevent.myMuonJetPairs_noLSF.begin(),myRECOevent.myMuonJetPairs_noLSF.end(),::wrTools::comparePairMassPointerTAddJet);
        JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_noLSF[0].first->pT,myRECOevent.myMuonJetPairs_noLSF[0].first->eta,myRECOevent.myMuonJetPairs_noLSF[0].first->phi,myRECOevent.myMuonJetPairs_noLSF[0].first->SDmass*myRECOevent.myMuonJetPairs_noLSF[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
        JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

        myRECOevent.leadAK8JetMuonMassVal_noLSF = (JetVector + myRECOevent.myMuonJetPairs_noLSF[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
	if(m_isMC){
          myRECOevent.leadAK8JetMuonMassVal_noLSF_MuResolUp = (JetVector + myRECOevent.myMuonJetPairs_noLSF[0].second->p4()*myRECOevent.leadBoostMuonScale[1]).mass();
          myRECOevent.leadAK8JetMuonMassVal_noLSF_MuResolDown = (JetVector + myRECOevent.myMuonJetPairs_noLSF[0].second->p4()*myRECOevent.leadBoostMuonScale[2]).mass();
	}
  }
  if(myRECOevent.myMuonJetPairs_noLSF_JECUp.size() > 0){
        JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_noLSF_JECUp[0].first->pT,myRECOevent.myMuonJetPairs_noLSF_JECUp[0].first->eta,myRECOevent.myMuonJetPairs_noLSF_JECUp[0].first->phi,myRECOevent.myMuonJetPairs_noLSF_JECUp[0].first->SDmass*myRECOevent.myMuonJetPairs_noLSF_JECUp[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
        JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

        myRECOevent.leadAK8JetMuonMassVal_noLSF_JECUp = (JetVector + myRECOevent.myMuonJetPairs_noLSF_JECUp[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  }
  if(myRECOevent.myMuonJetPairs_noLSF_JECDown.size() > 0){
        JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_noLSF_JECDown[0].first->pT,myRECOevent.myMuonJetPairs_noLSF_JECDown[0].first->eta,myRECOevent.myMuonJetPairs_noLSF_JECDown[0].first->phi,myRECOevent.myMuonJetPairs_noLSF_JECDown[0].first->SDmass*myRECOevent.myMuonJetPairs_noLSF_JECDown[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
        JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

        myRECOevent.leadAK8JetMuonMassVal_noLSF_JECDown = (JetVector + myRECOevent.myMuonJetPairs_noLSF_JECDown[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  }
  if(myRECOevent.myMuonJetPairs_noLSF_JERUp.size() > 0){
        JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_noLSF_JERUp[0].first->pT,myRECOevent.myMuonJetPairs_noLSF_JERUp[0].first->eta,myRECOevent.myMuonJetPairs_noLSF_JERUp[0].first->phi,myRECOevent.myMuonJetPairs_noLSF_JERUp[0].first->SDmass*myRECOevent.myMuonJetPairs_noLSF_JERUp[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
        JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

        myRECOevent.leadAK8JetMuonMassVal_noLSF_JERUp = (JetVector + myRECOevent.myMuonJetPairs_noLSF_JERUp[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
}
  if(myRECOevent.myMuonJetPairs_noLSF_JERDown.size() > 0){
        JetVector_temp.SetPtEtaPhiM(myRECOevent.myMuonJetPairs_noLSF_JERDown[0].first->pT,myRECOevent.myMuonJetPairs_noLSF_JERDown[0].first->eta,myRECOevent.myMuonJetPairs_noLSF_JERDown[0].first->phi,myRECOevent.myMuonJetPairs_noLSF_JERDown[0].first->SDmass*myRECOevent.myMuonJetPairs_noLSF_JERDown[0].first->SDmassCorr);

        math::XYZTLorentzVector JetVector;
        JetVector.SetXYZT(JetVector_temp.X(),JetVector_temp.Y(),JetVector_temp.Z(),JetVector_temp.T());

        myRECOevent.leadAK8JetMuonMassVal_noLSF_JERDown = (JetVector + myRECOevent.myMuonJetPairs_noLSF_JERDown[0].second->p4()*myRECOevent.leadBoostMuonScale[0]).mass();
  }

}
bool cmsWRextension::passResRECO(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES SELECTION CALL" << std::endl;
  std::cout << "RES LEPTON SELECTION CALL" << std::endl;
  if ( !resolvedMuonSelection(iEvent, myEvent) || !resolvedJetSelection(iEvent, myEvent) ) return false;
  resolvedElectronSelection(iEvent, myEvent);
  std::cout << "RES JET SELECTION CALL" << std::endl;
//  if ( !resolvedJetSelection(iEvent, myEvent) )  return false;

  if (myEvent.myResCandJets.size() < 2) {
    return false;
  } else {
    myEvent.ResCutProgress++;
    std::sort(myEvent.myResCandJets.begin(), myEvent.myResCandJets.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    double dR_pair = sqrt(::wrTools::dR2(myEvent.myResCandJets[0]->eta,myEvent.myResCandJets[1]->eta,myEvent.myResCandJets[0]->phi,myEvent.myResCandJets[1]->phi));
    std::cout<< "Jets dR_pair: " << dR_pair << std::endl;
    if (dR_pair < 0.4) return false;
     myEvent.resJetDR = dR_pair;
  }
  myEvent.ResCutProgress++;

  std::cout << "dR(AK4 jets, subleading muon) SELECTION" << std::endl;
  double dR_pair21 = sqrt(::wrTools::dR2(myEvent.resolvedANAMuons[1]->eta(),myEvent.myResCandJets[0]->eta,myEvent.resolvedANAMuons[1]->phi(),myEvent.myResCandJets[0]->phi));
  double dR_pair22 = sqrt(::wrTools::dR2(myEvent.resolvedANAMuons[1]->eta(),myEvent.myResCandJets[1]->eta,myEvent.resolvedANAMuons[1]->phi(),myEvent.myResCandJets[1]->phi));
  if (dR_pair21 < 0.4 || dR_pair22 < 0.4) return false;
  myEvent.ResCutProgress++;

  double dR_pair11 = sqrt(::wrTools::dR2(myEvent.resolvedANAMuons[0]->eta(),myEvent.myResCandJets[0]->eta,myEvent.resolvedANAMuons[0]->phi(),myEvent.myResCandJets[0]->phi));
  double dR_pair12 = sqrt(::wrTools::dR2(myEvent.resolvedANAMuons[0]->eta(),myEvent.myResCandJets[1]->eta,myEvent.resolvedANAMuons[0]->phi(),myEvent.myResCandJets[1]->phi));
  if (dR_pair11 < 0.4 || dR_pair12 < 0.4) return false;

  std::cout << "dR between muons" << std::endl;
  double dR_pair = sqrt(::wrTools::dR2(myEvent.resolvedANAMuons[0]->eta(),myEvent.resolvedANAMuons[1]->eta(),myEvent.resolvedANAMuons[0]->phi(),myEvent.resolvedANAMuons[1]->phi()));
  std::cout << "Muons dR_pair: " << dR_pair << std::endl;
  if (dR_pair < 0.4) return false;
  myEvent.ResCutProgress++;

  std::cout << "subleading Muon pT" << std::endl;
  if (myEvent.resolvedANAMuons[1]->pt()*myEvent.secondResMuonScale[0] < 53) return false;
  myEvent.ResCutProgress++;

  std::cout << "subleading Muon ISO" << std::endl;
  if (myEvent.resolvedANAMuons[1]->isolationR03().sumPt/myEvent.resolvedANAMuons[1]->pt() > 0.1) return false;
  myEvent.ResCutProgress++;

  std::cout << "RES OBJECT SELECTIONS PASSED" << std::endl;

  const pat::Muon* mu1 =  myEvent.resolvedANAMuons[0];
  const pat::Muon* mu2 =  myEvent.resolvedANAMuons[1];
//  TLorentzVector mu1;
//  mu1.SetPtEtaPhiM(myEvent.resolvedANAMuons[0]->pt()*myEvent.leadResMuonScale[0], myEvent.resolvedANAMuons[0]->eta(),myEvent.resolvedANAMuons[0]->phi(),myEvent.resolvedANAMuons[0]->mass());
//  TLorentzVector mu2;
//  mu2.SetPtEtaPhiM(myEvent.resolvedANAMuons[1]->pt()*myEvent.secondResMuonScale[0], myEvent.resolvedANAMuons[1]->eta(), myEvent.resolvedANAMuons[1]->phi(), myEvent.resolvedANAMuons[1]->mass());

  //MLL
  double mll = (mu1->p4()*myEvent.leadResMuonScale[0]+mu2->p4()*myEvent.secondResMuonScale[0]).mass();
  myEvent.resMLL = mll;
//  if (mll < 200) return false;  // 2017
//  if (mll < 150) return false;// korea
//  if (mll < 175) return false;// middle
  std::cout << "resMLL: " << myEvent.resMLL << std::endl;
//  std::cout << "RES MLL PASSED" << std::endl;
  myEvent.ResCutProgress++;

  //CHECK DR ASSOCIATIONS
    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets[0]->pT,myEvent.myResCandJets[0]->eta,myEvent.myResCandJets[0]->phi,myEvent.myResCandJets[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets[1]->pT,myEvent.myResCandJets[1]->eta,myEvent.myResCandJets[1]->phi,myEvent.myResCandJets[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());
    
  //CHECK 4 OBJECT MASS
  double resMass = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();
  double resMass_Up = (mu1->p4()*myEvent.leadResMuonScale[1] + mu2->p4()*myEvent.secondResMuonScale[1] + jet1 + jet2).mass();
  double resMass_Down = (mu1->p4()*myEvent.leadResMuonScale[2] + mu2->p4()*myEvent.secondResMuonScale[2] + jet1 + jet2).mass();

  if (resMass < 800){ myEvent.resolvedRECOmassAbove600 = false; }
  else { myEvent.resolvedRECOmassAbove600 = true; }

  myEvent.resolvedRECOmass = resMass; 
  myEvent.resolvedRECOmass_MuResolUp = resMass_Up;
  myEvent.resolvedRECOmass_MuResolDown = resMass_Down;

  myEvent.resSubleadMuJet1dR = dR_pair21;
  myEvent.resSubleadMuJet2dR = dR_pair22;

  myEvent.resLeadMuJet1dR = dR_pair11;
  myEvent.resLeadMuJet2dR = dR_pair12;


  return true;

}
std::vector<bool> cmsWRextension::passResRECO_Fast(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES SELECTION CALL" << std::endl;
  std::cout << "RES LEPTON SELECTION CALL" << std::endl;

  std::vector<bool> FalseReturn;
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);

  std::vector<bool> EachRegionReturn;

  resolvedElectronSelection(iEvent, myEvent);

  std::cout << "RES JET SELECTION CALL" << std::endl;
  if ( !resolvedJetSelection(iEvent, myEvent) || !resolvedMuonSelection(iEvent, myEvent) )  return FalseReturn;

//  if ( !resolvedMuonSelection(iEvent, myEvent) ) return FalseReturn;

  std::cout << "subleading Muon pT" << std::endl;
  if (myEvent.resolvedANAMuons[1]->tunePMuonBestTrack()->pt()*myEvent.secondResMuonScale[0] < 53) return FalseReturn;
  myEvent.ResCutProgress++;

  std::cout << "subleading Muon ISO" << std::endl;
  if (myEvent.resolvedANAMuons[1]->isolationR03().sumPt/myEvent.resolvedANAMuons[1]->pt() > 0.1) return FalseReturn;
  myEvent.ResCutProgress++;

  std::cout << "checking additional leptons" << std::endl;
  if(myEvent.NresolvedANAElectronCands > 0 ||myEvent.NresolvedANAMuonCands > 2) return FalseReturn;
  myEvent.ResCutProgress++;

  std::cout << "checking triggers" << std::endl;
  if(!myEvent.muonTrigPassBit) return FalseReturn;
  myEvent.ResCutProgress++;
  
  std::cout << "checking number of jets" << std::endl;
  if (myEvent.myResCandJets.size() < 2 && myEvent.myResCandJets_JECUp.size() < 2 && myEvent.myResCandJets_JECDown.size() < 2 && myEvent.myResCandJets_JERUp.size() < 2 && myEvent.myResCandJets_JERDown.size() < 2) {
    return FalseReturn;
  } else {
    std::sort(myEvent.myResCandJets.begin(), myEvent.myResCandJets.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResCandJets_JECUp.begin(), myEvent.myResCandJets_JECUp.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResCandJets_JECDown.begin(), myEvent.myResCandJets_JECDown.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResCandJets_JERUp.begin(), myEvent.myResCandJets_JERUp.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResCandJets_JERDown.begin(), myEvent.myResCandJets_JERDown.end(), ::wrTools::compareEtCandidatePointerTAddJet);
  }

  std::cout << "dR between muons" << std::endl;
  double dR_pair = sqrt(::wrTools::dR2(myEvent.resolvedANAMuons[0]->eta(),myEvent.resolvedANAMuons[1]->eta(),myEvent.resolvedANAMuons[0]->phi(),myEvent.resolvedANAMuons[1]->phi()));
  std::cout << "Muons dR_pair: " << dR_pair << std::endl;
  if (dR_pair < 0.4) return FalseReturn;

  std::cout << "RES OBJECT SELECTIONS PASSED" << std::endl;
  const pat::Muon* mu1 =  myEvent.resolvedANAMuons[0];
  const pat::Muon* mu2 =  myEvent.resolvedANAMuons[1];

  double mll = (mu1->p4()*myEvent.leadResMuonScale[0]+mu2->p4()*myEvent.secondResMuonScale[0]).mass();
  myEvent.resMLL = mll;
//  if (mll < 200) return FalseReturn;  // Applied later on - only here for cutflow studies


  if(myEvent.myResCandJets.size() > 1){
    myEvent.ResCutProgress++;
    bool nominalPass = true;

    double dR_pair12 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets[1]->eta,mu1->phi(),myEvent.myResCandJets[1]->phi));
    double dR_pair21 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets[0]->eta,mu2->phi(),myEvent.myResCandJets[0]->phi));
    double dR_pair22 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets[1]->eta,mu2->phi(),myEvent.myResCandJets[1]->phi));
    double dR_pair11 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets[0]->eta,mu1->phi(),myEvent.myResCandJets[0]->phi));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(myEvent.myResCandJets[0]->eta,myEvent.myResCandJets[1]->eta,myEvent.myResCandJets[0]->phi,myEvent.myResCandJets[1]->phi));

/*    if (dR_pair12 < 0.4) return FalseReturn; 
    if (dR_pair21 < 0.4) return FalseReturn; 
    if (dR_pair22 < 0.4) return FalseReturn; 
    if (dR_pair11 < 0.4) return FalseReturn;*/
    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    myEvent.ResCutProgress++;

    if (dR_Jetpair11 < 0.4) nominalPass = false;
//    if (dR_Jetpair11 < 0.4) return FalseReturn;
//    if (dR_pair < 0.4) return FalseReturn;

    myEvent.ResCutProgress++;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets[0]->pT,myEvent.myResCandJets[0]->eta,myEvent.myResCandJets[0]->phi,myEvent.myResCandJets[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets[1]->pT,myEvent.myResCandJets[1]->eta,myEvent.myResCandJets[1]->phi,myEvent.myResCandJets[1]->E);

    std::cout << "jet1Vec_temp->X(): " << jet1Vec_temp->X() << "jet1Vec_temp->Y(): " << jet1Vec_temp->Y() << "jet1Vec_temp->Z(): " << jet1Vec_temp->Z() << "jet1Vec_temp->T(): " << jet1Vec_temp->T() << std::endl;

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double resMass = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();
    double mll = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0]).mass();
    double mjj = (jet1 + jet2).mass();
    double mljj = (mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();
    double resMass_Up = (mu1->p4()*myEvent.leadResMuonScale[1] + mu2->p4()*myEvent.secondResMuonScale[1] + jet1 + jet2).mass();
    double resMass_Down = (mu1->p4()*myEvent.leadResMuonScale[2] + mu2->p4()*myEvent.secondResMuonScale[2] + jet1 + jet2).mass();

    std::cout << "resMass: " << resMass << " mll: " << mll << " mjj: " << mjj << " mljj: " << mljj << std::endl;

    myEvent.resolvedRECOmass = resMass;
    myEvent.resolvedRECOmass_MuResolUp = resMass_Up;
    myEvent.resolvedRECOmass_MuResolDown = resMass_Down;

    myEvent.resSubleadMuJet1dR = dR_pair21;
    myEvent.resSubleadMuJet2dR = dR_pair22;

    myEvent.resLeadMuJet1dR = dR_pair11;
    myEvent.resLeadMuJet2dR = dR_pair12;

    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResCandJets_JECUp.size() > 1){
    bool nominalPass = true;

    double dR_pair12 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JECUp[1]->eta,mu1->phi(),myEvent.myResCandJets_JECUp[1]->phi));
    double dR_pair21 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JECUp[0]->eta,mu2->phi(),myEvent.myResCandJets_JECUp[0]->phi));
    double dR_pair22 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JECUp[1]->eta,mu2->phi(),myEvent.myResCandJets_JECUp[1]->phi));
    double dR_pair11 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JECUp[0]->eta,mu1->phi(),myEvent.myResCandJets_JECUp[0]->phi));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(myEvent.myResCandJets_JECUp[0]->eta,myEvent.myResCandJets_JECUp[1]->eta,myEvent.myResCandJets_JECUp[0]->phi,myEvent.myResCandJets_JECUp[1]->phi));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JECUp[0]->pT,myEvent.myResCandJets_JECUp[0]->eta,myEvent.myResCandJets_JECUp[0]->phi,myEvent.myResCandJets_JECUp[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JECUp[1]->pT,myEvent.myResCandJets_JECUp[1]->eta,myEvent.myResCandJets_JECUp[1]->phi,myEvent.myResCandJets_JECUp[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double resMass = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();

//    if (resMass < 800) nominalPass = false;

    myEvent.resolvedRECOmass_JECUp = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResCandJets_JECDown.size() > 1){
    bool nominalPass = true;

    double dR_pair12 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JECDown[1]->eta,mu1->phi(),myEvent.myResCandJets_JECDown[1]->phi));
    double dR_pair21 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JECDown[0]->eta,mu2->phi(),myEvent.myResCandJets_JECDown[0]->phi));
    double dR_pair22 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JECDown[1]->eta,mu2->phi(),myEvent.myResCandJets_JECDown[1]->phi));
    double dR_pair11 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JECDown[0]->eta,mu1->phi(),myEvent.myResCandJets_JECDown[0]->phi));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(myEvent.myResCandJets_JECDown[0]->eta,myEvent.myResCandJets_JECDown[1]->eta,myEvent.myResCandJets_JECDown[0]->phi,myEvent.myResCandJets_JECDown[1]->phi));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JECDown[0]->pT,myEvent.myResCandJets_JECDown[0]->eta,myEvent.myResCandJets_JECDown[0]->phi,myEvent.myResCandJets_JECDown[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JECDown[1]->pT,myEvent.myResCandJets_JECDown[1]->eta,myEvent.myResCandJets_JECDown[1]->phi,myEvent.myResCandJets_JECDown[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double resMass = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();

//    if (resMass < 800) nominalPass = false;

    myEvent.resolvedRECOmass_JECDown = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResCandJets_JERUp.size() > 1){
    std::cout << "Enough JERUp jets" << std::endl;
    bool nominalPass = true;

    double dR_pair12 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JERUp[1]->eta,mu1->phi(),myEvent.myResCandJets_JERUp[1]->phi));
    double dR_pair21 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JERUp[0]->eta,mu2->phi(),myEvent.myResCandJets_JERUp[0]->phi));
    double dR_pair22 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JERUp[1]->eta,mu2->phi(),myEvent.myResCandJets_JERUp[1]->phi));
    double dR_pair11 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JERUp[0]->eta,mu1->phi(),myEvent.myResCandJets_JERUp[0]->phi));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(myEvent.myResCandJets_JERUp[0]->eta,myEvent.myResCandJets_JERUp[1]->eta,myEvent.myResCandJets_JERUp[0]->phi,myEvent.myResCandJets_JERUp[1]->phi));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JERUp[0]->pT,myEvent.myResCandJets_JERUp[0]->eta,myEvent.myResCandJets_JERUp[0]->phi,myEvent.myResCandJets_JERUp[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JERUp[1]->pT,myEvent.myResCandJets_JERUp[1]->eta,myEvent.myResCandJets_JERUp[1]->phi,myEvent.myResCandJets_JERUp[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double resMass = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();

//    if (resMass < 800) nominalPass = false;

    myEvent.resolvedRECOmass_JERUp = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResCandJets_JERDown.size() > 1){
    bool nominalPass = true;

    double dR_pair12 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JERDown[1]->eta,mu1->phi(),myEvent.myResCandJets_JERDown[1]->phi));
    double dR_pair21 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JERDown[0]->eta,mu2->phi(),myEvent.myResCandJets_JERDown[0]->phi));
    double dR_pair22 = sqrt(::wrTools::dR2(mu2->eta(),myEvent.myResCandJets_JERDown[1]->eta,mu2->phi(),myEvent.myResCandJets_JERDown[1]->phi));
    double dR_pair11 = sqrt(::wrTools::dR2(mu1->eta(),myEvent.myResCandJets_JERDown[0]->eta,mu1->phi(),myEvent.myResCandJets_JERDown[0]->phi));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(myEvent.myResCandJets_JERDown[0]->eta,myEvent.myResCandJets_JERDown[1]->eta,myEvent.myResCandJets_JERDown[0]->phi,myEvent.myResCandJets_JERDown[1]->phi));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JERDown[0]->pT,myEvent.myResCandJets_JERDown[0]->eta,myEvent.myResCandJets_JERDown[0]->phi,myEvent.myResCandJets_JERDown[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResCandJets_JERDown[1]->pT,myEvent.myResCandJets_JERDown[1]->eta,myEvent.myResCandJets_JERDown[1]->phi,myEvent.myResCandJets_JERDown[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double resMass = (mu1->p4()*myEvent.leadResMuonScale[0] + mu2->p4()*myEvent.secondResMuonScale[0] + jet1 + jet2).mass();

//    if (resMass < 800) nominalPass = false;

    myEvent.resolvedRECOmass_JERDown = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }

  return EachRegionReturn;
}
bool cmsWRextension::passFSBResRECO(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES FSB SELECTION CALL" << std::endl;
  std::cout << "RES FSB LEPTON SELECTION CALL" << std::endl;
  if ( !resolvedFSBleptonSelection(iEvent, myEvent) || !resolvedFSBJetSelection(iEvent, myEvent)) return false;
  std::cout << "RES FSB JET SELECTION CALL" << std::endl;
//  if ( !resolvedFSBJetSelection(iEvent, myEvent) )  return false;

  if (myEvent.myResFSBCandJets.size() < 2) {
    return false;
  } else {
    myEvent.ResFSBCutProgress++;
    std::sort(myEvent.myResFSBCandJets.begin(), myEvent.myResFSBCandJets.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    double dR_pair = sqrt(::wrTools::dR2(myEvent.myResFSBCandJets[0]->eta,myEvent.myResFSBCandJets[1]->eta,myEvent.myResFSBCandJets[0]->phi,myEvent.myResFSBCandJets[1]->phi));
    std::cout<< "FSB Jets dR_pair: " << dR_pair << std::endl;
    if (dR_pair < 0.4) return false;
     myEvent.resFSBJetDR = dR_pair;
  }
  myEvent.ResFSBCutProgress++;

  std::cout << "dR(AK4 jets, FSB muon) SELECTION" << std::endl;
  double dR_jet1_muon = sqrt(::wrTools::dR2(myEvent.resFSBMuon->eta(),myEvent.myResFSBCandJets[0]->eta,myEvent.resFSBMuon->phi(),myEvent.myResFSBCandJets[0]->phi));
  double dR_jet2_muon = sqrt(::wrTools::dR2(myEvent.resFSBMuon->eta(),myEvent.myResFSBCandJets[1]->eta,myEvent.resFSBMuon->phi(),myEvent.myResFSBCandJets[1]->phi));
  std::cout << "dR_jet1_muon: " << dR_jet1_muon << " dR_jet2_muon: " << dR_jet2_muon << std::endl;
//  if (dR_jet1_muon < 0.4) return false;
//  if (dR_jet2_muon < 0.4) return false;
  myEvent.ResFSBCutProgress++;

  std::cout << "dR between FSB leptons" << std::endl;
  double dR_pair = sqrt(::wrTools::dR2(myEvent.resFSBMuon->eta(),myEvent.resFSBElec->eta(),myEvent.resFSBMuon->phi(),myEvent.resFSBElec->phi()));
  std::cout << "FSB leptons dR_pair: " << dR_pair << std::endl;
  if (dR_pair < 0.4) return false;
  myEvent.ResFSBCutProgress++;

  std::cout << "RES OBJECT SELECTIONS PASSED" << std::endl;
  std::cout << "RES FSB JET1 WITH PT,ETA,PHI: " << myEvent.myResFSBCandJets[0]->pT << "," << myEvent.myResFSBCandJets[0]->eta << "," << myEvent.myResFSBCandJets[0]->phi << std::endl;
  std::cout << "RES FSB JET1 WITH PT,ETA,PHI: " << myEvent.myResFSBCandJets[1]->pT << "," << myEvent.myResFSBCandJets[1]->eta << "," << myEvent.myResFSBCandJets[1]->phi << std::endl;


  const pat::Muon*     mu   =  myEvent.resFSBMuon;
  const pat::Electron* el   =  myEvent.resFSBElec;
  TLorentzVector* jet1Vec_temp = new TLorentzVector();
  TLorentzVector* jet2Vec_temp = new TLorentzVector();
  jet1Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets[0]->pT,myEvent.myResFSBCandJets[0]->eta,myEvent.myResFSBCandJets[0]->phi,myEvent.myResFSBCandJets[0]->E);
  jet2Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets[1]->pT,myEvent.myResFSBCandJets[1]->eta,myEvent.myResFSBCandJets[1]->phi,myEvent.myResFSBCandJets[1]->E);

  math::XYZTLorentzVector jet1;
  math::XYZTLorentzVector jet2;
  std::cout << "jet1Vec_temp X,Y,Z,T: " << jet1Vec_temp->X() << "," << jet1Vec_temp->Y() << "," << jet1Vec_temp->Z() << "," << jet1Vec_temp->T() << std::endl;
  std::cout << "jet1Vec_temp X,Y,Z,T: " << jet2Vec_temp->X() << "," << jet2Vec_temp->Y() << "," << jet2Vec_temp->Z() << "," << jet2Vec_temp->T() << std::endl;
  jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
  jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

  //MLL
  auto corrP4  = myEvent.resFSBElec->p4();
  if(m_isMC && m_era == "2016"){
    auto corrP4  = myEvent.resFSBElec->p4()*myEvent.resFSBElec->userFloat("ecalTrkEnergyPostCorr") / myEvent.resFSBElec->energy();
    double mll = (mu->p4()*myEvent.leadFSBMuonScale[0]+corrP4).mass();
  }
  double mll = (mu->p4()*myEvent.leadFSBMuonScale[0]+corrP4).mass();
//  myEvent.resMLL = mll;
  if (mll < 200) return false;  // 2017
//  if (mll < 150) return false;// korea
//  if (mll < 175) return false;// middle
  std::cout << "RES MLL PASSED" << std::endl;
  myEvent.ResFSBCutProgress++;

  //CHECK DR ASSOCIATIONS
  double dR_pair12 = sqrt(::wrTools::dR2(mu->eta(),jet2.eta(),mu->phi(),jet2.phi()));
  std::cout << "RES 12" << dR_pair12<< std::endl;
  double dR_pair21 = sqrt(::wrTools::dR2(el->eta(),jet1.eta(),el->phi(),jet1.phi()));
  std::cout << "RES 21" << dR_pair21<< std::endl;
  double dR_pair22 = sqrt(::wrTools::dR2(el->eta(),jet2.eta(),el->phi(),jet2.phi()));
  std::cout << "RES 22" << dR_pair22<< std::endl;
  double dR_pair11 = sqrt(::wrTools::dR2(mu->eta(),jet1.eta(),mu->phi(),jet1.phi()));
  std::cout << "RES 11" << dR_pair11<< std::endl;

  if (dR_pair12 < 0.4) return false;
  if (dR_pair21 < 0.4) return false;
  if (dR_pair22 < 0.4) return false;
  if (dR_pair11 < 0.4) return false;
  std::cout << "RES FOUR OBJECT SEPARATION PASSED" << std::endl;
  myEvent.ResFSBCutProgress++;
    
  //CHECK 4 OBJECT MASS
  double resMass = (mu->p4()*myEvent.leadFSBMuonScale[0] + el->p4() + jet1 + jet2).mass();

  std::cout << "RES FSB resMass: " << resMass << std::endl;
  if (resMass < 800){ myEvent.resolvedFSBRECOmassAbove600 = false;}
  else { myEvent.resolvedFSBRECOmassAbove600 = true;}

  myEvent.resolvedFSBRECOmass = resMass; 

  myEvent.resFSBElecJet1dR = dR_pair21;
  myEvent.resFSBElecJet2dR = dR_pair22;

  myEvent.resFSBMuonJet1dR = dR_pair11;
  myEvent.resFSBMuonJet2dR = dR_pair12;


  return true;

}
std::vector<bool> cmsWRextension::passFSBResRECO_Fast(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "RES FSB SELECTION CALL" << std::endl;
  std::cout << "RES FSB LEPTON SELECTION CALL" << std::endl;

  std::vector<bool> FalseReturn;
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);
  FalseReturn.push_back(false);

  std::vector<bool> EachRegionReturn;
  myEvent.ResFSBCutProgress++;

  if ( !resolvedFSBleptonSelection(iEvent, myEvent) || !resolvedFSBJetSelection(iEvent, myEvent)) return FalseReturn;

  if(myEvent.NresolvedANAFSBLeptonCands != 2) return FalseReturn;
  myEvent.ResFSBCutProgress++;

  if(!myEvent.muonTrigPassBit) return FalseReturn;
  myEvent.ResFSBCutProgress++;

  std::cout << "RES FSB JET SELECTION CALL" << std::endl;
//  if ( !resolvedFSBJetSelection(iEvent, myEvent) )  return FalseReturn;

  if (myEvent.myResFSBCandJets.size() < 2 && myEvent.myResFSBCandJets_JECUp.size() < 2 && myEvent.myResFSBCandJets_JECDown.size() < 2 && myEvent.myResFSBCandJets_JERUp.size() < 2 && myEvent.myResFSBCandJets_JERDown.size() < 2) {
    return FalseReturn;
  } else {
    std::sort(myEvent.myResFSBCandJets.begin(), myEvent.myResFSBCandJets.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResFSBCandJets_JECUp.begin(), myEvent.myResFSBCandJets_JECUp.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResFSBCandJets_JECDown.begin(), myEvent.myResFSBCandJets_JECDown.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResFSBCandJets_JERUp.begin(), myEvent.myResFSBCandJets_JERUp.end(), ::wrTools::compareEtCandidatePointerTAddJet);
    std::sort(myEvent.myResFSBCandJets_JERDown.begin(), myEvent.myResFSBCandJets_JERDown.end(), ::wrTools::compareEtCandidatePointerTAddJet);
  }

  double dR_pair = sqrt(::wrTools::dR2(myEvent.resFSBMuon->eta(),myEvent.resFSBElec->eta(),myEvent.resFSBMuon->phi(),myEvent.resFSBElec->phi()));
  if (dR_pair < 0.4) return FalseReturn;


  const pat::Muon*     mu   =  myEvent.resFSBMuon;
  const pat::Electron* el   =  myEvent.resFSBElec;

//  std::cout << "DOFAST RES FSB JET1 WITH PT,ETA,PHI: " << myEvent.myResFSBCandJets[0]->pT << "," << myEvent.myResFSBCandJets[0]->eta << "," << myEvent.myResFSBCandJets[0]->phi << std::endl;
//  std::cout << "DOFAST RES FSB JET1 WITH PT,ETA,PHI: " << myEvent.myResFSBCandJets[1]->pT << "," << myEvent.myResFSBCandJets[1]->eta << "," << myEvent.myResFSBCandJets[1]->phi << std::endl;

  std::cout << "DOFAST RES FSB MUON WITH PT,ETA,PHI: " << myEvent.resFSBMuon->pt() << "," << myEvent.resFSBMuon->eta() << "," << myEvent.resFSBMuon->phi() << std::endl;
  std::cout << "DOFAST RES FSB ELECTRON WITH PT,ETA,PHI: " << myEvent.resFSBElec->pt() << "," << myEvent.resFSBElec->eta() << "," << myEvent.resFSBElec->phi() << std::endl;

  auto corrP4  = myEvent.resFSBElec->p4();
/*  if(m_isMC && m_era == "2016"){
    auto corrP4  = myEvent.resFSBElec->p4()*myEvent.resFSBElec->userFloat("ecalTrkEnergyPostCorr") / myEvent.resFSBElec->energy();
    double mll = (mu->p4()*myEvent.leadFSBMuonScale[0]+corrP4).mass();
  }*/
  //MLL
  double mll = (mu->p4()*myEvent.leadFSBMuonScale[0]+corrP4).mass();
  if (mll < 200) return FalseReturn; 
//  myEvent.ResFSBCutProgress++;

  if(myEvent.myResFSBCandJets.size() > 1){
    myEvent.ResFSBCutProgress++;
    bool nominalPass = true;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets[0]->pT,myEvent.myResFSBCandJets[0]->eta,myEvent.myResFSBCandJets[0]->phi,myEvent.myResFSBCandJets[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets[1]->pT,myEvent.myResFSBCandJets[1]->eta,myEvent.myResFSBCandJets[1]->phi,myEvent.myResFSBCandJets[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double dR_pair12 = sqrt(::wrTools::dR2(mu->eta(),jet2.eta(),mu->phi(),jet2.phi()));
    double dR_pair21 = sqrt(::wrTools::dR2(el->eta(),jet1.eta(),el->phi(),jet1.phi()));
    double dR_pair22 = sqrt(::wrTools::dR2(el->eta(),jet2.eta(),el->phi(),jet2.phi()));
    double dR_pair11 = sqrt(::wrTools::dR2(mu->eta(),jet1.eta(),mu->phi(),jet1.phi()));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(jet1.eta(),jet2.eta(),jet1.phi(),jet2.phi()));

    std::cout << "dR_pair12: " << dR_pair12 << std::endl;
    std::cout << "dR_pair21: " << dR_pair21 << std::endl;
    std::cout << "dR_pair22: " << dR_pair22 << std::endl;
    std::cout << "dR_pair11: " << dR_pair11 << std::endl;

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
/*    if (dR_pair12 < 0.4) return FalseReturn; 
    if (dR_pair21 < 0.4) return FalseReturn; 
    if (dR_pair22 < 0.4) return FalseReturn; 
    if (dR_pair11 < 0.4) return FalseReturn; */
    myEvent.ResFSBCutProgress++;

    if (dR_Jetpair11 < 0.4) nominalPass = false;
//    if (dR_Jetpair11 < 0.4) return FalseReturn;
//    if (dR_pair < 0.4) return FalseReturn;
    myEvent.ResFSBCutProgress++;

//    if (mll < 200) return FalseReturn;
    myEvent.ResFSBCutProgress++;


    double resMass = (mu->p4()*myEvent.leadFSBMuonScale[0] + el->p4() + jet1 + jet2).mass();
    std::cout << "DOFAST RES FSB resMass: " << resMass << std::endl;
    double resMass_Up = (mu->p4()*myEvent.leadFSBMuonScale[1] + el->p4() + jet1 + jet2).mass();
    double resMass_Down = (mu->p4()*myEvent.leadFSBMuonScale[2] + el->p4() + jet1 + jet2).mass();

    myEvent.resolvedFSBRECOmass = resMass;
    myEvent.resolvedFSBRECOmass_MuResolUp = resMass_Up;
    myEvent.resolvedFSBRECOmass_MuResolDown = resMass_Down;

    myEvent.resFSBElecJet1dR = dR_pair21;
    myEvent.resFSBElecJet2dR = dR_pair22;

    myEvent.resFSBMuonJet1dR = dR_pair11;
    myEvent.resFSBMuonJet2dR = dR_pair12;

    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResFSBCandJets_JECUp.size() > 1){
    bool nominalPass = true;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JECUp[0]->pT,myEvent.myResFSBCandJets_JECUp[0]->eta,myEvent.myResFSBCandJets_JECUp[0]->phi,myEvent.myResFSBCandJets_JECUp[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JECUp[1]->pT,myEvent.myResFSBCandJets_JECUp[1]->eta,myEvent.myResFSBCandJets_JECUp[1]->phi,myEvent.myResFSBCandJets_JECUp[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double dR_pair12 = sqrt(::wrTools::dR2(mu->eta(),jet2.eta(),mu->phi(),jet2.phi()));
    double dR_pair21 = sqrt(::wrTools::dR2(el->eta(),jet1.eta(),el->phi(),jet1.phi()));
    double dR_pair22 = sqrt(::wrTools::dR2(el->eta(),jet2.eta(),el->phi(),jet2.phi()));
    double dR_pair11 = sqrt(::wrTools::dR2(mu->eta(),jet1.eta(),mu->phi(),jet1.phi()));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(jet1.eta(),jet2.eta(),jet1.phi(),jet2.phi()));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    double resMass = (mu->p4()*myEvent.leadFSBMuonScale[0] + el->p4() + jet1 + jet2).mass();

    if (resMass < 800) nominalPass = false;

    myEvent.resolvedFSBRECOmass_JECUp = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResFSBCandJets_JECDown.size() > 1){
    bool nominalPass = true;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JECDown[0]->pT,myEvent.myResFSBCandJets_JECDown[0]->eta,myEvent.myResFSBCandJets_JECDown[0]->phi,myEvent.myResFSBCandJets_JECDown[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JECDown[1]->pT,myEvent.myResFSBCandJets_JECDown[1]->eta,myEvent.myResFSBCandJets_JECDown[1]->phi,myEvent.myResFSBCandJets_JECDown[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double dR_pair12 = sqrt(::wrTools::dR2(mu->eta(),jet2.eta(),mu->phi(),jet2.phi()));
    double dR_pair21 = sqrt(::wrTools::dR2(el->eta(),jet1.eta(),el->phi(),jet1.phi()));
    double dR_pair22 = sqrt(::wrTools::dR2(el->eta(),jet2.eta(),el->phi(),jet2.phi()));
    double dR_pair11 = sqrt(::wrTools::dR2(mu->eta(),jet1.eta(),mu->phi(),jet1.phi()));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(jet1.eta(),jet2.eta(),jet1.phi(),jet2.phi()));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    double resMass = (mu->p4()*myEvent.leadFSBMuonScale[0] + el->p4() + jet1 + jet2).mass();

    if (resMass < 800) nominalPass = false;

    myEvent.resolvedFSBRECOmass_JECDown = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResFSBCandJets_JERUp.size() > 1){
    bool nominalPass = true;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JERUp[0]->pT,myEvent.myResFSBCandJets_JERUp[0]->eta,myEvent.myResFSBCandJets_JERUp[0]->phi,myEvent.myResFSBCandJets_JERUp[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JERUp[1]->pT,myEvent.myResFSBCandJets_JERUp[1]->eta,myEvent.myResFSBCandJets_JERUp[1]->phi,myEvent.myResFSBCandJets_JERUp[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double dR_pair12 = sqrt(::wrTools::dR2(mu->eta(),jet2.eta(),mu->phi(),jet2.phi()));
    double dR_pair21 = sqrt(::wrTools::dR2(el->eta(),jet1.eta(),el->phi(),jet1.phi()));
    double dR_pair22 = sqrt(::wrTools::dR2(el->eta(),jet2.eta(),el->phi(),jet2.phi()));
    double dR_pair11 = sqrt(::wrTools::dR2(mu->eta(),jet1.eta(),mu->phi(),jet1.phi()));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(jet1.eta(),jet2.eta(),jet1.phi(),jet2.phi()));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    double resMass = (mu->p4()*myEvent.leadFSBMuonScale[0] + el->p4() + jet1 + jet2).mass();

    if (resMass < 800) nominalPass = false;

    myEvent.resolvedFSBRECOmass_JERUp = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }
  if(myEvent.myResFSBCandJets_JERDown.size() > 1){
    bool nominalPass = true;

    TLorentzVector* jet1Vec_temp = new TLorentzVector();
    TLorentzVector* jet2Vec_temp = new TLorentzVector();
    jet1Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JERDown[0]->pT,myEvent.myResFSBCandJets_JERDown[0]->eta,myEvent.myResFSBCandJets_JERDown[0]->phi,myEvent.myResFSBCandJets_JERDown[0]->E);
    jet2Vec_temp->SetPtEtaPhiE(myEvent.myResFSBCandJets_JERDown[1]->pT,myEvent.myResFSBCandJets_JERDown[1]->eta,myEvent.myResFSBCandJets_JERDown[1]->phi,myEvent.myResFSBCandJets_JERDown[1]->E);

    math::XYZTLorentzVector jet1;
    math::XYZTLorentzVector jet2;
    jet1.SetXYZT(jet1Vec_temp->X(),jet1Vec_temp->Y(),jet1Vec_temp->Z(),jet1Vec_temp->T());
    jet2.SetXYZT(jet2Vec_temp->X(),jet2Vec_temp->Y(),jet2Vec_temp->Z(),jet2Vec_temp->T());

    double dR_pair12 = sqrt(::wrTools::dR2(mu->eta(),jet2.eta(),mu->phi(),jet2.phi()));
    double dR_pair21 = sqrt(::wrTools::dR2(el->eta(),jet1.eta(),el->phi(),jet1.phi()));
    double dR_pair22 = sqrt(::wrTools::dR2(el->eta(),jet2.eta(),el->phi(),jet2.phi()));
    double dR_pair11 = sqrt(::wrTools::dR2(mu->eta(),jet1.eta(),mu->phi(),jet1.phi()));
    double dR_Jetpair11 = sqrt(::wrTools::dR2(jet1.eta(),jet2.eta(),jet1.phi(),jet2.phi()));

    if (dR_pair12 < 0.4) nominalPass = false;
    if (dR_pair21 < 0.4) nominalPass = false;
    if (dR_pair22 < 0.4) nominalPass = false;
    if (dR_pair11 < 0.4) nominalPass = false;
    if (dR_Jetpair11 < 0.4) nominalPass = false;

    double resMass = (mu->p4()*myEvent.leadFSBMuonScale[0] + el->p4() + jet1 + jet2).mass();

    if (resMass < 800) nominalPass = false;

    myEvent.resolvedFSBRECOmass_JERDown = resMass;
    EachRegionReturn.push_back(nominalPass);
  }else{
    EachRegionReturn.push_back(false);
  }

  return EachRegionReturn;
}
bool cmsWRextension::objectCompareGEN(const edm::Event& iEvent, eventBits& myEvent) {
  bool fat = true; 
  if(myEvent.myGenMuons.size() < 2 || myEvent.myGenPartons.size() < 2) {
    std::cout << "EVENT FAILS, HARD PROCESS NOT FOUND " << myEvent.myGenMuons.size()<<" muons "<<  myEvent.myGenJets.size()<<" partons"<< std::endl;
    return false;
  }
  if (myEvent.myAK8GenJets.size() < 1 ) {
    std::cout << "EVENT FAILS BOOST GEN, NO FAT JETS FOUND" << std::endl; 
    fat = false;
  }

  const reco::GenParticle* mu1 =  myEvent.firstMuon;
  const reco::GenParticle* mu2 =  myEvent.secondMuon;
  const reco::GenParticle*  parton1 = myEvent.myGenPartons[0];
  const reco::GenParticle*  parton2 = myEvent.myGenPartons[1];
  //MAKE HARDPROCESS DR ASSOCIATIONS
  double dR_pair12 = sqrt(::wrTools::dR2(mu1->eta(),parton2->eta(),mu1->phi(),parton2->phi()));
  double dR_pair21 = sqrt(::wrTools::dR2(mu2->eta(),parton1->eta(),mu2->phi(),parton1->phi()));
  double dR_pair22 = sqrt(::wrTools::dR2(mu2->eta(),parton2->eta(),mu2->phi(),parton2->phi()));
  double dR_pair11 = sqrt(::wrTools::dR2(mu1->eta(),parton1->eta(),mu1->phi(),parton1->phi()));

  myEvent.resSubleadMuParton1dR = dR_pair21;
  myEvent.resSubleadMuParton2dR = dR_pair22;
  myEvent.resLeadMuParton1dR = dR_pair11;
  myEvent.resLeadMuParton2dR = dR_pair12;

 
  myEvent.muon1muon2dR       = sqrt(::wrTools::dR2(mu1->eta(),mu2->eta(),mu1->phi(),mu2->phi()));
  myEvent.parton1parton2dR   = sqrt(::wrTools::dR2(parton1->eta(),parton2->eta(),parton1->phi(),parton2->phi()));
  //FOR Z SIDEBAND
  myEvent.muon1muon2Mass = (mu1->p4() +mu2->p4()).mass();
  //FIND MININUM DR
  if (dR_pair21 < dR_pair22) {
    myEvent.minPartonMuon2dR = dR_pair21;
  } else {
    myEvent.minPartonMuon2dR = dR_pair22;

  }
  //BOOSTED CHECKS 
  if (fat) {
    const reco::GenJet*      fatJet = myEvent.myAK8GenJets[0];

    double dPhi = fabs(::wrTools::dPhi(mu1->phi(), fatJet->phi()));

    myEvent.fatJetMuon1dPhi = dPhi;
    myEvent.fatJetMuon1dR   = sqrt(::wrTools::dR2(mu1->eta(),fatJet->eta(),mu1->phi(),fatJet->phi()));
    myEvent.fatJetMuon2dR   = sqrt(::wrTools::dR2(mu2->eta(),fatJet->eta(),mu2->phi(),fatJet->phi()));
    //MAKE DIOBJECT 
    myEvent.leadAK8JetMuonMassValGEN = (fatJet->p4() + mu1->p4()).mass();
    myEvent.leadAK8JetMuonPtValGEN   = (fatJet->p4() + mu1->p4()).pt();
    myEvent.leadAK8JetMuonEtaValGEN  = (fatJet->p4() + mu1->p4()).eta();
  }

  myEvent.resolvedGENmass = (mu1->p4() + mu2->p4() + parton1->p4() + parton2->p4()).mass();
  myEvent.resolvedGENpt   = (mu1->p4() + mu2->p4() + parton1->p4() + parton2->p4()).mass();
  myEvent.resolvedGENeta  = (mu1->p4() + mu2->p4() + parton1->p4() + parton2->p4()).mass();

  return true;
}
bool cmsWRextension::passZsidebandCutGEN(const edm::Event& iEvent, eventBits& myEvent) {
  //TRUE MEANS IT IS _NOT_ IN THE Z SIDEBAND
  if (myEvent.muon1muon2Mass < 50.0) {
    return false; 
    //YOU PROBABLY DON'T EVEN HAVE TWO MUONS!
  }
  if (myEvent.muon1muon2Mass < 200) {
    return false;
  }
  return true;
}
//dR cut setting
static const float DRCUT = 0.4;
//static const float DRCUT = 0.5;
//static const float DRCUT = 0.6;
//static const float DRCUT = 0.7;
//static const float DRCUT = 0.8;
bool cmsWRextension::passBoostGEN(const edm::Event& iEvent, eventBits& myEvent) {

  if(myEvent.myAK8GenJets.size() < 1) return false;

  if(myEvent.firstMuon->pt() < 200) return false;
  if(myEvent.myAK8GenJets[0]->pt() < 200) return false;

  //IT SHOULD BE NEAR AT LEAST ONE OF THE QUARKS
  if ( ! ((myEvent.resSubleadMuParton1dR < DRCUT) || (myEvent.resSubleadMuParton2dR < DRCUT))) return false;

  if ( myEvent.leadAK8JetMuonMassValGEN < 200) return false;

  if ( myEvent.fatJetMuon2dR > DRCUT) return false;

  if ( myEvent.fatJetMuon1dPhi < 2.0) return false;
  
  return true;
}

bool cmsWRextension::passResGEN(const edm::Event& iEvent, eventBits& myEvent) {
    
  if (myEvent.firstMuon == NULL || myEvent.secondMuon == NULL) {
    std::cout << "firstMuon or secondMuon is uninitialized: passResGEN fails" << std::endl;
    return false;
  }

  if(myEvent.firstMuon->pt() < 60) return false;
  if(myEvent.secondMuon->pt() < 53) return false;

  if ( myEvent.resolvedGENmass < 800) return false;

  if ( myEvent.resSubleadMuParton1dR < DRCUT) return false;
  if ( myEvent.resSubleadMuParton2dR < DRCUT) return false;
  if ( myEvent.resLeadMuParton1dR    < 0.4) return false;
  if ( myEvent.resLeadMuParton2dR    < 0.4) return false;

  if ( myEvent.muon1muon2dR          < 0.4) return false;

  return true;

}
bool cmsWRextension::WRresonanceStudy(const edm::Event& iEvent, eventBits& myEvent) {
  std::cout << "Starting WRresonanceStudy" << std::endl;
  edm::Handle<std::vector<reco::GenParticle>> genParticles;
  iEvent.getByToken(m_genParticleToken, genParticles);

  //LOOP OVER GEN PARTICLES
  //9900024 WR 9900014 NRu 9900012 NRe 9900016 NRt
  bool foundIt = false;
  for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
    if( ! iParticle->isHardProcess() ) continue;  //ONLY HARD PROCESS AND NOT INCOMING
    if( iParticle->status() == 21 )    continue;

    if ( abs(iParticle->pdgId()) == 34 ) {
    //  std::cout << "WR with mass: "<< iParticle->mass() << std::endl;
      myEvent.wrShellMass = iParticle->mass();
      foundIt = true;
    }
    
    
  }
  if (!foundIt) {
    const reco::GenParticle* lostLepton   = 0;
    const reco::GenParticle* lostNeutrino = 0;

 //   std::cout << "DISASTER!" << std::endl;
    for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
      if( ! iParticle->isHardProcess() ) continue;  //ONLY HARD PROCESS AND NOT INCOMING
//      std::cout << "STATUS: " << iParticle->status() << " PDGID: " << iParticle->pdgId() << " MOTHER: " << iParticle->mother()->pdgId() << std::endl;

      if( abs( iParticle->mother()->pdgId() ) <= 6 ) {//CAME FROM A QUARK
        if( abs( iParticle->pdgId() ) == 13 || abs( iParticle->pdgId() ) == 11 ) //HERE'S A LEPTON
          lostLepton = &(*iParticle);
        if( abs( iParticle->pdgId() ) == 9900014 || abs( iParticle->pdgId() ) == 9900012) //HERE'S A RIGHT-HANDED NEUTRINO
          lostNeutrino = &(*iParticle);
      }
    }
    if(lostLepton == 0 || lostNeutrino == 0) {
      std::cout <<"STILL A MESS!!!"<<std::endl;
    }
    myEvent.wrShellMass = (lostLepton->p4() + lostNeutrino->p4()).mass();
  }
 
  return foundIt;

}
//TIGHTER VERSIONS
//bool cmsWRextension::passBoostTightGEN(const edm::Event& iEvent, eventBits& myEvent) {
//
//  //IT SHOULD BE NEAR AT LEAST ONE OF THE QUARKS
//  if ( ! ((myEvent.resSubleadMuParton1dR < 0.4) || (myEvent.resSubleadMuParton2dR < 0.4))) return false;
//
//  if ( myEvent.leadAK8JetMuonMassValGEN < 200) return false;
//
//  if ( myEvent.fatJetMuon2dR > 0.8) return false;
//
//  if ( myEvent.fatJetMuon1dPhi < 2.0) return false;
//  
//  return true;
//}
//
//bool cmsWRextension::passResTightGEN(const edm::Event& iEvent, eventBits& myEvent) {
//
//  if ( myEvent.resolvedGENmass < 600) return false;
//
//  //THE SECOND MUON SHOULD BE NICE AND FAR AWAY
//  if ( myEvent.resSubleadMuParton1dR < 0.8) return false;
//  if ( myEvent.resSubleadMuParton2dR < 0.8) return false;
//  if ( myEvent.resLeadMuParton1dR    < 0.4) return false;
//  if ( myEvent.resLeadMuParton2dR    < 0.4) return false;
//
//  return true;
//
//}
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
    flavor = 3;

    m_allEvents.book((fs->mkdir("allEvents")), 3, m_outputTag, 0, m_isSignal);
    m_eventsFailResFailBoostGEN.book((fs->mkdir("eventsFailResFailBoostGEN")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassResPassBoostGEN.book((fs->mkdir("eventsPassResPassBoostGEN")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassResFailBoostGEN.book((fs->mkdir("eventsPassResFailBoostGEN")), 3, m_outputTag, 0, m_isSignal);
    m_eventsFailResPassBoostGEN.book((fs->mkdir("eventsFailResPassBoostGEN")), 3, m_outputTag, 0, m_isSignal);

    m_eventsFailResFailBoostRECO.book((fs->mkdir("eventsFailResFailBoostRECO")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassResPassBoostRECO.book((fs->mkdir("eventsPassResPassBoostRECO")), 3, m_outputTag, 0,m_isSignal);
    m_eventsPassResFailBoostRECO.book((fs->mkdir("eventsPassResFailBoostRECO")), 3, m_outputTag, 0, m_isSignal);
    m_eventsFailResPassBoostRECO.book((fs->mkdir("eventsFailResPassBoostRECO")), 3, m_outputTag, 0, m_isSignal);

    m_eventsPassResLowMassCRRECO.book((fs->mkdir("eventsPassResLowMassCRRECO")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassResZMASSRECO.book((fs->mkdir("eventsPassResZMASSRECO")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassBoostZMASSRECO.book((fs->mkdir("eventsFailResPassBoostRECO2016VETOZMASS")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassResFSBRECO.book((fs->mkdir("eventsPassResFSBRECO")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassBoostFSBRECO.book((fs->mkdir("eventsPassBoostFSBRECO_D")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassResFSBLowMassCRRECO.book((fs->mkdir("eventsPassResFSBLowMassCRRECO")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassResFSBLowMassCRRECO_failBox.book((fs->mkdir("eventsPassResFSBLowMassCRRECO_failBox")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassResFSBLowMassCRRECO_oppoBox.book((fs->mkdir("eventsPassResFSBLowMassCRRECO_oppoBox")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassBoostLowMassCRRECO.book((fs->mkdir("eventsPassBoostLowMassCRRECO")), 3, m_outputTag, 0, m_isSignal);
    m_eventsPassBoostFSBLowMassCRRECO.book((fs->mkdir("eventsPassBoostFSBLowMassCRRECO")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassBoostFSBLowMassCRRECO_failBox.book((fs->mkdir("eventsPassBoostFSBLowMassCRRECO_failBox")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassBoostFSBLowMassCRRECO_oppoBox.book((fs->mkdir("eventsPassBoostFSBLowMassCRRECO_oppoBox")), 3, m_outputTag, 1, m_isSignal);
    m_eventsPassJetSelection.book((fs->mkdir("eventsPassJetSelection")), 3, m_outputTag, 0, m_isSignal);

  }
  if (m_doGen && !m_doReco && !m_doFast) {
    std::cout << "BOOKING PLOTS FLAVOR 1" << std::endl;
  //flavor 1
    flavor = 1;
    m_allEvents.book((fs->mkdir("allEvents")), 1, m_outputTag, 0, m_isSignal);
    m_eventsFailResFailBoostGEN.book((fs->mkdir("eventsFailResFailBoostGEN")), 1, m_outputTag, 0, m_isSignal);
    m_eventsPassResPassBoostGEN.book((fs->mkdir("eventsPassResPassBoostGEN")), 1, m_outputTag, 0, m_isSignal);
    m_eventsPassResFailBoostGEN.book((fs->mkdir("eventsPassResFailBoostGEN")), 1, m_outputTag, 0, m_isSignal);
    m_eventsFailResPassBoostGEN.book((fs->mkdir("eventsFailResPassBoostGEN")), 1, m_outputTag, 0, m_isSignal);

  }
  if (!m_doGen && m_doReco && !m_doFast) {
    std::cout << "BOOKING PLOTS FLAVOR 2" << std::endl;
  //flavor 2
    flavor = 2;
    m_allEvents.book((fs->mkdir("allEvents")), 2, m_outputTag, 0, m_isSignal);
    m_eventsFailResFailBoostRECO.book((fs->mkdir("eventsFailResFailBoostRECO")), 2, m_outputTag, 0, m_isSignal);
    m_eventsPassResPassBoostRECO.book((fs->mkdir("eventsPassResPassBoostRECO")), 2, m_outputTag, 0, m_isSignal);
    m_eventsPassResFailBoostRECO.book((fs->mkdir("eventsPassResFailBoostRECO")), 2, m_outputTag, 0, m_isSignal);
    m_eventsFailResPassBoostRECO.book((fs->mkdir("eventsFailResPassBoostRECO")), 2, m_outputTag, 0, m_isSignal);

    m_eventsPassResLowMassCRRECO.book((fs->mkdir("eventsPassResLowMassCRRECO")), 2, m_outputTag, 0, m_isSignal);
    m_eventsPassResZMASSRECO.book((fs->mkdir("eventsPassResZMASSRECO")), 2, m_outputTag, 0, m_isSignal);
    m_eventsPassResFSBRECO.book((fs->mkdir("eventsPassResFSBRECO")), 2, m_outputTag, 1, m_isSignal);
    m_eventsPassResFSBLowMassCRRECO.book((fs->mkdir("eventsPassResFSBLowMassCRRECO")), 2, m_outputTag, 1, m_isSignal);
    m_eventsPassResFSBLowMassCRRECO_failBox.book((fs->mkdir("eventsPassResFSBLowMassCRRECO_failBox")), 2, m_outputTag, 1, m_isSignal);
    m_eventsPassResFSBLowMassCRRECO_oppoBox.book((fs->mkdir("eventsPassResFSBLowMassCRRECO_oppoBox")), 2, m_outputTag, 1, m_isSignal);

    m_eventsPassBoostZMASSRECO.book((fs->mkdir("eventsFailResPassBoostRECO2016VETOZMASS")), 2, m_outputTag, 0, m_isSignal);
    m_eventsPassBoostLowMassCRRECO.book((fs->mkdir("eventsPassBoostLowMassCRRECO")), 2, m_outputTag, 0, m_isSignal);    
    m_eventsPassBoostFSBRECO.book((fs->mkdir("eventsPassBoostFSBRECO_D")), 2, m_outputTag, 2, m_isSignal);
    m_eventsPassBoostFSBLowMassCRRECO.book((fs->mkdir("eventsPassBoostFSBLowMassCRRECO")), 2, m_outputTag, 2, m_isSignal);
    m_eventsPassBoostFSBLowMassCRRECO_failBox.book((fs->mkdir("eventsPassBoostFSBLowMassCRRECO_failBox")), 2, m_outputTag, 2, m_isSignal);
    m_eventsPassBoostFSBLowMassCRRECO_oppoBox.book((fs->mkdir("eventsPassBoostFSBLowMassCRRECO_oppoBox")), 2, m_outputTag, 2, m_isSignal);
    m_eventsPassJetSelection.book((fs->mkdir("eventsPassJetSelection")), 2, m_outputTag, 0, m_isSignal);
  }
  if (m_doReco && m_doFast) {
    flavor = 5;
    std::cout << "BOOKING PLOTS FLAVOR 5" << std::endl;
    m_allEvents.book((fs->mkdir("allEvents")), 5, m_outputTag, false, m_isSignal);

    m_allOnShellEvents.book((fs->mkdir("allOnShellEvents")), 5, m_outputTag, false, m_isSignal);
    m_allOffShellEvents.book((fs->mkdir("allOffShellEvents")), 5, m_outputTag, false, m_isSignal);
    m_eventsFailResPassBoostRECO_onShell.book((fs->mkdir( "eventsFailResPassBoostRECO_onShell" )),            5, m_outputTag, false, m_isSignal);
    m_eventsFailResPassBoostRECO_offShell.book((fs->mkdir("eventsFailResPassBoostRECO_offShell")),            5, m_outputTag, false, m_isSignal);

    m_eventsFailResPassBoostRECO.book((fs->mkdir("eventsFailResPassBoostRECO")),            5, m_outputTag, false, m_isSignal);
    m_eventsPassBoostZMASSRECO.book((fs->mkdir("eventsPassBoostZMASSRECO")),                5, m_outputTag, false, m_isSignal);
    m_eventsPassBoostFSBRECO.book((fs->mkdir( "eventsPassBoostFSBRECO")),                   5, m_outputTag, true, m_isSignal);
    m_eventsPassResFailBoostRECO.book((fs->mkdir("eventsPassResFailBoostRECO")),            5, m_outputTag, false, m_isSignal);
    m_eventsPassResZMASSRECO.book((fs->mkdir("eventsPassResZMASSRECO")),                    5, m_outputTag, false, m_isSignal);
    m_eventsPassResFSBRECO.book((fs->mkdir("eventsPassResFSBRECO")),                        5, m_outputTag, true, m_isSignal);
/*    m_eventsPassResFailBoostRECO_mll450.book((fs->mkdir("eventsPassResFailBoostRECO_mll450")),            5, m_outputTag, false);
    m_eventsPassResFailBoostRECO_mll400.book((fs->mkdir("eventsPassResFailBoostRECO_mll400")),            5, m_outputTag, false);
    m_eventsPassResFailBoostRECO_mll500.book((fs->mkdir("eventsPassResFailBoostRECO_mll500")),            5, m_outputTag, false);
    m_eventsPassResFailBoostRECO_mll550.book((fs->mkdir("eventsPassResFailBoostRECO_mll550")),            5, m_outputTag, false);
    m_eventsPassResFailBoostRECO_mll600.book((fs->mkdir("eventsPassResFailBoostRECO_mll600")),            5, m_outputTag, false);
    m_eventsPassResFSBRECO_mll450.book((fs->mkdir("eventsPassResFSBRECO_mll450")),                        5, m_outputTag, true);
    m_eventsPassResFSBRECO_mll400.book((fs->mkdir("eventsPassResFSBRECO_mll400")),                        5, m_outputTag, true);
    m_eventsPassResFSBRECO_mll500.book((fs->mkdir("eventsPassResFSBRECO_mll500")),                        5, m_outputTag, true);
    m_eventsPassResFSBRECO_mll550.book((fs->mkdir("eventsPassResFSBRECO_mll550")),                        5, m_outputTag, true);
    m_eventsPassResFSBRECO_mll600.book((fs->mkdir("eventsPassResFSBRECO_mll600")),                        5, m_outputTag, true);*/


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
