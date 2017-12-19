#ifndef wrTools_h
#define wrTools_h



#include "tools.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include <vector>

namespace wrTools {

 bool compareEtGenParticlePointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2); 
 bool compareEtGenParticle(reco::GenParticle particle1, reco::GenParticle particle2); 
 bool compareEtJetPointer(const reco::GenJet* jet1, const reco::GenJet* jet2); 
 bool particleIsFrom(const reco::Candidate* particle, int pdgId);
 bool particleIsFromABS(const reco::Candidate* particle, unsigned int pdgId);
 bool particleInGenJet(const reco::Candidate* particle, const reco::GenJet* jet);
 bool compareEtCandidatePointer(const reco::Candidate* cand1, const reco::Candidate* cand2);
 bool compareEtLorentzVectorPointer(const math::XYZTLorentzVector* lv1, const math::XYZTLorentzVector* lv2);
 bool comparePairMassPointer(std::pair< const reco::Candidate*, const reco::Candidate* > pair1, std::pair< const reco::Candidate*, const reco::Candidate* > pair2);
 bool compareLvPairMassPointer(std::pair< const reco::Candidate*, const math::XYZTLorentzVector* > pair1, std::pair< const reco::Candidate*, const math::XYZTLorentzVector* > pair2);
 int characterizeEvent(std::vector<const reco::GenParticle*> myGenParticles);



};

















#endif
