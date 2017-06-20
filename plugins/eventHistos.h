//CMSSW CLASSES
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/GenJet.h"
//ROOT CLASSES
#include "TH1D.h"
#include "TH2D.h"
#include "TDirectory.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventBits.h"
#include "tools.h"

class eventHistos {
  public:
  eventHistos();
  void book(TDirectory* histoFolder, uint16_t flavor);
  void fill(eventBits event);



  private:
  void fillGen();
  void fillReco();

 
  TDirectory* m_histoFolder;
  uint32_t m_flavor;

  //GEN PLOTS
  TH1D m_parton1Et;
  TH1D m_parton2Et;
  TH1D m_muonHighestEt;
  TH1D m_muonSecondHighestEt;

  TH1D m_parton1Eta;
  TH1D m_parton2Eta;
  TH1D m_muonHighestEtEta;
  TH1D m_muonSecondHighestEtEta;

  TH1D m_dRparton1parton2;
  TH1D m_dRmuon1muon2;
  TH1D m_dRparton1muon2;
  TH1D m_dRparton1muon1;
  TH1D m_dRparton2muon2;
  TH1D m_dRparton2muon1;

  TH1D m_firstPartonJetEtTotal;
  TH1D m_secondPartonJetEtTotal;
  TH1D m_firstPartonJetEtHadronic;
  TH1D m_secondPartonJetEtHadronic;
  TH1D m_firstPartonJetEtEM;
  TH1D m_secondPartonJetEtEM;
  TH1D m_firstPartonJetEtInvisible;
  TH1D m_secondPartonJetEtInvisible;

  TH1D m_leadSubleadingJetsMuonsMass ;
  TH1D m_leadSubleadingPartonsMuonsMass ;

  TH1D m_leadSubleadingJetsMuonsPt ;
  TH1D m_leadSubleadingPartonsMuonsPt ;

  TH1D m_leadSubleadingJetsMuonsEta ;
  TH1D m_leadSubleadingPartonsMuonsEta ;
  //RECO PLOTS

};
