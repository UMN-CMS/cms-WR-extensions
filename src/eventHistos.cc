#include "ExoAnalysis/cmsWRextensions/interface/eventHistos.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH2D.h"
//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/eventBits.h"
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"





eventHistos::eventHistos () {}




void eventHistos::book(TFileDirectory histoFolder, uint16_t flavor, std::string tag, int Region, bool isSignal) {

  //CREATED FOLDER IN HISTO
  m_flavor = flavor;
  m_histoFolder = histoFolder;
  m_FSB = Region;

  std::cout<<"TAGGING EVENTS WITH: "  <<tag<<std::endl;
  m_metaData = m_histoFolder.make<TH1D>("metadata", "events metadata", 10, 0.0, 10);
  m_metaData->GetXaxis()->SetBinLabel(1, tag.c_str());

  std::cout<<"TAG SET TO: "<<m_metaData->GetXaxis()->GetBinLabel(1)<<std::endl;

  if (m_flavor != 4 && m_flavor != 5) {
  //MAKE GEN PLOTS
    std::cout << "HERE WE CONSTRUCT THE PLOTS" << std::endl;
    m_eventsWeight = m_histoFolder.make<TH1D>("eventsWeight","number of events weighted", 1, 0.0, 1);

    m_RECOpasses   = m_histoFolder.make<TH1D>("RECOpasses","number of events passing each RECO category", 2, 0.5, 2.5);

    m_neutrinoDecays = m_histoFolder.make<TH1D>("neutrinoDecays", "; # neutrino decays to neutrino",                                                       2, -.5, 1.5);

    m_nLeptons       = m_histoFolder.make<TH1D>("nLeptons"     , "; # Leptons; Events per #"     ,                                                       10, -.5, 9.5);
    m_nMuons         = m_histoFolder.make<TH1D>("nMuons"       , "; # Muons; Events per #"       ,                                                       10, -.5, 9.5);
    m_nTaus          = m_histoFolder.make<TH1D>("nTaus"        , "; # Taus; Events per #"        ,                                                       10, -.5, 9.5);
    m_nElectrons     = m_histoFolder.make<TH1D>("nElectrons"   , "; # Electrons; Events per #"   ,                                                       10, -.5, 9.5);
    m_nLightPartons  = m_histoFolder.make<TH1D>("nLightPartons", "; # LightPartons; Events per #",                                                       10, -.5, 9.5);
    m_nTops          = m_histoFolder.make<TH1D>("nTops"        , "; # Tops; Events per #"        ,                                                       10, -.5, 9.5);
    m_nBs            = m_histoFolder.make<TH1D>("nBs"          , "; # Bs; Events per #"          ,                                                       10, -.5, 9.5);
    m_nPartons       = m_histoFolder.make<TH1D>("nPartons"     , "; # Partons; Events per #"     ,                                                       10, -.5, 9.5);
    
    m_NRenergy	     = m_histoFolder.make<TH1D>("NRenergy"     , "; NR Energy; Energy (GeV)"     ,                                                       50, -.5, 5000.0);
                                                                                 
    m_eventFlavor    = m_histoFolder.make<TH1D>("eventFlavor"       , "; Event Flavor; # Events with flavor"       ,                                                       10, -.5, 9.5);

    m_wrShellMass    = m_histoFolder.make<TH1D>("wrShellMass",   "; WR mass (GeV); # Events per bin", 100, 0, 7000);


    m_cutProgress       = m_histoFolder.make<TH1D>("cutProgress"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_cutProgressNoWeight       = m_histoFolder.make<TH1D>("cutProgressNoWeight"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);

    m_ResCutProgress       = m_histoFolder.make<TH1D>("ResCutProgress"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_ResCutProgressNoWeight       = m_histoFolder.make<TH1D>("ResCutProgressNoWeight"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_FSBcutProgress    = m_histoFolder.make<TH1D>("FSBcutProgress"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   10, -.5, 9.5);
    m_FSBcutProgressNoWeight    = m_histoFolder.make<TH1D>("FSBcutProgressNoWeight"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   10, -.5, 9.5);
    m_ResFSBCutProgress    = m_histoFolder.make<TH1D>("ResFSBcutProgress"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   20, -.5, 19.5);
    m_ResFSBCutProgressNoWeight    = m_histoFolder.make<TH1D>("ResFSBcutProgressNoWeight"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   20, -.5, 19.5);

    
    m_parton1Et =                       m_histoFolder.make<TH1D>("parton1Et", "Parton 1 Et;Et (GeV); ",                         80, 0.0, 4000);
    m_parton2Et =                       m_histoFolder.make<TH1D>("parton2Et", "Parton 2 Et;Et (GeV); ",                         80, 0.0, 4000);
    m_muon1Et =                         m_histoFolder.make<TH1D>("muon1Et", "Muon 1 Et;Et (GeV); ",                             80, 0.0, 4000);
    m_muon2Et =                         m_histoFolder.make<TH1D>("muon2Et", "Muon 2 Et;Et (GeV); ",                             80, 0.0, 4000);

    m_parton1Eta =                      m_histoFolder.make<TH1D>("parton1Eta", "Parton 1 Eta;Eta;",                        80, -4.0, 4.0);
    m_parton2Eta =                      m_histoFolder.make<TH1D>("parton2Eta", "Parton 2 Eta;Eta;",                        80, -4.0, 4.0);
    m_muon1Eta =                        m_histoFolder.make<TH1D>("muon1Eta", "Muon 1 eta;Eta;",                            80, -4.0, 4.0);
    m_muon2Eta =                        m_histoFolder.make<TH1D>("muon2Eta", "Muon 2 eta;Eta;",                            80, -4.0, 4.0);
    m_firstPartonJetEta     =           m_histoFolder.make<TH1D>("firstPartonJetEta", "Jet 1 Eta;Eta;",                    80, -4.0, 4.0);
    m_secondPartonJetEta    =           m_histoFolder.make<TH1D>("secondPartonJetEta", "Jet 2 Eta;Eta;",                   80, -4.0, 4.0);
    m_firstPartonAK8JetEta  =           m_histoFolder.make<TH1D>("firstPartonAK8JetEta", "AK8Jet 1 Eta;Eta;",              80, -4.0, 4.0); 
    m_secondPartonAK8JetEta =           m_histoFolder.make<TH1D>("secondPartonAK8JetEta", "AK8Jet 2 Eta;Eta;",             80, -4.0, 4.0);

    m_parton1Phi             =          m_histoFolder.make<TH1D>("parton1Phi", "Parton 1 Phi;Phi (rad);",                        80, -4.0, 4.0);        
    m_parton2Phi             =          m_histoFolder.make<TH1D>("parton2Phi", "Parton 2 Phi;Phi (rad);",                        80, -4.0, 4.0);       
    m_muon1Phi               =          m_histoFolder.make<TH1D>("muon1Phi", "Muon 1 Phi;Phi (rad);",                            80, -4.0, 4.0);        
    m_muon2Phi               =          m_histoFolder.make<TH1D>("muon2Phi", "Muon 2 Phi;Phi (rad);",                            80, -4.0, 4.0);        
    m_firstPartonJetPhi      =          m_histoFolder.make<TH1D>("firstPartonJetPhi", "Jet 1 Phi;Phi (rad);",                    80, -4.0, 4.0);     
    m_secondPartonJetPhi     =          m_histoFolder.make<TH1D>("secondPartonJetPhi", "Jet 2 Phi;Phi (rad);",                   80, -4.0, 4.0);       
    m_firstPartonAK8JetPhi   =          m_histoFolder.make<TH1D>("firstPartonAK8JetPhi", "AK8Jet 1 Phi;Phi (rad);",              80, -4.0, 4.0);        
    m_secondPartonAK8JetPhi  =          m_histoFolder.make<TH1D>("secondPartonAK8JetPhi", "AK8Jet 1 Phi;Phi (rad);",             80, -4.0, 4.0);           

    m_nSecondElectronCands  =           m_histoFolder.make<TH1D>("nSecondElectronCands", "number of second electron cands"      , 5, -0.5, 4.5);
    m_secondElecJetDR       =           m_histoFolder.make<TH1D>("secondElecJetDR"     , "selected jet second electron cand dR" , 50, 0.0, 1.0);
    m_secondElecPt          =           m_histoFolder.make<TH1D>("m_secondElecPt"      , "second electron cand pt"              , 50, 0.0, 2000.0);


    m_dRparton1parton2 =                m_histoFolder.make<TH1D>("dRparton1parton2", "deltaR between partons;delta R;",                       120, 0.0, 12.0);
    m_dRmuon1muon2 =                    m_histoFolder.make<TH1D>("dRmuon1muon2", "deltaR between muons;delta R;",                             120, 0.0, 12.0);
    m_dRparton1muon2 =                  m_histoFolder.make<TH1D>("dRparton1muon2", "deltaR between parton1 and muon2;delta R;",               120, 0.0, 12.0);
    m_dRparton1muon1 =                  m_histoFolder.make<TH1D>("dRparton1muon1", "deltaR between parton1 and muon1;delta R;",               120, 0.0, 12.0);
    m_dRparton2muon2 =                  m_histoFolder.make<TH1D>("dRparton2muon2", "deltaR between parton2 and muon2;delta R;",               120, 0.0, 12.0);
    m_dRparton2muon1 =                  m_histoFolder.make<TH1D>("dRparton2muon1", "deltaR between parton2 and muon1;delta R;",               120, 0.0, 12.0);
    m_dRparton1jet =                    m_histoFolder.make<TH1D>("dRparton1jet", "deltaR between parton1 and closest jet;delta R;",            40, 0.0,  0.4);
    m_dRparton2jet =                    m_histoFolder.make<TH1D>("dRparton2jet", "deltaR between parton2 and closest jet;delta R;",            40, 0.0,  0.4);
    m_dRparton1AK8jet =                 m_histoFolder.make<TH1D>("dRparton1AK8jet", "deltaR between parton1 and closest AK8 jet;delta R;",     40, 0.0,  0.8);
    m_dRparton2AK8jet =                 m_histoFolder.make<TH1D>("dRparton2AK8jet", "deltaR between parton2 and closest AK8 jet;delta R;",     40, 0.0,  0.8);
    m_minPartonMuon2dR =                m_histoFolder.make<TH1D>("minPartonMuon2dR", "minPartonMuon2dR", 40, 0.0, 2.0);

    m_firstPartonJetEt =                m_histoFolder.make<TH1D>("firstPartonJetEt", " Jet Et for Leading Parton;Et (GeV);",                      80,0.0,4000);
    m_secondPartonJetEt =               m_histoFolder.make<TH1D>("secondPartonJetEt"," Jet Et for Subleading Parton;Et (GeV);",                   80,0.0,4000);
    m_firstPartonJetEtHadronic =        m_histoFolder.make<TH1D>("firstPartonJetEtHadronic", "Hadronic Jet Et for Leading Parton;Et (GeV);",      80,0.0,4000);
    m_secondPartonJetEtHadronic =       m_histoFolder.make<TH1D>("secondPartonJetEtHadronic","Hadronic Jet Et for Subleading Parton;Et (GeV);",   80,0.0,4000);
    m_firstPartonJetEtEM =              m_histoFolder.make<TH1D>("firstPartonJetEtEM", "EM Jet Et for Leading Parton;Et (GeV);",                  80,0.0,4000);
    m_secondPartonJetEtEM =             m_histoFolder.make<TH1D>("secondPartonJetEtEM","EM Jet Et for Subleading Parton;Et (GeV);",               80,0.0,4000);
    m_firstPartonJetEtInvisible =       m_histoFolder.make<TH1D>("firstPartonJetEtInvisible", "Invisible Jet Et for Leading Parton;Et (GeV);",    80,0.0,4000);
    m_secondPartonJetEtInvisible =      m_histoFolder.make<TH1D>("secondPartonJetEtInvisible","Invisible Jet Et for Subleading Parton;Et (GeV);", 80,0.0,4000);

    m_firstPartonAK8JetEt =             m_histoFolder.make<TH1D>("firstPartonAK8JetEt", " AK8Jet Et for Leading Parton;Et (GeV);",                      80,0.0,4000);
    m_secondPartonAK8JetEt =            m_histoFolder.make<TH1D>("secondPartonAK8JetEt"," AK8Jet Et for Subleading Parton;Et (GeV);",                   80,0.0,4000);
    m_firstPartonAK8JetEtHadronic =     m_histoFolder.make<TH1D>("firstPartonAK8JetEtHadronic", "Hadronic AK8Jet Et for Leading Parton;Et (GeV);",      80,0.0,4000);
    m_secondPartonAK8JetEtHadronic =    m_histoFolder.make<TH1D>("secondPartonAK8JetEtHadronic","Hadronic AK8Jet Et for Subleading Parton;Et (GeV);",   80,0.0,4000);
    m_firstPartonAK8JetEtEM =           m_histoFolder.make<TH1D>("firstPartonAK8JetEtEM", "EM AK8Jet Et for Leading Parton;Et (GeV);",                  80,0.0,4000);
    m_secondPartonAK8JetEtEM =          m_histoFolder.make<TH1D>("secondPartonAK8JetEtEM","EM AK8Jet Et for Subleading Parton;Et (GeV);",               80,0.0,4000);
    m_firstPartonAK8JetEtInvisible =    m_histoFolder.make<TH1D>("firstPartonAK8JetEtInvisible", "Invisible AK8Jet Et for Leading Parton;Et (GeV);",    80,0.0,4000);
    m_secondPartonAK8JetEtInvisible =   m_histoFolder.make<TH1D>("secondPartonAK8JetEtInvisible","Invisible AK8Jet Et for Subleading Parton;Et (GeV);", 80,0.0,4000);


    m_leadSubleadingJetsMuonsMass  =    m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsMass","Four Object Mass of the 2 leading Jets and Muons;Mass (GeV);",      80, 0.0,4000);
    m_leadSubleadingAK8JetsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsMass","Four Object Mass of the 2 leading AK8Jets and Muons;Mass (GeV);",80, 0.0,4000);
    m_leadSubleadingPartonsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsMass","Four Object Mass of the 2 leading Partons and Muons;Mass (GeV);",80, 0.0,4000);
    m_leadAK8JetMuonMass  =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,240, 0.0,6000);
    m_leadAK8JetMuonMass_noLSF  =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,240, 0.0,6000);
    m_leadAK8JetDiMuonMass  =           m_histoFolder.make<TH1D>("leadAK8JetDiMuonMass","3 Object Mass of the leading Jet and DiMuon;Mass (GeV);"                         ,60, 0.0,6000);
    m_leadAK8JetElectronMass  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                         ,240, 0.0,6000);
    m_leadAK8JetElectronMass_noISO  =     m_histoFolder.make<TH1D>("leadAK8JetElectronMass_noISO","2 Object Mass of the leading Jet and nonISO Electron;Mass (GeV);"            ,60, 0.0,6000);

    m_leadSubleadingJetsMuonsPt  =      m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsPt","Four Object Pt of the 2 leading Jets and Muons; Pt (GeV);",          80, 0.0,4000);
    m_leadSubleadingAK8JetsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsPt","Four Object Pt of the 2 leading AK8Jets and Muons; Pt (GeV);",    80, 0.0,4000);
    m_leadSubleadingPartonsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsPt","Four Object Pt of the 2 leading Partons and Muons; Pt (GeV);",    80, 0.0,4000);
    m_leadAK8JetMuonPt  =             m_histoFolder.make<TH1D>("leadAK8JetMuonPt","2 Object Pt of the leading Jet and Muon; Pt (GeV);"                             ,80, 0.0,4000);
    m_leadAK8JetElectronPt  =             m_histoFolder.make<TH1D>("leadAK8JetElectronPt","2 Object Pt of the leading Jet and Electron; Pt (GeV);"                             ,80, 0.0,4000);
    m_leadAK8JetElectronPt_noISO  =       m_histoFolder.make<TH1D>("leadAK8JetElectronPt_noISO","2 Object Pt of the leading Jet and nonISO Electron; Pt (GeV);"            ,80, 0.0,4000);

    //m_leadSubleadingJetsMuonsEta  =     m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsEta","Four Object Eta of the 2 leading Jets and Muons",        80, -4.0,4.0);
    //m_leadSubleadingAK8JetsMuonsEta  =  m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsEta","Four Object Eta of the 2 leading AK8Jets and Muons",  80, -4.0,4.0);
    //m_leadSubleadingPartonsMuonsEta  =  m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsEta","Four Object Eta of the 2 leading Partons and Muons",  80, -4.0,4.0);
    //m_leadAK8JetMuonEta  =              m_histoFolder.make<TH1D>("leadAK8JetMuonEta","2 Object Eta of the leading Jet and Muon"                           ,80, -4.0,4.0);
    m_leadAK8JetMuonDR  =              m_histoFolder.make<TH1D>("leadAK8JetMuonDR","2 Object delta R of the leading Jet and Muon; dR (rad);"                     ,60, -6.4,6.4);
    m_leadAK8JetMuonPhi  =              m_histoFolder.make<TH1D>("leadAK8JetMuonAbsdphi","2 Object delta Phi of the leading Jet and Muon; #phi (rad);"                     ,60, 2.0,3.2);
    m_leadAK8JetElectronPhi  =          m_histoFolder.make<TH1D>("leadAK8JetElectronAbsdphi","2 Object delta Phi of the leading Jet and Electron; #phi (rad);"                     ,60, 2.0,3.2);
    m_leadAK8JetElectronPhi_noISO  =    m_histoFolder.make<TH1D>("leadAK8JetElectronAbsdphi_noISO","2 Object delta Phi of the leading Jet and nonISO Electron; #phi (rad);"           ,60, 2.0,3.2);

    m_leadAK8JetMuonJetMuonEnergyFraction =   m_histoFolder.make<TH1D>("leadAK8JetMuonJetMuonEnergyFraction", "muon energy fraction of ak8jet in the selected jet lepton pair; percent muon energy;", 100, 0.0, 1.0);
    m_leadAK8JetElectronJetMuonEnergyFraction =   m_histoFolder.make<TH1D>("leadAK8JetElectronJetMuonEnergyFraction", "muon energy fraction of ak8jet in the selected jet electron pair; percent muon energy;", 100, 0.0, 1.0);
    m_leadAK8JetElectronJetMuonEnergyFraction_noISO =   m_histoFolder.make<TH1D>("leadAK8JetElectronJetMuonEnergyFraction_noISO", "muon energy fraction of ak8jet in the selected jet non ISO electron pair; percent muon energy;", 100, 0.0, 1.0);

    m_electronTrigger =   m_histoFolder.make<TH1D>("electronTrigger", "electron trigger bit; TriggerBit;", 2, -0.5, 1.5);

    m_muonTrigger =   m_histoFolder.make<TH1D>("muonTrigger", "muon trigger bit; TriggerBit;", 2, -0.5, 1.5);

    m_EtPlacementMuon2 =                m_histoFolder.make<TH1D>("EtPlacementMuon2", "Et ranking idx of muon 2;index;",                                 5, -.5, 4.5);
    m_nVertices =                    m_histoFolder.make<TH1D>("nVertices", ";#  Vertices;",                                                       70, -.5, 69.5);

    m_vertexDiffTan  =               m_histoFolder.make<TH1D>("vertexDiffTan", "GEN vs RECO primary vertex tangential difference"    ,             100, 0.0, 10);
    m_vertexDiffLon  =               m_histoFolder.make<TH1D>("vertexDiffLon", "GEN vs RECO primary vertex longitudinal difference"  ,             100, 0.0, 10);
    //m_nJets =                           m_histoFolder.make<TH1D>("nJets", "# of matched jets",                                                   5, -.5, 4.5);
    m_nAK8Jets =                        m_histoFolder.make<TH1D>("nAK8Jets", ";#  AK8Jets;",                                                       10, -.5, 9.5);
    m_nMuonCands =                    m_histoFolder.make<TH1D>("nMuonCands", ";#  Muons;",                                                       10, -.5, 9.5);
    m_nElectronCands50 =                    m_histoFolder.make<TH1D>("nElectronCands50" , ";#  Electrons Pt > 50;" ,                                        10, -.5, 9.5);
    m_nElectronCands100 =                   m_histoFolder.make<TH1D>("nElectronCands100", ";#  Electrons Pt > 100;",                                        10, -.5, 9.5);
    m_nElectronCands150 =                   m_histoFolder.make<TH1D>("nElectronCands150", ";#  Electrons Pt > 150;",                                        10, -.5, 9.5);
    m_nElectronCands200 =                   m_histoFolder.make<TH1D>("nElectronCands200", ";#  Electrons Pt > 200;",                                        10, -.5, 9.5);
    m_nElectronCands50_noISO =                    m_histoFolder.make<TH1D>("nElectronCands50_noISO" , ";#  Non Isolated Electrons Pt > 50;" ,                                        10, -.5, 9.5);
    m_nElectronCands200_noISO =                   m_histoFolder.make<TH1D>("nElectronCands200_noISO", ";#  Non Isolated Electrons Pt > 200;",                                        10, -.5, 9.5);
    m_nMuons10 =                        m_histoFolder.make<TH1D>("nMuons10", ";#  Muons above 10 GeV;",                                                       10, -.5, 9.5);
    m_nAK8Jets40 =                      m_histoFolder.make<TH1D>("nAK8Jets40", ";#  AK8Jets above 40 GeV;",                                                       10, -.5, 9.5);
    m_nCandidateJets =                      m_histoFolder.make<TH1D>("nCandidateJets", ";#  AK8Jets;",                                  10, -.5, 9.5);
    m_nAdditionalHEEP =                    m_histoFolder.make<TH1D>("nAdditionalHEEP", ";#  Electrons passing HEEP;",                                            10, -.5, 9.5);
    m_nAdditionalHEEP_noISO =             m_histoFolder.make<TH1D>("nAdditionalHEEP_noISO", ";#  Electrons passing HEEP (non ISO);",                           10, -.5, 9.5);
    m_nHighPtMuonsOutsideJet =                    m_histoFolder.make<TH1D>("nHighPtMuonsOutsideJet", ";#  Muons passing High pT ID;",                                            10, -.5, 9.5);

    m_secondHighPtMuonPt  =                 m_histoFolder.make<TH1D>("secondHighPtMuonPt"  ,"2nd High Muon Pt; 2nd High-p_{T} Muon p_{T} (GeV);"  ,60,0.0,3000 );
    m_secondHighPtMuonPhi =                 m_histoFolder.make<TH1D>("secondHighPtMuonPhi" ,"2nd High Muon Phi; 2nd High-p_{T} Muon #phi (rad);" ,30,-4.0,4.0 );
    m_secondHighPtMuonEta =                 m_histoFolder.make<TH1D>("secondHighPtMuonEta" ,"2nd High Muon Eta; 2nd High-p_{T} Muon #eta (rad);" ,30,-3.0,3.0 );
    m_selectedJetSecondHighPtMuonDR   =                 m_histoFolder.make<TH1D>("selectedJetSecondHighPtMuonDR"  ,"Selected Jet 2nd High Muon DR; #Delta R_{Selected Jet, 2nd High Muon}", 30, -4.0, 4.0);

    m_selectedMuonPt  =                 m_histoFolder.make<TH1D>("selectedMuonPt"  ,"Selected Muon Pt; High-p_{T} Muon p_{T} (GeV);"  ,60,0.0,3000 ); 
    m_selectedElectronPt  =                 m_histoFolder.make<TH1D>("selectedElectronPt"  ,"Selected Electron pT; Electron p_{T} (GeV);"  ,40,0.0,2000 ); 
    m_selectedJetPt   =                 m_histoFolder.make<TH1D>("selectedJetPt"   ,"Selected Jet Pt; Jet p_{T} (GeV);"   ,40,0.0,2000 );  
    m_selectedMuonPhi =                 m_histoFolder.make<TH1D>("selectedMuonPhi" ,"Selected Muon Phi; High-p_{T} Muon #phi (rad);" ,30,-4.0,4.0 );  
    m_selectedElectronPhi =                 m_histoFolder.make<TH1D>("selectedElectronPhi" ,"Selected Electron Phi; Electron #phi (rad);" ,30,-4.0,4.0 );  
    m_selectedJetPhi  =                 m_histoFolder.make<TH1D>("selectedJetPhi"  ,"Selected Jet Phi; Jet #phi (rad);"  ,30,-4.0,4.0 );   
    m_selectedMuonEta =                 m_histoFolder.make<TH1D>("selectedMuonEta" ,"Selected Muon Eta; High-p_{T} Muon #eta (rad);" ,30,-3.0,3.0 );   
    m_selectedElectronEta =                 m_histoFolder.make<TH1D>("selectedElectronEta" ,"Selected Electron Eta; Electron #eta (rad);" ,30,-3.0,3.0 );
    m_selectedElectronEta_Weight1 =                 m_histoFolder.make<TH1D>("selectedElectronEta_NoWeight" ,"Selected Electron Eta; Electron #eta (rad);" ,30,-3.0,3.0 );   
    m_selectedJetEta  =                 m_histoFolder.make<TH1D>("selectedJetEta"  ,"Selected Jet Eta; Jet #eta (rad);"  ,30,-3.0,3.0 ); 
    m_selectedJetMass  =                 m_histoFolder.make<TH1D>("selectedJetMass"  ,"Selected Jet Mass; Jet softdrop mass (GeV);"  ,60,0.0, 2000.0 );
    m_selectedJetTau21  =                 m_histoFolder.make<TH1D>("selectedJetTau21"  ,"Selected Jet Tau21; Jet #tau_{21};"  ,30,0.0, 1.0 );
    m_selectedJetMaxDRGenDaughters  =                 m_histoFolder.make<TH1D>("selectedJetMaxDRGenDaughters"  ,"Selected Jet Max DR GenDaughters; Max #Delta R_{Selected Jet, WR Daughters}", 30, 0.0, 4.0);
    m_selectedJetLSF3   =                 m_histoFolder.make<TH1D>("selectedJetLSF3"   ,"Selected Jet LSF_{3}; Jet LSF_{3};"   ,40,0.0,1.0 );
    m_selectedJetMaxSubJetCSV   =                 m_histoFolder.make<TH1D>("selectedJetMaxSubJetCSV"   ,"Selected Jet Max Subjet CSV; Max Subjet CSV;"   ,40,0.0,1.0 );
    m_selectedJetEnergy   =                 m_histoFolder.make<TH1D>("selectedJetEnergy"   ,"Selected Jet energy; Energy;"   ,50,0.0,5000.0 );
    m_selectedJetEnergyUncorr   =                 m_histoFolder.make<TH1D>("selectedJetEnergyUncorr"   ,"Selected Jet energy; Energy;"   ,50,0.0,5000.0 );


    m_DrDaughters =                  m_histoFolder.make<TH1D>("DrDaughters" , "DR WR DAughters; #DeltaR W_{R} Daughters", 30, 0.0, 4.0);
    m_nWRDaughters=                  m_histoFolder.make<TH1D>("nWRDaughters" , "Number of WR Daughters; Number of W_{R} Daughters", 10, -0.5, 9.5);
    m_capturedBothDaughtersInSingleJet= m_histoFolder.make<TH1D>("capturedBothDaughtersInSingleJet", "Captured Both Daughters in Single Jet; W_{R} Daughters in Single Jet", 4, -1.5,2.5);
    m_pickedCorrectJet =                m_histoFolder.make<TH1D>("pickedCorrectJet", "Picked Correct Jet; Selected Jet Contains both Daughters", 4, -1.5, 2.5);
    m_dPhiLeadMuonJetWithDaughters =   m_histoFolder.make<TH1D>("dPhiLeadMuonJetWithDaughters", "dPhi Lead Muon and Jet with WR Daughters; #Delta#phi (rad)", 30.,0,4.0);
    m_selectedIncorrectJetMass =   m_histoFolder.make<TH1D>("selectedIncorrectJetMass", "Selected Incorrect Jet Mass; soft-drop mass (GeV)", 60,0.0, 2000.0 );
    m_JetWithDaughtersMass =   m_histoFolder.make<TH1D>("JetWithDaughtersMass", "Jet With Daughters Mass; soft-drop mass (GeV)", 60,0.0, 2000.0 );
    m_secondGENMuonRECOjetDR = m_histoFolder.make<TH1D>("secondGENMuonRECOjetDR", "Second GENMuon RECO Jet DR; #DeltaR Sub-lead GEN Muon RECO jet", 30, 0.0, 6.0);
    m_secondRECOMuonRECOjetDR = m_histoFolder.make<TH1D>("secondRECOMuonRECOjetDR", "Second RECO Muon RECO Jet DR; #DeltaR Sub-lead RECO Muon RECO jet", 30, -3.0, 3.0);

    m_selectedElectron_noISO_Pt  =      m_histoFolder.make<TH1D>("selectedElectron_noISO_Pt"   ,"Selected nonISO Electron pT; Electron p_{T} (GeV);"  ,40,0.0,2000 ); 
    m_selectedElectron_noISO_Eta =      m_histoFolder.make<TH1D>("selectedElectron_noISO_Eta"  ,"Selected nonISO Electron Eta; Electron #eta (rad);" ,30,-3.0,3.0 );   
    m_selectedElectron_noISO_Phi =      m_histoFolder.make<TH1D>("selectedElectron_noISO_Phi"  ,"Selected nonISO Electron Phi; Electron #phi (rad);" ,30,-4.0,4.0 );  
    m_selectedJet_EleNoISO_Pt   =       m_histoFolder.make<TH1D>("selectedJet_EleNoISO_Pt"     ,"Selected (with nonISO Electron) Jet Pt; Jet p_{T} (GeV);"   ,40,0.0,2000 );  
    m_selectedJet_EleNoISO_Phi  =       m_histoFolder.make<TH1D>("selectedJet_EleNoISO_Phi"    ,"Selected (with nonISO Electron) Jet Phi; Jet #phi (rad);"  ,30,-4.0,4.0 );   
    m_selectedJet_EleNoISO_Eta  =       m_histoFolder.make<TH1D>("selectedJet_EleNoISO_Eta"    ,"Selected (with nonISO Electron) Jet Eta; Jet #eta (rad);"  ,30,-3.0,3.0 ); 
    m_selectedJet_EleNoISO_Mass  =      m_histoFolder.make<TH1D>("selectedJet_EleNoISO_Mass"   ,"Selected (with nonISO Electron) Jet Mass; Jet softdrop mass (GeV);"  ,60,0.0, 300.0 );
    m_selectedJet_EleNoISO_Tau21  =     m_histoFolder.make<TH1D>("selectedJet_EleNoISO_Tau21"  ,"Selected (with nonISO Electron) Jet Tau21; Jet #tau_{21};"  ,30,0.0, 1.0 );

    m_subleadMuon_selJetdPhi       =    m_histoFolder.make<TH1D>("subleadMuonSelJetdPhi" ,   "Sublead Muon Selected Jet dPhi; #phi (rad);"  ,80,  0.0, 4.0);
    m_subleadMuon_selElectronPhi  =    m_histoFolder.make<TH1D>("subleadMuonSelElectrondPhi", "Sublead Muon Selected Electron dPhi; #phi (rad);" ,80,  0.0, 4.0);
    m_subleadMuon_selElectronMass  =    m_histoFolder.make<TH1D>("subleadMuonSelElectronMass", "Sublead Muon Selected Electron Mass; Mass (GeV);" ,100, 0.0,2000);
    m_subleadMuon_selElectronZMass =    m_histoFolder.make<TH1D>("subleadMuonSelElectronZMass","Sublead Muon Selected Electron Mass; Mass (GeV);" ,100, 0.0,200);
    m_subleadMuon_selElectronPt    =    m_histoFolder.make<TH1D>("subleadMuonSelElectronPt"  , "Sublead Muon Selected Electron Pt; Pt (GeV);"   ,100, 0.0,2000);
    m_subleadMuon_selMuondPhi  =    m_histoFolder.make<TH1D>("subleadMuonSelMuondPhi", "Sublead Muon Selected Muon dPhi; #phi (rad);" ,80,  0.0, 4.0);
    m_subleadMuon_selMuondR    =    m_histoFolder.make<TH1D>("subleadMuonSelMuondR",   "Sublead Muon Selected Muon dR; #dR ;" ,80,  0.0, 6.0);
    m_secondGENMuon_selMuondR    =    m_histoFolder.make<TH1D>("subleadMuonSelMuondR",   "Sublead Muon Selected Muon dR; #dR ;" ,80,  0.0, 6.0);
    m_subleadMuon_selMuonMass  =    m_histoFolder.make<TH1D>("subleadMuonSelMuonMass", "Sublead Muon Selected Muon Mass; Mass (GeV);" ,100, 0.0,2000);
    m_subleadMuon_selMuonZMass =    m_histoFolder.make<TH1D>("subleadMuonSelMuonZMass","Sublead Muon Selected Muon Mass; Mass (GeV);" ,100, 0.0,200);
    m_subleadMuon_selMuonZMass_Weight1 =    m_histoFolder.make<TH1D>("subleadMuonSelMuonZMass_NoWeight","Sublead Muon Selected Muon Mass; Mass (GeV);" ,100, 0.0,200);
    m_subleadMuon_selMuonPt    =    m_histoFolder.make<TH1D>("subleadMuonSelMuonPt"  , "Sublead Muon Selected Muon Pt; Pt (GeV);"   ,100, 0.0,2000);

    m_subleadMuonPt		    =    m_histoFolder.make<TH1D>("subleadMuonPt"           ,  "Sublead Muon Pt; Pt  (GeV);"              ,100,  0.0, 1000);
    m_subleadMuonEt                 =    m_histoFolder.make<TH1D>("subleadMuonEt"           ,  "Sublead Muon Et; Et  (GeV);"              ,100,  0.0, 1000);
    m_subleadMuonEta                =    m_histoFolder.make<TH1D>("subleadMuonEta"          ,  "Sublead Muon Eta; Loose Muon #eta (rad);"                    , 60, -3.0,  3.0);
    m_subleadMuonPhi                =    m_histoFolder.make<TH1D>("subleadMuonPhi"          ,  "Sublead Muon Phi; Loose Muon #phi (rad);"              , 80, -4.0,  4.0);

    m_dRmuon2                      =    m_histoFolder.make<TH1D>("dRmuon2"                 ,   "Sublead Muon - 2nd Gen Muon delta R; deltaR;"         , 60, 0.0, 6.0);
    m_secondMuonWRjetdR          =    m_histoFolder.make<TH1D>("secondMuonWRjetdR"     ,   "Sublead Muon - Gen WR decay quarks delta R; deltaR;"         , 60, 0.0, 6.0);

    m_genLSF                      =   m_histoFolder.make<TH1D>("genLSF"                 ,  "gen-level LSF;"          , 20, 0.0, 1.0);

    m_MET                          =    m_histoFolder.make<TH1D>("MET"                 ,"MET;MET (GeV);# events",                       100, 0.0,2000); //THIS LINE WORKS
    m_MET_selJetdPhi               =    m_histoFolder.make<TH1D>("METselJetdPhi"      ,"MET Selected Jet dPhi; #phi (rad);",      80, 0.0 ,4.0);
    m_MET_selMuondPhi              =    m_histoFolder.make<TH1D>("METselMuondPhi"     ,"MET Selected Muon dPhi; #phi (rad);",     80, 0.0 ,4.0);
    m_MET_selElectrondPhi              =    m_histoFolder.make<TH1D>("METselElectrondPhi"     ,"MET Selected Electron dPhi; #phi (rad);",     80, 0.0 ,4.0);
    m_MET_selJetMass               =    m_histoFolder.make<TH1D>("METselJetMass"      ,"MET Selected Jet Mass; Mass (GeV);",     100,0.0 ,2000);
    m_MET_selMuonMass              =    m_histoFolder.make<TH1D>("METselMuonMass"     ,"MET Selected Muon Mass; Mass (GeV);",    100,0.0 ,2000);
    m_MET_selElectronMass              =    m_histoFolder.make<TH1D>("METselElectronMass"     ,"MET Selected Electron Mass; Mass (GeV);",    100,0.0 ,2000);
    m_MET_selJetPt                 =    m_histoFolder.make<TH1D>("METselJetPt"        ,"MET Selected Jet Pt; Pt (GeV);",       100,0.0 ,2000);
    m_MET_selMuonPt                =    m_histoFolder.make<TH1D>("METselMuonPt"       ,"MET Selected Muon Pt; Pt (GeV);",      100,0.0 ,2000);
    m_MET_selElectronPt                =    m_histoFolder.make<TH1D>("METselElectronPt"       ,"MET Selected Electron Pt; Pt (GeV);",      100,0.0 ,2000);

    m_selectedJetTransMET          =    m_histoFolder.make<TH1D>("selectedJetTransMET",";MET Transverse to Select Jet (GeV);", 100, 0.0, 500);

    m_MuonWeight                   =    m_histoFolder.make<TH1D>("MuonWeight",";Muon Weight", 40, 0.0, 2.0);
    m_PUWeight                     =    m_histoFolder.make<TH1D>("PUWeight",";Pileup Weight", 40, 0.0, 2.0);

    m_HEEP_SF                      =    m_histoFolder.make<TH1D>("HEEP_SF",";HEEP Weight", 40, 0.0, 2.0);
    m_HEEP_SF_E                      =    m_histoFolder.make<TH1D>("HEEP_SF_E",";HEEP Weight Endcap Electron", 40, 0.0, 2.0);
    m_HEEP_SF_B                      =    m_histoFolder.make<TH1D>("HEEP_SF_B",";HEEP Weight Barrel Electron", 40, 0.0, 2.0);
    m_FSBfinalEventWeight_E             =    m_histoFolder.make<TH1D>("FSBfinalEventWeight_E",";final EventWeight FSB Endcap", 40, 0.0, 2.0);
    m_FSBfinalEventWeight_B             =    m_histoFolder.make<TH1D>("FSBfinalEventWeight_B",";final EventWeight FSB Barrel", 40, 0.0, 2.0);

    m_HEEP_SF_noISO                 =    m_histoFolder.make<TH1D>("HEEP_SF_noISO",";HEEP Weight nonISO", 40, 0.0, 2.0);                                        
    m_HEEP_SF_E_noISO               =    m_histoFolder.make<TH1D>("HEEP_SF_E_noISO",";HEEP Weight Endcap nonISO Electron", 40, 0.0, 2.0);                          
    m_HEEP_SF_B_noISO               =    m_histoFolder.make<TH1D>("HEEP_SF_B_noISO",";HEEP Weight Barrel nonISO Electron", 40, 0.0, 2.0);                       
    m_FSBfinalEventWeight_E_noISO   =    m_histoFolder.make<TH1D>("FSBfinalEventWeight_E_noISO",";final EventWeight FSB Endcap nonISO", 40, 0.0, 2.0);        
    m_FSBfinalEventWeight_B_noISO   =    m_histoFolder.make<TH1D>("FSBfinalEventWeight_B_noISO",";final EventWeight FSB Barrel nonISO", 40, 0.0, 2.0);         

    m_Electron_Reco_SF_noISO        =    m_histoFolder.make<TH1D>("finalEventWeight_noISO",";finalEventWeight nonISO", 40, 0.0, 2.0);

    m_finalEventWeight             =    m_histoFolder.make<TH1D>("finalEventWeight",";finalEventWeight", 40, 0.0, 2.0);
    m_Electron_Reco_SF             =    m_histoFolder.make<TH1D>("finalEventWeight",";finalEventWeight", 40, 0.0, 2.0);

    m_selElectron_barrel_dEtaInSeed        =  m_histoFolder.make<TH1D>("selElectron_barrel_dEtaInSeed"      ,";selected Electron barrel dEtaInSeed"      , 100, 0.0, .01); 
    m_selElectron_barrel_dPhiIn            =  m_histoFolder.make<TH1D>("selElectron_barrel_dPhiIn"          ,";selected Electron barrel dPhiIn"          , 100, 0.0, .1); 
    m_selElectron_barrel_HoverE            =  m_histoFolder.make<TH1D>("selElectron_barrel_HoverE"          ,";selected Electron barrel HoverE"          , 100, 0.0, 2.0); 
    m_selElectron_barrel_sig_ietaieta_5x5  =  m_histoFolder.make<TH1D>("selElectron_barrel_sig_ietaieta_5x5",";selected Electron barrel sig_ietaieta_5x5", 100, 0.0, .05);
//    m_selElectron_barrel_E2x5vE5x5         =  m_histoFolder.make<TH1D>("selElectron_barrel_E2x5vE5x5"       ,";selected Electron barrel E2x5vE5x5"       , 100, 0.0, ); 
    m_selElectron_barrel_EM_had_depIso     =  m_histoFolder.make<TH1D>("selElectron_barrel_EM_had_depIso"   ,";selected Electron barrel EM_had_depIso"   , 100, 0.0, 20); 
    m_selElectron_barrel_trackIso          =  m_histoFolder.make<TH1D>("selElectron_barrel_trackIso"        ,";selected Electron barrel trackIso"        , 100, 0.0, 10); 
    m_selElectron_barrel_innerLostHits     =  m_histoFolder.make<TH1D>("selElectron_barrel_innerLostHits"   ,";selected Electron barrel innerLostHits"   ,   5, 0.0, 1); 
    m_selElectron_barrel_dxy               =  m_histoFolder.make<TH1D>("selElectron_barrel_dxy"             ,";selected Electron barrel dxy"             , 100, 0.0, .1); 
                                                                                                                                                  
    m_selElectron_endcap_dEtaInSeed        =  m_histoFolder.make<TH1D>("selElectron_endcap_dEtaInSeed"      ,";selected Electron endcap dEtaInSeed"      , 100, 0.0, .01); 
    m_selElectron_endcap_dPhiIn            =  m_histoFolder.make<TH1D>("selElectron_endcap_dPhiIn"          ,";selected Electron endcap dPhiIn"          , 100, 0.0, .1); 
    m_selElectron_endcap_HoverE            =  m_histoFolder.make<TH1D>("selElectron_endcap_HoverE"          ,";selected Electron endcap HoverE"          , 100, 0.0, 2.0); 
    m_selElectron_endcap_sig_ietaieta_5x5  =  m_histoFolder.make<TH1D>("selElectron_endcap_sig_ietaieta_5x5",";selected Electron endcap sig_ietaieta_5x5", 100, 0.0, .05);
//    m_selElectron_endcap_E2x5vE5x5         =  m_histoFolder.make<TH1D>("selElectron_endcap_E2x5vE5x5"       ,";selected Electron endcap E2x5vE5x5"       , 100, 0.0, ); 
    m_selElectron_endcap_EM_had_depIso     =  m_histoFolder.make<TH1D>("selElectron_endcap_EM_had_depIso"   ,";selected Electron endcap EM_had_depIso"   , 100, 0.0, 20); 
    m_selElectron_endcap_trackIso          =  m_histoFolder.make<TH1D>("selElectron_endcap_trackIso"        ,";selected Electron endcap trackIso"        , 100, 0.0, 10); 
    m_selElectron_endcap_innerLostHits     =  m_histoFolder.make<TH1D>("selElectron_endcap_innerLostHits"   ,";selected Electron endcap innerLostHits"   ,   5, 0.0, 1); 
    m_selElectron_endcap_dxy               =  m_histoFolder.make<TH1D>("selElectron_endcap_dxy"             ,";selected Electron endcap dxy"             , 100, 0.0, .1); 

    m_selElectron_noISO_barrel_dEtaInSeed        =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_dEtaInSeed"      ,";selected nonISO Electron barrel dEtaInSeed"      , 100, 0.0, .006); 
    m_selElectron_noISO_barrel_dPhiIn            =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_dPhiIn"          ,";selected nonISO Electron barrel dPhiIn"          , 100, 0.0, .06); 
    m_selElectron_noISO_barrel_HoverE            =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_HoverE"          ,";selected nonISO Electron barrel HoverE"          , 100, 0.0, 1.0); 
    m_selElectron_noISO_barrel_sig_ietaieta_5x5  =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_sig_ietaieta_5x5",";selected nonISO Electron barrel sig_ietaieta_5x5", 100, 0.0, .03);
    m_selElectron_noISO_barrel_EM_had_depIso     =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_EM_had_depIso"   ,";selected nonISO Electron barrel EM_had_depIso"   , 100, 0.0, 10); 
    m_selElectron_noISO_barrel_trackIso          =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_trackIso"        ,";selected nonISO Electron barrel trackIso"        , 50, 0.0, 15); 
    m_selElectron_noISO_barrel_innerLostHits     =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_innerLostHits"   ,";selected nonISO Electron barrel innerLostHits"   ,   5, 0.0, 1); 
    m_selElectron_noISO_barrel_dxy               =  m_histoFolder.make<TH1D>("selElectron_noISO_barrel_dxy"             ,";selected nonISO Electron barrel dxy"             , 100, 0.0, .05); 
    m_selElectron_noISO_endcap_dEtaInSeed        =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_dEtaInSeed"      ,";selected nonISO Electron endcap dEtaInSeed"      , 100, 0.0, .006); 
    m_selElectron_noISO_endcap_dPhiIn            =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_dPhiIn"          ,";selected nonISO Electron endcap dPhiIn"          , 100, 0.0, .06); 
    m_selElectron_noISO_endcap_HoverE            =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_HoverE"          ,";selected nonISO Electron endcap HoverE"          , 100, 0.0, 1.0); 
    m_selElectron_noISO_endcap_sig_ietaieta_5x5  =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_sig_ietaieta_5x5",";selected nonISO Electron endcap sig_ietaieta_5x5", 100, 0.0, .03);
    m_selElectron_noISO_endcap_EM_had_depIso     =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_EM_had_depIso"   ,";selected nonISO Electron endcap EM_had_depIso"   , 100, 0.0, 10); 
    m_selElectron_noISO_endcap_trackIso          =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_trackIso"        ,";selected nonISO Electron endcap trackIso"        , 50, 0.0, 15); 
    m_selElectron_noISO_endcap_innerLostHits     =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_innerLostHits"   ,";selected nonISO Electron endcap innerLostHits"   ,   5, 0.0, 1); 
    m_selElectron_noISO_endcap_dxy               =  m_histoFolder.make<TH1D>("selElectron_noISO_endcap_dxy"             ,";selected nonISO Electron endcap dxy"             , 100, 0.0, .05); 

    m_dRlsfLep_subleadMuon                       =  m_histoFolder.make<TH1D>("dRlsfLep_subleadMuon" , ";delta R between lsf lepton and subleading Muon", 100, 0.0, 1.0); 
    //GEN Z
    m_genZmass                                   =  m_histoFolder.make<TH1D>("genZmass",";Gen Z Mass (GeV);" , 50, 0.0, 1000);
    m_genZpt                                     =  m_histoFolder.make<TH1D>("genZpt",  ";Gen Z Pt (GeV);"   , 50, 0.0, 1000);

    m_genZptVSreco                               =  m_histoFolder.make<TH2D>("genZptVSreco", "Gen Z Pt versus reco mumu pt;  RECO mumu pt (GeV) ; GEN Z pt (GeV)" , 50, 0.0, 1000, 50, 0.0, 1000);
  //RESOLVED ANA   
    m_resJetDR                   = m_histoFolder.make<TH1D>("resJetDR"                ,";resolved jets deltaR", 80, 0.0, 8.0);
    m_resolvedRECOmass           = m_histoFolder.make<TH1D>("resolvedRECOmass"        ,";resolved RECO 4-object mass", 60, 0.0, 6000);
    m_resolvedGENmass            = m_histoFolder.make<TH1D>("resolvedGENmass"         ,";resolved GEN 4-object mass" , 60, 0.0, 6000);

    m_resSubleadMuJet1dR         = m_histoFolder.make<TH1D>("resSubleadMuJet1dR"      ,";resolved sublead muon lead jet dR"     , 80, 0.0, 8.0);
    m_resSubleadMuJet2dR         = m_histoFolder.make<TH1D>("resSubleadMuJet2dR"      ,";resolved sublead muon sublead jet dR"  , 80, 0.0, 8.0);
    m_resLeadMuJet1dR            = m_histoFolder.make<TH1D>("resLeadMuJet1dR"         ,";resolved lead muon lead jet dR"        , 80, 0.0, 8.0);
    m_resLeadMuJet2dR            = m_histoFolder.make<TH1D>("resLeadMuJet2dR"         ,";resolved lead muon sublead jet dR"     , 80, 0.0, 8.0);
                                                                                  
    m_resSubleadMuParton1dR      = m_histoFolder.make<TH1D>("resSubleadMuParton1dR"   ,";resolved second muon lead parton dR"   , 80, 0.0, 8.0);
    m_resSubleadMuParton2dR      = m_histoFolder.make<TH1D>("resSubleadMuParton2dR"   ,";resolved second muon sublead parton dR", 80, 0.0, 8.0);
    m_resLeadMuParton1dR         = m_histoFolder.make<TH1D>("resLeadMuParton1dR"      ,";resolved first muon lead parton dR"    , 80, 0.0, 8.0);
    m_resLeadMuParton2dR         = m_histoFolder.make<TH1D>("resLeadMuParton2dR"      ,";resolved first muon sublead parton dR" , 80, 0.0, 8.0);

    m_resMLL                     = m_histoFolder.make<TH1D>("resMLL"                  ,";resolved lepton-lepton mass"           , 100, 0.0, 2000);
    m_resolvedSubleadMuPt        = m_histoFolder.make<TH1D>("resolvedSubleadMuPt"     ,";resolved Sublead Muon Pt"              , 100, 0.0, 1000);

    m_resFSBJetDR                = m_histoFolder.make<TH1D>("resFSBJetDR"             ,";resolvedFSB jets deltaR", 80, 0.0, 8.0);
    m_resolvedFSBRECOmass        = m_histoFolder.make<TH1D>("resolvedFSBRECOmass"     ,";resolvedFSB RECO 4-object mass", 60, 0.0, 6000);

    m_resFSBElecJet1dR         = m_histoFolder.make<TH1D>("resFSBElecJet1dR"   ,";resolvedFSB electron lead jet dR"     , 80, 0.0, 8.0);
    m_resFSBElecJet2dR         = m_histoFolder.make<TH1D>("resFSBElecJet2dR"   ,";resolvedFSB electron sublead jet dR"  , 80, 0.0, 8.0);
    m_resFSBMuonJet1dR         = m_histoFolder.make<TH1D>("resFSBMuonJet1dR"      ,";resolvedFSB muon lead jet dR"        , 80, 0.0, 8.0);
    m_resFSBMuonJet2dR         = m_histoFolder.make<TH1D>("resFSBMuonJet2dR"      ,";resolvedFSB muon sublead jet dR"     , 80, 0.0, 8.0);

    m_resFSBElec_pt              = m_histoFolder.make<TH1D>("resFSBElec_pt ", ";resolved FSB electron pt"  , 100, 0.0, 2000);
    m_resFSBElec_phi             = m_histoFolder.make<TH1D>("resFSBElec_phi", ";resolved FSB electron phi" , 80, -4.0, 4.0);
    m_resFSBElec_eta             = m_histoFolder.make<TH1D>("resFSBElec_eta", ";resolved FSB electron eta" , 80, -4.0, 4.0);

    m_resFSBMuon_pt              = m_histoFolder.make<TH1D>("resFSBMuon_pt ", ";resolved FSB muon pt"      , 100, 0.0, 2000);
    m_resFSBMuon_phi             = m_histoFolder.make<TH1D>("resFSBMuon_phi", ";resolved FSB muon phi"     , 80, -4.0, 4.0);
    m_resFSBMuon_eta             = m_histoFolder.make<TH1D>("resFSBMuon_eta", ";resolved FSB muon eta"     , 80, -4.0, 4.0);
    
    m_genE_v_recoE				 =  m_histoFolder.make<TH2D>("genE_v_recoE"  , "Neutrino genE vs recoE"         , 50, 0.0, 5000, 50, 0.0, 5000);

//2D LSF PLOTS
    m_genLSF_v_recoLSF                           =  m_histoFolder.make<TH2D>("genLSF_v_recoLSF"  , "genLSF vs recoLSF"         , 20, 0.0, 1.0, 20, 0.0, 1.0);
    m_recoLSF_v_selJetPt                         =  m_histoFolder.make<TH2D>("recoLSF_v_selJetPt", "recoLSF vs selected Jet Pt", 20, 0.0, 2000, 20, 0.0, 1.0);
    m_lsfLepDR_v_recoLSF                         =  m_histoFolder.make<TH2D>("lsfLepDR_v_recoLSF", "lsf lepton and subleadMuon dR vs recoLSF", 100, 0.0, 1.0, 20, 0.0, 1.0); 
    m_lsfLepDR_v_selJetPt                        =  m_histoFolder.make<TH2D>("lsfLepDR_v_selJetPt","lsf lepton and subleadMuon dR vs selected Jet pT", 100, 0.0, 1.0, 20, 0.0, 2000); 


//
//
//  } else if (m_flavor == 2) {
//  //MAKE RECO PLOTS
//
//
//
//
//  } else if (m_flavor == 3) {
//  //MAKE GEN & RECO PLOTS
//
//
//  } else {
//  //NOTHING FOR NOW
//  }
//
  } else if (m_flavor == 4) {
    m_eventsWeight = m_histoFolder.make<TH1D>("eventsWeight","number of events weighted", 1, 0.0, 1);

//THIS FLAVOR IS FOR THE STREAMLINED ANALYSIS.  IT ONLY CONTAINS PLOTS NECESSARY FOR THE FINAL ANALYSIS
  }else if (m_flavor == 5){
    float binBoundaries[12] = {200, 300, 400, 500, 600, 700, 800, 900, 1000, 1150, 1500, 6000};


    std::cout << "Creating Flavor 5 plots" << std::endl;
    m_eventsWeight = m_histoFolder.make<TH1D>("eventsWeight","number of events weighted", 1, 0.0, 1);
    m_ResCutProgress       = m_histoFolder.make<TH1D>("ResCutProgress"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_ResCutProgressNoWeight       = m_histoFolder.make<TH1D>("ResCutProgressNoWeight"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_ResFSBCutProgress    = m_histoFolder.make<TH1D>("ResFSBcutProgress"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   20, -.5, 19.5);
    m_ResFSBCutProgressNoWeight    = m_histoFolder.make<TH1D>("ResFSBcutProgressNoWeight"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   20, -.5, 19.5);
    m_cutProgress       = m_histoFolder.make<TH1D>("cutProgress"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_cutProgressNoWeight       = m_histoFolder.make<TH1D>("cutProgressNoWeight"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_FSBcutProgress    = m_histoFolder.make<TH1D>("FSBcutProgress"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   10, -.5, 9.5);
    m_FSBcutProgressNoWeight    = m_histoFolder.make<TH1D>("FSBcutProgressNoWeight"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   10, -.5, 9.5);
    m_wrShellMass    = m_histoFolder.make<TH1D>("wrShellMass",   "; WR mass (GeV); # Events per bin", 100, 0, 7000);
    m_leadAK8JetMuonMass      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_JECUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JECUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_JECDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JECDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_JERUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JERUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_JERDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JERDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_PUUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_PUUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_PUDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_PUDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuIDUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuIDUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuIDDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuIDDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuTrigUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuTrigUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuTrigDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuTrigDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuResolUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuResolUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuResolDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuResolDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JECUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JECUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JECDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JECDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JERUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JERUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JERDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JERDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_PUUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_PUUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_PUDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_PUDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuIDUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuIDUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuIDDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuIDDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuTrigUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuTrigUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuTrigDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuTrigDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuResolUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuResolUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuResolDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuResolDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_JECUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JECUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_JECDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JECDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_JERUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JERUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_JERDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JERDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_PUUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_PUUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_PUDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_PUDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_MuLUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_MuLUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_MuLDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_MuLDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_HEEPUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_HEEPUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_HEEPDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_HEEPDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_ElHLTUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_ElHLTUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetElectronMass_ElHLTDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_ElHLTDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass  =           m_histoFolder.make<TH1D>("resolvedRECOmass","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_MuResIDUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_MuResIDUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_MuResIDDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_MuResIDDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_MuTrigUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_MuTrigUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_MuTrigDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_MuTrigDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_PUUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_PUUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_PUDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_PUDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_MuResolUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_MuResolUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_MuResolDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_MuResolDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_JECUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_JECUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_JECDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_JECDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_JERUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_JERUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_JERDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_JERDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_JECUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_JECUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_JECDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_JECDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_JERUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_JERUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_JERDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_JERDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_PUUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_PUUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_PUDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_PUDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_MuIDUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_MuIDUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_MuIDDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_MuIDDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_HEEPUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_HEEPUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedFSBRECOmass_HEEPDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_HEEPDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
   m_resolvedFSBRECOmass_MuTrigUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_MuTrigUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
   m_resolvedFSBRECOmass_MuTrigDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_MuTrigDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
   m_resolvedFSBRECOmass_MuResolUp  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_MuResolUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
   m_resolvedFSBRECOmass_MuResolDown  =           m_histoFolder.make<TH1D>("resolvedFSBRECOmass_MuResolDown","4 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
   if(isSignal){
    m_Scale_muR1_muF1 = m_histoFolder.make<TH1D>("m_Scale_muR1_muF1","number of events weighted", 1, 0.0, 1);
    m_Scale_muR2_muF1 = m_histoFolder.make<TH1D>("m_Scale_muR2_muF1","number of events weighted", 1, 0.0, 1);
    m_Scale_muRp5_muF1 = m_histoFolder.make<TH1D>("m_Scale_muRp5_muF1","number of events weighted", 1, 0.0, 1);
    m_Scale_muR1_muF2 = m_histoFolder.make<TH1D>("m_Scale_muR1_muF2","number of events weighted", 1, 0.0, 1);
    m_Scale_muR2_muF2 = m_histoFolder.make<TH1D>("m_Scale_muR2_muF2","number of events weighted", 1, 0.0, 1);
    m_Scale_muR1_muFp5 = m_histoFolder.make<TH1D>("m_Scale_muR1_muFp5","number of events weighted", 1, 0.0, 1);
    m_Scale_muRp5_muFp5 = m_histoFolder.make<TH1D>("m_Scale_muRp5_muFp5","number of events weighted", 1, 0.0, 1);
    m_Error_1047 = m_histoFolder.make<TH1D>("m_Error_1047","number of events weighted", 1, 0.0, 1);
    m_Error_1048 = m_histoFolder.make<TH1D>("m_Error_1048","number of events weighted", 1, 0.0, 1);
    m_Error_1049 = m_histoFolder.make<TH1D>("m_Error_1049","number of events weighted", 1, 0.0, 1);
    m_Error_1050 = m_histoFolder.make<TH1D>("m_Error_1050","number of events weighted", 1, 0.0, 1);
    m_Error_1051 = m_histoFolder.make<TH1D>("m_Error_1051","number of events weighted", 1, 0.0, 1);
    m_Error_1052 = m_histoFolder.make<TH1D>("m_Error_1052","number of events weighted", 1, 0.0, 1);
    m_Error_1053 = m_histoFolder.make<TH1D>("m_Error_1053","number of events weighted", 1, 0.0, 1);
    m_Error_1054 = m_histoFolder.make<TH1D>("m_Error_1054","number of events weighted", 1, 0.0, 1);
    m_Error_1055 = m_histoFolder.make<TH1D>("m_Error_1055","number of events weighted", 1, 0.0, 1);
    m_Error_1056 = m_histoFolder.make<TH1D>("m_Error_1056","number of events weighted", 1, 0.0, 1);
    m_Error_1057 = m_histoFolder.make<TH1D>("m_Error_1057","number of events weighted", 1, 0.0, 1);
    m_Error_1058 = m_histoFolder.make<TH1D>("m_Error_1058","number of events weighted", 1, 0.0, 1);
    m_Error_1059 = m_histoFolder.make<TH1D>("m_Error_1059","number of events weighted", 1, 0.0, 1);
    m_Error_1060 = m_histoFolder.make<TH1D>("m_Error_1060","number of events weighted", 1, 0.0, 1);
    m_Error_1061 = m_histoFolder.make<TH1D>("m_Error_1061","number of events weighted", 1, 0.0, 1);
    m_Error_1062 = m_histoFolder.make<TH1D>("m_Error_1062","number of events weighted", 1, 0.0, 1);
    m_Error_1063 = m_histoFolder.make<TH1D>("m_Error_1063","number of events weighted", 1, 0.0, 1);
    m_Error_1064 = m_histoFolder.make<TH1D>("m_Error_1064","number of events weighted", 1, 0.0, 1);
    m_Error_1065 = m_histoFolder.make<TH1D>("m_Error_1065","number of events weighted", 1, 0.0, 1);
    m_Error_1066 = m_histoFolder.make<TH1D>("m_Error_1066","number of events weighted", 1, 0.0, 1);
    m_Error_1067 = m_histoFolder.make<TH1D>("m_Error_1067","number of events weighted", 1, 0.0, 1);
    m_Error_1068 = m_histoFolder.make<TH1D>("m_Error_1068","number of events weighted", 1, 0.0, 1);
    m_Error_1069 = m_histoFolder.make<TH1D>("m_Error_1069","number of events weighted", 1, 0.0, 1);
    m_Error_1070 = m_histoFolder.make<TH1D>("m_Error_1070","number of events weighted", 1, 0.0, 1);
    m_Error_1071 = m_histoFolder.make<TH1D>("m_Error_1071","number of events weighted", 1, 0.0, 1);
    m_Error_1072 = m_histoFolder.make<TH1D>("m_Error_1072","number of events weighted", 1, 0.0, 1);
    m_Error_1073 = m_histoFolder.make<TH1D>("m_Error_1073","number of events weighted", 1, 0.0, 1);
    m_Error_1074 = m_histoFolder.make<TH1D>("m_Error_1074","number of events weighted", 1, 0.0, 1);
    m_Error_1075 = m_histoFolder.make<TH1D>("m_Error_1075","number of events weighted", 1, 0.0, 1);
    m_Error_1076 = m_histoFolder.make<TH1D>("m_Error_1076","number of events weighted", 1, 0.0, 1);
    m_Error_1077 = m_histoFolder.make<TH1D>("m_Error_1077","number of events weighted", 1, 0.0, 1);
    m_Error_1078 = m_histoFolder.make<TH1D>("m_Error_1078","number of events weighted", 1, 0.0, 1);
    m_Error_1079 = m_histoFolder.make<TH1D>("m_Error_1079","number of events weighted", 1, 0.0, 1);
    m_Error_1080 = m_histoFolder.make<TH1D>("m_Error_1080","number of events weighted", 1, 0.0, 1);
    m_Error_1081 = m_histoFolder.make<TH1D>("m_Error_1081","number of events weighted", 1, 0.0, 1);
    m_Error_1082 = m_histoFolder.make<TH1D>("m_Error_1082","number of events weighted", 1, 0.0, 1);
    m_Error_1083 = m_histoFolder.make<TH1D>("m_Error_1083","number of events weighted", 1, 0.0, 1);
    m_Error_1084 = m_histoFolder.make<TH1D>("m_Error_1084","number of events weighted", 1, 0.0, 1);
    m_Error_1085 = m_histoFolder.make<TH1D>("m_Error_1085","number of events weighted", 1, 0.0, 1);
    m_Error_1086 = m_histoFolder.make<TH1D>("m_Error_1086","number of events weighted", 1, 0.0, 1);
    m_Error_1087 = m_histoFolder.make<TH1D>("m_Error_1087","number of events weighted", 1, 0.0, 1);
    m_Error_1088 = m_histoFolder.make<TH1D>("m_Error_1088","number of events weighted", 1, 0.0, 1);
    m_Error_1089 = m_histoFolder.make<TH1D>("m_Error_1089","number of events weighted", 1, 0.0, 1);
    m_Error_1090 = m_histoFolder.make<TH1D>("m_Error_1090","number of events weighted", 1, 0.0, 1);
    m_Error_1091 = m_histoFolder.make<TH1D>("m_Error_1091","number of events weighted", 1, 0.0, 1);
    m_Error_1092 = m_histoFolder.make<TH1D>("m_Error_1092","number of events weighted", 1, 0.0, 1);
    m_Error_1093 = m_histoFolder.make<TH1D>("m_Error_1093","number of events weighted", 1, 0.0, 1);
    m_Error_1094 = m_histoFolder.make<TH1D>("m_Error_1094","number of events weighted", 1, 0.0, 1);
    m_Error_1095 = m_histoFolder.make<TH1D>("m_Error_1095","number of events weighted", 1, 0.0, 1);
    m_Error_1096 = m_histoFolder.make<TH1D>("m_Error_1096","number of events weighted", 1, 0.0, 1);
    m_Error_1097 = m_histoFolder.make<TH1D>("m_Error_1097","number of events weighted", 1, 0.0, 1);
    m_Error_1098 = m_histoFolder.make<TH1D>("m_Error_1098","number of events weighted", 1, 0.0, 1);
    m_Error_1099 = m_histoFolder.make<TH1D>("m_Error_1099","number of events weighted", 1, 0.0, 1);
    m_Error_1100 = m_histoFolder.make<TH1D>("m_Error_1100","number of events weighted", 1, 0.0, 1);
    m_Error_1101 = m_histoFolder.make<TH1D>("m_Error_1101","number of events weighted", 1, 0.0, 1);
    m_Error_1102 = m_histoFolder.make<TH1D>("m_Error_1102","number of events weighted", 1, 0.0, 1);
    m_Error_1103 = m_histoFolder.make<TH1D>("m_Error_1103","number of events weighted", 1, 0.0, 1);
    m_Error_1104 = m_histoFolder.make<TH1D>("m_Error_1104","number of events weighted", 1, 0.0, 1);
    m_Error_1105 = m_histoFolder.make<TH1D>("m_Error_1105","number of events weighted", 1, 0.0, 1);
    m_Error_1106 = m_histoFolder.make<TH1D>("m_Error_1106","number of events weighted", 1, 0.0, 1);
    m_Error_1107 = m_histoFolder.make<TH1D>("m_Error_1107","number of events weighted", 1, 0.0, 1);
    m_Error_1108 = m_histoFolder.make<TH1D>("m_Error_1108","number of events weighted", 1, 0.0, 1);
    m_Error_1109 = m_histoFolder.make<TH1D>("m_Error_1109","number of events weighted", 1, 0.0, 1);
    m_Error_1110 = m_histoFolder.make<TH1D>("m_Error_1110","number of events weighted", 1, 0.0, 1);
    m_Error_1111 = m_histoFolder.make<TH1D>("m_Error_1111","number of events weighted", 1, 0.0, 1);
    m_Error_1112 = m_histoFolder.make<TH1D>("m_Error_1112","number of events weighted", 1, 0.0, 1);
    m_Error_1113 = m_histoFolder.make<TH1D>("m_Error_1113","number of events weighted", 1, 0.0, 1);
    m_Error_1114 = m_histoFolder.make<TH1D>("m_Error_1114","number of events weighted", 1, 0.0, 1);
    m_Error_1115 = m_histoFolder.make<TH1D>("m_Error_1115","number of events weighted", 1, 0.0, 1);
    m_Error_1116 = m_histoFolder.make<TH1D>("m_Error_1116","number of events weighted", 1, 0.0, 1);
    m_Error_1117 = m_histoFolder.make<TH1D>("m_Error_1117","number of events weighted", 1, 0.0, 1);
    m_Error_1118 = m_histoFolder.make<TH1D>("m_Error_1118","number of events weighted", 1, 0.0, 1);
    m_Error_1119 = m_histoFolder.make<TH1D>("m_Error_1119","number of events weighted", 1, 0.0, 1);
    m_Error_1120 = m_histoFolder.make<TH1D>("m_Error_1120","number of events weighted", 1, 0.0, 1);
    m_Error_1121 = m_histoFolder.make<TH1D>("m_Error_1121","number of events weighted", 1, 0.0, 1);
    m_Error_1122 = m_histoFolder.make<TH1D>("m_Error_1122","number of events weighted", 1, 0.0, 1);
    m_Error_1123 = m_histoFolder.make<TH1D>("m_Error_1123","number of events weighted", 1, 0.0, 1);
    m_Error_1124 = m_histoFolder.make<TH1D>("m_Error_1124","number of events weighted", 1, 0.0, 1);
    m_Error_1125 = m_histoFolder.make<TH1D>("m_Error_1125","number of events weighted", 1, 0.0, 1);
    m_Error_1126 = m_histoFolder.make<TH1D>("m_Error_1126","number of events weighted", 1, 0.0, 1);
    m_Error_1127 = m_histoFolder.make<TH1D>("m_Error_1127","number of events weighted", 1, 0.0, 1);
    m_Error_1128 = m_histoFolder.make<TH1D>("m_Error_1128","number of events weighted", 1, 0.0, 1);
    m_Error_1129 = m_histoFolder.make<TH1D>("m_Error_1129","number of events weighted", 1, 0.0, 1);
    m_Error_1130 = m_histoFolder.make<TH1D>("m_Error_1130","number of events weighted", 1, 0.0, 1);
    m_Error_1131 = m_histoFolder.make<TH1D>("m_Error_1131","number of events weighted", 1, 0.0, 1);
    m_Error_1132 = m_histoFolder.make<TH1D>("m_Error_1132","number of events weighted", 1, 0.0, 1);
    m_Error_1133 = m_histoFolder.make<TH1D>("m_Error_1133","number of events weighted", 1, 0.0, 1);
    m_Error_1134 = m_histoFolder.make<TH1D>("m_Error_1134","number of events weighted", 1, 0.0, 1);
    m_Error_1135 = m_histoFolder.make<TH1D>("m_Error_1135","number of events weighted", 1, 0.0, 1);
    m_Error_1136 = m_histoFolder.make<TH1D>("m_Error_1136","number of events weighted", 1, 0.0, 1);
    m_Error_1137 = m_histoFolder.make<TH1D>("m_Error_1137","number of events weighted", 1, 0.0, 1);
    m_Error_1138 = m_histoFolder.make<TH1D>("m_Error_1138","number of events weighted", 1, 0.0, 1);
    m_Error_1139 = m_histoFolder.make<TH1D>("m_Error_1139","number of events weighted", 1, 0.0, 1);
    m_Error_1140 = m_histoFolder.make<TH1D>("m_Error_1140","number of events weighted", 1, 0.0, 1);
    m_Error_1141 = m_histoFolder.make<TH1D>("m_Error_1141","number of events weighted", 1, 0.0, 1);
    m_Error_1142 = m_histoFolder.make<TH1D>("m_Error_1142","number of events weighted", 1, 0.0, 1);
    m_Error_1143 = m_histoFolder.make<TH1D>("m_Error_1143","number of events weighted", 1, 0.0, 1);
    m_Error_1144 = m_histoFolder.make<TH1D>("m_Error_1144","number of events weighted", 1, 0.0, 1);
    m_Error_1145 = m_histoFolder.make<TH1D>("m_Error_1145","number of events weighted", 1, 0.0, 1);
    m_Error_1146 = m_histoFolder.make<TH1D>("m_Error_1146","number of events weighted", 1, 0.0, 1);
    m_Error_alphasUp = m_histoFolder.make<TH1D>("m_Error_alphasUp","number of events weighted", 1, 0.0, 1);
    m_Error_alphasDown = m_histoFolder.make<TH1D>("m_Error_alphasDown","number of events weighted", 1, 0.0, 1);


    m_leadAK8JetMuonMass_Scale_muR2_muF1      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Scale_muR2_muF1","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Scale_muRp5_muF1      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Scale_muRp5_muF1","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Scale_muR1_muF2      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Scale_muR1_muF2","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Scale_muR2_muF2      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Scale_muR2_muF2","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Scale_muR1_muFp5      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Scale_muR1_muFp5","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Scale_muRp5_muFp5      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Scale_muRp5_muFp5","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1047      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1047","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1048      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1048","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1049      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1049","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1050      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1050","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1051      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1051","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1052      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1052","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1053      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1053","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1054      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1054","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1055      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1055","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1056      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1056","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1057      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1057","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1058      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1058","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1059      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1059","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1060      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1060","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1061      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1061","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1062      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1062","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1063      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1063","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1064      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1064","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1065      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1065","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1066      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1066","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1067      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1067","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1068      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1068","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1069      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1069","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1070      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1070","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1071      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1071","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1072      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1072","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1073      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1073","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1074      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1074","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1075      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1075","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1076      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1076","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1077      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1077","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1078      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1078","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1079      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1079","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1080      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1080","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1081      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1081","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1082      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1082","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1083      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1083","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1084      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1084","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1085      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1085","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1086      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1086","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1087      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1087","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1088      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1088","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1089      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1089","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1090      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1090","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1091      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1091","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1092      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1092","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1093      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1093","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1094      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1094","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1095      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1095","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1096      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1096","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1097      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1097","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1098      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1098","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1099      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1099","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1100      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1100","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1101      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1101","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1102      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1102","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1103      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1103","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1104      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1104","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1105      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1105","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1106      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1106","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1107      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1107","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1108      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1108","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1109      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1109","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1110      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1110","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1111      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1111","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1112      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1112","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1113      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1113","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1114      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1114","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1115      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1115","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1116      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1116","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1117      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1117","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1118      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1118","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1119      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1119","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1120      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1120","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1121      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1121","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1122      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1122","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1123      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1123","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1124      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1124","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1125      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1125","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1126      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1126","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1127      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1127","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1128      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1128","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1129      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1129","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1130      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1130","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1131      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1131","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1132      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1132","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1133      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1133","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1134      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1134","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1135      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1135","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1136      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1136","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1137      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1137","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1138      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1138","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1139      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1139","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1140      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1140","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1141      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1141","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1142      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1142","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1143      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1143","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1144      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1144","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1145      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1145","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_Error_1146      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_Error_1146","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_alphasUp      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_alphasUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);
    m_leadAK8JetMuonMass_alphasDown      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_alphasDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1700, 0, 8500);//11, binBoundaries);


    m_resolvedRECOmass_Scale_muR2_muF1  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Scale_muR2_muF1","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Scale_muRp5_muF1  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Scale_muRp5_muF1","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Scale_muR1_muF2  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Scale_muR1_muF2","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Scale_muR2_muF2  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Scale_muR2_muF2","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Scale_muR1_muFp5  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Scale_muR1_muFp5","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Scale_muRp5_muFp5  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Scale_muRp5_muFp5","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1047  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1047","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1048  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1048","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1049  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1049","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1050  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1050","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1051  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1051","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1052  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1052","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1053  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1053","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1054  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1054","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1055  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1055","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1056  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1056","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1057  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1057","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1058  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1058","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1059  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1059","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1060  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1060","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1061  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1061","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1062  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1062","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1063  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1063","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1064  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1064","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1065  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1065","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1066  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1066","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1067  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1067","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1068  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1068","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1069  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1069","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1070  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1070","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1071  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1071","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1072  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1072","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1073  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1073","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1074  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1074","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1075  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1075","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1076  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1076","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1077  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1077","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1078  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1078","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1079  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1079","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1080  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1080","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1081  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1081","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1082  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1082","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1083  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1083","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1084  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1084","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1085  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1085","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1086  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1086","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1087  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1087","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1088  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1088","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1089  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1089","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1090  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1090","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1091  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1091","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1092  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1092","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1093  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1093","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1094  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1094","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1095  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1095","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1096  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1096","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1097  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1097","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1098  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1098","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1099  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1099","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1100  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1100","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1101  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1101","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1102  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1102","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1103  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1103","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1104  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1104","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1105  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1105","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1106  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1106","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1107  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1107","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1108  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1108","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1109  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1109","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1110  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1110","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1111  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1111","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1112  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1112","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1113  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1113","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1114  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1114","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1115  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1115","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1116  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1116","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1117  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1117","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1118  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1118","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1119  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1119","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1120  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1120","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1121  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1121","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1122  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1122","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1123  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1123","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1124  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1124","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1125  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1125","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1126  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1126","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1127  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1127","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1128  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1128","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1129  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1129","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1130  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1130","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1131  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1131","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1132  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1132","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1133  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1133","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1134  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1134","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1135  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1135","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1136  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1136","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1137  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1137","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1138  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1138","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1139  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1139","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1140  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1140","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1141  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1141","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1142  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1142","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1143  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1143","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1144  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1144","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1145  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1145","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_Error_1146  =           m_histoFolder.make<TH1D>("resolvedRECOmass_Error_1146","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_alphasUp  =           m_histoFolder.make<TH1D>("resolvedRECOmass_alphasUp","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);
    m_resolvedRECOmass_alphasDown  =           m_histoFolder.make<TH1D>("resolvedRECOmass_alphasDown","2 Object Mass;Mass (GeV);"                 ,1700, 0, 8500);//11, binBoundaries);

 
   }
//    m_evtNum                               =  m_histoFolder.make<TH1D>("evtNum", "Event number; ",  200000000, 0.5, 200000000.5);
//    m_lumiSec				   = m_histoFolder.make<TH1D>("lumiSec", "Lumi Seciton;",   2000000, 0.5, 2000000.5);
//    m_evtNum				= m_histoFolder.make<TVectorT<double>(200);
    
/*    tree_evtNumVSlumiSec = m_histoFolder.make<TTree>("evtNumVSlumiSec","title_evtNumVSlumiSec");

    std::cout << "Creating evtNum branch" << std::endl;
    tree_evtNumVSlumiSec->Branch("evtNum", &evtNum, "evtNum/D");
    std::cout << "Creating lumiSec branch" << std::endl;
    tree_evtNumVSlumiSec->Branch("lumiSec", &lumiSec, "lumiSec/D");
    tree_evtNumVSlumiSec->Branch("BoostSRmass", &BoostSRmass, "BoostSRInvariantMass/D");
    std::cout << "Done with booking" << std::endl;*/

    
  }

}
void eventHistos::fill(eventBits& event, int systematicRegion, bool isSignal) {
 // m_outputTag->SetString("blahblah");
  std::cout << "systematicRegion: " << systematicRegion << std::endl;
  std::cout << "event.m_flavor: " << event.m_flavor << std::endl;
  if(event.m_flavor == 1) fillGen(event);
  else if(event.m_flavor == 2) {
    fillReco(event);
    fillCutProgress(event);
    fillWeight(event, event.m_flavor, isSignal);
  }
  else if(event.m_flavor == 3) {
    fillGen(event);
    fillReco(event);
    fillCutProgress(event);
    fillWeight(event, event.m_flavor, isSignal);
  } else if (event.m_flavor == 4) {
    fillWeight(event, event.m_flavor, isSignal);
  } else if (event.m_flavor == 5){
    fillWeight(event, event.m_flavor, isSignal);
    fillCutProgress(event);
    if (systematicRegion == 1){
      fillCombine_Nominal(event, isSignal);
    }else if(systematicRegion == 2){
      fillCombine_JECUp(event);
    }else if(systematicRegion == 3){
      fillCombine_JECDown(event);
    }else if(systematicRegion == 4){
      fillCombine_JERUp(event);
    }else if(systematicRegion == 5){
      fillCombine_JERDown(event);
    }else if(systematicRegion == 6){
      fillCombine_PUUp(event);
    }else if(systematicRegion == 7){
      fillCombine_PUDown(event);
    }else if(systematicRegion == 8){
      fillCombine_MuBoostIDUp(event);
    }else if(systematicRegion == 9){
      fillCombine_MuBoostIDDown(event);
    }else if(systematicRegion == 10){
      fillCombine_MuLUp(event);
    }else if(systematicRegion == 11){
      fillCombine_MuLDown(event);
    }else if(systematicRegion == 12){
      fillCombine_HEEPUp(event);
    }else if(systematicRegion == 13){
      fillCombine_HEEPDown(event);
    }else if(systematicRegion == 16){
      fillCombine_MuTrigUp(event);
    }else if(systematicRegion == 17){
      fillCombine_MuTrigDown(event);
    }else if(systematicRegion == 18){
      fillCombine_MuResolUp(event);
    }else if(systematicRegion == 19){
      fillCombine_MuResolDown(event);
    }else if(systematicRegion == 20){
      fillCombine_MuResIDUp(event);
    }else if(systematicRegion == 21){
      fillCombine_MuResIDDown(event);
    }else if(systematicRegion == 24){
      fillCombine_AK4JECUp(event);
    }else if(systematicRegion == 25){
      fillCombine_AK4JECDown(event);
    }else if(systematicRegion == 26){
      fillCombine_AK4JERUp(event);
    }else if(systematicRegion == 27){
      fillCombine_AK4JERDown(event);
    }else if(systematicRegion == 28){
      fillCombine_FSBResMuIDUp(event);
    }else if(systematicRegion == 29){
      fillCombine_FSBResMuIDDown(event);
    }else if(systematicRegion == 32){
      fillCombine_ElHLTUp(event);
    }else if(systematicRegion == 33){
      fillCombine_ElHLTDown(event);
    }

  }
  else return;
}

//SPECIFIC 
void eventHistos::fillCutProgress(eventBits& event) {
  double weight = 0.0;
  if(m_FSB == 1){
    weight = event.FSBweight;
  }else if(m_FSB == 2){
    weight = event.FSBweight_noISO;
  }else{
    weight = event.weight;
  }
  std::cout << "Filling Cut Progress" << std::endl;
  int toFill = event.cutProgress;
  int FSBtoFill = event.FSBcutProgress;
  int REStoFill = event.ResCutProgress;
  int RESFSBtoFill = event.ResFSBCutProgress;
  while (toFill > 0) {
    m_cutProgress->Fill(toFill , weight);
    m_cutProgressNoWeight->Fill(toFill);
    toFill--;
  }
  while (FSBtoFill > 0) {
    m_FSBcutProgress->Fill(FSBtoFill , weight);
    m_FSBcutProgressNoWeight->Fill(FSBtoFill);
    FSBtoFill--;
  }
  while (REStoFill > 0){
    m_ResCutProgress->Fill(REStoFill, weight);
    m_ResCutProgressNoWeight->Fill(REStoFill);
    REStoFill--;
  }
  while (RESFSBtoFill > 0){
    m_ResFSBCutProgress->Fill(RESFSBtoFill, event.FSBweight);
    m_ResFSBCutProgressNoWeight->Fill(RESFSBtoFill);
    RESFSBtoFill--;
  }
  std::cout << "Finishing filling cut progress" << std::endl;
}
void eventHistos::fillWeight(eventBits& event, int systematicRegion, bool isSignal) {
  std::cout << "Filling Weights" << std::endl;
  std::cout << "event.count: " << event.count << std::endl;
  std::cout << "m_eventsWeight->Integral(): " << m_eventsWeight->Integral() << std::endl;
  m_eventsWeight->Fill(0.5, event.count);
  if (systematicRegion == 5 && isSignal){
    m_Scale_muR1_muF1->Fill(0.5, event.PDFWeights_Scale.at(0));
    m_Scale_muR2_muF1->Fill(0.5, event.PDFWeights_Scale.at(5));
    m_Scale_muRp5_muF1->Fill(0.5, event.PDFWeights_Scale.at(10));
    m_Scale_muR1_muF2->Fill(0.5, event.PDFWeights_Scale.at(15));
    m_Scale_muR2_muF2->Fill(0.5, event.PDFWeights_Scale.at(20));
    m_Scale_muR1_muFp5->Fill(0.5, event.PDFWeights_Scale.at(30));
    m_Scale_muRp5_muFp5->Fill(0.5, event.PDFWeights_Scale.at(40));
    m_Error_1047->Fill(0.5, event.PDFWeights_Error.at(1));
    m_Error_1048->Fill(0.5, event.PDFWeights_Error.at(2));
    m_Error_1049->Fill(0.5, event.PDFWeights_Error.at(3));
    m_Error_1050->Fill(0.5, event.PDFWeights_Error.at(4));
    m_Error_1051->Fill(0.5, event.PDFWeights_Error.at(5));
    m_Error_1052->Fill(0.5, event.PDFWeights_Error.at(6));
    m_Error_1053->Fill(0.5, event.PDFWeights_Error.at(7));
    m_Error_1054->Fill(0.5, event.PDFWeights_Error.at(8));
    m_Error_1055->Fill(0.5, event.PDFWeights_Error.at(9));
    m_Error_1056->Fill(0.5, event.PDFWeights_Error.at(10));
    m_Error_1057->Fill(0.5, event.PDFWeights_Error.at(11));
    m_Error_1058->Fill(0.5, event.PDFWeights_Error.at(12));
    m_Error_1059->Fill(0.5, event.PDFWeights_Error.at(13));
    m_Error_1060->Fill(0.5, event.PDFWeights_Error.at(14));
    m_Error_1061->Fill(0.5, event.PDFWeights_Error.at(15));
    m_Error_1062->Fill(0.5, event.PDFWeights_Error.at(16));
    m_Error_1063->Fill(0.5, event.PDFWeights_Error.at(17));
    m_Error_1064->Fill(0.5, event.PDFWeights_Error.at(18));
    m_Error_1065->Fill(0.5, event.PDFWeights_Error.at(19));
    m_Error_1066->Fill(0.5, event.PDFWeights_Error.at(20));
    m_Error_1067->Fill(0.5, event.PDFWeights_Error.at(21));
    m_Error_1068->Fill(0.5, event.PDFWeights_Error.at(22));
    m_Error_1069->Fill(0.5, event.PDFWeights_Error.at(23));
    m_Error_1070->Fill(0.5, event.PDFWeights_Error.at(24));
    m_Error_1071->Fill(0.5, event.PDFWeights_Error.at(25));
    m_Error_1072->Fill(0.5, event.PDFWeights_Error.at(26));
    m_Error_1073->Fill(0.5, event.PDFWeights_Error.at(27));
    m_Error_1074->Fill(0.5, event.PDFWeights_Error.at(28));
    m_Error_1075->Fill(0.5, event.PDFWeights_Error.at(29));
    m_Error_1076->Fill(0.5, event.PDFWeights_Error.at(30));
    m_Error_1077->Fill(0.5, event.PDFWeights_Error.at(31));
    m_Error_1078->Fill(0.5, event.PDFWeights_Error.at(32));
    m_Error_1079->Fill(0.5, event.PDFWeights_Error.at(33));
    m_Error_1080->Fill(0.5, event.PDFWeights_Error.at(34));
    m_Error_1081->Fill(0.5, event.PDFWeights_Error.at(35));
    m_Error_1082->Fill(0.5, event.PDFWeights_Error.at(36));
    m_Error_1083->Fill(0.5, event.PDFWeights_Error.at(37));
    m_Error_1084->Fill(0.5, event.PDFWeights_Error.at(38));
    m_Error_1085->Fill(0.5, event.PDFWeights_Error.at(39));
    m_Error_1086->Fill(0.5, event.PDFWeights_Error.at(40));
    m_Error_1087->Fill(0.5, event.PDFWeights_Error.at(41));
    m_Error_1088->Fill(0.5, event.PDFWeights_Error.at(42));
    m_Error_1089->Fill(0.5, event.PDFWeights_Error.at(43));
    m_Error_1090->Fill(0.5, event.PDFWeights_Error.at(44));
    m_Error_1091->Fill(0.5, event.PDFWeights_Error.at(45));
    m_Error_1092->Fill(0.5, event.PDFWeights_Error.at(46));
    m_Error_1093->Fill(0.5, event.PDFWeights_Error.at(47));
    m_Error_1094->Fill(0.5, event.PDFWeights_Error.at(48));
    m_Error_1095->Fill(0.5, event.PDFWeights_Error.at(49));
    m_Error_1096->Fill(0.5, event.PDFWeights_Error.at(50));
    m_Error_1097->Fill(0.5, event.PDFWeights_Error.at(51));
    m_Error_1098->Fill(0.5, event.PDFWeights_Error.at(52));
    m_Error_1099->Fill(0.5, event.PDFWeights_Error.at(53));
    m_Error_1100->Fill(0.5, event.PDFWeights_Error.at(54));
    m_Error_1101->Fill(0.5, event.PDFWeights_Error.at(55));
    m_Error_1102->Fill(0.5, event.PDFWeights_Error.at(56));
    m_Error_1103->Fill(0.5, event.PDFWeights_Error.at(57));
    m_Error_1104->Fill(0.5, event.PDFWeights_Error.at(58));
    m_Error_1105->Fill(0.5, event.PDFWeights_Error.at(59));
    m_Error_1106->Fill(0.5, event.PDFWeights_Error.at(60));
    m_Error_1107->Fill(0.5, event.PDFWeights_Error.at(61));
    m_Error_1108->Fill(0.5, event.PDFWeights_Error.at(62));
    m_Error_1109->Fill(0.5, event.PDFWeights_Error.at(63));
    m_Error_1110->Fill(0.5, event.PDFWeights_Error.at(64));
    m_Error_1111->Fill(0.5, event.PDFWeights_Error.at(65));
    m_Error_1112->Fill(0.5, event.PDFWeights_Error.at(66));
    m_Error_1113->Fill(0.5, event.PDFWeights_Error.at(67));
    m_Error_1114->Fill(0.5, event.PDFWeights_Error.at(68));
    m_Error_1115->Fill(0.5, event.PDFWeights_Error.at(69));
    m_Error_1116->Fill(0.5, event.PDFWeights_Error.at(70));
    m_Error_1117->Fill(0.5, event.PDFWeights_Error.at(71));
    m_Error_1118->Fill(0.5, event.PDFWeights_Error.at(72));
    m_Error_1119->Fill(0.5, event.PDFWeights_Error.at(73));
    m_Error_1120->Fill(0.5, event.PDFWeights_Error.at(74));
    m_Error_1121->Fill(0.5, event.PDFWeights_Error.at(75));
    m_Error_1122->Fill(0.5, event.PDFWeights_Error.at(76));
    m_Error_1123->Fill(0.5, event.PDFWeights_Error.at(77));
    m_Error_1124->Fill(0.5, event.PDFWeights_Error.at(78));
    m_Error_1125->Fill(0.5, event.PDFWeights_Error.at(79));
    m_Error_1126->Fill(0.5, event.PDFWeights_Error.at(80));
    m_Error_1127->Fill(0.5, event.PDFWeights_Error.at(81));
    m_Error_1128->Fill(0.5, event.PDFWeights_Error.at(82));
    m_Error_1129->Fill(0.5, event.PDFWeights_Error.at(83));
    m_Error_1130->Fill(0.5, event.PDFWeights_Error.at(84));
    m_Error_1131->Fill(0.5, event.PDFWeights_Error.at(85));
    m_Error_1132->Fill(0.5, event.PDFWeights_Error.at(86));
    m_Error_1133->Fill(0.5, event.PDFWeights_Error.at(87));
    m_Error_1134->Fill(0.5, event.PDFWeights_Error.at(88));
    m_Error_1135->Fill(0.5, event.PDFWeights_Error.at(89));
    m_Error_1136->Fill(0.5, event.PDFWeights_Error.at(90));
    m_Error_1137->Fill(0.5, event.PDFWeights_Error.at(91));
    m_Error_1138->Fill(0.5, event.PDFWeights_Error.at(92));
    m_Error_1139->Fill(0.5, event.PDFWeights_Error.at(93));
    m_Error_1140->Fill(0.5, event.PDFWeights_Error.at(94));
    m_Error_1141->Fill(0.5, event.PDFWeights_Error.at(95));
    m_Error_1142->Fill(0.5, event.PDFWeights_Error.at(96));
    m_Error_1143->Fill(0.5, event.PDFWeights_Error.at(97));
    m_Error_1144->Fill(0.5, event.PDFWeights_Error.at(98));
    m_Error_1145->Fill(0.5, event.PDFWeights_Error.at(99));
    m_Error_1146->Fill(0.5, event.PDFWeights_Error.at(100));
    m_Error_alphasUp->Fill(0.5, event.PDFWeights_AlphaS.at(0));
    m_Error_alphasDown->Fill(0.5, event.PDFWeights_AlphaS.at(1));


  }
  std::cout << "Done Filling Weight" << std::endl;
}
void eventHistos::fillGen(eventBits& event) {
//  std::cout << "Making GEN plots" << std::endl;
//  if(!(event.passesGenCuts())) {
//    std::cout << "ERROR! THIS EVENT SHOULD HAVE FAILED" <<std::endl;
//    return;
//  }
  double weight = 0.0;
  if(m_FSB == 1)
    weight = event.FSBweight;
  else if(m_FSB == 2)
    weight = event.FSBweight_noISO;
  else
    weight = event.weight;

  std::cout << "Filling Event plots" << std::endl;
  std::cout << "event.neutrinoDecays: " << event.neutrinoDecays << std::endl;
  std::cout << "m_neutrinoDecays->Integral(): " << m_neutrinoDecays->Integral() << std::endl;
  m_neutrinoDecays ->Fill(event.neutrinoDecays , weight) ;
  std::cout << "FILLING NEUTRINO DECAY WITH: " << event.neutrinoDecays << " WEIGHT: " << weight << std::endl;
  m_nLeptons       ->Fill(event.mynLeptons     , weight) ;
  m_nMuons         ->Fill(event.mynMuons       , weight) ;
  m_nTaus          ->Fill(event.mynTaus        , weight) ;
  m_nElectrons     ->Fill(event.mynElectrons   , weight) ;
  m_nLightPartons  ->Fill(event.mynLightPartons, weight) ;
  m_nTops          ->Fill(event.mynTops        , weight) ;
  m_nBs            ->Fill(event.mynBs          , weight) ;
  m_nPartons       ->Fill(event.mynPartons     , weight) ;

  m_NRenergy	   ->Fill(event.NRenergy       , weight) ;

  m_RECOpasses     ->Fill(event.RECOcategory) ;

  m_parton1Et->Fill(event.parton1EtVal, weight);
  m_parton2Et->Fill(event.parton2EtVal, weight);
  m_muon1Et->Fill(event.muon1EtVal, weight);
  m_muon2Et->Fill(event.muon2EtVal, weight);
  m_firstPartonJetEt->Fill(event.firstPartonJetEtVal, weight);
  m_secondPartonJetEt->Fill(event.secondPartonJetEtVal);
  m_firstPartonAK8JetEt->Fill(event.firstPartonAK8JetEtVal, weight);
  m_secondPartonAK8JetEt->Fill(event.secondPartonAK8JetEtVal, weight);

  m_parton1Eta->Fill(event.parton1EtaVal, weight);
  m_parton2Eta->Fill(event.parton2EtaVal, weight);
  m_muon1Eta->Fill(event.muon1EtaVal, weight);
  m_muon2Eta->Fill(event.muon2EtaVal, weight);
  m_firstPartonJetEta->Fill(event.firstPartonJetEtaVal, weight);
  m_secondPartonJetEta->Fill(event.secondPartonJetEtaVal, weight);
  m_firstPartonAK8JetEta->Fill(event.firstPartonAK8JetEtaVal, weight);
  m_secondPartonAK8JetEta->Fill(event.secondPartonAK8JetEtaVal, weight);

  m_parton1Phi->Fill(event.parton1PhiVal, weight);
  m_parton2Phi->Fill(event.parton2PhiVal, weight);
  m_muon1Phi->Fill(event.muon1PhiVal, weight);
  m_muon2Phi->Fill(event.muon2PhiVal, weight);
  m_firstPartonJetPhi->Fill(event.firstPartonJetPhiVal, weight);
  m_secondPartonJetPhi->Fill(event.secondPartonJetPhiVal, weight);
  m_firstPartonAK8JetPhi->Fill(event.firstPartonAK8JetPhiVal, weight);
  m_secondPartonAK8JetPhi->Fill(event.secondPartonAK8JetPhiVal, weight);

  m_dRparton1parton2->Fill(event.dRparton1parton2Val, weight);
  m_dRmuon1muon2->Fill(event.dRmuon1muon2Val, weight);
  m_dRparton1muon2->Fill(event.dRparton1muon2Val, weight);
  m_dRparton1muon1->Fill(event.dRparton1muon1Val, weight);
  m_dRparton2muon2->Fill(event.dRparton2muon2Val, weight);
  m_dRparton2muon1->Fill(event.dRparton2muon1Val, weight);
  m_dRparton1jet->Fill(event.dRparton1jetVal, weight);
  m_dRparton2jet->Fill(event.dRparton2jetVal, weight);
  m_dRparton1AK8jet->Fill(event.dRparton1AK8jetVal, weight);
  m_dRparton2AK8jet->Fill(event.dRparton2AK8jetVal, weight);
  m_minPartonMuon2dR->Fill(event.minPartonMuon2dR, weight);


  m_genLSF                  ->   Fill(event.myGenLSF, weight);

  m_wrShellMass->Fill(event.wrShellMass, weight);

  //Z STUFF
  m_genZmass        -> Fill(event.genZmass, weight);
  m_genZpt          -> Fill(event.genZpt  , weight);

  m_genZptVSreco    -> Fill(event.subleadMuon_selMuonPt, event.genZpt, weight);

  m_firstPartonJetEtHadronic->Fill(event.firstPartonJetEtHadronicVal, weight);
  m_secondPartonJetEtHadronic->Fill(event.secondPartonJetEtHadronicVal, weight);
  m_firstPartonJetEtEM->Fill(event.firstPartonJetEtEMVal, weight);
  m_secondPartonJetEtEM->Fill(event.secondPartonJetEtEMVal, weight);
  m_firstPartonJetEtInvisible->Fill(event.firstPartonJetEtInvisibleVal, weight);
  m_secondPartonJetEtInvisible->Fill(event.secondPartonJetEtInvisibleVal, weight);
  

  m_firstPartonAK8JetEtHadronic->Fill(event.firstPartonAK8JetEtHadronicVal, weight);
  m_secondPartonAK8JetEtHadronic->Fill(event.secondPartonAK8JetEtHadronicVal, weight);
  m_firstPartonAK8JetEtEM->Fill(event.firstPartonAK8JetEtEMVal, weight);
  m_secondPartonAK8JetEtEM->Fill(event.secondPartonAK8JetEtEMVal, weight);
  m_firstPartonAK8JetEtInvisible->Fill(event.firstPartonAK8JetEtInvisibleVal, weight);
  m_secondPartonAK8JetEtInvisible->Fill(event.secondPartonAK8JetEtInvisibleVal, weight);

  m_leadSubleadingPartonsMuonsMass->Fill(event.leadSubleadingPartonsMuonsMassVal, weight);

  m_leadSubleadingPartonsMuonsPt->Fill(event.leadSubleadingPartonsMuonsPtVal, weight);

  //m_leadSubleadingJetsMuonsEta->Fill(event.leadSubleadingJetsMuonsEtaVal, weight);
  //m_leadSubleadingPartonsMuonsEta->Fill(event.leadSubleadingPartonsMuonsEtaVal, weight);
  //m_leadSubleadingAK8JetsMuonsEta->Fill(event.leadSubleadingAK8JetsMuonsEtaVal, weight);
  //m_leadAK8JetMuonEta->Fill(event.leadAK8JetMuonEtaVal, weight);
  m_resJetDR             ->Fill(event.resJetDR             , weight); 
  m_resSubleadMuJet1dR   ->Fill(event.resSubleadMuJet1dR   , weight);
  m_resSubleadMuJet2dR   ->Fill(event.resSubleadMuJet2dR   , weight);
  m_resLeadMuJet1dR      ->Fill(event.resLeadMuJet1dR      , weight); 
  m_resLeadMuJet1dR      ->Fill(event.resLeadMuJet1dR      , weight); 
  m_resolvedRECOmass     ->Fill(event.resolvedRECOmass     , weight); 
  m_resolvedGENmass      ->Fill(event.resolvedGENmass      , weight);
  m_resSubleadMuParton1dR->Fill(event.resSubleadMuParton1dR, weight);
  m_resSubleadMuParton2dR->Fill(event.resSubleadMuParton2dR, weight);
  m_resLeadMuParton1dR   ->Fill(event.resLeadMuParton1dR   , weight);
  m_resLeadMuParton2dR   ->Fill(event.resLeadMuParton2dR   , weight);
 
  m_resMLL               ->Fill(event.resMLL               , weight);
  m_resolvedSubleadMuPt  ->Fill(event.resolvedSubleadMuPt  , weight);

  m_resFSBJetDR          ->Fill(event.resFSBJetDR            , weight) ;
  m_resolvedFSBRECOmass  ->Fill(event.resolvedFSBRECOmass    , weight) ; 
  m_resFSBElecJet1dR     ->Fill(event.resFSBElecJet1dR       , weight) ;
  m_resFSBElecJet2dR     ->Fill(event.resFSBElecJet2dR       , weight) ;
  m_resFSBMuonJet1dR     ->Fill(event.resFSBMuonJet1dR       , weight) ;
  m_resFSBMuonJet2dR     ->Fill(event.resFSBMuonJet2dR       , weight) ;
                                                      
                                                      
  m_resFSBElec_pt        ->Fill(event.resFSBElec_pt          , weight) ;
  m_resFSBElec_phi       ->Fill(event.resFSBElec_phi         , weight) ;
  m_resFSBElec_eta       ->Fill(event.resFSBElec_eta         , weight) ;
                                                     
  m_resFSBMuon_pt        ->Fill(event.resFSBMuon_pt          , weight) ;
  m_resFSBMuon_phi       ->Fill(event.resFSBMuon_phi         , weight) ;
  m_resFSBMuon_eta       ->Fill(event.resFSBMuon_eta         , weight) ;


}
void eventHistos::fillReco(eventBits& event) {
  double weight = 0.0;
  if(m_FSB == 1)
    weight = event.FSBweight;
  else if(m_FSB == 2)
    weight = event.FSBweight_noISO;
  else
    weight = event.weight;

  std::cout << "fillRECO" << std::endl;
  std::cout << "event.leadAK8JetMuonMassVal: " << event.leadAK8JetMuonMassVal << std::endl;
  std::cout << "weight: " << weight << std::endl;
  std::cout << "event.leadAK8JetElectronMassVal: " << event.leadAK8JetElectronMassVal << std::endl;
  m_wrShellMass->Fill(event.wrShellMass, weight);
  m_leadSubleadingJetsMuonsPt->Fill(event.leadSubleadingJetsMuonsPtVal, weight);
  m_leadSubleadingAK8JetsMuonsPt->Fill(event.leadSubleadingAK8JetsMuonsPtVal, weight);
  m_leadAK8JetMuonPt->Fill(event.leadAK8JetMuonPtVal, weight);
  m_leadAK8JetElectronPt->Fill(event.leadAK8JetElectronPtVal, weight);
  m_leadAK8JetElectronPt_noISO->Fill(event.leadAK8JetElectronPtVal_noISO, weight);
  m_leadSubleadingJetsMuonsMass->Fill(event.leadSubleadingJetsMuonsMassVal, weight);
  m_leadSubleadingAK8JetsMuonsMass->Fill(event.leadSubleadingAK8JetsMuonsMassVal, weight);
  m_leadAK8JetMuonMass->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_leadAK8JetDiMuonMass->Fill(event.leadAK8JetDiMuonMassVal, weight);
  m_leadAK8JetElectronMass->Fill(event.leadAK8JetElectronMassVal, weight);
  m_leadAK8JetElectronMass_noISO->Fill(event.leadAK8JetElectronMassVal_noISO, weight);
  m_subleadMuon_selJetdPhi ->   Fill(event.subleadMuon_selJetdPhi ,weight); 
  m_subleadMuon_selMuondPhi-> Fill(event.subleadMuon_selMuondPhi,weight);
  m_subleadMuon_selMuonMass-> Fill(event.subleadMuon_selMuonMass,weight);
  m_subleadMuon_selMuonZMass->Fill(event.subleadMuon_selMuonMass,weight);
  m_subleadMuon_selMuonZMass_Weight1->Fill(event.subleadMuon_selMuonMass,1.0);
  m_subleadMuon_selMuonPt  -> Fill(event.subleadMuon_selMuonPt  ,weight); 

  m_subleadMuon_selElectronPhi-> Fill(event.subleadMuon_selElectronPhi,weight);
  m_subleadMuon_selElectronMass-> Fill(event.subleadMuon_selElectronMass,weight);
  m_subleadMuon_selElectronZMass->Fill(event.subleadMuon_selElectronMass,weight);
  m_subleadMuon_selElectronPt  -> Fill(event.subleadMuon_selElectronPt  ,weight); 

  m_subleadMuonPt           ->   Fill(event.subleadMuonPt           ,weight);
  m_subleadMuonEt           ->   Fill(event.subleadMuonEt           ,weight); 
  m_subleadMuonEta          ->   Fill(event.subleadMuonEta          ,weight); 
  m_subleadMuonPhi          ->   Fill(event.subleadMuonPhi          ,weight); 
  m_dRmuon2                 ->   Fill(event.dRmuon2, weight);
  m_secondMuonWRjetdR       ->   Fill(event.secondMuonWRjetdR, weight);

  m_nSecondElectronCands   -> Fill(event.nSecondElectronCands, weight);
  m_secondElecJetDR        -> Fill(event.secondElecJetDR     , weight);
  m_secondElecPt           -> Fill(event.secondElecPt        , weight);

                                                               
  m_MET                    ->Fill(event.MET                    ,weight); 
  m_MET_selJetdPhi         ->Fill(event.MET_selJetdPhi         ,weight); 
  m_MET_selMuondPhi        ->Fill(event.MET_selMuondPhi        ,weight); 
  m_MET_selElectrondPhi        ->Fill(event.MET_selElectrondPhi        ,weight); 
  m_MET_selJetMass         ->Fill(event.MET_selJetMass         ,weight); 
  m_MET_selMuonMass        ->Fill(event.MET_selMuonMass        ,weight); 
  m_MET_selElectronMass        ->Fill(event.MET_selElectronMass        ,weight); 
  m_MET_selJetPt           ->Fill(event.MET_selJetPt           ,weight); 
  m_MET_selMuonPt          ->Fill(event.MET_selMuonPt          ,weight); 
  m_MET_selElectronPt          ->Fill(event.MET_selElectronPt          ,weight); 

  m_selectedJetTransMET    ->Fill(event.selectedJetTransMET    ,weight);

  m_secondHighPtMuonPt->Fill(event.secondHighPtMuonPt, weight);
  m_secondHighPtMuonPhi->Fill(event.secondHighPtMuonPhi, weight);
  m_secondHighPtMuonEta->Fill(event.secondHighPtMuonEta, weight);
  m_selectedJetSecondHighPtMuonDR->Fill(event.selectedJetSecondHighPtMuonDR, weight);

  m_selectedMuonPt  ->Fill(event.selectedMuonPt  ,weight); 
  m_selectedElectronPt  ->Fill(event.selectedElectronPt  ,weight); 
  m_selectedJetPt   ->Fill(event.selectedJetPt   ,weight);  
  m_selectedMuonPhi ->Fill(event.selectedMuonPhi ,weight);  
  m_selectedElectronPhi ->Fill(event.selectedElectronPhi ,weight);  
  m_selectedJetPhi  ->Fill(event.selectedJetPhi  ,weight);   
  m_selectedMuonEta ->Fill(event.selectedMuonEta ,weight);   
  m_selectedElectronEta ->Fill(event.selectedElectronEta ,weight);   
  m_selectedElectronEta_Weight1->Fill(event.selectedElectronEta, 1.0);
  m_selectedJetEta  ->Fill(event.selectedJetEta  ,weight); 
  m_selectedJetMass->Fill(event.selectedJetMass  , weight);
  m_selectedJetTau21->Fill(event.selectedJetTau21, weight);
  m_selectedJetMaxDRGenDaughters->Fill(event.MaxDR_genDaughter_CandJet, weight);
  m_selectedJetLSF3->Fill(event.selectedJetLSF3, weight);
  m_selectedJetEnergy->Fill(event.selectedJetEnergy, weight);
  m_selectedJetEnergyUncorr->Fill(event.selectedJetEnergyUncorr, weight);
  m_recoLSF_v_selJetPt  ->Fill(event.selectedJetPt,event.selectedJetLSF3, weight);
  m_genLSF_v_recoLSF ->Fill(event.myGenLSF,event.selectedJetLSF3, weight);
  m_genE_v_recoE->Fill(event.NRenergy, event.selectedJetEnergy, weight);
  m_selectedJetMaxSubJetCSV->Fill(event.selectedJetMaxSubJetCSV, weight);

  m_dRlsfLep_subleadMuon -> Fill(event.mydRlsfLep_subleadMuon, weight);
  m_lsfLepDR_v_recoLSF   -> Fill(event.mydRlsfLep_subleadMuon, event.selectedJetLSF3, weight);
  m_lsfLepDR_v_selJetPt  -> Fill(event.mydRlsfLep_subleadMuon, event.selectedJetPt, weight);

  m_secondGENMuon_selMuondR ->Fill(event.secondGENMuon_selMuondR, weight);
  m_subleadMuon_selMuondR   ->Fill(event.subleadMuon_selMuondR, weight);

  m_DrDaughters->Fill(event.dR_Daughters, weight);
  m_nWRDaughters->Fill(event.nDaughters, weight);
  m_capturedBothDaughtersInSingleJet->Fill(event.capturedBothDaughtersInSingleJet, weight);
  m_pickedCorrectJet->Fill(event.pickedCorrectJet, weight);
  m_dPhiLeadMuonJetWithDaughters->Fill(event.dPhi_LeadMuonJetWithDaughters, weight);
  m_selectedIncorrectJetMass->Fill(event.selectedIncorrectJetMass, weight);
  m_JetWithDaughtersMass->Fill(event.JetWithDaughtersMass, weight);
  m_secondGENMuonRECOjetDR->Fill(event.secondGENMuonRECOjetDR, weight);
  m_secondRECOMuonRECOjetDR->Fill(event.secondRECOMuonRECOjetDR, weight);

  m_selectedElectron_noISO_Pt  ->Fill(event.selectedElectron_noISO_Pt  ,weight); 
  m_selectedElectron_noISO_Phi ->Fill(event.selectedElectron_noISO_Phi ,weight);  
  m_selectedElectron_noISO_Eta ->Fill(event.selectedElectron_noISO_Eta ,weight);   

  m_selectedJet_EleNoISO_Pt    ->Fill(event.selectedJet_EleNoISO_Pt   ,weight);  
  m_selectedJet_EleNoISO_Phi   ->Fill(event.selectedJet_EleNoISO_Phi  ,weight);   
  m_selectedJet_EleNoISO_Eta   ->Fill(event.selectedJet_EleNoISO_Eta  ,weight); 
  m_selectedJet_EleNoISO_Mass  ->Fill(event.selectedJet_EleNoISO_Mass  , weight);
  m_selectedJet_EleNoISO_Tau21 ->Fill(event.selectedJet_EleNoISO_Tau21, weight);

  m_EtPlacementMuon2->Fill(event.secondInDecayMuon, weight);
  m_nVertices->Fill(event.nVtx, weight);

  m_vertexDiffTan->Fill(event.myVertexDiffTan,weight);
  m_vertexDiffLon->Fill(event.myVertexDiffLon,weight);
  
  //m_nJets->Fill(event.myGenJets.size(), weight);
  m_nAK8Jets->Fill(event.ak8jetCands, weight);
  m_nMuonCands->Fill(event.muonCands, weight);
  m_nElectronCands50-> Fill(event.electronCands50 , weight);
  m_nElectronCands100->Fill(event.electronCands100, weight);
  m_nElectronCands150->Fill(event.electronCands150, weight);
  m_nElectronCands200->Fill(event.electronCands200, weight);
  m_nElectronCands50_noISO-> Fill(event.electronCands50_noISO , weight);
  m_nElectronCands200_noISO->Fill(event.electronCands200_noISO, weight);
  m_nMuons10->Fill(event.muons10, weight);
  m_nAdditionalHEEP->Fill(event.nAdditionalHEEP, weight);
  m_nAdditionalHEEP_noISO->Fill(event.nAdditionalHEEP_noISO, weight);
  m_nHighPtMuonsOutsideJet->Fill(event.nHighPtMuonsOutsideJet, weight);
  m_nAK8Jets40->Fill(event.ak8jets40, weight);
  m_nCandidateJets->Fill(event.myAddJetCandsHighPt_noLSF.size(), weight);
  m_leadAK8JetMuonDR->Fill(event.leadAK8JetMuonDR,weight);
  m_leadAK8JetMuonPhi->Fill(event.leadAK8JetMuonPhiVal, weight);
  m_leadAK8JetElectronPhi->Fill(event.leadAK8JetElectronPhiVal, weight);
  m_leadAK8JetElectronPhi_noISO->Fill(event.leadAK8JetElectronPhiVal_noISO, weight);


  m_leadAK8JetMuonJetMuonEnergyFraction->Fill(event.leadAK8JetMuonJetMuonEnergyFraction, weight);
  m_leadAK8JetElectronJetMuonEnergyFraction->Fill(event.leadAK8JetElectronJetMuonEnergyFraction, weight);
  m_leadAK8JetElectronJetMuonEnergyFraction_noISO->Fill(event.leadAK8JetElectronJetMuonEnergyFraction_noISO, weight);

  m_electronTrigger->Fill(event.electronTrigger, weight);
  m_muonTrigger->Fill(event.muonTrigger, weight);

  m_MuonWeight->Fill(event.Muon_LooseID_Weight, weight);
  m_PUWeight->Fill(event.puWeight, weight);

  m_HEEP_SF->Fill(event.HEEP_SF, weight);
  m_HEEP_SF_E->Fill(event.HEEP_SF_E, weight);
  m_HEEP_SF_B->Fill(event.HEEP_SF_B, weight);
  m_FSBfinalEventWeight_E->Fill(event.FSBweight_E, weight);
  m_FSBfinalEventWeight_B->Fill(event.FSBweight_B, weight);

  m_HEEP_SF_noISO->Fill(event.HEEP_SF_noISO, weight);
  m_HEEP_SF_E_noISO->Fill(event.HEEP_SF_E_noISO, weight);
  m_HEEP_SF_B_noISO->Fill(event.HEEP_SF_B_noISO, weight);
  m_FSBfinalEventWeight_E_noISO->Fill(event.FSBweight_E_noISO, weight);
  m_FSBfinalEventWeight_B_noISO->Fill(event.FSBweight_B_noISO, weight);

  m_finalEventWeight->Fill(weight, weight);
  m_Electron_Reco_SF->Fill(event.egamma_SF, weight);
  m_Electron_Reco_SF_noISO->Fill(event.egamma_SF_noISO, weight);

  m_selElectron_barrel_dEtaInSeed        ->Fill(event.selElectron_barrel_dEtaInSeed       , weight); 
  m_selElectron_barrel_dPhiIn            ->Fill(event.selElectron_barrel_dPhiIn           , weight); 
  m_selElectron_barrel_HoverE            ->Fill(event.selElectron_barrel_HoverE           , weight); 
  m_selElectron_barrel_sig_ietaieta_5x5  ->Fill(event.selElectron_barrel_sig_ietaieta_5x5 , weight); 
//  m_selElectron_barrel_E2x5vE5x5       ->Fill(event.m_selElectron_barrel_E2x5vE5x5      , weight); 
  m_selElectron_barrel_EM_had_depIso     ->Fill(event.selElectron_barrel_EM_had_depIso    , weight); 
  m_selElectron_barrel_trackIso          ->Fill(event.selElectron_barrel_trackIso         , weight); 
  m_selElectron_barrel_innerLostHits     ->Fill(event.selElectron_barrel_innerLostHits    , weight); 
  m_selElectron_barrel_dxy               ->Fill(event.selElectron_barrel_dxy              , weight); 
  m_selElectron_endcap_dEtaInSeed        ->Fill(event.selElectron_endcap_dEtaInSeed       , weight); 
  m_selElectron_endcap_dPhiIn            ->Fill(event.selElectron_endcap_dPhiIn           , weight); 
  m_selElectron_endcap_HoverE            ->Fill(event.selElectron_endcap_HoverE           , weight); 
  m_selElectron_endcap_sig_ietaieta_5x5  ->Fill(event.selElectron_endcap_sig_ietaieta_5x5 , weight); 
//  m_selElectron_endcap_E2x5vE5x5       ->Fill(event.m_selElectron_endcap_E2x5vE5x5      , weight); 
  m_selElectron_endcap_EM_had_depIso     ->Fill(event.selElectron_endcap_EM_had_depIso    , weight); 
  m_selElectron_endcap_trackIso          ->Fill(event.selElectron_endcap_trackIso         , weight); 
  m_selElectron_endcap_innerLostHits     ->Fill(event.selElectron_endcap_innerLostHits    , weight); 
  m_selElectron_endcap_dxy               ->Fill(event.selElectron_endcap_dxy              , weight); 

  m_selElectron_noISO_barrel_dEtaInSeed        ->Fill(event.selElectron_noISO_barrel_dEtaInSeed       , weight); 
  m_selElectron_noISO_barrel_dPhiIn            ->Fill(event.selElectron_noISO_barrel_dPhiIn           , weight); 
  m_selElectron_noISO_barrel_HoverE            ->Fill(event.selElectron_noISO_barrel_HoverE           , weight); 
  m_selElectron_noISO_barrel_sig_ietaieta_5x5  ->Fill(event.selElectron_noISO_barrel_sig_ietaieta_5x5 , weight); 
  m_selElectron_noISO_barrel_EM_had_depIso     ->Fill(event.selElectron_noISO_barrel_EM_had_depIso    , weight); 
  m_selElectron_noISO_barrel_trackIso          ->Fill(event.selElectron_noISO_barrel_trackIso         , weight); 
  m_selElectron_noISO_barrel_innerLostHits     ->Fill(event.selElectron_noISO_barrel_innerLostHits    , weight); 
  m_selElectron_noISO_barrel_dxy               ->Fill(event.selElectron_noISO_barrel_dxy              , weight); 
  m_selElectron_noISO_endcap_dEtaInSeed        ->Fill(event.selElectron_noISO_endcap_dEtaInSeed       , weight); 
  m_selElectron_noISO_endcap_dPhiIn            ->Fill(event.selElectron_noISO_endcap_dPhiIn           , weight); 
  m_selElectron_noISO_endcap_HoverE            ->Fill(event.selElectron_noISO_endcap_HoverE           , weight); 
  m_selElectron_noISO_endcap_sig_ietaieta_5x5  ->Fill(event.selElectron_noISO_endcap_sig_ietaieta_5x5 , weight); 
  m_selElectron_noISO_endcap_EM_had_depIso     ->Fill(event.selElectron_noISO_endcap_EM_had_depIso    , weight); 
  m_selElectron_noISO_endcap_trackIso          ->Fill(event.selElectron_noISO_endcap_trackIso         , weight); 
  m_selElectron_noISO_endcap_innerLostHits     ->Fill(event.selElectron_noISO_endcap_innerLostHits    , weight); 
  m_selElectron_noISO_endcap_dxy               ->Fill(event.selElectron_noISO_endcap_dxy              , weight); 

}

void eventHistos::fillCombine_Nominal(eventBits& event, bool isSignal) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }
  
  std::cout << "fillCombine_Nominal" << std::endl;
  std::cout << "doFast event.weight : " << event.weight << std::endl;
  std::cout << "doFast event.leadAK8JetMuonMassVal: " << event.leadAK8JetMuonMassVal << std::endl;
  std::cout << "doFast event.resolvedRECOmass: " << event.resolvedRECOmass << std::endl;
  m_leadAK8JetMuonMass->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_leadAK8JetElectronMass->Fill(event.leadAK8JetElectronMassVal, weight);
  m_resolvedRECOmass->Fill(event.resolvedRECOmass, weight);
  m_resolvedFSBRECOmass->Fill(event.resolvedFSBRECOmass, weight);
  m_wrShellMass->Fill(event.wrShellMass, weight);
  if(isSignal){
    m_leadAK8JetMuonMass_Scale_muR2_muF1->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Scale.at(5));
    m_leadAK8JetMuonMass_Scale_muRp5_muF1->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Scale.at(10));
    m_leadAK8JetMuonMass_Scale_muR1_muF2->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Scale.at(15));
    m_leadAK8JetMuonMass_Scale_muR2_muF2->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Scale.at(20));
    m_leadAK8JetMuonMass_Scale_muR1_muFp5->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Scale.at(30));
    m_leadAK8JetMuonMass_Scale_muRp5_muFp5->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Scale.at(40));

    m_leadAK8JetMuonMass_Error_1047->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(1));
    m_leadAK8JetMuonMass_Error_1048->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(2));
    m_leadAK8JetMuonMass_Error_1049->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(3));
    m_leadAK8JetMuonMass_Error_1050->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(4));
    m_leadAK8JetMuonMass_Error_1051->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(5));
    m_leadAK8JetMuonMass_Error_1052->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(6));
    m_leadAK8JetMuonMass_Error_1053->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(7));
    m_leadAK8JetMuonMass_Error_1054->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(8));
    m_leadAK8JetMuonMass_Error_1055->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(9));
    m_leadAK8JetMuonMass_Error_1056->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(10));
    m_leadAK8JetMuonMass_Error_1057->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(11));
    m_leadAK8JetMuonMass_Error_1058->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(12));
    m_leadAK8JetMuonMass_Error_1059->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(13));
    m_leadAK8JetMuonMass_Error_1060->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(14));
    m_leadAK8JetMuonMass_Error_1061->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(15));
    m_leadAK8JetMuonMass_Error_1062->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(16));
    m_leadAK8JetMuonMass_Error_1063->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(17));
    m_leadAK8JetMuonMass_Error_1064->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(18));
    m_leadAK8JetMuonMass_Error_1065->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(19));
    m_leadAK8JetMuonMass_Error_1066->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(20));
    m_leadAK8JetMuonMass_Error_1067->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(21));
    m_leadAK8JetMuonMass_Error_1068->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(22));
    m_leadAK8JetMuonMass_Error_1069->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(23));
    m_leadAK8JetMuonMass_Error_1070->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(24));
    m_leadAK8JetMuonMass_Error_1071->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(25));
    m_leadAK8JetMuonMass_Error_1072->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(26));
    m_leadAK8JetMuonMass_Error_1073->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(27));
    m_leadAK8JetMuonMass_Error_1074->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(28));
    m_leadAK8JetMuonMass_Error_1075->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(29));
    m_leadAK8JetMuonMass_Error_1076->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(30));
    m_leadAK8JetMuonMass_Error_1077->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(31));
    m_leadAK8JetMuonMass_Error_1078->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(32));
    m_leadAK8JetMuonMass_Error_1079->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(33));
    m_leadAK8JetMuonMass_Error_1080->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(34));
    m_leadAK8JetMuonMass_Error_1081->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(35));
    m_leadAK8JetMuonMass_Error_1082->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(36));
    m_leadAK8JetMuonMass_Error_1083->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(37));
    m_leadAK8JetMuonMass_Error_1084->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(38));
    m_leadAK8JetMuonMass_Error_1085->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(39));
    m_leadAK8JetMuonMass_Error_1086->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(40));
    m_leadAK8JetMuonMass_Error_1087->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(41));
    m_leadAK8JetMuonMass_Error_1088->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(42));
    m_leadAK8JetMuonMass_Error_1089->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(43));
    m_leadAK8JetMuonMass_Error_1090->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(44));
    m_leadAK8JetMuonMass_Error_1091->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(45));
    m_leadAK8JetMuonMass_Error_1092->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(46));
    m_leadAK8JetMuonMass_Error_1093->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(47));
    m_leadAK8JetMuonMass_Error_1094->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(48));
    m_leadAK8JetMuonMass_Error_1095->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(49));
    m_leadAK8JetMuonMass_Error_1096->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(50));
    m_leadAK8JetMuonMass_Error_1097->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(51));
    m_leadAK8JetMuonMass_Error_1098->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(52));
    m_leadAK8JetMuonMass_Error_1099->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(53));
    m_leadAK8JetMuonMass_Error_1100->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(54));
    m_leadAK8JetMuonMass_Error_1101->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(55));
    m_leadAK8JetMuonMass_Error_1102->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(56));
    m_leadAK8JetMuonMass_Error_1103->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(57));
    m_leadAK8JetMuonMass_Error_1104->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(58));
    m_leadAK8JetMuonMass_Error_1105->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(59));
    m_leadAK8JetMuonMass_Error_1106->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(60));
    m_leadAK8JetMuonMass_Error_1107->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(61));
    m_leadAK8JetMuonMass_Error_1108->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(62));
    m_leadAK8JetMuonMass_Error_1109->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(63));
    m_leadAK8JetMuonMass_Error_1110->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(64));
    m_leadAK8JetMuonMass_Error_1111->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(65));
    m_leadAK8JetMuonMass_Error_1112->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(66));
    m_leadAK8JetMuonMass_Error_1113->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(67));
    m_leadAK8JetMuonMass_Error_1114->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(68));
    m_leadAK8JetMuonMass_Error_1115->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(69));
    m_leadAK8JetMuonMass_Error_1116->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(70));
    m_leadAK8JetMuonMass_Error_1117->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(71));
    m_leadAK8JetMuonMass_Error_1118->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(72));
    m_leadAK8JetMuonMass_Error_1119->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(73));
    m_leadAK8JetMuonMass_Error_1120->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(74));
    m_leadAK8JetMuonMass_Error_1121->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(75));
    m_leadAK8JetMuonMass_Error_1122->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(76));
    m_leadAK8JetMuonMass_Error_1123->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(77));
    m_leadAK8JetMuonMass_Error_1124->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(78));
    m_leadAK8JetMuonMass_Error_1125->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(79));
    m_leadAK8JetMuonMass_Error_1126->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(80));
    m_leadAK8JetMuonMass_Error_1127->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(81));
    m_leadAK8JetMuonMass_Error_1128->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(82));
    m_leadAK8JetMuonMass_Error_1129->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(83));
    m_leadAK8JetMuonMass_Error_1130->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(84));
    m_leadAK8JetMuonMass_Error_1131->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(85));
    m_leadAK8JetMuonMass_Error_1132->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(86));
    m_leadAK8JetMuonMass_Error_1133->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(87));
    m_leadAK8JetMuonMass_Error_1134->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(88));
    m_leadAK8JetMuonMass_Error_1135->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(89));
    m_leadAK8JetMuonMass_Error_1136->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(90));
    m_leadAK8JetMuonMass_Error_1137->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(91));
    m_leadAK8JetMuonMass_Error_1138->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(92));
    m_leadAK8JetMuonMass_Error_1139->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(93));
    m_leadAK8JetMuonMass_Error_1140->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(94));
    m_leadAK8JetMuonMass_Error_1141->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(95));
    m_leadAK8JetMuonMass_Error_1142->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(96));
    m_leadAK8JetMuonMass_Error_1143->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(97));
    m_leadAK8JetMuonMass_Error_1144->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(98));
    m_leadAK8JetMuonMass_Error_1145->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(99));
    m_leadAK8JetMuonMass_Error_1146->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_Error.at(100));
    m_leadAK8JetMuonMass_alphasUp->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_AlphaS.at(0));
    m_leadAK8JetMuonMass_alphasDown->Fill(event.leadAK8JetMuonMassVal, weight*event.PDFWeights_AlphaS.at(1));


    m_resolvedRECOmass_Scale_muR2_muF1->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Scale.at(5));
    m_resolvedRECOmass_Scale_muRp5_muF1->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Scale.at(10));
    m_resolvedRECOmass_Scale_muR1_muF2->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Scale.at(15));
    m_resolvedRECOmass_Scale_muR2_muF2->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Scale.at(20));
    m_resolvedRECOmass_Scale_muR1_muFp5->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Scale.at(30));
    m_resolvedRECOmass_Scale_muRp5_muFp5->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Scale.at(40));
    m_resolvedRECOmass_Error_1047->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(1));
    m_resolvedRECOmass_Error_1048->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(2));
    m_resolvedRECOmass_Error_1049->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(3));
    m_resolvedRECOmass_Error_1050->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(4));
    m_resolvedRECOmass_Error_1051->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(5));
    m_resolvedRECOmass_Error_1052->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(6));
    m_resolvedRECOmass_Error_1053->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(7));
    m_resolvedRECOmass_Error_1054->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(8));
    m_resolvedRECOmass_Error_1055->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(9));
    m_resolvedRECOmass_Error_1056->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(10));
    m_resolvedRECOmass_Error_1057->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(11));
    m_resolvedRECOmass_Error_1058->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(12));
    m_resolvedRECOmass_Error_1059->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(13));
    m_resolvedRECOmass_Error_1060->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(14));
    m_resolvedRECOmass_Error_1061->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(15));
    m_resolvedRECOmass_Error_1062->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(16));
    m_resolvedRECOmass_Error_1063->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(17));
    m_resolvedRECOmass_Error_1064->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(18));
    m_resolvedRECOmass_Error_1065->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(19));
    m_resolvedRECOmass_Error_1066->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(20));
    m_resolvedRECOmass_Error_1067->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(21));
    m_resolvedRECOmass_Error_1068->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(22));
    m_resolvedRECOmass_Error_1069->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(23));
    m_resolvedRECOmass_Error_1070->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(24));
    m_resolvedRECOmass_Error_1071->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(25));
    m_resolvedRECOmass_Error_1072->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(26));
    m_resolvedRECOmass_Error_1073->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(27));
    m_resolvedRECOmass_Error_1074->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(28));
    m_resolvedRECOmass_Error_1075->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(29));
    m_resolvedRECOmass_Error_1076->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(30));
    m_resolvedRECOmass_Error_1077->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(31));
    m_resolvedRECOmass_Error_1078->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(32));
    m_resolvedRECOmass_Error_1079->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(33));
    m_resolvedRECOmass_Error_1080->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(34));
    m_resolvedRECOmass_Error_1081->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(35));
    m_resolvedRECOmass_Error_1082->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(36));
    m_resolvedRECOmass_Error_1083->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(37));
    m_resolvedRECOmass_Error_1084->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(38));
    m_resolvedRECOmass_Error_1085->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(39));
    m_resolvedRECOmass_Error_1086->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(40));
    m_resolvedRECOmass_Error_1087->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(41));
    m_resolvedRECOmass_Error_1088->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(42));
    m_resolvedRECOmass_Error_1089->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(43));
    m_resolvedRECOmass_Error_1090->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(44));
    m_resolvedRECOmass_Error_1091->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(45));
    m_resolvedRECOmass_Error_1092->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(46));
    m_resolvedRECOmass_Error_1093->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(47));
    m_resolvedRECOmass_Error_1094->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(48));
    m_resolvedRECOmass_Error_1095->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(49));
    m_resolvedRECOmass_Error_1096->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(50));
    m_resolvedRECOmass_Error_1097->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(51));
    m_resolvedRECOmass_Error_1098->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(52));
    m_resolvedRECOmass_Error_1099->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(53));
    m_resolvedRECOmass_Error_1100->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(54));
    m_resolvedRECOmass_Error_1101->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(55));
    m_resolvedRECOmass_Error_1102->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(56));
    m_resolvedRECOmass_Error_1103->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(57));
    m_resolvedRECOmass_Error_1104->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(58));
    m_resolvedRECOmass_Error_1105->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(59));
    m_resolvedRECOmass_Error_1106->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(60));
    m_resolvedRECOmass_Error_1107->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(61));
    m_resolvedRECOmass_Error_1108->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(62));
    m_resolvedRECOmass_Error_1109->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(63));
    m_resolvedRECOmass_Error_1110->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(64));
    m_resolvedRECOmass_Error_1111->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(65));
    m_resolvedRECOmass_Error_1112->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(66));
    m_resolvedRECOmass_Error_1113->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(67));
    m_resolvedRECOmass_Error_1114->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(68));
    m_resolvedRECOmass_Error_1115->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(69));
    m_resolvedRECOmass_Error_1116->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(70));
    m_resolvedRECOmass_Error_1117->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(71));
    m_resolvedRECOmass_Error_1118->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(72));
    m_resolvedRECOmass_Error_1119->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(73));
    m_resolvedRECOmass_Error_1120->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(74));
    m_resolvedRECOmass_Error_1121->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(75));
    m_resolvedRECOmass_Error_1122->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(76));
    m_resolvedRECOmass_Error_1123->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(77));
    m_resolvedRECOmass_Error_1124->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(78));
    m_resolvedRECOmass_Error_1125->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(79));
    m_resolvedRECOmass_Error_1126->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(80));
    m_resolvedRECOmass_Error_1127->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(81));
    m_resolvedRECOmass_Error_1128->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(82));
    m_resolvedRECOmass_Error_1129->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(83));
    m_resolvedRECOmass_Error_1130->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(84));
    m_resolvedRECOmass_Error_1131->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(85));
    m_resolvedRECOmass_Error_1132->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(86));
    m_resolvedRECOmass_Error_1133->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(87));
    m_resolvedRECOmass_Error_1134->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(88));
    m_resolvedRECOmass_Error_1135->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(89));
    m_resolvedRECOmass_Error_1136->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(90));
    m_resolvedRECOmass_Error_1137->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(91));
    m_resolvedRECOmass_Error_1138->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(92));
    m_resolvedRECOmass_Error_1139->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(93));
    m_resolvedRECOmass_Error_1140->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(94));
    m_resolvedRECOmass_Error_1141->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(95));
    m_resolvedRECOmass_Error_1142->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(96));
    m_resolvedRECOmass_Error_1143->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(97));
    m_resolvedRECOmass_Error_1144->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(98));
    m_resolvedRECOmass_Error_1145->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(99));
    m_resolvedRECOmass_Error_1146->Fill(event.resolvedRECOmass, weight*event.PDFWeights_Error.at(100));
    m_resolvedRECOmass_alphasUp->Fill(event.resolvedRECOmass, weight*event.PDFWeights_AlphaS.at(0));
    m_resolvedRECOmass_alphasDown->Fill(event.resolvedRECOmass, weight*event.PDFWeights_AlphaS.at(1));


  }
//  m_evtNum->Fill(event.eventNumber);
//  m_lumiSec->Fill(event.lumiSection);
  evtNum = event.eventNumber;
  lumiSec = event.lumiSection;
  BoostSRmass = event.leadAK8JetMuonMassVal;
//  tree_evtNumVSlumiSec->Fill();
}
void eventHistos::fillCombine_JECUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_JECUp->Fill(event.leadAK8JetMuonMassVal_JECUp, weight);
  m_leadAK8JetMuonMass_noLSF_JECUp->Fill(event.leadAK8JetMuonMassVal_noLSF_JECUp, weight);
  m_leadAK8JetElectronMass_JECUp->Fill(event.leadAK8JetElectronMassVal_JECUp, weight);

}
void eventHistos::fillCombine_JECDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_JECDown->Fill(event.leadAK8JetMuonMassVal_JECDown, weight);
  m_leadAK8JetMuonMass_noLSF_JECDown->Fill(event.leadAK8JetMuonMassVal_noLSF_JECDown, weight);
  m_leadAK8JetElectronMass_JECDown->Fill(event.leadAK8JetElectronMassVal_JECDown, weight);

}
void eventHistos::fillCombine_JERUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_JERUp->Fill(event.leadAK8JetMuonMassVal_JERUp, weight);
  m_leadAK8JetMuonMass_noLSF_JERUp->Fill(event.leadAK8JetMuonMassVal_noLSF_JERUp, weight);
  m_leadAK8JetElectronMass_JERUp->Fill(event.leadAK8JetElectronMassVal_JERUp, weight);

}
void eventHistos::fillCombine_JERDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_JERDown->Fill(event.leadAK8JetMuonMassVal_JERDown, weight);
  m_leadAK8JetMuonMass_noLSF_JERDown->Fill(event.leadAK8JetMuonMassVal_noLSF_JERDown, weight);
  m_leadAK8JetElectronMass_JERDown->Fill(event.leadAK8JetElectronMassVal_JERDown, weight);

}
void eventHistos::fillCombine_PUUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.puWeight_Up/event.puWeight;
    }else{
      weight = event.weight*event.puWeight_Up/event.puWeight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_PUUp->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_PUUp->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_leadAK8JetElectronMass_PUUp->Fill(event.leadAK8JetElectronMassVal, weight);
  m_resolvedRECOmass_PUUp->Fill(event.resolvedRECOmass, weight);
  m_resolvedFSBRECOmass_PUUp->Fill(event.resolvedFSBRECOmass, weight);

}
void eventHistos::fillCombine_PUDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.puWeight_Down/event.puWeight;
    }else{
      weight = event.weight*event.puWeight_Down/event.puWeight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_PUDown->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_PUDown->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_leadAK8JetElectronMass_PUDown->Fill(event.leadAK8JetElectronMassVal, weight);
  m_resolvedRECOmass_PUDown->Fill(event.resolvedRECOmass, weight);
  m_resolvedFSBRECOmass_PUDown->Fill(event.resolvedFSBRECOmass, weight);
}
void eventHistos::fillCombine_MuBoostIDUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightUp*event.Muon_LooseID_WeightUp*event.Muon_LooseTkIso_WeightUp*event.Muon_RECO_WeightUp/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_LooseID_Weight*event.Muon_RECO_Weight);
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightUp*event.Muon_LooseID_WeightUp*event.Muon_LooseTkIso_WeightUp*event.Muon_RECO_WeightUp/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_LooseID_Weight*event.Muon_RECO_Weight);
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuIDUp->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuIDUp->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);

}
void eventHistos::fillCombine_MuBoostIDDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightDown*event.Muon_LooseID_WeightDown*event.Muon_LooseTkIso_WeightDown*event.Muon_RECO_WeightDown/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_LooseID_Weight*event.Muon_RECO_Weight);
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightDown*event.Muon_LooseID_WeightDown*event.Muon_LooseTkIso_WeightDown*event.Muon_RECO_WeightDown/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_LooseID_Weight*event.Muon_RECO_Weight);
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuIDDown->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuIDDown->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);

}
void eventHistos::fillCombine_MuLUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_LooseID_WeightUp/event.Muon_LooseID_Weight;
    }else{
      weight = event.weight*event.Muon_LooseID_WeightUp/event.Muon_LooseID_Weight;
    }
  }else{
    weight = 1.;
  }
  m_leadAK8JetElectronMass_MuLUp->Fill(event.leadAK8JetElectronMassVal, weight);

}
void eventHistos::fillCombine_MuLDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_LooseID_WeightDown/event.Muon_LooseID_Weight;
    }else{
      weight = event.weight*event.Muon_LooseID_WeightDown/event.Muon_LooseID_Weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetElectronMass_MuLDown->Fill(event.leadAK8JetElectronMassVal, weight);

}
void eventHistos::fillCombine_HEEPUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      if(event.HEEP_SF == 0 || event.egamma_SF == 0){
        weight = event.FSBweight*event.HEEP_SF_Up*event.egamma_SF_Up;
      }else{
        weight = event.FSBweight*event.HEEP_SF_Up*event.egamma_SF_Up/(event.HEEP_SF*event.egamma_SF);
      }
    }else{
      weight = event.weight*event.HEEP_SF_Up*event.egamma_SF_Up/(event.HEEP_SF*event.egamma_SF);
    }
  }else{
    weight = 1.;
  }
  m_leadAK8JetElectronMass_HEEPUp->Fill(event.leadAK8JetElectronMassVal, weight);
  m_resolvedFSBRECOmass_HEEPUp->Fill(event.resolvedFSBRECOmass, weight);

}
void eventHistos::fillCombine_ElHLTUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      if(event.egamma_SF_HLT == 0){
        weight = event.FSBweight*event.egamma_SF_HLT_Up;
      }else{
        weight = event.FSBweight*event.egamma_SF_HLT_Up/(event.egamma_SF_HLT);
      }
    }else{
      weight = event.weight*event.egamma_SF_HLT_Up/(event.egamma_SF_HLT);
    }
  }else{
    weight = 1.;
  }
  m_leadAK8JetElectronMass_ElHLTUp->Fill(event.leadAK8JetElectronMassVal, weight);
}
void eventHistos::fillCombine_ElHLTDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      if(event.egamma_SF_HLT == 0){
        weight = event.FSBweight*event.egamma_SF_HLT_Down;
      }else{
        weight = event.FSBweight*event.egamma_SF_HLT_Down/(event.egamma_SF_HLT);
      }
    }else{
      weight = event.weight*event.egamma_SF_HLT_Down/(event.egamma_SF_HLT);
    }
  }else{
    weight = 1.;
  }
  m_leadAK8JetElectronMass_ElHLTDown->Fill(event.leadAK8JetElectronMassVal, weight);
}
void eventHistos::fillCombine_HEEPDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      if(event.HEEP_SF == 0 || event.egamma_SF == 0){
        weight = event.FSBweight*event.HEEP_SF_Down*event.egamma_SF_Down;
      }else{
        weight = event.FSBweight*event.HEEP_SF_Down*event.egamma_SF_Down/(event.HEEP_SF*event.egamma_SF);
      }
    }else{
      weight = event.weight*event.HEEP_SF_Down*event.egamma_SF_Down/(event.HEEP_SF*event.egamma_SF);
    }
  }else{
    weight = 1.;
  }
  m_leadAK8JetElectronMass_HEEPDown->Fill(event.leadAK8JetElectronMassVal, weight);
  m_resolvedFSBRECOmass_HEEPDown->Fill(event.resolvedFSBRECOmass, weight);
}
void eventHistos::fillCombine_MuTrigUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_Trig_WeightUp/event.Muon_Trig_Weight;
    }else{
      weight = event.weight*event.Muon_Trig_WeightUp/event.Muon_Trig_Weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuTrigUp->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuTrigUp->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_resolvedRECOmass_MuTrigUp->Fill(event.resolvedRECOmass, weight);
  m_resolvedFSBRECOmass_MuTrigUp->Fill(event.resolvedFSBRECOmass, weight);

}
void eventHistos::fillCombine_MuTrigDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_Trig_WeightDown/event.Muon_Trig_Weight;
    }else{
      weight = event.weight*event.Muon_Trig_WeightDown/event.Muon_Trig_Weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuTrigDown->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuTrigDown->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_resolvedRECOmass_MuTrigDown->Fill(event.resolvedRECOmass, weight);
  m_resolvedFSBRECOmass_MuTrigDown->Fill(event.resolvedFSBRECOmass, weight);
}
void eventHistos::fillCombine_MuResolUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuResolUp->Fill(event.leadAK8JetMuonMassVal_MuResolUp, weight);
  m_leadAK8JetMuonMass_noLSF_MuResolUp->Fill(event.leadAK8JetMuonMassVal_noLSF_MuResolUp, weight);
  m_resolvedRECOmass_MuResolUp->Fill(event.resolvedRECOmass_MuResolUp, weight);
  m_resolvedFSBRECOmass_MuResolUp->Fill(event.resolvedFSBRECOmass_MuResolUp, weight);  
}
void eventHistos::fillCombine_MuResolDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuResolDown->Fill(event.leadAK8JetMuonMassVal_MuResolDown, weight);
  m_leadAK8JetMuonMass_noLSF_MuResolDown->Fill(event.leadAK8JetMuonMassVal_noLSF_MuResolDown, weight);
  m_resolvedRECOmass_MuResolDown->Fill(event.resolvedRECOmass_MuResolDown, weight);
  m_resolvedFSBRECOmass_MuResolDown->Fill(event.resolvedFSBRECOmass_MuResolDown, weight);
}
void eventHistos::fillCombine_MuResIDUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightUp*event.Muon_HighPtID2nd_WeightUp*event.Muon_LooseTkIso_WeightUp*event.Muon_LooseTkIso2nd_WeightUp*event.Muon_RECO_WeightUp*event.Muon_RECO2nd_WeightUp/(event.Muon_LooseTkIso_Weight*event.Muon_LooseTkIso2nd_Weight*event.Muon_HighPtID_Weight*event.Muon_HighPtID2nd_Weight*event.Muon_RECO_Weight*event.Muon_RECO2nd_Weight);
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightUp*event.Muon_HighPtID2nd_WeightUp*event.Muon_LooseTkIso_WeightUp*event.Muon_LooseTkIso2nd_WeightUp*event.Muon_RECO_WeightUp*event.Muon_RECO2nd_WeightUp/(event.Muon_LooseTkIso_Weight*event.Muon_LooseTkIso2nd_Weight*event.Muon_HighPtID_Weight*event.Muon_HighPtID2nd_Weight*event.Muon_RECO_Weight*event.Muon_RECO2nd_Weight);
    }
  }else{
    weight = 1.;
  }

  m_resolvedRECOmass_MuResIDUp->Fill(event.resolvedRECOmass, weight);

}
void eventHistos::fillCombine_MuResIDDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightDown*event.Muon_HighPtID2nd_WeightDown*event.Muon_LooseTkIso_WeightDown*event.Muon_LooseTkIso2nd_WeightDown*event.Muon_RECO_WeightDown*event.Muon_RECO2nd_WeightDown/(event.Muon_LooseTkIso_Weight*event.Muon_LooseTkIso2nd_Weight*event.Muon_HighPtID_Weight*event.Muon_HighPtID2nd_Weight*event.Muon_RECO_Weight*event.Muon_RECO2nd_Weight);
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightDown*event.Muon_HighPtID2nd_WeightDown*event.Muon_LooseTkIso_WeightDown*event.Muon_LooseTkIso2nd_WeightDown*event.Muon_RECO_WeightDown*event.Muon_RECO2nd_WeightDown/(event.Muon_LooseTkIso_Weight*event.Muon_LooseTkIso2nd_Weight*event.Muon_HighPtID_Weight*event.Muon_HighPtID2nd_Weight*event.Muon_RECO_Weight*event.Muon_RECO2nd_Weight);
    }
  }else{
    weight = 1.;
  }

  m_resolvedRECOmass_MuResIDDown->Fill(event.resolvedRECOmass, weight);

}
void eventHistos::fillCombine_AK4JECUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_resolvedRECOmass_JECUp->Fill(event.resolvedRECOmass_JECUp, weight);
  m_resolvedFSBRECOmass_JECUp->Fill(event.resolvedFSBRECOmass_JECUp, weight);
}
void eventHistos::fillCombine_AK4JECDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_resolvedRECOmass_JECDown->Fill(event.resolvedRECOmass_JECDown, weight);
  m_resolvedFSBRECOmass_JECDown->Fill(event.resolvedFSBRECOmass_JECDown, weight);
}
void eventHistos::fillCombine_AK4JERUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  std::cout << "event.resolvedRECOmass_JERUp: " << event.resolvedRECOmass_JERUp << std::endl;
  std::cout << "weight: " << weight << std::endl;
  m_resolvedRECOmass_JERUp->Fill(event.resolvedRECOmass_JERUp, weight);
  m_resolvedFSBRECOmass_JERUp->Fill(event.resolvedFSBRECOmass_JERUp, weight);
}
void eventHistos::fillCombine_AK4JERDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight;
    }else{
      weight = event.weight;
    }
  }else{
    weight = 1.;
  }

  m_resolvedRECOmass_JERDown->Fill(event.resolvedRECOmass_JERDown, weight);
  m_resolvedFSBRECOmass_JERDown->Fill(event.resolvedFSBRECOmass_JERDown, weight);
}
void eventHistos::fillCombine_FSBResMuIDUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightUp*event.Muon_LooseTkIso_WeightUp*event.Muon_RECO_WeightUp/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_RECO_Weight);
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightUp*event.Muon_LooseTkIso_WeightUp*event.Muon_RECO_WeightUp/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_RECO_Weight);
    }
  }else{
    weight = 1.;
  }

  m_resolvedFSBRECOmass_MuIDUp->Fill(event.resolvedFSBRECOmass, weight);
}
void eventHistos::fillCombine_FSBResMuIDDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightDown*event.Muon_LooseTkIso_WeightDown*event.Muon_RECO_WeightDown/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_RECO_Weight);
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightDown*event.Muon_LooseTkIso_WeightDown*event.Muon_RECO_WeightDown/(event.Muon_LooseTkIso_Weight*event.Muon_HighPtID_Weight*event.Muon_RECO_Weight);
    }
  }else{
    weight = 1.;
  }

  m_resolvedFSBRECOmass_MuIDDown->Fill(event.resolvedFSBRECOmass, weight);
}
