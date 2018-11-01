#ifndef wrTools_h
#define wrTools_h



#include "tools.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "BaconAna/DataFormats/interface/TAddJet.hh"
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TMath.h"
#include <math.h>
#include <vector>

namespace wrTools {

 double smear(double mean, double sigma, int seed);
 bool compareEtGenParticlePointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2); 
 bool compareEtGenParticle(reco::GenParticle particle1, reco::GenParticle particle2); 
 bool compareEtJetPointer(const reco::GenJet* jet1, const reco::GenJet* jet2); 
 bool particleIsFrom(const reco::Candidate* particle, int pdgId);
 bool particleIsFromABS(const reco::Candidate* particle, unsigned int pdgId);
 bool particleInGenJet(const reco::Candidate* particle, const reco::GenJet* jet);
 bool compareEtCandidatePointer(const reco::Candidate* cand1, const reco::Candidate* cand2);
 bool compareEtCandidatePointerTAddJet(const baconhep::TAddJet* cand1, const baconhep::TAddJet* cand2);
 bool compareEtLorentzVectorPointer(const math::XYZTLorentzVector* lv1, const math::XYZTLorentzVector* lv2);
 bool comparePairMassPointer(std::pair< const reco::Candidate*, const reco::Candidate* > pair1, std::pair< const reco::Candidate*, const reco::Candidate* > pair2);
 bool comparePairMassPointerTAddJet(std::pair< const baconhep::TAddJet*, const reco::Candidate* > pair1, std::pair< const baconhep::TAddJet*, const reco::Candidate* > pair2);
 bool compareLvPairMassPointer(std::pair< const reco::Candidate*, const math::XYZTLorentzVector* > pair1, std::pair< const reco::Candidate*, const math::XYZTLorentzVector* > pair2);
 double dR(double eta1, double eta2, double phi1, double phi2);
 int characterizeEvent(std::vector<const reco::GenParticle*> myGenParticles);
 std::vector<const pat::TriggerObjectStandAlone*> getMatchedObjs(const float eta,const float phi,const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const float maxDeltaR=0.1);
 bool checkFilters(const float eta,const float phi,const std::vector<pat::TriggerObjectStandAlone>& trigObjs,const std::vector<std::string>& filterNames,const float maxDeltaR=0.1);



};

















#endif
