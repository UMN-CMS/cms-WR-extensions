#ifndef eventHistos_h
#define eventHistos_h

//CMSSW CLASSES
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/GenJet.h"
//ROOT CLASSES
#include "TH1D.h"
#include "TH2D.h"
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

class eventHistos {
  public:
  eventHistos();
  void book(TFileDirectory histoFolder, uint16_t flavor, std::string tag, int FSB);
  void fill(eventBits& event, int systematicRegion);



  private:
  void fillGen(eventBits& event);
  void fillReco(eventBits& event);
  void fillWeight(eventBits& event);
  void fillCutProgress(eventBits& event);
  void fillCombine_Nominal(eventBits& event);
  void fillCombine_JECUp(eventBits& event);
  void fillCombine_JECDown(eventBits& event);
  void fillCombine_JERUp(eventBits& event);
  void fillCombine_JERDown(eventBits& event);
  void fillCombine_PUUp(eventBits& event);
  void fillCombine_PUDown(eventBits& event);
  void fillCombine_MuHPtUp(eventBits& event);
  void fillCombine_MuHPtDown(eventBits& event);
  void fillCombine_MuLUp(eventBits& event);
  void fillCombine_MuLDown(eventBits& event);
  void fillCombine_HEEPUp(eventBits& event);
  void fillCombine_HEEPDown(eventBits& event);
 
  TFileDirectory m_histoFolder;
  uint32_t m_flavor;
  int m_FSB;

  //METADATA
  TH1D* m_metaData;

  //GEN PLOTS
  TH1D* m_eventsWeight;
  TH1D* m_finalEventWeight;

  TH1D* m_FSBfinalEventWeight_E;
  TH1D* m_FSBfinalEventWeight_B;

  TH1D* m_FSBfinalEventWeight_E_noISO;
  TH1D* m_FSBfinalEventWeight_B_noISO;

  TH1D* m_nLeptons     ;
  TH1D* m_nMuons       ;
  TH1D* m_nTaus        ;
  TH1D* m_nElectrons   ;
  TH1D* m_nLightPartons;
  TH1D* m_nTops        ;
  TH1D* m_nBs          ;
  TH1D* m_nPartons     ;
  TH1D* m_nAdditionalHEEP;
  TH1D* m_nAdditionalHEEP_noISO;

  TH1D* m_eventFlavor       ;

  TH1D* m_cutProgress;
  TH1D* m_FSBcutProgress;

  TH1D* m_parton1Et;
  TH1D* m_parton2Et;
  TH1D* m_muon1Et;
  TH1D* m_muon2Et;
  TH1D* m_firstPartonJetEt;
  TH1D* m_secondPartonJetEt;
  TH1D* m_firstPartonAK8JetEt;
  TH1D* m_secondPartonAK8JetEt;

  TH1D* m_parton1Eta;
  TH1D* m_parton2Eta;
  TH1D* m_muon1Eta;
  TH1D* m_muon2Eta;
  TH1D* m_firstPartonJetEta;
  TH1D* m_secondPartonJetEta;
  TH1D* m_firstPartonAK8JetEta;
  TH1D* m_secondPartonAK8JetEta;

  TH1D* m_parton1Phi;
  TH1D* m_parton2Phi;
  TH1D* m_muon1Phi;
  TH1D* m_muon2Phi;
  TH1D* m_firstPartonJetPhi;
  TH1D* m_secondPartonJetPhi;
  TH1D* m_firstPartonAK8JetPhi;
  TH1D* m_secondPartonAK8JetPhi;

  TH1D* m_secondMuonWRjetdR;

  TH1D* m_genLSF;
  TH2D* m_recoLSF_v_selJetPt;
  TH2D* m_genLSF_v_recoLSF;
  TH1D* m_dRparton1parton2;
  TH1D* m_dRmuon1muon2;
  TH1D* m_dRparton1muon2;
  TH1D* m_dRparton1muon1;
  TH1D* m_dRparton2muon2;
  TH1D* m_dRparton2muon1;
  TH1D* m_dRparton1jet;
  TH1D* m_dRparton2jet;
  TH1D* m_dRparton1AK8jet;
  TH1D* m_dRparton2AK8jet;

  TH1D* m_firstPartonJetEtHadronic;
  TH1D* m_secondPartonJetEtHadronic;
  TH1D* m_firstPartonJetEtEM;
  TH1D* m_secondPartonJetEtEM;
  TH1D* m_firstPartonJetEtInvisible;
  TH1D* m_secondPartonJetEtInvisible;

  TH1D* m_firstPartonAK8JetEtHadronic;
  TH1D* m_secondPartonAK8JetEtHadronic;
  TH1D* m_firstPartonAK8JetEtEM;
  TH1D* m_secondPartonAK8JetEtEM;
  TH1D* m_firstPartonAK8JetEtInvisible;
  TH1D* m_secondPartonAK8JetEtInvisible;

  TH1D* m_leadSubleadingJetsMuonsMass ;
  TH1D* m_leadSubleadingAK8JetsMuonsMass ;
  TH1D* m_leadSubleadingPartonsMuonsMass ;
  TH1D* m_leadAK8JetMuonMass;
  TH1D* m_leadAK8JetDiMuonMass;
  TH1D* m_leadAK8JetElectronMass;
  TH1D* m_leadAK8JetElectronMass_noISO;

  TH1D* m_leadAK8JetElectronMass_JECUp;
  TH1D* m_leadAK8JetElectronMass_JECDown;

  TH1D* m_leadAK8JetElectronMass_JERUp;
  TH1D* m_leadAK8JetElectronMass_JERDown;

  TH1D* m_leadAK8JetElectronMass_PUUp;
  TH1D* m_leadAK8JetElectronMass_PUDown;

  TH1D* m_leadAK8JetElectronMass_MuLUp;
  TH1D* m_leadAK8JetElectronMass_MuLDown;

  TH1D* m_leadAK8JetElectronMass_HEEPUp;
  TH1D* m_leadAK8JetElectronMass_HEEPDown;

  TH1D* m_leadAK8JetMuonMass_JECUp;
  TH1D* m_leadAK8JetMuonMass_JECDown;

  TH1D* m_leadAK8JetMuonMass_JERUp;
  TH1D* m_leadAK8JetMuonMass_JERDown;

  TH1D* m_leadAK8JetMuonMass_PUUp;
  TH1D* m_leadAK8JetMuonMass_PUDown;

  TH1D* m_leadAK8JetMuonMass_MuHPtUp;
  TH1D* m_leadAK8JetMuonMass_MuHPtDown;

  TH1D* m_leadAK8JetMuonMass_MuLUp;
  TH1D* m_leadAK8JetMuonMass_MuLDown;

  TH1D* m_leadSubleadingJetsMuonsPt ;
  TH1D* m_leadSubleadingAK8JetsMuonsPt ;
  TH1D* m_leadSubleadingPartonsMuonsPt ;
  TH1D* m_leadAK8JetMuonPt;
  TH1D* m_leadAK8JetElectronPt;
  TH1D* m_leadAK8JetElectronPt_noISO;

  TH1D* m_leadSubleadingJetsMuonsEta ;
  TH1D* m_leadSubleadingAK8JetsMuonsEta ;
  TH1D* m_leadSubleadingPartonsMuonsEta ;
  TH1D* m_leadAK8JetMuonEta;
  TH1D* m_leadAK8JetElectronEta;
  TH1D* m_leadAK8JetElectronEta_noISO;

  TH1D* m_leadAK8JetMuonPhi;
  TH1D* m_leadAK8JetElectronPhi;
  TH1D* m_leadAK8JetElectronPhi_noISO;

  TH1D* m_leadAK8JetMuonJetMuonEnergyFraction;
  TH1D* m_leadAK8JetElectronJetMuonEnergyFraction;
  TH1D* m_leadAK8JetElectronJetMuonEnergyFraction_noISO;

  TH1D* m_electronTrigger;
  TH1D* m_muonTrigger;
  //RECO PLOTS
  TH1D* m_selectedMuonPt; 
  TH1D* m_selectedMuonPhi;
  TH1D* m_selectedMuonEta;

  TH1D* m_selectedElectronPt; 
  TH1D* m_selectedElectronPhi;
  TH1D* m_selectedElectronEta;
  TH1D* m_selectedElectronEta_Weight1;

  TH1D* m_selectedElectron_noISO_Pt; 
  TH1D* m_selectedElectron_noISO_Phi;
  TH1D* m_selectedElectron_noISO_Eta;

  TH1D* m_selectedJetPt;
  TH1D* m_selectedJetPhi;
  TH1D* m_selectedJetEta;
  TH1D* m_selectedJetMass;
  TH1D* m_selectedJetPrunedMass;
  TH1D* m_selectedJetTau21;
  TH1D* m_selectedJetMaxDRGenDaughters;
  TH1D* m_selectedJetLSF3;
  TH1D* m_selectedJetMaxSubJetCSV;

  TH1D* m_DrDaughters;
  TH1D* m_nWRDaughters;
  TH1D* m_capturedBothDaughtersInSingleJet;
  TH1D* m_pickedCorrectJet;
  TH1D* m_dPhiLeadMuonJetWithDaughters;
  TH1D* m_selectedIncorrectJetMass;
  TH1D* m_JetWithDaughtersMass;
  TH1D* m_secondGENMuonRECOjetDR;
  TH1D* m_secondRECOMuonRECOjetDR;

  TH1D* m_selectedJet_EleNoISO_Pt;
  TH1D* m_selectedJet_EleNoISO_Phi;
  TH1D* m_selectedJet_EleNoISO_Eta;
  TH1D* m_selectedJet_EleNoISO_Mass;
  TH1D* m_selectedJet_EleNoISO_Tau21;

  TH1D* m_subleadMuonEt;
  TH1D* m_subleadMuonEta;
  TH1D* m_subleadMuonPhi;

  TH1D* m_EtPlacementMuon2;
  TH1D* m_nJets;
  TH1D* m_nAK8Jets;
  TH1D* m_nMuonCands;
  TH1D* m_nElectronCands50;
  TH1D* m_nElectronCands100;
  TH1D* m_nElectronCands150;
  TH1D* m_nElectronCands200;
  TH1D* m_nElectronCands50_noISO;
  TH1D* m_nElectronCands200_noISO;
  TH1D* m_nMuons10;
  TH1D* m_nAK8Jets40;

  TH1D* m_subleadMuon_selJetdPhi;
  TH1D* m_subleadMuon_selElectronPhi;
  TH1D* m_subleadMuon_selElectronMass;
  TH1D* m_subleadMuon_selElectronZMass;
  TH1D* m_subleadMuon_selElectronPt;
  TH1D* m_subleadMuon_selMuondPhi;
  TH1D* m_subleadMuon_selMuonMass;
  TH1D* m_subleadMuon_selMuonZMass;
  TH1D* m_subleadMuon_selMuonZMass_Weight1;
  TH1D* m_subleadMuon_selMuonPt;

  TH1D* m_dRmuon2;

  TH1D* m_MET;
  TH1D* m_MET_selJetdPhi;
  TH1D* m_MET_selElectrondPhi;
  TH1D* m_MET_selMuondPhi;
  TH1D* m_MET_selJetMass;
  TH1D* m_MET_selElectronMass;
  TH1D* m_MET_selMuonMass;
  TH1D* m_MET_selJetPt;
  TH1D* m_MET_selElectronPt;
  TH1D* m_MET_selMuonPt;

  TH1D* m_selectedJetTransMET;

  TH1D* m_nVertices;

  TH1D* m_vertexDiffTan;
  TH1D* m_vertexDiffLon;

  TH1D* m_MuonWeight;
  TH1D* m_PUWeight;

  TH1D* m_HEEP_SF;
  TH1D* m_HEEP_SF_E;
  TH1D* m_HEEP_SF_B;
  TH1D* m_Electron_Reco_SF;

  TH1D* m_HEEP_SF_noISO;
  TH1D* m_HEEP_SF_E_noISO;
  TH1D* m_HEEP_SF_B_noISO;
  TH1D* m_Electron_Reco_SF_noISO;

  TH1D* m_selElectron_barrel_dEtaInSeed      ; 
  TH1D* m_selElectron_barrel_dPhiIn          ; 
  TH1D* m_selElectron_barrel_HoverE          ; 
  TH1D* m_selElectron_barrel_sig_ietaieta_5x5;
  TH1D* m_selElectron_barrel_E2x5vE5x5       ; 
  TH1D* m_selElectron_barrel_EM_had_depIso   ; 
  TH1D* m_selElectron_barrel_trackIso        ; 
  TH1D* m_selElectron_barrel_trackPnt        ; 
  TH1D* m_selElectron_barrel_innerLostHits   ; 
  TH1D* m_selElectron_barrel_dxy             ; 

  TH1D* m_selElectron_endcap_dEtaInSeed      ; 
  TH1D* m_selElectron_endcap_dPhiIn          ; 
  TH1D* m_selElectron_endcap_HoverE          ; 
  TH1D* m_selElectron_endcap_sig_ietaieta_5x5;
  TH1D* m_selElectron_endcap_E2x5vE5x5       ; 
  TH1D* m_selElectron_endcap_EM_had_depIso   ; 
  TH1D* m_selElectron_endcap_trackIso        ; 
  TH1D* m_selElectron_endcap_trackPnt        ; 
  TH1D* m_selElectron_endcap_innerLostHits   ; 
  TH1D* m_selElectron_endcap_dxy             ; 

  TH1D* m_selElectron_noISO_barrel_dEtaInSeed      ; 
  TH1D* m_selElectron_noISO_barrel_dPhiIn          ; 
  TH1D* m_selElectron_noISO_barrel_HoverE          ; 
  TH1D* m_selElectron_noISO_barrel_sig_ietaieta_5x5;
  TH1D* m_selElectron_noISO_barrel_E2x5vE5x5       ; 
  TH1D* m_selElectron_noISO_barrel_EM_had_depIso   ; 
  TH1D* m_selElectron_noISO_barrel_trackIso        ; 
  TH1D* m_selElectron_noISO_barrel_trackPnt        ; 
  TH1D* m_selElectron_noISO_barrel_innerLostHits   ; 
  TH1D* m_selElectron_noISO_barrel_dxy             ; 

  TH1D* m_selElectron_noISO_endcap_dEtaInSeed      ; 
  TH1D* m_selElectron_noISO_endcap_dPhiIn          ; 
  TH1D* m_selElectron_noISO_endcap_HoverE          ; 
  TH1D* m_selElectron_noISO_endcap_sig_ietaieta_5x5;
  TH1D* m_selElectron_noISO_endcap_E2x5vE5x5       ; 
  TH1D* m_selElectron_noISO_endcap_EM_had_depIso   ; 
  TH1D* m_selElectron_noISO_endcap_trackIso        ; 
  TH1D* m_selElectron_noISO_endcap_trackPnt        ; 
  TH1D* m_selElectron_noISO_endcap_innerLostHits   ; 
  TH1D* m_selElectron_noISO_endcap_dxy             ; 

};

#endif
