#ifndef eventBits_h
#define eventBits_h

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
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

  const reco::GenParticle* highestEtMuon;
  const reco::GenParticle* secondHighestEtMuon;

  const reco::GenParticle* highestEtParton;
  const reco::GenParticle* secondHighestEtParton;
 

private:






};

















#endif
