#include "ExoAnalysis/cmsWRextensions/interface/tools.h"


namespace wrTools {
  
  bool compareEtGenParticlePointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2) {
    if ( particle1->et() > particle2->et() ) return true;
    return false;
  }
  bool compareEtGenParticle(reco::GenParticle particle1, reco::GenParticle particle2) {
    if ( particle1.et() > particle2.et() ) return true;
    return false;
  }
  bool compareEtJetPointer(const reco::GenJet* jet1, const reco::GenJet* jet2) {
    if (jet1->et() > jet2->et() ) return true;
    return false;
  }
  bool compareEtCandidatePointer(const reco::Candidate* cand1, const reco::Candidate* cand2) { 
    if ( cand1->et() > cand2->et() ) return true;
    return false;
  }
  bool comparePairMassPointer(std::pair< const reco::Candidate*, const reco::Candidate* > pair1, std::pair< const reco::Candidate*, const reco::Candidate* > pair2) {
    if ( (pair1.first->p4() + pair1.second->p4()).mass() > (pair2.first->p4() + pair2.second->p4()).mass() )  return true;
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
  bool particleInGenJet(const reco::Candidate* particle, const reco::GenJet* jet) {
    //loop over jet candidates
    std::vector<const reco::Candidate*> constituents = jet->getJetConstituentsQuick();
    for(std::vector<const reco::Candidate*>::const_iterator iConst = constituents.begin(); iConst != constituents.end(); iConst++) {
      if((*iConst)->pdgId() != particle->pdgId()) continue; //WRONG PDGID
      if(abs((*iConst)->pt() - particle->pt()) >= 1.0) continue; //PT NOT CLOSE ENOUGH
      
      std::cout << "Found daughter with PT: "<<(*iConst)->pt() <<" compared to: "<<particle->pt() << std::endl;
      return true;
    }
    



    return false;
  }
  int characterizeEvent(std::vector<const reco::GenParticle*> myGenParticles) {
    

    return 0;
  }
}
