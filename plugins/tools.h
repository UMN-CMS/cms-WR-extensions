#ifndef wrTools_h
#define wrTools_h



#include "tools.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include <vector>

namespace wrTools {

 bool compareEtPointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2); 
 bool compareEt(reco::GenParticle particle1, reco::GenParticle particle2); 
 bool compareEtJetPointer(const reco::GenJet* jet1, const reco::GenJet* jet2); 
 bool particleIsFrom(const reco::Candidate* particle, int pdgId);
 bool particleIsFromABS(const reco::Candidate* particle, unsigned int pdgId);




};

















#endif
