#include "ExoAnalysis/cmsWRextensions/interface/eventBits.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"

eventBits::eventBits() {
  run = -1;
  lumi = -1;
  event = 0;
  weight = 1.0;
  passesWR2016 = false;

  mySubleadMuon = NULL;
  myElectronCand = NULL;
  myMuonCand = NULL;

  count = 0;

  cutProgress = -1;
  FSBcutProgress = -1;

  secondInDecayMuon = -10000.0;

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
  leadAK8JetMuonPtVal = -10000.0;
  leadAK8JetMuonEtaVal = -10000.0;

  leadAK8JetElectronMassVal = -10000.0;
  leadAK8JetElectronPtVal = -10000.0;
  leadAK8JetElectronEtaVal = -10000.0;

  leadAK8JetMuonJetMuonEnergyFraction = -10000.0;

  leadAK8JetElectronJetMuonEnergyFraction = -10000.0;

  electronTrigger = -10000.0;
  muonTrigger = -10000.0;

  selectedMuonEt = -10000.0;
  selectedMuonPhi = -10000.0;
  selectedMuonEta = -10000.0;

  selectedElectronEt = -10000.0;
  selectedElectronPhi = -10000.0;
  selectedElectronEta = -10000.0;

  selectedJetEt = -10000.0;
  selectedJetPhi = -10000.0;
  selectedJetEta = -10000.0;

  subleadMuonEt = -10000.0;
  subleadMuonEta = -10000.0;
  subleadMuonPhi = -10000.0;

  subleadMuon_selJetdPhi = -10000.0;
  subleadMuon_selMuondPhi = -10000.0;
  subleadMuon_selMuonMass = -10000.0;
  subleadMuon_selMuonPt = -10000.0;

  subleadMuon_selElectronPhi = -10000.0;
  subleadMuon_selElectronMass = -10000.0;
  subleadMuon_selElectronPt = -10000.0;

  muons10            = -1;

  mynLeptons         = -1;
  mynMuons           = -1;
  mynTaus            = -1;
  mynElectrons       = -1;
  mynLightPartons    = -1;
  mynTops            = -1;
  mynBs              = -1;
  mynPartons         = -1;

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
}
void eventBits::clear() {
  run = -1;
  lumi = -1;
  event = 0;

  count = 0;
  
  cutProgress = 0;
  FSBcutProgress = 0;

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
  leadAK8JetMuonPtVal = 0.0;
  leadAK8JetMuonEtaVal = 0.0;

  leadAK8JetElectronMassVal = 0.0;
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
bool eventBits::passesGenCuts() {

  if(muon1EtVal <= 20)               return false;
  if(muon2EtVal <= 20)               return false;
  if(parton1EtVal <= 20)             return false;
  if(parton2EtVal <= 20)             return false;

  return true;


}
