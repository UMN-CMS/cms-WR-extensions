// -*- C++ -*-
//
// Package:    ExoAnalysis/analyzeLastBin
// Class:      analyzeLastBin
// 
/**\class analyzeLastBin analyzeLastBin.cc ExoAnalysis/analyzeLastBin/plugins/analyzeLastBin.cc

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
#include "analyzeLastBin.h"
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
analyzeLastBin::analyzeLastBin(const edm::ParameterSet& iConfig):
  m_highMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("highMuons"))),
  m_highElectronToken (consumes<std::vector<pat::Electron>> (iConfig.getParameter<edm::InputTag>("highElectrons"))),
  m_electronToken2 (consumes<std::vector<pat::Electron> > (iConfig.getParameter<edm::InputTag>("electrons2"))),
  m_eleIdFullInfoMapToken (consumes<edm::ValueMap<vid::CutFlowResult>> (iConfig.getParameter<edm::InputTag>("eleFullInfoMap"))),
  m_regMuonToken (consumes<std::vector<pat::Muon>> (iConfig.getParameter<edm::InputTag>("regMuons"))),
  m_recoJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("recoJets"))),
  m_AK4recoCHSJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK4recoCHSJets"))),
  m_PackedPFCandiates (consumes<std::vector<pat::PackedCandidate> > (iConfig.getParameter<edm::InputTag>("packedpfcandidate"))),
  m_AK8recoPUPPIJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("AK8recoPUPPIJets"))),
  m_AK8recoPUPPISubJetsToken (consumes<std::vector<pat::Jet>> (iConfig.getParameter<edm::InputTag>("subJetName"))),
  m_offlineVerticesToken (consumes<std::vector<reco::Vertex>> (iConfig.getParameter<edm::InputTag>("vertices"))),
  m_metToken (consumes<std::vector<pat::MET>> (iConfig.getParameter<edm::InputTag>("met"))),
  m_PUInfoToken (consumes< std::vector<PileupSummaryInfo>> (iConfig.getParameter<edm::InputTag>("edmPileupInfo"))),
  m_rhoLabel    (consumes<double> (iConfig.getParameter<edm::InputTag>("rhoLabel"))),
  m_wantHardProcessMuons (iConfig.getUntrackedParameter<bool>("wantHardProcessMuons",true)),
  m_doTrig (iConfig.getUntrackedParameter<bool>("doTrig",false)),
  m_jettiness (iConfig.getUntrackedParameter<std::string>("jettinessPUPPI")),
  JECUName   (iConfig.getUntrackedParameter<std::string>("jecUncName","")),
  //m_MCL (iConfig.getUntrackedParameter<double>("MCL", 400)),
  //m_MCU (iConfig.getUntrackedParameter<double>("MCU", 8000)),
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
  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/analyzeLastBins/data/";
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

analyzeLastBin::~analyzeLastBin() {
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
   if(m_doTrig)
     delete m_genericTriggerEventFlag;

}

//
// member functions
//
// ------------ method called for each event  ------------
void analyzeLastBin::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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
      muonTrigPass = passMuonTrig(iEvent, myRECOevent);
      myRECOevent.muonTrigPassBit = muonTrigPass;
  }


  myRECOevent.eventNumber = iEvent.id().event();
  myRECOevent.lumiSection = iEvent.luminosityBlock();

  myRECOevent.hasPVertex = myEventInfo.PVselection(vertices);
  std::cout << "myRECOevent.hasPVertex: " << myRECOevent.hasPVertex << endl;

  if(myRECOevent.hasPVertex){
  myRECOevent.nVtx = myEventInfo.nVtx;
  myRECOevent.PVertex = myEventInfo.PVertex;
  }
  myRECOevent.isMC = m_isMC;


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

  //here we start tracking our progress through the main cuts 
  myRECOevent.cutProgress = 1;
  myRECOevent.ResCutProgress = 1;
  myRECOevent.ResFSBCutProgress = 1;

//Playing with PackedCandidates - Delete 

  edm::Handle<std::vector<pat::PackedCandidate>> PFCandidates;
  iEvent.getByToken(m_PackedPFCandiates, PFCandidates);
  assert(PFCandidates.isValid());
  
  int nPFCand = 0;
  
  for(std::vector<pat::PackedCandidate>::const_iterator pfcand = PFCandidates->begin(); pfcand != PFCandidates->end(); pfcand++) {
     nPFCand = nPFCand + 1;
     std::cout << "nPFCand: " << nPFCand << std::endl;

     std::cout << "pfcand->energy(): " << pfcand->energy() << endl;

  }

  


  //THIS PART OF THE CODE RUNS THE ANALYSIS IN FAST MODE.  THE GOAL HERE IS TO PRODUCE ALL THE NECESSARY INFORMATION FOR HIGGS COMBINE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (m_doFast && myRECOevent.hasPVertex){
    std::cout << "Inside doFast running" << std::endl;

    std::vector<bool> passesResRECOAllRegions = passResRECO_Fast(iEvent , myRECOevent);

    if(myRECOevent.NresolvedANAElectronCands > 0)  tooManyResElectrons = true;
    if(myRECOevent.NresolvedANAMuonCands > 2)      tooManyResMuons = true;

    bool tooManyResLeptons = tooManyResElectrons || tooManyResMuons;

    std::cout << "passesResRECOAllRegions[0]: " << passesResRECOAllRegions[0] << " muonTrigPass: " << muonTrigPass << "ZMASSres: " << ZMASSres << " tooManyResLeptons: " << tooManyResLeptons << " myRECOevent.hasPVertex: " << myRECOevent.hasPVertex << " myRECOevent.resolvedRECOmass: " << myRECOevent.resolvedRECOmass << std::endl;


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
      myRECOevent.Muon_LooseTkIso_Weight = Muon_LooseTkIso_Weights[0];
      myRECOevent.Muon_HighPtID2nd_Weight = Muon_HighPtID2nd_Weights[0];
      myRECOevent.Muon_LooseTkIso2nd_Weight = Muon_LooseTkIso2nd_Weights[0];
      myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
      myRECOevent.Muon_RECO2nd_Weight = Muon_Reco2nd_Weights[0];
      myRECOevent.Muon_Trig_Weight = Muon_Trig_Weights[0];

      std::cout << "Setting Resolved SR Weight" << std::endl;

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
        muonTrigPass = passMuonTrig(iEvent, myRECOevent);
      }
      if(myRECOevent.myMuonJetPairs.size() > 0){
        addMuons = additionalMuons(iEvent, myRECOevent, false, false, 0, false);
        addElectrons = additionalElectrons(iEvent, myRECOevent, false, 0);
      }
      if(m_isMC && (addMuons || addMuons_JECUp || addMuons_JECDown || addMuons_JERUp || addMuons_JERDown)) {
	std::vector<double> Muon_LooseID_Weights;
        if(m_era == "2016") {
          Muon_LooseID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2016");
        }else if(m_era == "2017"){
          Muon_LooseID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2017");
        }else if(m_era == "2018"){
          Muon_LooseID_Weights = myMuons.MuonHighPTIDweight(myRECOevent.mySubleadMuon->pt(), myRECOevent.mySubleadMuon->eta(), "2018");
        }
        myRECOevent.Muon_LooseID_Weight = Muon_LooseID_Weights[0];
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
        myRECOevent.Muon_RECO_Weight = Muon_Reco_Weights[0];
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
      }
      //Fill histograms individually
      
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

      }
      ZMASS_Nom = 0;
      addMuons = false;
    

    addMuons = false;
    addElectrons = false;

    }
  }

  std::cout << "TIME TO FILL ALL EVENTS" << std::endl;
  m_allEvents.fill(myRECOevent, 1, m_isSignal);
  std::cout << "Done with event" << std::endl;
  
}
// A B
// C D
// DON'T CALL THIS UNLESS YOU PASS THE NOISO ELECTRON EVENT SELECTION
bool analyzeLastBin::sameSign(eventBits& myEvent, bool noISO) {
  if (noISO) {
    if (abs(myEvent.myElectronCand_noISO->charge() + myEvent.mySubleadMuon->charge()) == 2)
      return true;
  } else {
    if (abs(myEvent.myElectronCand->charge() + myEvent.mySubleadMuon->charge()) == 2)
      return true;
  }
  return false;
}
void analyzeLastBin::vertexDiff(eventBits& myEvent) {
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
bool analyzeLastBin::passElectronTrig(const edm::Event& iEvent, eventBits& myRECOevent) {
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

bool analyzeLastBin::passMuonTrig(const edm::Event& iEvent, eventBits& myRECOevent) {
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
  if(passTriggers) myRECOevent.muonTrigger = 1.;
  else myRECOevent.muonTrigger = 0.;

  return passTriggers;
}

bool analyzeLastBin::preSelectBoostReco(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myRECOevent) {
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
  electronSelection(iEvent, myRECOevent, m_isSignal, m_era);
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
bool analyzeLastBin::preSelectReco_Fast(const edm::Event& iEvent, const edm::EventSetup &iSetup, eventBits& myRECOevent) {
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
  electronSelection(iEvent, myRECOevent, m_isSignal, m_era);
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

bool analyzeLastBin::selectHighPtISOMuon(const edm::Event& iEvent, eventBits& myEvent) {

  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);

  for (std::vector<pat::Muon>::const_iterator iParticle = highMuons->begin(); iParticle != highMuons->end(); iParticle++) {
    std::cout<<iParticle->pt()<<std::endl;
    //NEEDS UPDATING
  }


  return false;
}
bool analyzeLastBin::additionalMuons(const edm::Event& iEvent, eventBits& myEvent, bool flavorSideband, bool ZPeak, int JetCorrectionRegion, bool ISO) {
  std::cout << "Round-up of the rag-tag muons" << std::endl;
  edm::Handle<std::vector<pat::Muon>> regMuons;
  iEvent.getByToken(m_regMuonToken, regMuons);
  std::vector<const pat::Muon*> allMuons;

  edm::Handle<std::vector<pat::Muon>> highMuons;
  iEvent.getByToken(m_highMuonToken, highMuons);

  double jetPhi = 0;
  double jetEta = 0;

  for(std::vector<pat::Muon>::const_iterator iMuon = highMuons->begin(); iMuon != highMuons->end(); iMuon++) {
    if ( iMuon->tunePMuonBestTrack()->pt() < 53 || fabs(iMuon->eta()) > 2.4) continue;  //10 GeV is designed to capture slow muons from Z->MUMU
    if ( ! iMuon->isHighPtMuon(*myEvent.PVertex) ) continue;  //highpT MuonID
    std::cout << "Additional Muon passes highpT ID" << std::endl;
    std::cout << "additional Muon pT: " << iMuon->tunePMuonBestTrack()->pt() << std::endl;
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
      if(myEvent.mySubleadMuon->tunePMuonBestTrack()->pt() < 200){
        myEvent.secondBoostMuonScale = myMuons.RochesterMethod_MCSmear(myEvent.mySubleadMuon, m_era);
      }else{
        myEvent.secondBoostMuonScale = myMuons.GE(myEvent.mySubleadMuon, m_era);
      }
    }
    else{
      if(myEvent.mySubleadMuon->tunePMuonBestTrack()->pt() < 200){
        myEvent.secondBoostMuonScale = myMuons.RochesterMethod_DataScale(myEvent.mySubleadMuon, m_era);
      }else{
        myEvent.secondBoostMuonScale = myMuons.GE(myEvent.mySubleadMuon, m_era);
      }
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

bool analyzeLastBin::additionalElectrons(const edm::Event& iEvent, eventBits& myEvent, bool isFSB, int JetCorrectionRegion) {
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
bool analyzeLastBin::resolvedMuonSelection(const edm::Event& iEvent, eventBits& myEvent) {
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
    if(resolvedANAMuons[0]->pt() < 200){
      myEvent.leadResMuonScale = myMuons.RochesterMethod_MCSmear(resolvedANAMuons[0], m_era);
    }else{
      myEvent.leadResMuonScale = myMuons.GE(resolvedANAMuons[0], m_era);
    }

    if(resolvedANAMuons[1]->pt() < 200){
     myEvent.secondResMuonScale = myMuons.RochesterMethod_MCSmear(resolvedANAMuons[1], m_era);
    }else{
      myEvent.secondResMuonScale = myMuons.GE(resolvedANAMuons[1], m_era);
    }
  }
  else{
    if(resolvedANAMuons[0]->pt() < 200){
      myEvent.leadResMuonScale = myMuons.RochesterMethod_DataScale(resolvedANAMuons[0], m_era);
    }else{
      myEvent.leadResMuonScale = myMuons.GE(resolvedANAMuons[0], m_era);
    }

    if(resolvedANAMuons[1]->pt() < 200){
      myEvent.secondResMuonScale = myMuons.RochesterMethod_DataScale(resolvedANAMuons[1], m_era);
    }else{
      myEvent.secondResMuonScale = myMuons.GE(resolvedANAMuons[1], m_era);
    }
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
bool analyzeLastBin::resolvedElectronSelection(const edm::Event& iEvent, eventBits& myEvent) {
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
//   const bool heepIDVID = vidResult->cutFlowPassed();
   const bool heepIDVID = electronPassesHEEP(iEvent, myEvent, &(*iElectron), false);
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

  std::cout << "high pT lead electron" << std::endl;
  std::cout << "60 GeV lead electron" << std::endl;
  if (resolvedANAElectrons[0]->pt()/**myEvent.leadResElectronScale[0]*/ <= 60) return false;
  myEvent.ResElecCutProgress++;

  myEvent.resolvedANAElectrons = resolvedANAElectrons;

  myEvent.resolvedSubleadMuPt = resolvedANAElectrons[1]->pt();
   
  return true;
}
bool analyzeLastBin::electronSelection(const edm::Event& iEvent, eventBits& myEvent, bool isSignal, std::string year) {  //Flavor sideband
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
    //const bool heepIDVID = vidResult->cutFlowPassed();
    const bool heepIDVID = electronPassesHEEP(iEvent, myEvent, &(*iElec), false);

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
        ((vidResult->getCutResultByIndex(cutnrs::HEEPV70::ECALDRIVEN   )  == true) || (isSignal == true && (year == "2017" || year == "2018")))
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
bool analyzeLastBin::muonSelection(const edm::Event& iEvent, eventBits& myEvent) {
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
  std::cout << "Number of Muons: " << highMuons->size() << std::endl;
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
        myEvent.leadBoostMuonScale = myMuons.GE(highPTMuons[0], m_era);
      }
    }
    else{
      if (highPTMuons[0]->pt() < 200){
        myEvent.leadBoostMuonScale = myMuons.RochesterMethod_DataScale(highPTMuons[0], m_era);
      }else{
        myEvent.leadBoostMuonScale = myMuons.GE(highPTMuons[0], m_era);
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
bool analyzeLastBin::resolvedJetSelection(const edm::Event& iEvent, eventBits& myEvent) {
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

  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/analyzeLastBins/data/";

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
    if ((m_era == "2016" && CEMF > .90) || ((m_era == "2017" || m_era == "2018") && CEMF > .80)) continue; 

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
bool analyzeLastBin::jetSelection(const edm::Event& iEvent,  eventBits& myEvent) {
   
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

  std::string jecPathname = cmsswPath + "/src/ExoAnalysis/analyzeLastBins/data/";
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

    std::cout << " iJet->pt(): " <<  iJet->pt() << " iJet->eta(): " << iJet->eta() << " iJet->phi(): " << iJet->phi() << std::endl;
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
    if (m_era == "2016" && CEMF > .99) continue;
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
double analyzeLastBin::PUPPIweight(double puppipt, double puppieta, bool isMC){

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
bool analyzeLastBin::passBoostRECO(const edm::Event& iEvent, eventBits& myRECOevent) { //LOOP OVE. JET MUON PAIRS AND TAKE THE HIGHEST MASS ONE

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
void analyzeLastBin::passExtensionRECO_Fast(const edm::Event& iEvent, eventBits& myRECOevent) {

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
bool analyzeLastBin::passResRECO(const edm::Event& iEvent, eventBits& myEvent) {
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
std::vector<bool> analyzeLastBin::passResRECO_Fast(const edm::Event& iEvent, eventBits& myEvent) {
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
//dR cut setting
static const float DRCUT = 0.4;
//static const float DRCUT = 0.5;
//static const float DRCUT = 0.6;
//static const float DRCUT = 0.7;
//static const float DRCUT = 0.8;
bool analyzeLastBin::electronPassesHEEP(const edm::Event& iEvent, eventBits& myEvent, const pat::Electron* electron, bool noIso) {
  std::string year = m_era;
  bool isSignal = m_isSignal;
  const vid::CutFlowResult* vidResult =  electron->userData<vid::CutFlowResult>("heepElectronID_HEEPV70");
  if(vidResult == NULL) {
    std::cout << "ERROR CANNOT FIND ELECTRON VID RESULTS" << std::endl;
    return false;
  }
  edm::Handle<double> rhoHandle;
  iEvent.getByToken(m_rhoLabel, rhoHandle);
  double rho = *(rhoHandle.product());

  double electron_dEtaInSeed         = vidResult->getValueCutUpon(cutnrs::HEEPV70::DETAINSEED);
  double electron_dPhiIn             = vidResult->getValueCutUpon(cutnrs::HEEPV70::DPHIIN);
  double electron_HoverE             = vidResult->getValueCutUpon(cutnrs::HEEPV70::HADEM);
  double electron_sig_ietaieta_5x5   = vidResult->getValueCutUpon(cutnrs::HEEPV70::SIGMAIETAIETA);
  double electron_EM_had_depIso      = vidResult->getValueCutUpon(cutnrs::HEEPV70::EMHADD1ISO);
  double electron_trackIso           = vidResult->getValueCutUpon(cutnrs::HEEPV70::TRKISO);
  double electron_innerLostHits      = vidResult->getValueCutUpon(cutnrs::HEEPV70::MISSHITS);
  double electron_dxy                = vidResult->getValueCutUpon(cutnrs::HEEPV70::DXY);
  double electron_eta                = vidResult->getValueCutUpon(cutnrs::HEEPV70::ETA);
  double electron_et                 = vidResult->getValueCutUpon(cutnrs::HEEPV70::ET);


  bool pass_electron_et                 = vidResult->getCutResultByIndex(cutnrs::HEEPV70::ET           );
  bool pass_electron_eta                = vidResult->getCutResultByIndex(cutnrs::HEEPV70::ETA          );
  bool pass_electron_dEtaInSeed         = vidResult->getCutResultByIndex(cutnrs::HEEPV70::DETAINSEED   );
  bool pass_electron_dPhiIn             = vidResult->getCutResultByIndex(cutnrs::HEEPV70::DPHIIN       );
  bool pass_electron_sig_ietaieta_5x5   = vidResult->getCutResultByIndex(cutnrs::HEEPV70::SIGMAIETAIETA);       
  bool pass_electron_e2x5_over_5x5      = vidResult->getCutResultByIndex(cutnrs::HEEPV70::E2X5OVER5X5  );
  bool pass_electron_trackIso           = vidResult->getCutResultByIndex(cutnrs::HEEPV70::TRKISO       ) || noIso;
  bool pass_electron_dxy                = vidResult->getCutResultByIndex(cutnrs::HEEPV70::DXY          ); 
  bool pass_electron_innerLostHits      = vidResult->getCutResultByIndex(cutnrs::HEEPV70::MISSHITS     ); 
  bool pass_electron_isEcalDriven       = vidResult->getCutResultByIndex(cutnrs::HEEPV70::ECALDRIVEN   ) || (isSignal == true && (year == "2017" || year == "2018"));

  bool pass_electron_HoverE        = false;
  bool pass_electron_EM_had_depIso = false;

  if( (year == "2018") && (electron_eta > 1.566) ) {
    double HoverE_cutVal      = ::wrTools::HoverE2018_cutCalc(     rho, electron_eta, electron->energy());
    double EMhadDepIso_cutVal = ::wrTools::EMhadDepIso2018_cutCalc(rho, electron_eta, electron_et);
    pass_electron_HoverE             = (electron_HoverE        < HoverE_cutVal );
    pass_electron_EM_had_depIso      = (electron_EM_had_depIso < EMhadDepIso_cutVal ); 
  }
  else  {
    pass_electron_HoverE             = vidResult->getCutResultByIndex(cutnrs::HEEPV70::HADEM        );
    pass_electron_EM_had_depIso      = vidResult->getCutResultByIndex(cutnrs::HEEPV70::EMHADD1ISO   ); 
  }

  bool passesHEEP = pass_electron_et               &&
                    pass_electron_eta              &&
                    pass_electron_dEtaInSeed       &&
                    pass_electron_dPhiIn           &&
                    pass_electron_sig_ietaieta_5x5 &&
                    pass_electron_e2x5_over_5x5    &&
                    pass_electron_trackIso         &&
                    pass_electron_dxy              &&
                    pass_electron_innerLostHits    &&
                    pass_electron_isEcalDriven     &&
                    pass_electron_HoverE           &&
                    pass_electron_EM_had_depIso    ;

  //myEvent.HoverE_cutVal = HoverE_cutVal;
  //myEvent.EMhadDepIso_cutVal = EMhadDepIso_cutVal;

  //myEvent.HoverE_val = electron_HoverE;
  //myEvent.EMhadDepIso_val = electron_EM_had_depIso;

  return passesHEEP;

}
void analyzeLastBin::loadCMSSWPath() {
    char* cmsswPathChar = getenv("CMSSW_BASE");
    if (cmsswPathChar == NULL) {
        std::cout << "Warning in JetLoader::loadCMSSWPath : CMSSW_BASE not detected." << std::endl;
        cmsswPath = "";
    }
    cmsswPath = std::string(cmsswPathChar);
}
// ------------ method called at the beginning of each run -----------------------------
void analyzeLastBin::beginRun(const edm::Run& run, const edm::EventSetup& setup )
{
  std::cout << "INITIALIZING RUN SPECIFIC STUFF" << std::endl;
  if(m_doTrig) {
    if ( m_genericTriggerEventFlag->on() ) m_genericTriggerEventFlag->initRun( run, setup );
  }


}

// ------------ method called once each job just before starting event loop  ------------
void 
analyzeLastBin::beginJob()
{
  std::cout << "BOOKING PLOTS" << std::endl;
  edm::Service<TFileService> fs; 

  if (m_doReco && m_doFast) {
    flavor = 5;
    std::cout << "BOOKING PLOTS FLAVOR 5" << std::endl;
    m_allEvents.book((fs->mkdir("allEvents")), 5, m_outputTag, false, m_isSignal);

    m_eventsFailResPassBoostRECO_onShell.book((fs->mkdir( "eventsFailResPassBoostRECO_onShell" )),            5, m_outputTag, false, m_isSignal);
    m_eventsFailResPassBoostRECO_offShell.book((fs->mkdir("eventsFailResPassBoostRECO_offShell")),            5, m_outputTag, false, m_isSignal);

    m_eventsFailResPassBoostRECO.book((fs->mkdir("eventsFailResPassBoostRECO")),            5, m_outputTag, false, m_isSignal);
    m_eventsPassResFailBoostRECO.book((fs->mkdir("eventsPassResFailBoostRECO")),            5, m_outputTag, false, m_isSignal);

  }
}

// ------------ method called once each job just after ending the event loop  ------------
void 
analyzeLastBin::endJob() 
{

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
analyzeLastBin::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
