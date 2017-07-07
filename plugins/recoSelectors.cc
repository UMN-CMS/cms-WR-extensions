//local includes
#include "cmsWRextension.h"
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

////////////////////////////////THIS FILE CONTAINS CMSWREXTENSION MEMBER FUNCTIONS WHICH MAKE VARIOUS SELECTIONS AT RECO LEVEL///////////////
//////////////FOR MUONS TUNEP HIGHPT MUONS ARE USED//////////
///////////FROM THE AN///////////////////
//leading lepton with pT > 60 GeV
//subleading lepton with pT > 53 GeV
//at least two jets with pT > 40 GeV, consider only the leading and subleading in the following if more than two
//all leptons and jets with |η| < 2.4
//dilepton mass mll > 200 GeV: to suppress DY+jets contribution ∆R > 0.4 between all objects in the final state (leptons and jets) Mlljj > 600 GeV
using namespace edm;
bool cmsWRextension::selectWR2016Muons(const edm::Event& iEvent, eventBits& myEvent) {

   Handle<std::vector<pat::Muon>> recoMuons;
   iEvent.getByToken(m_recoMuonToken, recoMuons);
   std::vector<const pat::Muon*> candMuons;
   for (std::vector<pat::Muon>::const_iterator iParticle = recoMuons->begin(); iParticle != recoMuons->end(); iParticle++) {
     //NEEDS UPDATING
     if((iParticle->pt() > 53) && (abs(iParticle->eta()) < 2.4)) candMuons.push_back(&(*iParticle));
   }
   //SORT CANDIDATE MUON COLLECTION IN PT AND TAKE TOP TWO  ////IN AN EVENT WITH LOTS OF POSSIBLE MUONS, CUT ORDER COULD MATTER A BUNCH
   std::sort(candMuons.begin(), candMuons.end(), ::wrTools::compareEtCandidatePointer);

   return false;

}
bool cmsWRextension::selectHighPtISOMuon(const edm::Event& iEvent, eventBits& myEvent) {

   Handle<std::vector<pat::Muon>> recoMuons;
   iEvent.getByToken(m_recoMuonToken, recoMuons);

   for (std::vector<pat::Muon>::const_iterator iParticle = recoMuons->begin(); iParticle != recoMuons->end(); iParticle++) {
     std::cout<<iParticle->pt()<<std::endl;
     //NEEDS UPDATING
   }


   return false;
}
