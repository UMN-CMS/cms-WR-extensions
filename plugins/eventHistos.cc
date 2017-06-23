#include "eventHistos.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH2D.h"
//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "eventBits.h"
#include "tools.h"





eventHistos::eventHistos () {}




void eventHistos::book(TFileDirectory histoFolder, uint16_t flavor) {

  //CREATED FOLDER IN HISTO
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

    m_firstPartonJetEt =                m_histoFolder.make<TH1D>("firstPartonJetEt", " Jet Et for Leading Parton",            100,0.0,8000);
    m_secondPartonJetEt =               m_histoFolder.make<TH1D>("secondPartonJetEt"," Jet Et for Subleading Parton",         100,0.0,8000);
    m_firstPartonJetEtHadronic =        m_histoFolder.make<TH1D>("firstPartonJetEtHadronic", "Hadronic Jet Et for Leading Parton",      100,0.0,8000);
    m_secondPartonJetEtHadronic =       m_histoFolder.make<TH1D>("secondPartonJetEtHadronic","Hadronic Jet Et for Subleading Parton",   100,0.0,8000);
    m_firstPartonJetEtEM =              m_histoFolder.make<TH1D>("firstPartonJetEtEM", "EM Jet Et for Leading Parton",                  100,0.0,8000);
    m_secondPartonJetEtEM =             m_histoFolder.make<TH1D>("secondPartonJetEtEM","EM Jet Et for Subleading Parton",               100,0.0,8000);
    m_firstPartonJetEtInvisible =       m_histoFolder.make<TH1D>("firstPartonJetEtInvisible", "Invisible Jet Et for Leading Parton",    100,0.0,8000);
    m_secondPartonJetEtInvisible =      m_histoFolder.make<TH1D>("secondPartonJetEtInvisible","Invisible Jet Et for Subleading Parton", 100,0.0,8000);

    m_firstPartonAK8JetEt =             m_histoFolder.make<TH1D>("firstPartonAK8JetEt", " AK8Jet Et for Leading Parton",            100,0.0,8000);
    m_secondPartonAK8JetEt =            m_histoFolder.make<TH1D>("secondPartonAK8JetEt"," AK8Jet Et for Subleading Parton",         100,0.0,8000);
    m_firstPartonAK8JetEtHadronic =     m_histoFolder.make<TH1D>("firstPartonAK8JetEtHadronic", "Hadronic AK8Jet Et for Leading Parton",      100,0.0,8000);
    m_secondPartonAK8JetEtHadronic =    m_histoFolder.make<TH1D>("secondPartonAK8JetEtHadronic","Hadronic AK8Jet Et for Subleading Parton",   100,0.0,8000);
    m_firstPartonAK8JetEtEM =           m_histoFolder.make<TH1D>("firstPartonAK8JetEtEM", "EM AK8Jet Et for Leading Parton",                  100,0.0,8000);
    m_secondPartonAK8JetEtEM =          m_histoFolder.make<TH1D>("secondPartonAK8JetEtEM","EM AK8Jet Et for Subleading Parton",               100,0.0,8000);
    m_firstPartonAK8JetEtInvisible =    m_histoFolder.make<TH1D>("firstPartonAK8JetEtInvisible", "Invisible AK8Jet Et for Leading Parton",    100,0.0,8000);
    m_secondPartonAK8JetEtInvisible =   m_histoFolder.make<TH1D>("secondPartonAK8JetEtInvisible","Invisible AK8Jet Et for Subleading Parton", 100,0.0,8000);


    m_leadSubleadingJetsMuonsMass  =    m_histoFolder.make<TH1D>("leadingSubleadingJetsMuonsMass","Four Object Mass of the 2 leading Jets and Muons",      100, 0.0,8000);
    m_leadSubleadingAK8JetsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsMass","Four Object Mass of the 2 leading AK8Jets and Muons",100, 0.0,8000);
    m_leadSubleadingPartonsMuonsMass  = m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsMass","Four Object Mass of the 2 leading Partons and Muons",100, 0.0,8000);

    m_leadSubleadingAK8JetsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsPt","Four Object Pt of the 2 leading AK8Jets and Muons",    100, 0.0,8000);
    m_leadSubleadingPartonsMuonsPt  =   m_histoFolder.make<TH1D>("leadingSubleadingPartonsMuonsPt","Four Object Pt of the 2 leading Partons and Muons",    100, 0.0,8000);

    m_leadSubleadingAK8JetsMuonsEta  =  m_histoFolder.make<TH1D>("leadingSubleadingAK8JetsMuonsEta","Four Object Eta of the 2 leading AK8Jets and Muons",  100, -4.0,4.0);
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

//SPECIFIC 
void eventHistos::fillGen(eventBits& event) {
  std::cout << "Making GEN plots" << std::endl;
  if(!(event.passesGenCuts())) {
    std::cout << "ERROR! THIS EVENT SHOULD HAVE FAILED" <<std::endl;
    return;
  }
  m_parton1Et->Fill(event.parton1EtVal);
  m_parton2Et->Fill(event.parton2EtVal);
  m_muonHighestEt->Fill(event.muonHighestEtVal);
  m_muonSecondHighestEt->Fill(event.muonSecondHighestEtVal);
  m_firstPartonJetEt->Fill(event.firstPartonJetEtVal);
  m_secondPartonJetEt->Fill(event.secondPartonJetEtVal);
  m_firstPartonAK8JetEt->Fill(event.firstPartonAK8JetEtVal);
  m_secondPartonAK8JetEt->Fill(event.secondPartonAK8JetEtVal);

  m_parton1Eta->Fill(event.parton1EtaVal);
  m_parton2Eta->Fill(event.parton2EtaVal);
  m_muonHighestEtEta->Fill(event.muonHighestEtEtaVal);
  m_muonSecondHighestEtEta->Fill(event.muonSecondHighestEtEtaVal);
  m_firstPartonJetEta->Fill(event.firstPartonJetEtaVal);
  m_secondPartonJetEta->Fill(event.secondPartonJetEtaVal);
  m_firstPartonAK8JetEta->Fill(event.firstPartonAK8JetEtaVal);
  m_secondPartonAK8JetEta->Fill(event.secondPartonAK8JetEtaVal);

  m_parton1Phi->Fill(event.parton1PhiVal);
  m_parton2Phi->Fill(event.parton2PhiVal);
  m_muonHighestEtPhi->Fill(event.muonHighestEtPhiVal);
  m_muonSecondHighestEtPhi->Fill(event.muonSecondHighestEtPhiVal);
  m_firstPartonJetPhi->Fill(event.firstPartonJetPhiVal);
  m_secondPartonJetPhi->Fill(event.secondPartonJetPhiVal);
  m_firstPartonAK8JetPhi->Fill(event.firstPartonAK8JetPhiVal);
  m_secondPartonAK8JetPhi->Fill(event.secondPartonAK8JetPhiVal);

  m_dRparton1parton2->Fill(event.dRparton1parton2Val);
  m_dRmuon1muon2->Fill(event.dRmuon1muon2Val);
  m_dRparton1muon2->Fill(event.dRparton1muon2Val);
  m_dRparton1muon1->Fill(event.dRparton1muon1Val);
  m_dRparton2muon2->Fill(event.dRparton2muon2Val);
  m_dRparton2muon1->Fill(event.dRparton2muon1Val);

  m_firstPartonJetEtHadronic->Fill(event.firstPartonJetEtHadronicVal);
  m_secondPartonJetEtHadronic->Fill(event.secondPartonJetEtHadronicVal);
  m_firstPartonJetEtEM->Fill(event.firstPartonJetEtEMVal);
  m_secondPartonJetEtEM->Fill(event.secondPartonJetEtEMVal);
  m_firstPartonJetEtInvisible->Fill(event.firstPartonJetEtInvisibleVal);
  m_secondPartonJetEtInvisible->Fill(event.secondPartonJetEtInvisibleVal);
  

  m_firstPartonAK8JetEtHadronic->Fill(event.firstPartonAK8JetEtHadronicVal);
  m_secondPartonAK8JetEtHadronic->Fill(event.secondPartonAK8JetEtHadronicVal);
  m_firstPartonAK8JetEtEM->Fill(event.firstPartonAK8JetEtEMVal);
  m_secondPartonAK8JetEtEM->Fill(event.secondPartonAK8JetEtEMVal);
  m_firstPartonAK8JetEtInvisible->Fill(event.firstPartonAK8JetEtInvisibleVal);
  m_secondPartonAK8JetEtInvisible->Fill(event.secondPartonAK8JetEtInvisibleVal);

  m_leadSubleadingPartonsMuonsMass->Fill(event.leadSubleadingPartonsMuonsMassVal);
  m_leadSubleadingJetsMuonsMass->Fill(event.leadSubleadingJetsMuonsMassVal);
  m_leadSubleadingAK8JetsMuonsMass->Fill(event.leadSubleadingAK8JetsMuonsMassVal);

  m_leadSubleadingPartonsMuonsPt->Fill(event.leadSubleadingPartonsMuonsPtVal);
  m_leadSubleadingAK8JetsMuonsPt->Fill(event.leadSubleadingAK8JetsMuonsPtVal);

  m_leadSubleadingPartonsMuonsEta->Fill(event.leadSubleadingPartonsMuonsEtaVal);
  m_leadSubleadingAK8JetsMuonsEta->Fill(event.leadSubleadingAK8JetsMuonsEtaVal);



}
void eventHistos::fillReco(eventBits& event) {


}
