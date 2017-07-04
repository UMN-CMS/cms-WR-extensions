#include "tools.h"


namespace wrTools {
  
  bool compareEtPointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2) {
    if ( particle1->et() > particle2->et() ) return true;
    return false;
  }
  bool compareEt(reco::GenParticle particle1, reco::GenParticle particle2) {
    if ( particle1.et() > particle2.et() ) return true;
    return false;
  }
  bool compareEtJetPointer(const reco::GenJet* jet1, const reco::GenJet* jet2) {
    if (jet1->et() > jet2->et() ) return true;
    return false;
  }
  bool particleIsFrom(const reco::Candidate* particle, int pdgId) {
    for(unsigned int iMom = 0; iMom < particle->numberOfMothers(); iMom++) {//loop over mothers
      if(particle->mother(iMom)->pdgId() == pdgId) return true;
      if(particle->mother(iMom)->pdgId() == particle->pdgId()) return particleIsFrom(particle->mother(iMom), pdgId);
    }
    return false;
  }
  bool particleIsFromABS(const reco::Candidate* particle, unsigned int pdgId) {
    for(unsigned int iMom = 0; iMom < particle->numberOfMothers(); iMom++) {//loop over mothers
      if(abs(particle->mother(iMom)->pdgId()) == pdgId) return true;
      if(particle->mother(iMom)->pdgId() == particle->pdgId()) return particleIsFromABS(particle->mother(iMom), pdgId);
    }
    return false;
  }
}
