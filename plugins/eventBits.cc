#include "eventBits.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include <vector>

eventBits::eventBits() {
  run = -1;
  lumi = -1;
  event = 0;
  highestEtMuon         = -1;
  secondHighestEtMuon   = -1;
  highestEtParton       = -1;
  secondHighestEtParton = -1;
  firstPartonGenJet     = -1;
  secondPartonGenJet    = -1;




}
void eventBits::clear() {
  run = -1;
  lumi = -1;
  event = 0;
  highestEtMuon         = -1;
  secondHighestEtMuon   = -1;
  highestEtParton       = -1;
  secondHighestEtParton = -1;
  firstPartonGenJet     = -1;
  secondPartonGenJet    = -1;

  outgoingPartons.clear();
  outgoingMuons.clear();
  genJets.clear();
  selectedMuons.clear();
  MCmatchedMuons.clear();






}
bool eventBits::passesGenCuts() {

  if(highestEtMuon         == -1 || 
     secondHighestEtMuon   == -1 ||
     highestEtParton       == -1 ||
     secondHighestEtParton == -1) return false;
    
  if(getHighestEtMuon()->et() <= 20)         return false;
  if(getSecondHighestEtMuon()->et() <= 20)   return false;
  if(getHighestEtParton()->et() <= 20)       return false;
  if(getSecondHighestEtParton()->et() <= 20) return false;

  return true;


}
//indices in collections
const reco::GenParticle* eventBits::getHighestEtMuon() {
  if (highestEtMuon != -1) return &(outgoingMuons[highestEtMuon]);
  return 0;
}
const reco::GenParticle* eventBits::getSecondHighestEtMuon() {
  if (secondHighestEtMuon != -1) return &(outgoingMuons[secondHighestEtMuon]);
  return 0;
}
const reco::GenParticle* eventBits::getHighestEtParton() {
  if (highestEtParton != -1) return &(outgoingPartons[highestEtParton]);
  return 0;
}
const reco::GenParticle* eventBits::getSecondHighestEtParton() {
  if (secondHighestEtParton != -1) return &(outgoingPartons[secondHighestEtParton]);
  return 0;
}
const reco::GenJet* eventBits::getFirstPartonGenJet() {   //deltaR MATCHED
  if (firstPartonGenJet != -1) return &(genJets[firstPartonGenJet]);
  return 0;
}
const reco::GenJet* eventBits::getSecondPartonGenJet() {
  if (secondPartonGenJet != -1) return &(genJets[secondPartonGenJet]);
  return 0;
}

void eventBits::setHighestEtMuon(uint32_t index) {
  highestEtMuon = index;
}
void eventBits::setSecondHighestEtMuon(uint32_t index) {
  secondHighestEtMuon = index;
}

void eventBits::setHighestEtParton(uint32_t index) {
  highestEtParton = index;
}
void eventBits::setSecondHighestEtParton(uint32_t index) {
  secondHighestEtParton = index;
}

void eventBits::setFirstPartonGenJet(uint32_t index) {    //deltaR MATCHED
  firstPartonGenJet = index;
}
void eventBits::setSecondPartonGenJet(uint32_t index) {
  secondPartonGenJet = index;
}

