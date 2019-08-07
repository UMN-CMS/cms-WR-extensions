#include "ExoAnalysis/cmsWRextensions/interface/eventBits.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"

eventBits::eventBits() {
  run = -1;
  lumi = -1;
  event = 0;
  m_flavor = 0;
  weight = 1.0;
  FSBweight = 1.0;
  FSBweight_noISO = 1.0;
  passesWR2016 = false;
  nVtx = -1;
  nPU = -1;
  nPUmean = -1;

  //CUTFLOW BOOLS
  passPreSelectGen = false;
  passResGen       = false;
  passGenCounter   = false;

  mySubleadMuon = NULL;
  myElectronCand = NULL;
  myMuonCand = NULL;
  my2ndHighPtMuonCand = NULL;

  genSecondMuon = NULL;

  firstMuon = NULL;
  secondMuon = NULL;

  firstElectron = NULL;
  secondElectron = NULL;

  resFSBMuon = NULL;
  resFSBElec = NULL;

  NR = NULL;
  WR = NULL;

  myZ = NULL;

  muonTrigPassBit = false;

  secondElectronCand = NULL;

  PVertex = NULL;
  genVtx = NULL;
  myVertexDiff = NULL;

  daughterClusterVector = math::XYZTLorentzVector(0,0,0,0);

  count = 0;

  NresolvedANAMuonCands = -10000.0;
  NresolvedANAElectronCands = -10000.0;
  NresolvedANAFSBLeptonCands = -10000.0;

  cutProgress = -1;
  ResCutProgress = -1;
  ResFSBCutProgress = -1;
  ResElecCutProgress = -1;
  FSBcutProgress = -1;
  ResFSBCutProgress = -1;


  nSecondElectronCands = -10000.0;

  egamma_SF = 1.0;
  egamma_SF_Up = -10000.0;
  egamma_SF_Down = -10000.0;

  puWeight = 1.0;
  puWeight_Up = -10000.0;
  puWeight_Down = -10000.0;

  _prefiringweight = 1.0;
  _prefiringweightup = -10000.0;
  _prefiringweightdown = -10000.0;

  Muon_HighPtID_Weight = 1.0;
  Muon_HighPtID_WeightUp = -10000.0;
  Muon_HighPtID_WeightDown = -10000.0;

  Muon_HighPtID2nd_Weight = 1.0;
  Muon_HighPtID2nd_WeightUp = -10000.0;
  Muon_HighPtID2nd_WeightDown = -10000.0;

  Muon_LooseID_Weight = 1.0;
  Muon_LooseID_WeightUp = -10000.0;
  Muon_LooseID_WeightDown = -10000.0;

  Muon_LooseTkIso_Weight = 1.0;
  Muon_LooseTkIso_WeightUp = -10000.0;
  Muon_LooseTkIso_WeightDown = -10000.0;

  Muon_LooseTkIso2nd_Weight = 1.0;
  Muon_LooseTkIso2nd_WeightUp = -10000.0;
  Muon_LooseTkIso2nd_WeightDown = -10000.0;

  Muon_Trig_Weight = 1.0;
  Muon_Trig_WeightUp = -10000.0;
  Muon_Trig_WeightDown = -10000.0;

  HEEP_SF  = 1.0;
  HEEP_SF_Up  = -10000.0;
  HEEP_SF_Down  = -10000.0;
  HEEP_SF_E  = -10000.0;
  HEEP_SF_B  = -10000.0;

  HEEP_SF_noISO  = 1.0;
  HEEP_SF_E_noISO  = -10000.0;
  HEEP_SF_B_noISO  = -10000.0;

  secondInDecayMuon = -10000.0;

  mydRlsfLep_subleadMuon = -10000.0;
  //
  genZmass = -10000.0;
  genZpt   = -10000.0;

  boostedRECOmassAbove600 = false;
  boostedFSBRECOmassAbove600 = false;

  //RES ANALYSIS
  resolvedRECOmassAbove600 = false;
  resolvedFSBRECOmassAbove600 = false;

  resJetDR           = -10000.0; 
  resolvedRECOmass   = -10000.0; 
  resSubleadMuJet1dR = -10000.0; 
  resSubleadMuJet2dR = -10000.0;  
  resLeadMuJet1dR = -10000.0; 
  resLeadMuJet2dR = -10000.0;  

  resolvedRECOmass_MuResolUp = -10000.0;
  resolvedRECOmass_MuResolDown = -10000.0;

  resolvedRECOmass_JECUp   = -10000.0;
  resolvedRECOmass_JECDown   = -10000.0;
  resolvedRECOmass_JERUp   = -10000.0;
  resolvedRECOmass_JERDown   = -10000.0;

  resolvedFSBRECOmass = -10000.0;
  resolvedFSBRECOmass_JECUp = -10000.0;
  resolvedFSBRECOmass_JECDown = -10000.0;
  resolvedFSBRECOmass_JERUp = -10000.0;
  resolvedFSBRECOmass_JERDown = -10000.0;
  resolvedFSBRECOmass_MuResolUp = -10000.0;
  resolvedFSBRECOmass_MuResolDown = -10000.0;

  resFSBElec_pt  = -10000.0;
  resFSBElec_phi = -10000.0;
  resFSBElec_eta = -10000.0;

  resFSBMuon_pt  = -10000.0;
  resFSBMuon_phi = -10000.0;
  resFSBMuon_eta = -10000.0;

  nResFSBMuons = -10000.0;

  resMLL             = -10000.0;
  resolvedSubleadMuPt= -10000.0;

  //GEN THINGS
  muon1muon2dR               = -10000.0;
  parton1parton2dR           = -10000.0;
  muon1muon2Mass             = -10000.0;

  fatJetMuon1dR              = -10000.0;
  fatJetMuon1dPhi            = -10000.0;
  fatJetMuon2dR              = -10000.0;

  leadAK8JetMuonEtaValGEN    = -10000.0;
  leadAK8JetMuonPtValGEN     = -10000.0;
  leadAK8JetMuonMassValGEN   = -10000.0;

  resolvedGENmass            = -10000.0;
  resolvedGENpt              = -10000.0;
  resolvedGENeta             = -10000.0;

  resSubleadMuParton1dR      = -10000.0;
  resSubleadMuParton2dR      = -10000.0;
  resLeadMuParton1dR         = -10000.0;
  resLeadMuParton2dR         = -10000.0;
  
  minPartonMuon2dR           = -10000.0;

  parton1EtVal = -10000.0;
  parton2EtVal = -10000.0;
  muon1EtVal = -10000.0;
  muon2EtVal = -10000.0;
  firstPartonJetEtVal = -10000.0;
  secondPartonJetEtVal = -10000.0;
  firstPartonAK8JetEtVal = -10000.0;
  secondPartonAK8JetEtVal = -10000.0;

  parton1EtaVal = -10000.0;
  parton2EtaVal = -10000.0;
  muon1EtaVal = -10000.0;
  muon2EtaVal = -10000.0;
  firstPartonJetEtaVal = -10000.0;
  secondPartonJetEtaVal = -10000.0;
  firstPartonAK8JetEtaVal = -10000.0;
  secondPartonAK8JetEtaVal = -10000.0;

  parton1PhiVal = -10000.0;
  parton2PhiVal = -10000.0;
  muon1PhiVal = -10000.0;
  muon2PhiVal = -10000.0;
  firstPartonJetPhiVal = -10000.0;
  secondPartonJetPhiVal = -10000.0;
  firstPartonAK8JetPhiVal = -10000.0;
  secondPartonAK8JetPhiVal = -10000.0;
 
  secondElecJetDR = -10000.0;
  secondElecPt    = -10000.0;

  dRparton1parton2Val = -10000.0;
  dRmuon1muon2Val = -10000.0;
  dRparton1muon2Val = -10000.0;
  dRparton1muon1Val = -10000.0;
  dRparton2muon2Val = -10000.0;
  dRparton2muon1Val = -10000.0;
  dRparton1jetVal = -10000.0;
  dRparton2jetVal = -10000.0;
  dRparton1AK8jetVal = -10000.0;
  dRparton2AK8jetVal = -10000.0;

  secondMuonWRjetdR = -100000.0;

  myGenLSF = -10000.0;

  neutrinoDecays = -10000.0;

  firstPartonJetEtHadronicVal = -10000.0;
  secondPartonJetEtHadronicVal = -10000.0;
  firstPartonJetEtEMVal = -10000.0;
  secondPartonJetEtEMVal = -10000.0;
  firstPartonJetEtInvisibleVal = -10000.0;
  secondPartonJetEtInvisibleVal = -10000.0;

  leadSubleadingJetsMuonsMassVal = -10000.0;
  leadSubleadingPartonsMuonsMassVal = -10000.0;

  leadSubleadingJetsMuonsPtVal = -10000.0;
  leadSubleadingPartonsMuonsPtVal = -10000.0;

  leadSubleadingJetsMuonsEtaVal = -10000.0;
  leadSubleadingPartonsMuonsEtaVal = -10000.0;

  firstPartonAK8JetEtHadronicVal = -10000.0;
  secondPartonAK8JetEtHadronicVal = -10000.0;
  firstPartonAK8JetEtEMVal = -10000.0;
  secondPartonAK8JetEtEMVal = -10000.0;
  firstPartonAK8JetEtInvisibleVal = -10000.0;
  secondPartonAK8JetEtInvisibleVal = -10000.0;

  leadSubleadingAK8JetsMuonsMassVal = -10000.0;
  leadSubleadingAK8JetsMuonsPtVal = -10000.0;
  leadSubleadingAK8JetsMuonsEtaVal = -10000.0;

  leadAK8JetMuonMassVal = -10000.0;
  leadAK8JetMuonMassVal_JECUp = -10000.0;
  leadAK8JetMuonMassVal_JECDown = -10000.0;
  leadAK8JetMuonMassVal_JERUp = -10000.0;
  leadAK8JetMuonMassVal_JERDown = -10000.0;
  leadAK8JetMuonPtVal = -10000.0;
  leadAK8JetMuonMassVal_MuResolUp = -10000.0;
  leadAK8JetMuonMassVal_MuResolDown = -10000.0;
  leadAK8JetMuonMassVal_noLSF_MuResolUp = -10000.0;
  leadAK8JetMuonMassVal_noLSF_MuResolDown = -10000.0;
  leadAK8JetMuonEtaVal = -10000.0;

  leadAK8JetElectronMassVal = -10000.0;
  leadAK8JetElectronMassVal_JECUp = -10000.0;
  leadAK8JetElectronMassVal_JECDown = -10000.0;
  leadAK8JetElectronMassVal_JERUp = -10000.0;
  leadAK8JetElectronMassVal_JERDown = -10000.0;
  leadAK8JetElectronPtVal = -10000.0;
  leadAK8JetElectronEtaVal = -10000.0;

  leadAK8JetElectronMassVal_noISO = -10000.0;
  leadAK8JetElectronPtVal_noISO = -10000.0;
  leadAK8JetElectronEtaVal_noISO = -10000.0;

  leadAK8JetMuonJetMuonEnergyFraction = -10000.0;

  leadAK8JetElectronJetMuonEnergyFraction = -10000.0;
  leadAK8JetElectronJetMuonEnergyFraction_noISO = -10000.0;

  electronTrigger = -10000.0;
  muonTrigger = -10000.0;

  selectedMuonPt = -10000.0;
  selectedMuonPhi = -10000.0;
  selectedMuonEta = -10000.0;

  secondHighPtMuonPt = -10000.0;
  secondHighPtMuonPhi = -10000.0;
  secondHighPtMuonEta = -10000.0;
  selectedJetSecondHighPtMuonDR = -10000.0;

  selectedElectronPt = -10000.0;
  selectedElectronPhi = -10000.0;
  selectedElectronEta = -10000.0;

  selectedJetPt = -10000.0;
  selectedJetPhi = -10000.0;
  selectedJetEta = -10000.0;
  selectedJetMass= -10000.0;
  selectedJetTau21= -10000.0;
  selectedJetLSF3= -10000.0;
  selectedJetMaxSubJetCSV= -10000.0;
  selectedJetEnergy = -10000.0;
  selectedJetEnergyUncorr = -10000.0;

  selectedElectron_noISO_Pt = -10000.0;
  selectedElectron_noISO_Phi = -10000.0;
  selectedElectron_noISO_Eta = -10000.0;

  selectedJet_EleNoISO_Pt = -10000.0;
  selectedJet_EleNoISO_Phi = -10000.0;
  selectedJet_EleNoISO_Eta = -10000.0;
  selectedJet_EleNoISO_Mass= -10000.0;
  selectedJet_EleNoISO_Tau21= -10000.0;

  subleadMuonPt = -10000.0;
  subleadMuonEt = -10000.0;
  subleadMuonEta = -10000.0;
  subleadMuonPhi = -10000.0;

  lsfLeptonPt = -100000.0;
  lsfLeptonEta = -100000.0;
  lsfLeptonPhi = -100000.0;

  subleadMuon_selJetdPhi = -10000.0;
  subleadMuon_selMuondPhi = -10000.0;
  subleadMuon_selMuonMass = -10000.0;
  subleadMuon_selMuonPt = -10000.0;

  subleadMuon_selMuondR = -10000.0;
  secondGENMuon_selMuondR = -10000.0;

  subleadMuon_selElectronPhi = -10000.0;
  subleadMuon_selElectronMass = -10000.0;
  subleadMuon_selElectronPt = -10000.0;

  dRmuon2 = -10000.0;

  muons10            = -1;

  mynLeptons         = -1;
  mynMuons           = -1;
  mynTaus            = -1;
  mynElectrons       = -1;
  mynLightPartons    = -1;
  mynTops            = -1;
  mynBs              = -1;
  mynPartons         = -1;
  nAdditionalHEEP    = -1;
  nHighPtMuonsOutsideJet = -1;

  NRenergy = -10000.0;

  myEventFlavor      = -1;

  MET = -10000.0;
  MET_selJetdPhi = -10000.0;
  MET_selMuondPhi = -10000.0;
  MET_selElectrondPhi = -10000.0;
  MET_selJetMass = -10000.0;
  MET_selMuonMass = -10000.0;
  MET_selElectronMass = -10000.0;
  MET_selJetPt = -10000.0;
  MET_selMuonPt = -10000.0;
  MET_selElectronPt = -10000.0;

  selectedJetTransMET = -10000.0;

  myEventMass = -10000.0;
  //HEEP THINGS
  selElectron_barrel_dEtaInSeed       = -10000.0; 
  selElectron_barrel_dPhiIn           = -10000.0; 
  selElectron_barrel_HoverE           = -10000.0; 
  selElectron_barrel_sig_ietaieta_5x5 = -10000.0;
  selElectron_barrel_E2x5vE5x5        = -10000.0; 
  selElectron_barrel_EM_had_depIso    = -10000.0; 
  selElectron_barrel_trackIso         = -10000.0; 
  selElectron_barrel_trackPnt         = -10000.0; 
  selElectron_barrel_innerLostHits    = -10000.0; 
  selElectron_barrel_dxy              = -10000.0; 

  selElectron_endcap_dEtaInSeed       = -10000.0; 
  selElectron_endcap_dPhiIn           = -10000.0; 
  selElectron_endcap_HoverE           = -10000.0; 
  selElectron_endcap_sig_ietaieta_5x5 = -10000.0;
  selElectron_endcap_E2x5vE5x5        = -10000.0; 
  selElectron_endcap_EM_had_depIso    = -10000.0; 
  selElectron_endcap_trackIso         = -10000.0; 
  selElectron_endcap_trackPnt         = -10000.0; 
  selElectron_endcap_innerLostHits    = -10000.0; 
  selElectron_endcap_dxy              = -10000.0; 

  selElectron_noISO_barrel_dEtaInSeed       = -10000.0; 
  selElectron_noISO_barrel_dPhiIn           = -10000.0; 
  selElectron_noISO_barrel_HoverE           = -10000.0; 
  selElectron_noISO_barrel_sig_ietaieta_5x5 = -10000.0;
  selElectron_noISO_barrel_E2x5vE5x5        = -10000.0; 
  selElectron_noISO_barrel_EM_had_depIso    = -10000.0; 
  selElectron_noISO_barrel_trackIso         = -10000.0; 
  selElectron_noISO_barrel_trackPnt         = -10000.0; 
  selElectron_noISO_barrel_innerLostHits    = -10000.0; 
  selElectron_noISO_barrel_dxy              = -10000.0; 

  selElectron_noISO_endcap_dEtaInSeed       = -10000.0; 
  selElectron_noISO_endcap_dPhiIn           = -10000.0; 
  selElectron_noISO_endcap_HoverE           = -10000.0; 
  selElectron_noISO_endcap_sig_ietaieta_5x5 = -10000.0;
  selElectron_noISO_endcap_E2x5vE5x5        = -10000.0; 
  selElectron_noISO_endcap_EM_had_depIso    = -10000.0; 
  selElectron_noISO_endcap_trackIso         = -10000.0; 
  selElectron_noISO_endcap_trackPnt         = -10000.0; 
  selElectron_noISO_endcap_innerLostHits    = -10000.0; 
  selElectron_noISO_endcap_dxy              = -10000.0; 
}
void eventBits::clear() {
  run = -1;
  lumi = -1;
  event = 0;
  nVtx = -1;
  nPU = -1;
  nPUmean = -1;
  FSBweight = 1.0;

  count = 0;
  
  ResCutProgress = 0;
  cutProgress = 0;
  FSBcutProgress = 0;

  egamma_SF = 1.0;
  egamma_SF_Up = -10000.0;
  egamma_SF_Down = -10000.0;

  puWeight = 1.0;
  puWeight_Up = 0.0;
  puWeight_Down = 0.0;

  Muon_HighPtID_Weight = 1.0;
  Muon_HighPtID_WeightUp = 0.0;
  Muon_HighPtID_WeightDown = 0.0;

  Muon_LooseID_Weight = 1.0;
  Muon_LooseID_WeightUp = 0.0;
  Muon_LooseID_WeightDown = 0.0;

  HEEP_SF = 1.0;
  HEEP_SF_Up = 0.0;
  HEEP_SF_Down = 0.0;
  HEEP_SF_E  = 0.0;
  HEEP_SF_B  = 0.0;

  parton1EtVal = 0.0;
  parton2EtVal = 0.0;
  muon1EtVal = 0.0;
  muon2EtVal = 0.0;
  firstPartonJetEtVal = 0.0;
  secondPartonJetEtVal = 0.0;
  firstPartonAK8JetEtVal = 0.0;
  secondPartonAK8JetEtVal = 0.0;

  parton1EtaVal = 0.0;
  parton2EtaVal = 0.0;
  muon1EtaVal = 0.0;
  muon2EtaVal = 0.0;
  firstPartonJetEtaVal = 0.0;
  secondPartonJetEtaVal = 0.0;
  firstPartonAK8JetEtaVal = 0.0;
  secondPartonAK8JetEtaVal = 0.0;

  parton1PhiVal = 0.0;
  parton2PhiVal = 0.0;
  muon1PhiVal = 0.0;
  muon2PhiVal = 0.0;
  firstPartonJetPhiVal = 0.0;
  secondPartonJetPhiVal = 0.0;
  firstPartonAK8JetPhiVal = 0.0;
  secondPartonAK8JetPhiVal = 0.0;

  dRparton1parton2Val = 0.0;
  dRmuon1muon2Val = 0.0;
  dRparton1muon2Val = 0.0;
  dRparton1muon1Val = 0.0;
  dRparton2muon2Val = 0.0;
  dRparton2muon1Val = 0.0;
  dRparton1jetVal = 0.0;
  dRparton2jetVal = 0.0;

  firstPartonJetEtHadronicVal = 0.0;
  secondPartonJetEtHadronicVal = 0.0;
  firstPartonJetEtEMVal = 0.0;
  secondPartonJetEtEMVal = 0.0;
  firstPartonJetEtInvisibleVal = 0.0;
  secondPartonJetEtInvisibleVal = 0.0;

  leadSubleadingJetsMuonsMassVal = 0.0;
  leadSubleadingPartonsMuonsMassVal = 0.0;

  leadSubleadingJetsMuonsPtVal = 0.0;
  leadSubleadingPartonsMuonsPtVal = 0.0;

  leadSubleadingJetsMuonsEtaVal = 0.0;
  leadSubleadingPartonsMuonsEtaVal = 0.0;

  firstPartonAK8JetEtHadronicVal = 0.0;
  secondPartonAK8JetEtHadronicVal = 0.0;
  firstPartonAK8JetEtEMVal = 0.0;
  secondPartonAK8JetEtEMVal = 0.0;
  firstPartonAK8JetEtInvisibleVal = 0.0;
  secondPartonAK8JetEtInvisibleVal = 0.0;

  leadSubleadingAK8JetsMuonsMassVal = 0.0;
  leadSubleadingAK8JetsMuonsPtVal = 0.0;
  leadSubleadingAK8JetsMuonsEtaVal = 0.0;

  leadAK8JetMuonMassVal = 0.0;
  leadAK8JetMuonMassVal_JECDown = 0.0;
  leadAK8JetMuonMassVal_JECUp = 0.0;
  leadAK8JetMuonMassVal_JERDown = 0.0;
  leadAK8JetMuonMassVal_JERUp = 0.0;
  leadAK8JetMuonPtVal = 0.0;
  leadAK8JetMuonEtaVal = 0.0;

  leadAK8JetElectronMassVal = 0.0;
  leadAK8JetElectronMassVal_JECUp = 0.0;
  leadAK8JetElectronMassVal_JECDown = 0.0;
  leadAK8JetElectronMassVal_JERUp = 0.0;
  leadAK8JetElectronMassVal_JERDown = 0.0;
  leadAK8JetElectronPtVal = 0.0;
  leadAK8JetElectronEtaVal = 0.0;

  mynLeptons         = 0;
  mynMuons           = 0;
  mynTaus            = 0;
  mynElectrons       = 0;
  mynLightPartons    = 0;
  mynTops            = 0;
  mynBs              = 0;
  mynPartons         = 0;
  nAdditionalHEEP    = 0;

  myEventFlavor      = 0;

  MET = 0.0;
  MET_selJetdPhi = 0.0;
  MET_selMuondPhi = 0.0;
  MET_selElectrondPhi = 0.0;
  MET_selJetMass = 0.0;
  MET_selMuonMass = 0.0;
  MET_selElectronMass = 0.0;
  MET_selJetPt = 0.0;
  MET_selMuonPt = 0.0;
  MET_selElectronPt = 0.0;

 

  selectedJetTransMET = 0.0;

  myEventMass = 0.0;

}

eventBits::~eventBits() {
  delete genVtx;
  delete myVertexDiff;

  for (auto aJet : myAddJetCandsHighPt) { delete aJet; }
  for (auto aJet : myAddJetCandsHighPt_JECUp) { delete aJet; }
  for (auto aJet : myAddJetCandsHighPt_JECDown) { delete aJet; }
  for (auto aJet : myAddJetCandsHighPt_JERUp) { delete aJet; }
  for (auto aJet : myAddJetCandsHighPt_JERDown) { delete aJet; }

}

bool eventBits::passesGenCuts() {

  if(muon1EtVal <= 20)               return false;
  if(muon2EtVal <= 20)               return false;
  if(parton1EtVal <= 20)             return false;
  if(parton2EtVal <= 20)             return false;

  return true;
}
