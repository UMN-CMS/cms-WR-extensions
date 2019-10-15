#ifndef eventBits_h
#define eventBits_h

#include <stdint.h>
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "BaconAna/DataFormats/interface/TAddJet.hh"

#include "TLorentzVector.h"

class eventBits {
public:

  eventBits();
  ~eventBits();
  void clear();
  //Misc numbers
  int run;
  int lumi;
  int event;
  int m_flavor;
  double weight;
  double FSBweight;
  double FSBweight_E;
  double FSBweight_B;
  double FSBweight_noISO;
  double FSBweight_E_noISO;
  double FSBweight_B_noISO;

  int RECOcategory;

  bool isMC;
  bool hasPVertex;

  std::string outputTag;  //LABELLING THE DATA

  //DOES THIS EVENT COUNT AS AN EVENT OR A NEGATIVE EVENT?
  int count;

  //BASIC FUNCTIONS
  bool passesGenCuts();
 
  //CUTFLOW BOOLS
  bool passPreSelectGen;
  bool passResGen;
  bool passGenCounter;

  //EVENT COLLECTIONS
  std::vector<const reco::GenJet*> myGenJets;
  std::vector<const reco::GenJet*> myAK8GenJets;
  std::vector<const reco::GenParticle*> myGenPartons;
  std::vector<const reco::GenParticle*> myGenMuons;
  std::vector<const reco::GenParticle*> myGenLeptons;
  std::vector<const reco::GenParticle*> genWRDaughters;

  //GEN jet studies
  int nDaughters;
  double MaxDR_genDaughter_CandJet;
  double dR_Daughters;
  int capturedBothDaughtersInSingleJet;
  int pickedCorrectJet;
  double dPhi_LeadMuonJetWithDaughters;
  double selectedIncorrectJetMass;
  double JetWithDaughtersMass;
  int JetContainingBothDaughters;
  double secondGENMuonRECOjetDR;
  double secondRECOMuonRECOjetDR;
  int nMuonOutSideJetFSB;

  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> myElectronJetPairs;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> myElectronJetPairs_JECUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> myElectronJetPairs_JECDown;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> myElectronJetPairs_JERUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> myElectronJetPairs_JERDown;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Electron*>> myElectronJetPairs_noISO;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_JECUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_JECDown;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_JERUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_JERDown;

  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_noLSF;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_noLSF_JECUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_noLSF_JECDown;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_noLSF_JERUp;
  std::vector<std::pair<const baconhep::TAddJet*, const pat::Muon*>>     myMuonJetPairs_noLSF_JERDown;

  std::vector<const pat::Jet*>      myJetCandsHighPt;
  std::vector<const pat::Jet*>      myJetCands;
  std::vector<const pat::Electron*> myElectronCandsHighPt200;
  std::vector<const pat::Electron*> myElectronCandsHighPt150;
  std::vector<const pat::Electron*> myElectronCandsHighPt100;
  std::vector<const pat::Electron*> myElectronCandsHighPt50;
  std::vector<const pat::Electron*> myElectronCandsHighPt50_noISO;
  std::vector<const pat::Electron*> myElectronCandsHighPt200_noISO;
  std::vector<const pat::Muon*>     myMuonCandsHighPt;
  std::vector<const pat::Muon*>     resolvedANAMuons;
  std::vector<const pat::Electron*> resolvedANAElectrons;
  std::vector<const pat::Muon*>     myMuonCands;
  const pat::MET*                   myMET;

  std::vector<const baconhep::TAddJet*>      myResCandJets;
  std::vector<const baconhep::TAddJet*>      myResCandJets_JECUp;
  std::vector<const baconhep::TAddJet*>      myResCandJets_JECDown;
  std::vector<const baconhep::TAddJet*>      myResCandJets_JERUp;
  std::vector<const baconhep::TAddJet*>      myResCandJets_JERDown;

  std::vector<const baconhep::TAddJet*>      myResFSBCandJets;
  std::vector<const baconhep::TAddJet*>      myResFSBCandJets_JECUp;
  std::vector<const baconhep::TAddJet*>      myResFSBCandJets_JECDown;
  std::vector<const baconhep::TAddJet*>      myResFSBCandJets_JERUp;
  std::vector<const baconhep::TAddJet*>      myResFSBCandJets_JERDown;

  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_JECUp;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_JECDown;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_JERUp;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_JERDown;

  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_noLSF;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_noLSF_JECUp;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_noLSF_JECDown;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_noLSF_JERUp;
  std::vector<const baconhep::TAddJet*>   myAddJetCandsHighPt_noLSF_JERDown;

  const pat::Muon*                  resFSBMuon;
  const pat::Electron*              resFSBElec;

  std::vector<double> leadResMuonScale;
  std::vector<double> secondResMuonScale;
  std::vector<double> leadBoostMuonScale;
  std::vector<double> secondBoostMuonScale;
  std::vector<double> leadFSBMuonScale;
  

  const pat::Electron*              myElectronCand;
  const pat::Electron*              myElectronCand_noISO;
  const pat::Muon*                  myMuonCand;
  const pat::Muon*                  mySubleadMuon;
  const pat::Muon*                  my2ndHighPtMuonCand;

  const pat::Electron*              secondElectronCand;

  const reco::GenParticle*          firstMuon;
  const reco::GenParticle*          secondMuon;
  const reco::GenParticle*          firstElectron;
  const reco::GenParticle*          secondElectron;

  const reco::Candidate*            NR;
  const reco::Candidate*            WR;

  const reco::GenParticle*          myZ;

  const reco::GenParticle*          genSecondMuon;

  const reco::Vertex*               PVertex;

  math::XYZPoint*             genVtx;

  math::XYZTLorentzVector     daughterClusterVector;

  bool muonTrigPassBit;

  int secondInDecayMuon;

  //EVENT VALUES
  int neutrinoDecays;
  int cutProgress;
  int ResCutProgress;
  int ResElecCutProgress;
  int FSBcutProgress;
  int ResFSBCutProgress;
  bool passesWR2016;
  int muons10;
  int ak8jets40;

  math::XYZPoint*  myVertexDiff;
  double myVertexDiffTan;
  double myVertexDiffLon;

  int    muonCands;
  int    NresolvedANAMuonCands;
  int    NresolvedANAElectronCands;
  int    NresolvedANAFSBLeptonCands;
  int    electronCands50;
  int    electronCands100;
  int    electronCands150;
  int    electronCands200;
  int    electronCands200_noISO;
  int    electronCands50_noISO;
  int    ak8jetCands;
  int    mynLeptons     ;
  int    mynMuons       ;
  int    mynTaus        ;
  int    mynElectrons   ;
  int    mynLightPartons;
  int    mynTops        ;
  int    mynBs          ;
  int    mynPartons     ;
  int	 nAdditionalHEEP;
  int	 nAdditionalHEEP_noISO;
  int    nHighPtMuonsOutsideJet;
  int    nHighPtMuonsOutsideJet_JECUp;
  int    nHighPtMuonsOutsideJet_JECDown;
  int    nHighPtMuonsOutsideJet_JERUp;
  int    nHighPtMuonsOutsideJet_JERDown;

  double NRenergy;

  double eventNumber;
  double lumiSection;

  int    nSecondElectronCands;

  int	 nResFSBMuons;

  int    myEventFlavor  ;

  int	 nVtx		;
  int	 nPU		;
  int	 nPUmean	;

  float  puWeight	;
  float  puWeight_Up    ;
  float  puWeight_Down  ;

  double _prefiringweight;
  double _prefiringweightup;
  double _prefiringweightdown;

  double Muon_HighPtID_Weight;
  double Muon_HighPtID_WeightUp;
  double Muon_HighPtID_WeightDown;

  double Muon_HighPtID2nd_Weight;
  double Muon_HighPtID2nd_WeightUp;
  double Muon_HighPtID2nd_WeightDown;

  double Muon_LooseID_Weight;
  double Muon_LooseID_WeightUp;
  double Muon_LooseID_WeightDown;

  double Muon_LooseTkIso_Weight;
  double Muon_LooseTkIso_WeightUp;
  double Muon_LooseTkIso_WeightDown;

  double Muon_LooseTkIso2nd_Weight;
  double Muon_LooseTkIso2nd_WeightUp;
  double Muon_LooseTkIso2nd_WeightDown;

  double Muon_RECO_Weight;
  double Muon_RECO_WeightUp;
  double Muon_RECO_WeightDown;

  double Muon_RECO2nd_Weight;
  double Muon_RECO2nd_WeightUp;
  double Muon_RECO2nd_WeightDown;

  double Muon_Trig_Weight;
  double Muon_Trig_WeightUp;
  double Muon_Trig_WeightDown;

  double HEEP_SF        ;
  double HEEP_SF_Up     ;
  double HEEP_SF_Down   ;
  double HEEP_SF_E      ;
  double HEEP_SF_B      ;
  double egamma_SF      ;
  double egamma_SF_Up   ;
  double egamma_SF_Down ;
  double egamma_SF_HLT      ;
  double egamma_SF_HLT_Up   ;
  double egamma_SF_HLT_Down ;

  std::vector<double>   PDFWeights_Scale;
  std::vector<double> PDFWeights_Error;
  std::vector<double> PDFWeights_AlphaS;


  double HEEP_SF_noISO        ;
  double HEEP_SF_E_noISO      ;
  double HEEP_SF_B_noISO      ;
  double egamma_SF_noISO      ;

  double myGenLSF;

  double wrShellMass;

  //GEN Z
  double genZmass;
  double genZpt;

  bool boostedRECOmassAbove600;
  bool boostedFSBRECOmassAbove600;

  //RESOLVED ANA   
  bool resolvedRECOmassAbove600;
  bool resolvedFSBRECOmassAbove600;

  double resJetDR;
  double resolvedRECOmass;
  double resSubleadMuJet1dR;
  double resSubleadMuJet2dR;
  double resLeadMuJet1dR;
  double resLeadMuJet2dR;

  double resolvedRECOmass_MuResolUp;
  double resolvedRECOmass_MuResolDown;

  double resolvedRECOmass_JECUp;
  double resolvedRECOmass_JECDown;
  double resolvedRECOmass_JERUp;
  double resolvedRECOmass_JERDown;

  double resMLL;
  double resolvedSubleadMuPt;

  double resFSBJetDR;
  double resolvedFSBRECOmass; 
  double resFSBElecJet1dR;
  double resFSBElecJet2dR;
  double resFSBMuonJet1dR;
  double resFSBMuonJet2dR;

  double resolvedFSBRECOmass_JECUp;
  double resolvedFSBRECOmass_JECDown;
  double resolvedFSBRECOmass_JERUp;
  double resolvedFSBRECOmass_JERDown;
  double resolvedFSBRECOmass_MuResolUp;
  double resolvedFSBRECOmass_MuResolDown;

  double resFSBElec_pt ;
  double resFSBElec_phi;
  double resFSBElec_eta;

  double resFSBMuon_pt ;
  double resFSBMuon_p ;
  double resFSBMuon_phi;
  double resFSBMuon_eta;



  //GEN THINGS
  double muon1muon2dR;
  double muon1muon2Mass;

  double parton1parton2dR;

  double fatJetMuon1dR;
  double fatJetMuon1dPhi;
  double fatJetMuon2dR;

  double leadAK8JetMuonEtaValGEN;
  double leadAK8JetMuonPtValGEN;
  double leadAK8JetMuonMassValGEN;

  double resolvedGENmass;
  double resolvedGENpt;
  double resolvedGENeta;

  double resSubleadMuParton1dR;
  double resSubleadMuParton2dR;
  double resLeadMuParton1dR;
  double resLeadMuParton2dR;

  double minPartonMuon2dR;

  //HEEP THINGS
  double selElectron_barrel_dEtaInSeed      ; 
  double selElectron_barrel_dPhiIn          ; 
  double selElectron_barrel_HoverE          ; 
  double selElectron_barrel_sig_ietaieta_5x5;
  double selElectron_barrel_E2x5vE5x5       ; 
  double selElectron_barrel_EM_had_depIso   ; 
  double selElectron_barrel_trackIso        ; 
  double selElectron_barrel_trackPnt        ; 
  double selElectron_barrel_innerLostHits   ; 
  double selElectron_barrel_dxy             ; 

  double selElectron_endcap_dEtaInSeed      ; 
  double selElectron_endcap_dPhiIn          ; 
  double selElectron_endcap_HoverE          ; 
  double selElectron_endcap_sig_ietaieta_5x5;
  double selElectron_endcap_E2x5vE5x5       ; 
  double selElectron_endcap_EM_had_depIso   ; 
  double selElectron_endcap_trackIso        ; 
  double selElectron_endcap_trackPnt        ; 
  double selElectron_endcap_innerLostHits   ; 
  double selElectron_endcap_dxy             ; 

  double selElectron_noISO_barrel_dEtaInSeed      ; 
  double selElectron_noISO_barrel_dPhiIn          ; 
  double selElectron_noISO_barrel_HoverE          ; 
  double selElectron_noISO_barrel_sig_ietaieta_5x5;
  double selElectron_noISO_barrel_E2x5vE5x5       ; 
  double selElectron_noISO_barrel_EM_had_depIso   ; 
  double selElectron_noISO_barrel_trackIso        ; 
  double selElectron_noISO_barrel_trackPnt        ; 
  double selElectron_noISO_barrel_innerLostHits   ; 
  double selElectron_noISO_barrel_dxy             ; 

  double selElectron_noISO_endcap_dEtaInSeed      ; 
  double selElectron_noISO_endcap_dPhiIn          ; 
  double selElectron_noISO_endcap_HoverE          ; 
  double selElectron_noISO_endcap_sig_ietaieta_5x5;
  double selElectron_noISO_endcap_E2x5vE5x5       ; 
  double selElectron_noISO_endcap_EM_had_depIso   ; 
  double selElectron_noISO_endcap_trackIso        ; 
  double selElectron_noISO_endcap_trackPnt        ; 
  double selElectron_noISO_endcap_innerLostHits   ; 
  double selElectron_noISO_endcap_dxy             ; 

  double secondMuonWRjetdR;


  double parton1EtVal;
  double parton2EtVal;
  double muon1EtVal;
  double muon2EtVal;
  double firstPartonJetEtVal;
  double secondPartonJetEtVal;
  double firstPartonAK8JetEtVal;
  double secondPartonAK8JetEtVal;

  double parton1EtaVal;
  double parton2EtaVal;
  double muon1EtaVal;
  double muon2EtaVal;
  double firstPartonJetEtaVal;
  double secondPartonJetEtaVal;
  double firstPartonAK8JetEtaVal;
  double secondPartonAK8JetEtaVal;

  double parton1PhiVal;
  double parton2PhiVal;
  double muon1PhiVal;
  double muon2PhiVal;
  double firstPartonJetPhiVal;
  double secondPartonJetPhiVal;
  double firstPartonAK8JetPhiVal;
  double secondPartonAK8JetPhiVal;

  double dRparton1parton2Val;
  double dRmuon1muon2Val;
  double dRparton1muon2Val;
  double dRparton1muon1Val;
  double dRparton2muon2Val;
  double dRparton2muon1Val;
  double dRparton1jetVal;
  double dRparton2jetVal;
  double dRparton1AK8jetVal;
  double dRparton2AK8jetVal;

  double firstPartonJetEtHadronicVal;
  double secondPartonJetEtHadronicVal;
  double firstPartonJetEtEMVal;
  double secondPartonJetEtEMVal;
  double firstPartonJetEtInvisibleVal;
  double secondPartonJetEtInvisibleVal;

  double firstPartonAK8JetEtHadronicVal;
  double secondPartonAK8JetEtHadronicVal;
  double firstPartonAK8JetEtEMVal;
  double secondPartonAK8JetEtEMVal;
  double firstPartonAK8JetEtInvisibleVal;
  double secondPartonAK8JetEtInvisibleVal;

  double leadSubleadingAK8JetsMuonsMassVal;
  double leadSubleadingJetsMuonsMassVal;
  double leadSubleadingPartonsMuonsMassVal;
  double leadAK8JetMuonMassVal;
  double leadAK8JetMuonMassVal_noLSF;

  double leadAK8JetDiMuonMassVal;
  double leadAK8JetElectronMassVal;
  double leadAK8JetElectronMassVal_JECUp;
  double leadAK8JetElectronMassVal_JECDown;
  double leadAK8JetElectronMassVal_JERUp;
  double leadAK8JetElectronMassVal_JERDown;
  double leadAK8JetElectronMassVal_noISO;

  double leadAK8JetMuonMassVal_JECUp;
  double leadAK8JetMuonMassVal_JECDown;
  double leadAK8JetMuonMassVal_noLSF_JECUp;
  double leadAK8JetMuonMassVal_noLSF_JECDown;
  double leadAK8JetMuonPtVal_JECUp;
  double leadAK8JetMuonPtVal_JECDown;
  double leadAK8JetMuonEtaVal_JECUp;
  double leadAK8JetMuonEtaVal_JECDown;
  double leadAK8JetMuonPhiVal_JECUp;
  double leadAK8JetMuonPhiVal_JECDown;

  double leadAK8JetMuonMassVal_MuResolUp;
  double leadAK8JetMuonMassVal_MuResolDown;
  double leadAK8JetMuonMassVal_noLSF_MuResolUp;
  double leadAK8JetMuonMassVal_noLSF_MuResolDown;

  double leadAK8JetMuonMassVal_JERUp;
  double leadAK8JetMuonMassVal_JERDown;
  double leadAK8JetMuonMassVal_noLSF_JERUp;
  double leadAK8JetMuonMassVal_noLSF_JERDown;
  double leadAK8JetMuonPtVal_JERUp;
  double leadAK8JetMuonPtVal_JERDown;
  double leadAK8JetMuonEtaVal_JERUp;
  double leadAK8JetMuonEtaVal_JERDown;
  double leadAK8JetMuonPhiVal_JERUp;
  double leadAK8JetMuonPhiVal_JERDown;

  double leadSubleadingAK8JetsMuonsPtVal;
  double leadSubleadingJetsMuonsPtVal;
  double leadSubleadingPartonsMuonsPtVal;
  double leadAK8JetMuonPtVal;
  double leadAK8JetElectronPtVal;
  double leadAK8JetElectronPtVal_noISO;

  double leadSubleadingAK8JetsMuonsEtaVal;
  double leadSubleadingJetsMuonsEtaVal;
  double leadSubleadingPartonsMuonsEtaVal;
  double leadAK8JetMuonEtaVal;
  double leadAK8JetElectronEtaVal;
  double leadAK8JetElectronEtaVal_noISO;

  double leadAK8JetMuonDR;
  double leadAK8JetMuonPhiVal;
  double leadAK8JetMuonPhiValGEN;
  double leadAK8JetElectronPhiVal;
  double leadAK8JetElectronPhiVal_noISO;
  double leadAK8JetMuonJetMuonEnergyFraction;
  double leadAK8JetElectronJetMuonEnergyFraction;
  double leadAK8JetElectronJetMuonEnergyFraction_noISO;

  double electronTrigger;
  double muonTrigger;
  
  double selectedElectronPt;
  double selectedElectronPhi;
  double selectedElectronEta;

  double selectedElectron_noISO_Pt;
  double selectedElectron_noISO_Phi;
  double selectedElectron_noISO_Eta;

  double selectedMuonPt;
  double selectedMuonPhi;
  double selectedMuonEta;

  double secondHighPtMuonPt;
  double secondHighPtMuonPhi;
  double secondHighPtMuonEta;
  double selectedJetSecondHighPtMuonDR;

  double selectedJetPt;
  double selectedJetPhi;
  double selectedJetEta;
  double selectedJetMass;
  double selectedJetTau21;
  double selectedJetLSF3;
  double selectedJetMaxSubJetCSV;
  double selectedJetEnergy;
  double selectedJetEnergyUncorr;

  double mydRlsfLep_subleadMuon;

  double secondElecJetDR;
  double secondElecPt;

  double selectedJet_EleNoISO_Pt;
  double selectedJet_EleNoISO_Phi;
  double selectedJet_EleNoISO_Eta;
  double selectedJet_EleNoISO_Mass;
  double selectedJet_EleNoISO_Tau21;

  double selectedJetPt_JECUp;
  double selectedJetPhi_JECUp;
  double selectedJetEta_JECUp;
  double selectedJetMass_JECUp;
  double selectedJetTau21_JECUp;

  double selectedJetPt_JECDown;
  double selectedJetPhi_JECDown;
  double selectedJetEta_JECDown;
  double selectedJetMass_JECDown;
  double selectedJetTau21_JECDown;

  double selectedJetPt_JERUp;
  double selectedJetPhi_JERUp;
  double selectedJetEta_JERUp;
  double selectedJetMass_JERUp;
  double selectedJetTau21_JERUp;

  double selectedJetPt_JERDown;
  double selectedJetPhi_JERDown;
  double selectedJetEta_JERDown;
  double selectedJetMass_JERDown;
  double selectedJetTau21_JERDown;

  double subleadMuonPt;
  double subleadMuonEt;
  double subleadMuonEta;
  double subleadMuonPhi;

  double lsfLeptonPt;
  double lsfLeptonEta;
  double lsfLeptonPhi;

  double dRmuon2;

  double secondGENMuon_selMuondR;
  double subleadMuon_selMuondR;

  double subleadMuon_selJetdPhi_EleNonISO;
  double subleadMuon_selMuondPhi;
  double subleadMuon_selMuonMass;
  double subleadMuon_selMuonPt;
  double subleadMuon_selElectronPhi_EleNonISO;
  double subleadMuon_selElectronMass_EleNonISO;
  double subleadMuon_selElectronPt_EleNonISO;

  double subleadMuon_selJetdPhi;
  double subleadMuon_selElectronPhi;
  double subleadMuon_selElectronMass;
  double subleadMuon_selElectronPt;

  double MET;

  double MET_selJetdPhi;
  double MET_selMuondPhi;
  double MET_selElectrondPhi;

  double MET_selJetMass;
  double MET_selMuonMass;
  double MET_selElectronMass;

  double MET_selJetPt;
  double MET_selMuonPt;
  double MET_selElectronPt;

  double selectedJetTransMET;

  double myEventMass;
  double myEventSidebandMass;

  double myEventMass_JECUp;
  double myEventMass_JECDown;
  double myEventMass_JERUp;
  double myEventMass_JERDown;

private:



};

















#endif
