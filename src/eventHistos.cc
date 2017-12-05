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




void eventHistos::book(TFileDirectory histoFolder, uint16_t flavor) {

  //CREATED FOLDER IN HISTO
  m_flavor = flavor;
  m_histoFolder = histoFolder;

  if (m_flavor != 4) {
  //MAKE GEN PLOTS
    std::cout << "HERE WE CONSTRUCT THE PLOTS" << std::endl;
    m_eventsWeight = m_histoFolder.make<TH1D>("eventsWeight","number of events weighted", 1, 0.0, 1);



    m_nLeptons       = m_histoFolder.make<TH1D>("nLeptons"     , "# Leptons"     ,                                                       10, -.5, 9.5);;
    m_nMuons         = m_histoFolder.make<TH1D>("nMuons"       , "# Muons"       ,                                                       10, -.5, 9.5);;
    m_nTaus          = m_histoFolder.make<TH1D>("nTaus"        , "# Taus"        ,                                                       10, -.5, 9.5);;
    m_nElectrons     = m_histoFolder.make<TH1D>("nElectrons"   , "# Electrons"   ,                                                       10, -.5, 9.5);;
    m_nLightPartons  = m_histoFolder.make<TH1D>("nLightPartons", "# LightPartons",                                                       10, -.5, 9.5);;
    m_nTops          = m_histoFolder.make<TH1D>("nTops"        , "# Tops"        ,                                                       10, -.5, 9.5);;
    m_nBs            = m_histoFolder.make<TH1D>("nBs"          , "# Bs"          ,                                                       10, -.5, 9.5);;
    m_nPartons       = m_histoFolder.make<TH1D>("nPartons"     , "# Partons"     ,                                                       10, -.5, 9.5);;
                                                                                     
    m_eventFlavor    = m_histoFolder.make<TH1D>("eventFlavor"       , "Event Flavor"       ,                                                       10, -.5, 9.5);


    m_cutProgress       = m_histoFolder.make<TH1D>("cutProgress"     , "# Cuts Passed"     ,                                                       10, -.5, 9.5);;
    
    m_parton1Et =                       m_histoFolder.make<TH1D>("parton1Et", "Parton 1 Et;Et (GeV); ",                         80, 0.0, 4000);
    m_parton2Et =                       m_histoFolder.make<TH1D>("parton2Et", "Parton 2 Et;Et (GeV); ",                         80, 0.0, 4000);
    m_muon1Et =                         m_histoFolder.make<TH1D>("muon1Et", "Muon 1 Et;Et (GeV); ",                             80, 0.0, 4000);
    m_muon2Et =                         m_histoFolder.make<TH1D>("muon2Et", "Muon 2 Et;Et (GeV); ",                             80, 0.0, 4000);

    m_parton1Eta =                      m_histoFolder.make<TH1D>("parton1Eta", "Parton 1 Eta;Eta",                        80, -4.0, 4.0);
    m_parton2Eta =                      m_histoFolder.make<TH1D>("parton2Eta", "Parton 2 Eta;Eta",                        80, -4.0, 4.0);
    m_muon1Eta =                        m_histoFolder.make<TH1D>("muon1Eta", "Muon 1 eta;Eta",                            80, -4.0, 4.0);
    m_muon2Eta =                        m_histoFolder.make<TH1D>("muon2Eta", "Muon 2 eta;Eta",                            80, -4.0, 4.0);
    m_firstPartonJetEta     =           m_histoFolder.make<TH1D>("firstPartonJetEta", "Jet 1 Eta;Eta",                    80, -4.0, 4.0);
    m_secondPartonJetEta    =           m_histoFolder.make<TH1D>("secondPartonJetEta", "Jet 2 Eta;Eta",                   80, -4.0, 4.0);
    m_firstPartonAK8JetEta  =           m_histoFolder.make<TH1D>("firstPartonAK8JetEta", "AK8Jet 1 Eta;Eta",              80, -4.0, 4.0); 
    m_secondPartonAK8JetEta =           m_histoFolder.make<TH1D>("secondPartonAK8JetEta", "AK8Jet 2 Eta;Eta",             80, -4.0, 4.0);

    m_parton1Phi             =          m_histoFolder.make<TH1D>("parton1Phi", "Parton 1 Phi;Phi (rad)",                        80, -4.0, 4.0);        
    m_parton2Phi             =          m_histoFolder.make<TH1D>("parton2Phi", "Parton 2 Phi;Phi (rad)",                        80, -4.0, 4.0);       
    m_muon1Phi               =          m_histoFolder.make<TH1D>("muon1Phi", "Muon 1 Phi;Phi (rad)",                            80, -4.0, 4.0);        
    m_muon2Phi               =          m_histoFolder.make<TH1D>("muon2Phi", "Muon 2 Phi;Phi (rad)",                            80, -4.0, 4.0);        
    m_firstPartonJetPhi      =          m_histoFolder.make<TH1D>("firstPartonJetPhi", "Jet 1 Phi;Phi (rad)",                    80, -4.0, 4.0);     
    m_secondPartonJetPhi     =          m_histoFolder.make<TH1D>("secondPartonJetPhi", "Jet 2 Phi;Phi (rad)",                   80, -4.0, 4.0);       
    m_firstPartonAK8JetPhi   =          m_histoFolder.make<TH1D>("firstPartonAK8JetPhi", "AK8Jet 1 Phi;Phi (rad)",              80, -4.0, 4.0);        
    m_secondPartonAK8JetPhi  =          m_histoFolder.make<TH1D>("secondPartonAK8JetPhi", "AK8Jet 1 Phi;Phi (rad)",             80, -4.0, 4.0);           

    m_dRparton1parton2 =                m_histoFolder.make<TH1D>("dRparton1parton2", "deltaR between partons;delta R",                       120, 0.0, 12.0);
    m_dRmuon1muon2 =                    m_histoFolder.make<TH1D>("dRmuon1muon2", "deltaR between muons;delta R",                             120, 0.0, 12.0);
    m_dRparton1muon2 =                  m_histoFolder.make<TH1D>("dRparton1muon2", "deltaR between parton1 and muon2;delta R",               120, 0.0, 12.0);
    m_dRparton1muon1 =                  m_histoFolder.make<TH1D>("dRparton1muon1", "deltaR between parton1 and muon1;delta R",               120, 0.0, 12.0);
    m_dRparton2muon2 =                  m_histoFolder.make<TH1D>("dRparton2muon2", "deltaR between parton2 and muon2;delta R",               120, 0.0, 12.0);
    m_dRparton2muon1 =                  m_histoFolder.make<TH1D>("dRparton2muon1", "deltaR between parton2 and muon1;delta R",               120, 0.0, 12.0);
    m_dRparton1jet =                    m_histoFolder.make<TH1D>("dRparton1jet", "deltaR between parton1 and closest jet;delta R",            40, 0.0,  0.4);
    m_dRparton2jet =                    m_histoFolder.make<TH1D>("dRparton2jet", "deltaR between parton2 and closest jet;delta R",            40, 0.0,  0.4);
    m_dRparton1AK8jet =                 m_histoFolder.make<TH1D>("dRparton1AK8jet", "deltaR between parton1 and closest AK8 jet;delta R",     40, 0.0,  0.8);
    m_dRparton2AK8jet =                 m_histoFolder.make<TH1D>("dRparton2AK8jet", "deltaR between parton2 and closest AK8 jet;delta R",     40, 0.0,  0.8);

    m_firstPartonJetEt =                m_histoFolder.make<TH1D>("firstPartonJetEt", " Jet Et for Leading Parton;Et (GeV)",                      80,0.0,4000);
    m_secondPartonJetEt =               m_histoFolder.make<TH1D>("secondPartonJetEt"," Jet Et for Subleading Parton;Et (GeV)",                   80,0.0,4000);
    m_firstPartonJetEtHadronic =        m_histoFolder.make<TH1D>("firstPartonJetEtHadronic", "Hadronic Jet Et for Leading Parton;Et (GeV)",      80,0.0,4000);
    m_secondPartonJetEtHadronic =       m_histoFolder.make<TH1D>("secondPartonJetEtHadronic","Hadronic Jet Et for Subleading Parton;Et (GeV)",   80,0.0,4000);
    m_firstPartonJetEtEM =              m_histoFolder.make<TH1D>("firstPartonJetEtEM", "EM Jet Et for Leading Parton;Et (GeV)",                  80,0.0,4000);
    m_secondPartonJetEtEM =             m_histoFolder.make<TH1D>("secondPartonJetEtEM","EM Jet Et for Subleading Parton;Et (GeV)",               80,0.0,4000);
    m_firstPartonJetEtInvisible =       m_histoFolder.make<TH1D>("firstPartonJetEtInvisible", "Invisible Jet Et for Leading Parton;Et (GeV)",    80,0.0,4000);
    m_secondPartonJetEtInvisible =      m_histoFolder.make<TH1D>("secondPartonJetEtInvisible","Invisible Jet Et for Subleading Parton;Et (GeV)", 80,0.0,4000);

    m_firstPartonAK8JetEt =             m_histoFolder.make<TH1D>("firstPartonAK8JetEt", " AK8Jet Et for Leading Parton;Et (GeV)",                      80,0.0,4000);
    m_secondPartonAK8JetEt =            m_histoFolder.make<TH1D>("secondPartonAK8JetEt"," AK8Jet Et for Subleading Parton;Et (GeV)",                   80,0.0,4000);
    m_firstPartonAK8JetEtHadronic =     m_histoFolder.make<TH1D>("firstPartonAK8JetEtHadronic", "Hadronic AK8Jet Et for Leading Parton;Et (GeV)",      80,0.0,4000);
    m_secondPartonAK8JetEtHadronic =    m_histoFolder.make<TH1D>("secondPartonAK8JetEtHadronic","Hadronic AK8Jet Et for Subleading Parton;Et (GeV)",   80,0.0,4000);
    m_firstPartonAK8JetEtEM =           m_histoFolder.make<TH1D>("firstPartonAK8JetEtEM", "EM AK8Jet Et for Leading Parton;Et (GeV)",                  80,0.0,4000);
    m_secondPartonAK8JetEtEM =          m_histoFolder.make<TH1D>("secondPartonAK8JetEtEM","EM AK8Jet Et for Subleading Parton;Et (GeV)",               80,0.0,4000);
    m_firstPartonAK8JetEtInvisible =    m_histoFolder.make<TH1D>("firstPartonAK8JetEtInvisible", "Invisible AK8Jet Et for Leading Parton;Et (GeV)",    80,0.0,4000);
    m_secondPartonAK8JetEtInvisible =   m_histoFolder.make<TH1D>("secondPartonAK8JetEtInvisible","Invisible AK8Jet Et for Subleading Parton;Et (GeV)", 80,0.0,4000);


    m_leadSubleadingJetsMuonsMass  =    m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsMass","Four Object Mass of the 2 leading Jets and Muons;Mass (GeV)",      80, 0.0,4000);
    m_leadSubleadingAK8JetsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsMass","Four Object Mass of the 2 leading AK8Jets and Muons;Mass (GeV)",80, 0.0,4000);
    m_leadSubleadingPartonsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsMass","Four Object Mass of the 2 leading Partons and Muons;Mass (GeV)",80, 0.0,4000);
    m_leadAK8JetMuonMass  =             m_histoFolder.make<TH1D>("leadAK8JetMuonMass","2 Object Mass of the leading Jet and Muon;Mass (GeV)"                         ,80, 0.0,4000);

    m_leadSubleadingJetsMuonsPt  =      m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsPt","Four Object Pt of the 2 leading Jets and Muons",          80, 0.0,4000);
    m_leadSubleadingAK8JetsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsPt","Four Object Pt of the 2 leading AK8Jets and Muons",    80, 0.0,4000);
    m_leadSubleadingPartonsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsPt","Four Object Pt of the 2 leading Partons and Muons",    80, 0.0,4000);
    m_leadAK8JetMuonPt  =               m_histoFolder.make<TH1D>("leadAK8JetMuonPt","2 Object Pt of the leading Jet and Muon"                             ,80, 0.0,4000);

    //m_leadSubleadingJetsMuonsEta  =     m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsEta","Four Object Eta of the 2 leading Jets and Muons",        80, -4.0,4.0);
    //m_leadSubleadingAK8JetsMuonsEta  =  m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsEta","Four Object Eta of the 2 leading AK8Jets and Muons",  80, -4.0,4.0);
    //m_leadSubleadingPartonsMuonsEta  =  m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsEta","Four Object Eta of the 2 leading Partons and Muons",  80, -4.0,4.0);
    //m_leadAK8JetMuonEta  =              m_histoFolder.make<TH1D>("leadAK8JetMuonEta","2 Object Eta of the leading Jet and Muon"                           ,80, -4.0,4.0);

    m_leadAK8JetMuonPhi  =              m_histoFolder.make<TH1D>("leadAK8JetMuonAbsdphi","2 Object delta Phi of the leading Jet and Muon"                     ,60, 2.0,3.2);

    m_leadAK8JetMuonJetMuonEnergyFraction =   m_histoFolder.make<TH1D>("leadAK8JetMuonJetMuonEnergyFraction", "muon energy fraction of ak8jet in the selected jet muon pair", 100, 0.0, 1.0);

    m_EtPlacementMuon2 =                m_histoFolder.make<TH1D>("EtPlacementMuon2", "Et ranking idx of muon 2",                                 5, -.5, 4.5);
    //m_nJets =                           m_histoFolder.make<TH1D>("nJets", "# of matched jets",                                                   5, -.5, 4.5);
    m_nAK8Jets =                        m_histoFolder.make<TH1D>("nAK8Jets", "#  AK8Jets",                                                       10, -.5, 9.5);
    m_nMuonCands =                      m_histoFolder.make<TH1D>("nMuonCands", "#  Muons",                                                       10, -.5, 9.5);
    m_nMuons40 =                        m_histoFolder.make<TH1D>("nMuons40", "#  Muons above 40",                                                       10, -.5, 9.5);
    m_nAK8Jets40 =                      m_histoFolder.make<TH1D>("nAK8Jets40", "#  AK8Jets above 40",                                                       10, -.5, 9.5);

    m_selectedMuonEt  =                 m_histoFolder.make<TH1D>("selectedMuonEt"  ,"Selected Muon Et"  ,80,0.0,4000 ); 
    m_selectedJetEt   =                 m_histoFolder.make<TH1D>("selectedJetEt"   ,"Selected Jet Et"   ,80,0.0,4000 );  
    m_selectedMuonPhi =                 m_histoFolder.make<TH1D>("selectedMuonPhi" ,"Selected Muon Phi" ,80,-4.0,4.0 );  
    m_selectedJetPhi  =                 m_histoFolder.make<TH1D>("selectedJetPhi"  ,"Selected Jet Phi"  ,80,-4.0,4.0 );   
    m_selectedMuonEta =                 m_histoFolder.make<TH1D>("selectedMuonEta" ,"Selected Muon Eta" ,60,-3.0,3.0 );   
    m_selectedJetEta  =                 m_histoFolder.make<TH1D>("selectedJetEta"  ,"Selected Jet Eta"  ,60,-3.0,3.0 ); 

    m_subleadMuon_selJetdPhi       =    m_histoFolder.make<TH1D>("subleadMuonSelJetdPhi" ,"Sublead Muon Selected Jet dPhi"  ,80,  0.0, 4.0);
    m_subleadMuon_selMuondPhi      =    m_histoFolder.make<TH1D>("subleadMuonSelMuondPhi","Sublead Muon Selected Muon dPhi" ,80,  0.0, 4.0);
    m_subleadMuon_selMuonMass      =    m_histoFolder.make<TH1D>("subleadMuonSelMuonMass","Sublead Muon Selected Muon Mass" ,100, 0.0,2000);
    m_subleadMuon_selMuonPt        =    m_histoFolder.make<TH1D>("subleadMuonSelMuonPt"  ,"Sublead Muon Selected Muon Pt"   ,100, 0.0,2000);
    m_subleadMuonEt                =    m_histoFolder.make<TH1D>("subleadMuonEt"          ,"Sublead Muon Et"                 ,100, 0.0,2000);

    m_MET                          =    m_histoFolder.make<TH1D>("MET"                 ,"MET;MET (GeV);# events",                       100, 0.0,2000); //THIS LINE WORKS
    m_MET_selJetdPhi               =    m_histoFolder.make<TH1D>("METselJetdPhi"      ,"MET Selected Jet dPhi",      80, 0.0 ,4.0);
    m_MET_selMuondPhi              =    m_histoFolder.make<TH1D>("METselMuondPhi"     ,"MET Selected Muon dPhi",     80, 0.0 ,4.0);
    m_MET_selJetMass               =    m_histoFolder.make<TH1D>("METselJetMass"      ,"MET Selected Jet Mass",     100,0.0 ,2000);
    m_MET_selMuonMass              =    m_histoFolder.make<TH1D>("METselMuonMass"     ,"MET Selected Muon Mass",    100,0.0 ,2000);
    m_MET_selJetPt                 =    m_histoFolder.make<TH1D>("METselJetPt"        ,"MET Selected Jet Pt",       100,0.0 ,2000);
    m_MET_selMuonPt                =    m_histoFolder.make<TH1D>("METselMuonPt"       ,"MET Selected Muon Pt",      100,0.0 ,2000);

    m_selectedJetTransMET          =    m_histoFolder.make<TH1D>("selectedJetTransMET","MET Transverse to Select Jet", 100, 0.0, 500);
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

  }
}
void eventHistos::fill(eventBits& event) {
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
  }
  else return;
}

//SPECIFIC 
void eventHistos::fillCutProgress(eventBits& event) {
  std::cout << "Filling Cut Progress" << std::endl;
  m_cutProgress->Fill(event.cutProgress , event.weight);
}
void eventHistos::fillWeight(eventBits& event) {
  std::cout << "Filling Weights" << std::endl;
  m_eventsWeight->Fill(0.5, event.weight);
}
void eventHistos::fillGen(eventBits& event) {
//  std::cout << "Making GEN plots" << std::endl;
//  if(!(event.passesGenCuts())) {
//    std::cout << "ERROR! THIS EVENT SHOULD HAVE FAILED" <<std::endl;
//    return;
//  }

  std::cout << "Filling Event plots" << std::endl;
  m_nLeptons       ->Fill(event.mynLeptons     , event.weight) ;
  m_nMuons         ->Fill(event.mynMuons       , event.weight) ;
  m_nTaus          ->Fill(event.mynTaus        , event.weight) ;
  m_nElectrons     ->Fill(event.mynElectrons   , event.weight) ;
  m_nLightPartons  ->Fill(event.mynLightPartons, event.weight) ;
  m_nTops          ->Fill(event.mynTops        , event.weight) ;
  m_nBs            ->Fill(event.mynBs          , event.weight) ;
  m_nPartons       ->Fill(event.mynPartons     , event.weight) ;

  m_parton1Et->Fill(event.parton1EtVal, event.weight);
  std::cout << "FILLING 1.2"<<std::endl;
  m_parton2Et->Fill(event.parton2EtVal, event.weight);
  std::cout << "FILLING 1.3"<<std::endl;
  m_muon1Et->Fill(event.muon1EtVal, event.weight);
  std::cout << "FILLING 1.4"<<std::endl;
  m_muon2Et->Fill(event.muon2EtVal, event.weight);
  std::cout << "FILLING 1.5"<<std::endl;
  m_firstPartonJetEt->Fill(event.firstPartonJetEtVal, event.weight);
  std::cout << "FILLING 1.6"<<std::endl;
  m_secondPartonJetEt->Fill(event.secondPartonJetEtVal, event.weight);
  std::cout << "FILLING 1.7"<<std::endl;
  m_firstPartonAK8JetEt->Fill(event.firstPartonAK8JetEtVal, event.weight);
  std::cout << "FILLING 1.8"<<std::endl;
  m_secondPartonAK8JetEt->Fill(event.secondPartonAK8JetEtVal, event.weight);
  std::cout << "FILLING 1.9"<<std::endl;

  m_parton1Eta->Fill(event.parton1EtaVal, event.weight);
  m_parton2Eta->Fill(event.parton2EtaVal, event.weight);
  m_muon1Eta->Fill(event.muon1EtaVal, event.weight);
  m_muon2Eta->Fill(event.muon2EtaVal, event.weight);
  m_firstPartonJetEta->Fill(event.firstPartonJetEtaVal, event.weight);
  m_secondPartonJetEta->Fill(event.secondPartonJetEtaVal, event.weight);
  m_firstPartonAK8JetEta->Fill(event.firstPartonAK8JetEtaVal, event.weight);
  m_secondPartonAK8JetEta->Fill(event.secondPartonAK8JetEtaVal, event.weight);
  std::cout << "FILLING 2"<<std::endl;

  m_parton1Phi->Fill(event.parton1PhiVal, event.weight);
  m_parton2Phi->Fill(event.parton2PhiVal, event.weight);
  m_muon1Phi->Fill(event.muon1PhiVal, event.weight);
  m_muon2Phi->Fill(event.muon2PhiVal, event.weight);
  m_firstPartonJetPhi->Fill(event.firstPartonJetPhiVal, event.weight);
  m_secondPartonJetPhi->Fill(event.secondPartonJetPhiVal, event.weight);
  m_firstPartonAK8JetPhi->Fill(event.firstPartonAK8JetPhiVal, event.weight);
  m_secondPartonAK8JetPhi->Fill(event.secondPartonAK8JetPhiVal, event.weight);
  std::cout << "FILLING 3"<<std::endl;

  m_dRparton1parton2->Fill(event.dRparton1parton2Val, event.weight);
  m_dRmuon1muon2->Fill(event.dRmuon1muon2Val, event.weight);
  m_dRparton1muon2->Fill(event.dRparton1muon2Val, event.weight);
  m_dRparton1muon1->Fill(event.dRparton1muon1Val, event.weight);
  m_dRparton2muon2->Fill(event.dRparton2muon2Val, event.weight);
  m_dRparton2muon1->Fill(event.dRparton2muon1Val, event.weight);
  m_dRparton1jet->Fill(event.dRparton1jetVal, event.weight);
  m_dRparton2jet->Fill(event.dRparton2jetVal, event.weight);
  m_dRparton1AK8jet->Fill(event.dRparton1AK8jetVal, event.weight);
  m_dRparton2AK8jet->Fill(event.dRparton2AK8jetVal, event.weight);
  std::cout << "FILLING 4"<<std::endl;



  m_firstPartonJetEtHadronic->Fill(event.firstPartonJetEtHadronicVal, event.weight);
  m_secondPartonJetEtHadronic->Fill(event.secondPartonJetEtHadronicVal, event.weight);
  m_firstPartonJetEtEM->Fill(event.firstPartonJetEtEMVal, event.weight);
  m_secondPartonJetEtEM->Fill(event.secondPartonJetEtEMVal, event.weight);
  m_firstPartonJetEtInvisible->Fill(event.firstPartonJetEtInvisibleVal, event.weight);
  m_secondPartonJetEtInvisible->Fill(event.secondPartonJetEtInvisibleVal, event.weight);
  std::cout << "FILLING 5"<<std::endl;
  

  m_firstPartonAK8JetEtHadronic->Fill(event.firstPartonAK8JetEtHadronicVal, event.weight);
  m_secondPartonAK8JetEtHadronic->Fill(event.secondPartonAK8JetEtHadronicVal, event.weight);
  m_firstPartonAK8JetEtEM->Fill(event.firstPartonAK8JetEtEMVal, event.weight);
  m_secondPartonAK8JetEtEM->Fill(event.secondPartonAK8JetEtEMVal, event.weight);
  m_firstPartonAK8JetEtInvisible->Fill(event.firstPartonAK8JetEtInvisibleVal, event.weight);
  m_secondPartonAK8JetEtInvisible->Fill(event.secondPartonAK8JetEtInvisibleVal, event.weight);
  std::cout << "FILLING 6"<<std::endl;

  m_leadSubleadingPartonsMuonsMass->Fill(event.leadSubleadingPartonsMuonsMassVal, event.weight);
  std::cout << "FILLING 7"<<std::endl;

  m_leadSubleadingPartonsMuonsPt->Fill(event.leadSubleadingPartonsMuonsPtVal, event.weight);
  std::cout << "FILLING 8"<<std::endl;

  //m_leadSubleadingJetsMuonsEta->Fill(event.leadSubleadingJetsMuonsEtaVal, event.weight);
  //m_leadSubleadingPartonsMuonsEta->Fill(event.leadSubleadingPartonsMuonsEtaVal, event.weight);
  //m_leadSubleadingAK8JetsMuonsEta->Fill(event.leadSubleadingAK8JetsMuonsEtaVal, event.weight);
  //m_leadAK8JetMuonEta->Fill(event.leadAK8JetMuonEtaVal, event.weight);

  std::cout << "FILLING 9"<<std::endl;











}
void eventHistos::fillReco(eventBits& event) {
  m_leadSubleadingJetsMuonsPt->Fill(event.leadSubleadingJetsMuonsPtVal, event.weight);
  m_leadSubleadingAK8JetsMuonsPt->Fill(event.leadSubleadingAK8JetsMuonsPtVal, event.weight);
  m_leadAK8JetMuonPt->Fill(event.leadAK8JetMuonPtVal, event.weight);
  m_leadSubleadingJetsMuonsMass->Fill(event.leadSubleadingJetsMuonsMassVal, event.weight);
  m_leadSubleadingAK8JetsMuonsMass->Fill(event.leadSubleadingAK8JetsMuonsMassVal, event.weight);
  m_leadAK8JetMuonMass->Fill(event.leadAK8JetMuonMassVal, event.weight);
  m_subleadMuon_selJetdPhi ->Fill(event.subleadMuon_selJetdPhi ,event.weight); 
  m_subleadMuon_selMuondPhi->Fill(event.subleadMuon_selMuondPhi,event.weight);
  m_subleadMuon_selMuonMass->Fill(event.subleadMuon_selMuonMass,event.weight);
  m_subleadMuon_selMuonPt  ->Fill(event.subleadMuon_selMuonPt  ,event.weight); 
  m_subleadMuonEt          ->Fill(event.subleadMuonEt          ,event.weight); 
                                                               
  m_MET                    ->Fill(event.MET                    ,event.weight); 
  m_MET_selJetdPhi         ->Fill(event.MET_selJetdPhi         ,event.weight); 
  m_MET_selMuondPhi        ->Fill(event.MET_selMuondPhi        ,event.weight); 
  m_MET_selJetMass         ->Fill(event.MET_selJetMass         ,event.weight); 
  m_MET_selMuonMass        ->Fill(event.MET_selMuonMass        ,event.weight); 
  m_MET_selJetPt           ->Fill(event.MET_selJetPt           ,event.weight); 
  m_MET_selMuonPt          ->Fill(event.MET_selMuonPt          ,event.weight); 

  m_selectedJetTransMET    ->Fill(event.selectedJetTransMET    ,event.weight);

  m_selectedMuonEt  ->Fill(event.selectedMuonEt  ,event.weight); 
  m_selectedJetEt   ->Fill(event.selectedJetEt   ,event.weight);  
  m_selectedMuonPhi ->Fill(event.selectedMuonPhi ,event.weight);  
  m_selectedJetPhi  ->Fill(event.selectedJetPhi  ,event.weight);   
  m_selectedMuonEta ->Fill(event.selectedMuonEta ,event.weight);   
  m_selectedJetEta  ->Fill(event.selectedJetEta  ,event.weight); 

  m_EtPlacementMuon2->Fill(event.secondInDecayMuon, event.weight);
  //m_nJets->Fill(event.myGenJets.size(), event.weight);
  m_nAK8Jets->Fill(event.ak8jetCands, event.weight);
  m_nMuonCands->Fill(event.muonCands, event.weight);
  m_nMuons40->Fill(event.muons40, event.weight);
  m_nAK8Jets40->Fill(event.ak8jets40, event.weight);
  m_leadAK8JetMuonPhi->Fill(event.leadAK8JetMuonPhiVal, event.weight);


  m_leadAK8JetMuonJetMuonEnergyFraction->Fill(event.leadAK8JetMuonJetMuonEnergyFraction, event.weight);


}
