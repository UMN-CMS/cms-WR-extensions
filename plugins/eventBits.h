#ifndef eventBits_h
#define eventBits_h


class eventBits {
public:

  eventBits();
  void clear();
  //Misc numbers
  int run;
  int lumi;
  uint64_t event;

  //BASIC FUNCTIONS
  bool passesGenCuts();

  //EVENT VALUES
  double parton1EtVal;
  double parton2EtVal;
  double muonHighestEtVal;
  double muonSecondHighestEtVal;

  double parton1EtaVal;
  double parton2EtaVal;
  double muonHighestEtEtaVal;
  double muonSecondHighestEtEtaVal;

  double dRparton1parton2Val;
  double dRmuon1muon2Val;
  double dRparton1muon2Val;
  double dRparton1muon1Val;
  double dRparton2muon2Val;
  double dRparton2muon1Val;

  double firstPartonJetEtTotalVal;
  double secondPartonJetEtTotalVal;
  double firstPartonJetEtHadronicVal;
  double secondPartonJetEtHadronicVal;
  double firstPartonJetEtEMVal;
  double secondPartonJetEtEMVal;
  double firstPartonJetEtInvisibleVal;
  double secondPartonJetEtInvisibleVal;

  double leadSubleadingJetsMuonsMassVal;
  double leadSubleadingPartonsMuonsMassVal;

  double leadSubleadingJetsMuonsPtVal;
  double leadSubleadingPartonsMuonsPtVal;

  double leadSubleadingJetsMuonsEtaVal;
  double leadSubleadingPartonsMuonsEtaVal;
  

private:



};

















#endif
