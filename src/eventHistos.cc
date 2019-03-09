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




void eventHistos::book(TFileDirectory histoFolder, uint16_t flavor, std::string tag, int Region) {

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
                                                                                     
    m_eventFlavor    = m_histoFolder.make<TH1D>("eventFlavor"       , "; Event Flavor; # Events with flavor"       ,                                                       10, -.5, 9.5);


    m_cutProgress       = m_histoFolder.make<TH1D>("cutProgress"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_ResCutProgress       = m_histoFolder.make<TH1D>("ResCutProgress"     , "; # Cut Progress; Events passing cut level"     ,                                                       20, -.5, 19.5);
    m_FSBcutProgress    = m_histoFolder.make<TH1D>("FSBcutProgress"  , "; # Cut Progress in flavour sideband; Events passing cut level"     ,                                   10, -.5, 9.5);
    
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
    m_leadAK8JetMuonMass  =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,60, 0.0,6000);
    m_leadAK8JetMuonMass_noLSF  =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,60, 0.0,6000);
    m_leadAK8JetDiMuonMass  =           m_histoFolder.make<TH1D>("leadAK8JetDiMuonMass","3 Object Mass of the leading Jet and DiMuon;Mass (GeV);"                         ,60, 0.0,6000);
    m_leadAK8JetElectronMass  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                         ,60, 0.0,6000);
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
  //RESOLVED ANA   
    m_resJetDR                = m_histoFolder.make<TH1D>("resJetDR"             ,";resolved jets deltaR", 80, 0.0, 8.0);
    m_resolvedRECOmass        = m_histoFolder.make<TH1D>("resolvedRECOmass"     ,";resolved RECO 4-object mass", 60, 0.0, 6000);
    m_resolvedGENmass         = m_histoFolder.make<TH1D>("resolvedGENmass"      ,";resolved GEN 4-object mass" , 60, 0.0, 6000);

    m_resSubleadMuJet1dR      = m_histoFolder.make<TH1D>("resSubleadMuJet1dR"   ,";resolved sublead muon lead jet dR"     , 80, 0.0, 8.0);
    m_resSubleadMuJet2dR      = m_histoFolder.make<TH1D>("resSubleadMuJet2dR"   ,";resolved sublead muon sublead jet dR"  , 80, 0.0, 8.0);
    m_resLeadMuJet1dR         = m_histoFolder.make<TH1D>("resLeadMuJet1dR"      ,";resolved lead muon lead jet dR"        , 80, 0.0, 8.0);
    m_resLeadMuJet2dR         = m_histoFolder.make<TH1D>("resLeadMuJet2dR"      ,";resolved lead muon sublead jet dR"     , 80, 0.0, 8.0);
                                                                               
    m_resSubleadMuParton1dR   = m_histoFolder.make<TH1D>("resSubleadMuParton1dR",";resolved second muon lead parton dR"   , 80, 0.0, 8.0);
    m_resSubleadMuParton2dR   = m_histoFolder.make<TH1D>("resSubleadMuParton2dR",";resolved second muon sublead parton dR", 80, 0.0, 8.0);
    m_resLeadMuParton1dR      = m_histoFolder.make<TH1D>("resLeadMuParton1dR"   ,";resolved first muon lead parton dR"    , 80, 0.0, 8.0);
    m_resLeadMuParton2dR      = m_histoFolder.make<TH1D>("resLeadMuParton2dR"   ,";resolved first muon sublead parton dR" , 80, 0.0, 8.0);

    m_resMLL                  = m_histoFolder.make<TH1D>("resMLL"               ,";resolved lepton-lepton mass"           , 100, 0.0, 2000);
    m_resolvedSubleadMuPt     = m_histoFolder.make<TH1D>("resolvedSubleadMuPt"  ,";resolved Sublead Muon Pt"              , 100, 0.0, 1000);
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

    m_eventsWeight = m_histoFolder.make<TH1D>("eventsWeight","number of events weighted", 1, 0.0, 1);
    m_leadAK8JetMuonMass      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_JECUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JECUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_JECDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JECDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_JERUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JERUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_JERDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_JERDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_PUUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_PUUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_PUDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_PUDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuHPtUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuHPtUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuHPtDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuHPtDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuLUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuLUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_MuLDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_MuLDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF      =           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JECUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JECUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JECDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JECDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JERUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JERUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_JERDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_JERDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_PUUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_PUUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_PUDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_PUDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuHPtUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuHPtUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuHPtDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuHPtDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuLUp=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuLUp","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetMuonMass_noLSF_MuLDown=           m_histoFolder.make<TH1D>("leadAK8JetMuonMass_noLSF_MuLDown","2 Object Mass of the leading Jet and Muon;Mass (GeV);"                         ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_JECUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JECUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_JECDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JECDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_JERUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JERUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_JERDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_JERDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_PUUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_PUUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_PUDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_PUDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_MuLUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_MuLUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_MuLDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_MuLDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_HEEPUp  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_HEEPUp","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
    m_leadAK8JetElectronMass_HEEPDown  =           m_histoFolder.make<TH1D>("leadAK8JetElectronMass_HEEPDown","2 Object Mass of the leading Jet and Electron;Mass (GeV);"                 ,1160, 200, 6000);//11, binBoundaries);
  }
}
void eventHistos::fill(eventBits& event, int systematicRegion) {
 // m_outputTag->SetString("blahblah");
  std::cout << "systematicRegion: " << systematicRegion << std::endl;
  std::cout << "m_flavor: " << m_flavor << std::endl;
  if(m_flavor == 1) fillGen(event);
  else if(m_flavor == 2) {
    fillReco(event);
    fillCutProgress(event);
    fillWeight(event);
  }
  else if(m_flavor == 3) {
    fillGen(event);
    fillReco(event);
    fillCutProgress(event);
    fillWeight(event);
  } else if (m_flavor == 4) {
    fillWeight(event);
  } else if (m_flavor == 5){
    fillWeight(event);
    if (systematicRegion == 1){
      fillCombine_Nominal(event);
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
      fillCombine_MuHPtUp(event);
    }else if(systematicRegion == 9){
      fillCombine_MuHPtDown(event);
    }else if(systematicRegion == 10){
      fillCombine_MuLUp(event);
    }else if(systematicRegion == 11){
      fillCombine_MuLDown(event);
    }else if(systematicRegion == 12){
      fillCombine_HEEPUp(event);
    }else if(systematicRegion == 13){
      fillCombine_HEEPDown(event);
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
  while (toFill > 0) {
    m_cutProgress->Fill(toFill , weight);
    toFill--;
  }
  while (FSBtoFill > 0) {
    m_FSBcutProgress->Fill(FSBtoFill , weight);
    FSBtoFill--;
  }
  while (REStoFill > 0){
    m_ResCutProgress->Fill(REStoFill, weight);
    REStoFill--;
  }
}
void eventHistos::fillWeight(eventBits& event) {
  std::cout << "Filling Weights" << std::endl;
  m_eventsWeight->Fill(0.5, event.count);
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
  std::cout << "event.weight: " << event.weight << std::endl;
  std::cout << "event.leadAK8JetMuonMassVal: " << event.leadAK8JetMuonMassVal << std::endl;
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
  m_recoLSF_v_selJetPt  ->Fill(event.selectedJetPt,event.selectedJetLSF3, weight);
  m_genLSF_v_recoLSF ->Fill(event.myGenLSF,event.selectedJetLSF3, weight);
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

void eventHistos::fillCombine_Nominal(eventBits& event) {
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
  m_leadAK8JetMuonMass->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_leadAK8JetElectronMass->Fill(event.leadAK8JetElectronMassVal, weight);

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

}
void eventHistos::fillCombine_MuHPtUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightUp/event.Muon_HighPtID_Weight;
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightUp/event.Muon_HighPtID_Weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuHPtUp->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuHPtUp->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);

}
void eventHistos::fillCombine_MuHPtDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      weight = event.FSBweight*event.Muon_HighPtID_WeightDown/event.Muon_HighPtID_Weight;
    }else{
      weight = event.weight*event.Muon_HighPtID_WeightDown/event.Muon_HighPtID_Weight;
    }
  }else{
    weight = 1.;
  }

  m_leadAK8JetMuonMass_MuHPtDown->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuHPtDown->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);

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
  m_leadAK8JetMuonMass_MuLUp->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuLUp->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
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

  m_leadAK8JetMuonMass_MuLDown->Fill(event.leadAK8JetMuonMassVal, weight);
  m_leadAK8JetMuonMass_noLSF_MuLDown->Fill(event.leadAK8JetMuonMassVal_noLSF, weight);
  m_leadAK8JetElectronMass_MuLDown->Fill(event.leadAK8JetElectronMassVal, weight);

}
void eventHistos::fillCombine_HEEPUp(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      if(event.HEEP_SF == 0 || event.egamma_SF == 0){
        weight = event.FSBweight*event.HEEP_SF_Up;
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
}
void eventHistos::fillCombine_HEEPDown(eventBits& event) {
  double weight = 0.0;
  if(event.isMC){
    if(m_FSB == true){
      if(event.HEEP_SF == 0 || event.egamma_SF == 0){
        weight = event.FSBweight*event.HEEP_SF_Down;
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
}
