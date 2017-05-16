#ifndef eventBits_h
#define eventBits_h

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include <vector>

class eventBits {
public:

  eventBits();
  void clear();

  int run;
  int lumi;
  uint64_t event;
  
  std::vector<reco::GenParticle> outgoingPartons;
  std::vector<reco::GenParticle> outgoingMuons;
  std::vector<reco::GenJet>      genJets;

  const reco::GenParticle* highestEtMuon;
  const reco::GenParticle* secondHighestEtMuon;

  const reco::GenParticle* highestEtParton;
  const reco::GenParticle* secondHighestEtParton;

  const reco::GenParticle* firstPartonGenJet;    //deltaR MATCHED
  const reco::GenParticle* secondPartonGenJet;
 

private:






};

















#endif
