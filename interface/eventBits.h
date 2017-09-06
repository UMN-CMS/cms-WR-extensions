#ifndef eventBits_h
#define eventBits_h

#include <stdint.h>
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

class eventBits {
public:

  eventBits();
  void clear();
  //Misc numbers
  int run;
  int lumi;
  int event;
  double weight;

  //BASIC FUNCTIONS
  bool passesGenCuts();

  //EVENT COLLECTIONS
  std::vector<const reco::GenJet*> myGenJets;
  std::vector<const reco::GenJet*> myAK8GenJets;
  std::vector<const reco::GenParticle*> myGenPartons;
  std::vector<const reco::GenParticle*> myGenMuons;
  std::vector<std::pair<const pat::Jet*, const pat::Muon*>> myMuonJetPairs;
  std::vector<const pat::Jet*>  myJetCandsHighPt;
  std::vector<const pat::Jet*>  myJetCands;
  std::vector<const pat::Muon*> myMuonCandsHighPt;
  std::vector<const pat::Muon*> myMuonCands;
  const pat::MET*               myMET;
  int secondInDecayMuon;

  //EVENT VALUES
  bool passesWR2016;
  int muons40;
  int ak8jets40;

  int    muonCands;
  int    ak8jetCands;
  double parton1EtVal;
  double parton2EtVal;
  double muon1EtVal;
  double muon2EtVal;
  double firstPartonJetEtVal;
  double secondPartonJetEtVal;
  double firstPartonAK8JetEtVal;
  double secondPartonAK8JetEtVal;

  double parton1EtaVal;
  double parton2EtaVal;
  double muon1EtaVal;
  double muon2EtaVal;
  double firstPartonJetEtaVal;
  double secondPartonJetEtaVal;
  double firstPartonAK8JetEtaVal;
  double secondPartonAK8JetEtaVal;

  double parton1PhiVal;
  double parton2PhiVal;
  double muon1PhiVal;
  double muon2PhiVal;
  double firstPartonJetPhiVal;
  double secondPartonJetPhiVal;
  double firstPartonAK8JetPhiVal;
  double secondPartonAK8JetPhiVal;

  double dRparton1parton2Val;
  double dRmuon1muon2Val;
  double dRparton1muon2Val;
  double dRparton1muon1Val;
  double dRparton2muon2Val;
  double dRparton2muon1Val;
  double dRparton1jetVal;
  double dRparton2jetVal;
  double dRparton1AK8jetVal;
  double dRparton2AK8jetVal;

  double firstPartonJetEtHadronicVal;
  double secondPartonJetEtHadronicVal;
  double firstPartonJetEtEMVal;
  double secondPartonJetEtEMVal;
  double firstPartonJetEtInvisibleVal;
  double secondPartonJetEtInvisibleVal;

  double firstPartonAK8JetEtHadronicVal;
  double secondPartonAK8JetEtHadronicVal;
  double firstPartonAK8JetEtEMVal;
  double secondPartonAK8JetEtEMVal;
  double firstPartonAK8JetEtInvisibleVal;
  double secondPartonAK8JetEtInvisibleVal;

  double leadSubleadingAK8JetsMuonsMassVal;
  double leadSubleadingJetsMuonsMassVal;
  double leadSubleadingPartonsMuonsMassVal;
  double leadAK8JetMuonMassVal;

  double leadSubleadingAK8JetsMuonsPtVal;
  double leadSubleadingJetsMuonsPtVal;
  double leadSubleadingPartonsMuonsPtVal;
  double leadAK8JetMuonPtVal;

  double leadSubleadingAK8JetsMuonsEtaVal;
  double leadSubleadingJetsMuonsEtaVal;
  double leadSubleadingPartonsMuonsEtaVal;
  double leadAK8JetMuonEtaVal;

  double leadAK8JetMuonPhiVal;
  double leadAK8JetMuonJetMuonEnergyFraction;
  
  double selectedMuonEt;
  double selectedMuonPhi;
  double selectedMuonEta;

  double selectedJetEt;
  double selectedJetPhi;
  double selectedJetEta;

  double subleadMuon_selJetdPhi;
  double subleadMuon_selMuondPhi;
  double subleadMuon_selMuonMass;
  double subleadMuon_selMuonPt;
  double subleadMuonEt;

  double MET;
  double MET_selJetdPhi;
  double MET_selMuondPhi;
  double MET_selJetMass;
  double MET_selMuonMass;
  double MET_selJetPt;
  double MET_selMuonPt;

private:



};

















#endif
