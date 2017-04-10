#ifndef eventBits_h
#define eventBits_h

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>

class eventBits {
public:

  eventBits();
  ~eventBits();
  void clear();

  int run;
  int lumi;
  uint64_t event;
  
  std::vector<reco::GenParticle>* genParticles;
  std::vector<const reco::GenParticle*> outgoingPartons;
  std::vector<const reco::GenParticle*> outgoingMuons;

  const reco::GenParticle* highestEtMuon;
  const reco::GenParticle* secondHighestEtMuon;
  const reco::GenParticle* firstMuon;
  const reco::GenParticle* secondMuon;

  const reco::GenParticle* highestEtParton;
  const reco::GenParticle* secondHighestEtParton;
 

private:






};

















#endif
