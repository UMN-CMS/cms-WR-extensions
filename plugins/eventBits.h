#ifndef eventBits_h
#define eventBits_h

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include <vector>

class eventBits {
public:

  eventBits();
  void clear();
  //Misc numbers
  int run;
  int lumi;
  uint64_t event;
  //Collections
  std::vector<reco::GenParticle> outgoingPartons;
  std::vector<reco::GenParticle> outgoingMuons;
  std::vector<reco::GenJet>      genJets;
 
  std::vector<pat::Muon>         selectedMuons;
  std::vector<pat::Muon>         MCmatchedMuons;

  //indices in collections
  const reco::GenParticle* getHighestEtMuon();
  const reco::GenParticle* getSecondHighestEtMuon();

  const reco::GenParticle* getHighestEtParton();
  const reco::GenParticle* getSecondHighestEtParton();

  const reco::GenJet* getFirstPartonGenJet();    //deltaR MATCHED
  const reco::GenJet* getSecondPartonGenJet();

  void setHighestEtMuon(uint32_t index);
  void setSecondHighestEtMuon(uint32_t index);

  void setHighestEtParton(uint32_t index);
  void setSecondHighestEtParton(uint32_t index);

  void setFirstPartonGenJet(uint32_t index);    //deltaR MATCHED
  void setSecondPartonGenJet(uint32_t index);

  bool passesGenCuts();
private:

  int highestEtMuon;
  int secondHighestEtMuon;

  int highestEtParton;
  int secondHighestEtParton;

  int firstPartonGenJet;    //deltaR MATCHED
  int secondPartonGenJet;





};

















#endif
