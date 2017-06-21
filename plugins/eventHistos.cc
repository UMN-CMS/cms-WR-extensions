#include "eventHistos.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "TH2D.h"
//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "eventBits.h"
#include "tools.h"





eventHistos::eventHistos () {}




void eventHistos::book(TFileDirectory histoFolder, uint16_t flavor) {

  //CREATED FOLDER IN HISTOFILE WITH FLAVOR SPECIFIED
  m_flavor = flavor;
  m_histoFolder = histoFolder;

  if (m_flavor == 1) {
  //MAKE GEN PLOTS
    m_parton1Et =                       m_histoFolder.make<TH1D>("parton1Et", "Parton 1 Et",                         100, 0.0, 8000);
    m_parton2Et =                       m_histoFolder.make<TH1D>("parton2Et", "Parton 2 Et",                         100, 0.0, 8000);
    m_muonHighestEt =                   m_histoFolder.make<TH1D>("muonHighestEt", "Highest Et Muon Et",              100, 0.0, 8000);
    m_muonSecondHighestEt =             m_histoFolder.make<TH1D>("muonSecondHighestEt", "Second Highest Et Muon Et", 100, 0.0, 8000);

    m_parton1Eta =                      m_histoFolder.make<TH1D>("parton1Eta", "Parton 1 Eta",                        100, -4.0, 4.0);
    m_parton2Eta =                      m_histoFolder.make<TH1D>("parton2Eta", "Parton 2 Eta",                        100, -4.0, 4.0);
    m_muonHighestEtEta =                m_histoFolder.make<TH1D>("muonHighestEtEta", "Highest Et muon eta",           100, -4.0, 4.0);
    m_muonSecondHighestEtEta =          m_histoFolder.make<TH1D>("muonSecondHighestEtEta", "2nd Highest Et muon eta", 100, -4.0, 4.0);

    m_dRparton1parton2 =                m_histoFolder.make<TH1D>("dRparton1parton2", "deltaR between partons",         100, 0.0, 12.0);
    m_dRmuon1muon2 =                    m_histoFolder.make<TH1D>("dRmuon1muon2", "deltaR between muons",               100, 0.0, 12.0);
    m_dRparton1muon2 =                  m_histoFolder.make<TH1D>("dRparton1muon2", "deltaR between parton1 and muon2", 100, 0.0, 12.0);
    m_dRparton1muon1 =                  m_histoFolder.make<TH1D>("dRparton1muon1", "deltaR between parton1 and muon1", 100, 0.0, 12.0);
    m_dRparton2muon2 =                  m_histoFolder.make<TH1D>("dRparton2muon2", "deltaR between parton2 and muon2", 100, 0.0, 12.0);
    m_dRparton2muon1 =                  m_histoFolder.make<TH1D>("dRparton2muon1", "deltaR between parton2 and muon1", 100, 0.0, 12.0);

    m_firstPartonJetEtTotal =           m_histoFolder.make<TH1D>("firstPartonJetEtTotal", "Total Jet Et for Leading Parton",            100,0.0,8000);
    m_secondPartonJetEtTotal =          m_histoFolder.make<TH1D>("secondPartonJetEtTotal","Total Jet Et for Subleading Parton",         100,0.0,8000);
    m_firstPartonJetEtHadronic =        m_histoFolder.make<TH1D>("firstPartonJetEtHadronic", "Hadronic Jet Et for Leading Parton",      100,0.0,8000);
    m_secondPartonJetEtHadronic =       m_histoFolder.make<TH1D>("secondPartonJetEtHadronic","Hadronic Jet Et for Subleading Parton",   100,0.0,8000);
    m_firstPartonJetEtEM =              m_histoFolder.make<TH1D>("firstPartonJetEtEM", "EM Jet Et for Leading Parton",                  100,0.0,8000);
    m_secondPartonJetEtEM =             m_histoFolder.make<TH1D>("secondPartonJetEtEM","EM Jet Et for Subleading Parton",               100,0.0,8000);
    m_firstPartonJetEtInvisible =       m_histoFolder.make<TH1D>("firstPartonJetEtInvisible", "Invisible Jet Et for Leading Parton",    100,0.0,8000);
    m_secondPartonJetEtInvisible =      m_histoFolder.make<TH1D>("secondPartonJetEtInvisible","Invisible Jet Et for Subleading Parton", 100,0.0,8000);

    m_leadSubleadingJetsMuonsMass  =    m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsMass","Four Object Mass of the 2 leading Jets and Muons",      100, 0.0,8000);
    m_leadSubleadingPartonsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsMass","Four Object Mass of the 2 leading Partons and Muons",100, 0.0,8000);

    m_leadSubleadingJetsMuonsPt  =      m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsPt","Four Object Pt of the 2 leading Jets and Muons",          100, 0.0,8000);
    m_leadSubleadingPartonsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsPt","Four Object Pt of the 2 leading Partons and Muons",    100, 0.0,8000);

    m_leadSubleadingJetsMuonsEta  =     m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsEta","Four Object Eta of the 2 leading Jets and Muons",        100, -4.0,4.0);
    m_leadSubleadingPartonsMuonsEta  =  m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsEta","Four Object Eta of the 2 leading Partons and Muons",  100, -4.0,4.0);



    


  } else if (m_flavor == 2) {
  //MAKE RECO PLOTS




  } else if (m_flavor == 3) {
  //MAKE GEN & RECO PLOTS


  } else {
  //NOTHING FOR NOW
  }


}
void eventHistos::fill(eventBits& event) {
  if(m_flavor == 1) fillGen(event);
  else if(m_flavor == 2) fillReco(event);
  else if(m_flavor == 3) {
    fillGen(event);
    fillReco(event);
  }
  else return;
}

//SPECIFIC FILLER CLASSES
void eventHistos::fillGen(eventBits& event) {
  std::cout << "Making GEN plots" << std::endl;
  if(!(event.passesGenCuts())) {
    std::cout << "ERROR! THIS EVENT SHOULD HAVE FAILED" <<std::endl;
    return;
  }
  m_parton1Et->Fill(event.getHighestEtParton()->et());
  m_parton2Et->Fill(event.getSecondHighestEtParton()->et());
  m_muonHighestEt->Fill(event.getHighestEtMuon()->et());
  m_muonSecondHighestEt->Fill(event.getSecondHighestEtMuon()->et());

  m_parton1Eta->Fill(event.getHighestEtParton()->eta());
  m_parton2Eta->Fill(event.getSecondHighestEtParton()->eta());
  m_muonHighestEtEta->Fill(event.getHighestEtMuon()->eta());
  m_muonSecondHighestEtEta->Fill(event.getSecondHighestEtMuon()->eta());

  m_dRparton1parton2->Fill(deltaR2(*(event.getHighestEtParton()),*(event.getSecondHighestEtParton())));
  m_dRmuon1muon2->Fill(deltaR2(*(event.getHighestEtMuon()),*(event.getSecondHighestEtMuon())));
  m_dRparton1muon2->Fill(deltaR2(*(event.getHighestEtParton()),*(event.getSecondHighestEtMuon())));
  m_dRparton1muon1->Fill(deltaR2(*(event.getHighestEtParton()),*(event.getHighestEtMuon())));
  m_dRparton2muon2->Fill(deltaR2(*(event.getSecondHighestEtParton()),*(event.getSecondHighestEtMuon())));
  m_dRparton2muon1->Fill(deltaR2(*(event.getSecondHighestEtParton()),*(event.getHighestEtMuon())));

  m_firstPartonJetEtTotal->Fill(event.getFirstPartonGenJet()->et());   
  m_secondPartonJetEtTotal->Fill(event.getSecondPartonGenJet()->et());  
  m_firstPartonJetEtHadronic->Fill(event.getFirstPartonGenJet()->hadEnergy());
  m_secondPartonJetEtHadronic->Fill(event.getSecondPartonGenJet()->hadEnergy());
  m_firstPartonJetEtEM->Fill(event.getFirstPartonGenJet()->emEnergy());      
  m_secondPartonJetEtEM->Fill(event.getSecondPartonGenJet()->emEnergy());     
  m_firstPartonJetEtInvisible->Fill(event.getFirstPartonGenJet()->invisibleEnergy());
  m_secondPartonJetEtInvisible->Fill(event.getSecondPartonGenJet()->invisibleEnergy());

  m_leadSubleadingPartonsMuonsMass->Fill((event.getHighestEtParton()->p4()+event.getSecondHighestEtParton()->p4()+event.getHighestEtMuon()->p4()+event.getSecondHighestEtMuon()->p4()).mass());
  m_leadSubleadingJetsMuonsMass->Fill((event.getSecondPartonGenJet()->p4()+event.getFirstPartonGenJet()->p4()+event.getHighestEtMuon()->p4()+event.getSecondHighestEtMuon()->p4()).mass());

  m_leadSubleadingPartonsMuonsPt->Fill((event.getHighestEtParton()->p4()+event.getSecondHighestEtParton()->p4()+event.getHighestEtMuon()->p4()+event.getSecondHighestEtMuon()->p4()).pt());
  m_leadSubleadingJetsMuonsPt->Fill((event.getSecondPartonGenJet()->p4()+event.getFirstPartonGenJet()->p4()+event.getHighestEtMuon()->p4()+event.getSecondHighestEtMuon()->p4()).pt());

  m_leadSubleadingPartonsMuonsEta->Fill((event.getHighestEtParton()->p4()+event.getSecondHighestEtParton()->p4()+event.getHighestEtMuon()->p4()+event.getSecondHighestEtMuon()->p4()).eta());
  m_leadSubleadingJetsMuonsEta->Fill((event.getSecondPartonGenJet()->p4()+event.getFirstPartonGenJet()->p4()+event.getHighestEtMuon()->p4()+event.getSecondHighestEtMuon()->p4()).eta());



}
void eventHistos::fillReco(eventBits& event) {



}
