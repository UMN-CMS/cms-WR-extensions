#include "ExoAnalysis/cms-WR-extensions/interface/eventBits.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"

eventBits::eventBits() {
  run = -1;
  lumi = -1;
  event = 0;
  weight = 1.0;
  passesWR2016 = false;

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

  leadAK8JetMuonJetMuonEnergyFraction = -10000.0;

}
void eventBits::clear() {
  run = -1;
  lumi = -1;
  event = 0;

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



}
bool eventBits::passesGenCuts() {

  if(muon1EtVal <= 20)               return false;
  if(muon2EtVal <= 20)               return false;
  if(parton1EtVal <= 20)             return false;
  if(parton2EtVal <= 20)             return false;

  return true;


}
