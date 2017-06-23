#include "eventBits.h"

eventBits::eventBits() {
  run = -1;
  lumi = -1;
  event = 0;
  parton1EtVal = 0.0;
  parton2EtVal = 0.0;
  muonHighestEtVal = 0.0;
  muonSecondHighestEtVal = 0.0;

  parton1EtaVal = 0.0;
  parton2EtaVal = 0.0;
  muonHighestEtEtaVal = 0.0;
  muonSecondHighestEtEtaVal = 0.0;

  dRparton1parton2Val = 0.0;
  dRmuon1muon2Val = 0.0;
  dRparton1muon2Val = 0.0;
  dRparton1muon1Val = 0.0;
  dRparton2muon2Val = 0.0;
  dRparton2muon1Val = 0.0;

  firstPartonJetEtTotalVal = 0.0;
  secondPartonJetEtTotalVal = 0.0;
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





}
void eventBits::clear() {
  run = -1;
  lumi = -1;
  event = 0;
  parton1EtVal = 0.0;
  parton2EtVal = 0.0;
  muonHighestEtVal = 0.0;
  muonSecondHighestEtVal = 0.0;

  parton1EtaVal = 0.0;
  parton2EtaVal = 0.0;
  muonHighestEtEtaVal = 0.0;
  muonSecondHighestEtEtaVal = 0.0;

  dRparton1parton2Val = 0.0;
  dRmuon1muon2Val = 0.0;
  dRparton1muon2Val = 0.0;
  dRparton1muon1Val = 0.0;
  dRparton2muon2Val = 0.0;
  dRparton2muon1Val = 0.0;

  firstPartonJetEtTotalVal = 0.0;
  secondPartonJetEtTotalVal = 0.0;
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







}
bool eventBits::passesGenCuts() {

  if(muonHighestEtVal <= 20)         return false;
  if(muonSecondHighestEtVal <= 20)   return false;
  if(parton1EtVal <= 20)             return false;
  if(parton2EtVal <= 20)             return false;

  return true;


}
