#include "ExoAnalysis/cmsWRextensions/interface/tools.h"


namespace wrTools {
  
  double smear(double mean, double sigma, int seed) {
    TRandom3* randNumGen = new TRandom3(seed);
    double scale = randNumGen->Gaus(mean, sigma);
    return scale;
  }
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
  bool compareEtCandidatePointerTAddJet(const baconhep::TAddJet* cand1, const baconhep::TAddJet* cand2) {
    if ( cand1->pT > cand2->pT ) return true;
    return false;
  }
  bool compareEtLorentzVectorPointer(const math::XYZTLorentzVector* lv1, const math::XYZTLorentzVector* lv2) {
    if ( lv1->Et() > lv2->Et() ) return true;
    return false;
  }
  bool comparePairMassPointer(std::pair< const reco::Candidate*, const reco::Candidate* > pair1, std::pair< const reco::Candidate*, const reco::Candidate* > pair2) {
    if ( (pair1.first->p4() + pair1.second->p4()).mass() > (pair2.first->p4() + pair2.second->p4()).mass() )  return true;
    return false;
  }
  bool comparePairMassPointerTAddJet(std::pair< const baconhep::TAddJet*, const reco::Candidate* > pair1, std::pair< const baconhep::TAddJet*, const reco::Candidate* > pair2) {
    TLorentzVector *JetVector1_temp = new TLorentzVector();
    JetVector1_temp->SetPtEtaPhiM(pair1.first->pT,pair1.first->eta,pair1.first->phi,pair1.first->SDmass);
    TLorentzVector *JetVector2_temp = new TLorentzVector();
    JetVector2_temp->SetPtEtaPhiM(pair2.first->pT,pair2.first->eta,pair2.first->phi,pair2.first->SDmass);

    math::XYZTLorentzVector JetVector1;
    JetVector1.SetXYZT(JetVector1_temp->X(),JetVector1_temp->Y(),JetVector1_temp->Z(),JetVector1_temp->T());
    math::XYZTLorentzVector JetVector2;
    JetVector2.SetXYZT(JetVector2_temp->X(),JetVector2_temp->Y(),JetVector2_temp->Z(),JetVector2_temp->T());
    if ( (pair1.second->p4() + (JetVector1)).mass() > (pair2.second->p4() + (JetVector2)).mass() )  return true;
    return false;
  }
  bool compareLvPairMassPointer(std::pair< const reco::Candidate*, const math::XYZTLorentzVector* > pair1, std::pair< const reco::Candidate*, const math::XYZTLorentzVector* > pair2) {
    if ( (pair1.first->p4() + *pair1.second).mass() > (pair2.first->p4() + *pair2.second).mass() )  return true;
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

  std::vector<const pat::TriggerObjectStandAlone*> getMatchedObjs(const float eta,const float phi,const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const float maxDeltaR)
  {
    std::vector<const pat::TriggerObjectStandAlone*> matchedObjs;
    const float maxDR2 = maxDeltaR*maxDeltaR;
    for(auto& trigObj : trigObjs){
      const float dR2 = reco::deltaR2(eta,phi,trigObj.eta(),trigObj.phi());
      if(dR2<maxDR2) matchedObjs.push_back(&trigObj);
    }
    return matchedObjs;
  }

  bool checkFilters(const float eta,const float phi,const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const std::vector<std::string>& filterNames,const float maxDeltaR)
  {
    bool passAnyFilter=false;
    const auto matchedObjs = getMatchedObjs(eta,phi,trigObjs,maxDeltaR);
    for(auto& filterName : filterNames){
      for(const auto trigObj : matchedObjs){
    //normally would auto this but to make it clearer for the example
    const std::vector<std::string>& objFilters = trigObj->filterLabels();
    //I dont think filterLabels are sorted so use std::find to see if filterName is in 
    //the list of passed filters for this object
    if(std::find(objFilters.begin(),objFilters.end(),filterName)!=objFilters.end()){
      std::cout <<" object "<<eta<<" "<<phi<<" passes "<<filterName<<std::endl;
      passAnyFilter=true;
    }
      }//end loop over matched trigger objects
    }//end loop over filter lables
    return passAnyFilter;
  }
}
