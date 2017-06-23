#ifndef eventBits_h
#define eventBits_h

#include <stdint.h>

class eventBits {
public:

  eventBits();
  void clear();
  //Misc numbers
  int run;
  int lumi;
  int event;

  //BASIC FUNCTIONS
  bool passesGenCuts();

  //EVENT VALUES
  double parton1EtVal;
  double parton2EtVal;
  double muonHighestEtVal;
  double muonSecondHighestEtVal;
  double firstPartonJetEtVal;
  double secondPartonJetEtVal;
  double firstPartonAK8JetEtVal;
  double secondPartonAK8JetEtVal;

  double parton1EtaVal;
  double parton2EtaVal;
  double muonHighestEtEtaVal;
  double muonSecondHighestEtEtaVal;
  double firstPartonJetEtaVal;
  double secondPartonJetEtaVal;
  double firstPartonAK8JetEtaVal;
  double secondPartonAK8JetEtaVal;

  double parton1PhiVal;
  double parton2PhiVal;
  double muonHighestEtPhiVal;
  double muonSecondHighestEtPhiVal;
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

  double leadSubleadingAK8JetsMuonsPtVal;
  double leadSubleadingJetsMuonsPtVal;
  double leadSubleadingPartonsMuonsPtVal;

  double leadSubleadingAK8JetsMuonsEtaVal;
  double leadSubleadingJetsMuonsEtaVal;
  double leadSubleadingPartonsMuonsEtaVal;
  

private:



};

















#endif
