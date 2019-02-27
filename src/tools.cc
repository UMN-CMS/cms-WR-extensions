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
  bool compareMUFCandidatePointer(const pat::Jet* cand1, const pat::Jet* cand2) { 
    if ( cand1->muonEnergyFraction() < cand2->muonEnergyFraction() ) return true;
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
  double dPhi(double phi1, double phi2) {
    double raw_dphi = phi1 - phi2;
    if (fabs(raw_dphi) < ROOT::Math::Pi()) return raw_dphi;
    double region = std::round(raw_dphi / (2.*ROOT::Math::Pi()));
    return raw_dphi - 2.*ROOT::Math::Pi()*region;
  }
  double dR2(double eta1, double eta2, double phi1, double phi2) {
    double deta = eta1 - eta2;
    double dphi = std::abs(phi1-phi2); if (dphi>ROOT::Math::Pi()) dphi-=2*ROOT::Math::Pi();  
    return deta*deta + dphi*dphi;
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
      if(abs((*iConst)->eta() - particle->eta()) >= 0.1) continue; 
      if(abs((*iConst)->phi() - particle->phi()) >= 0.1) continue; 
      
      std::cout << "Found daughter with PT: "<<(*iConst)->pt() <<" compared to: "<<particle->pt() << std::endl;
      return true;
    }
    



    return false;
  }
  bool particleInPatJet(const reco::Candidate* particle, const pat::Jet* jet) {
    //loop over jet candidates
    std::vector<const reco::Candidate*> constituents = jet->getJetConstituentsQuick();
    for(std::vector<const reco::Candidate*>::const_iterator iConst = constituents.begin(); iConst != constituents.end(); iConst++) {
      if(abs((*iConst)->pt() - particle->pt()) >= 0.1) continue; //PT NOT CLOSE ENOUGH
      if(abs((*iConst)->eta() - particle->eta()) >= 0.1) continue; 
      if(abs((*iConst)->phi() - particle->phi()) >= 0.1) continue; 
      
      std::cout << "Found daughter with PT: "<<(*iConst)->pt() <<" compared to: "<<particle->pt() << std::endl;
      return true;
    }
    



    return false;
  }
  void printCutFlowResult(vid::CutFlowResult &cutflow){
    
    printf("    CutFlow name= %s    decision is %d\n", 
       cutflow.cutFlowName().c_str(),
       (int) cutflow.cutFlowPassed());
    int ncuts = cutflow.cutFlowSize();
    printf(" Index                               cut name              isMasked    value-cut-upon     pass?\n");
    for(int icut = 0; icut<ncuts; icut++){
      printf("  %2d      %50s    %d        %f          %d\n", icut,
         cutflow.getNameAtIndex(icut).c_str(),
         (int)cutflow.isCutMasked(icut),
         cutflow.getValueCutUpon(icut),
         (int)cutflow.getCutResultByIndex(icut));
    }
      
  }
  //TAKES AN INITIAL GEN PARTICLE AND EVOLVES IT TO A STATUS OVER 50
  const reco::GenParticle* evolveParticle(const reco::GenParticle* ip) {
    bool updated = true;
    const reco::GenParticle* fp = ip;
    while (fp->status() < 50 && fp->numberOfDaughters() > 0 && updated) {
      bool FoundDaughter = false;
      updated = false;
      for (size_t iii = 0; iii < fp->numberOfDaughters(); iii++) {
        if (fp->daughter(iii)->pdgId() == fp->pdgId()) {
          fp = dynamic_cast<const reco::GenParticle*> (fp->daughter(iii));
          FoundDaughter = true;
          updated = true;
        }
      }
      if (!FoundDaughter) {
        std::cout << "Okay we didn't find the daughter the pdgid of the orginal is " << fp->pdgId() << std::endl;
        for (size_t iii = 0; iii < fp->numberOfDaughters(); iii++) {
          std::cout << "daughter number " << iii << " pdgid is " << fp->daughter(iii)->pdgId() << std::endl;
        }
        return NULL;
      }
    }
    return fp; 


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
