#include "eventBits.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>

eventBits::eventBits() {
  run = -1;
  lumi = -1;
  event = 0;
}
void eventBits::clear() {


}
bool eventBits::passesGenCuts() {

  if(!highestEtMuon || 
     !secondHighestEtMuon ||
     !highestEtParton ||
     !secondHighestEtParton) return false;
    
  if(highestEtMuon->et() <= 20)         return false;
  if(secondHighestEtMuon->et() <= 20)   return false;
  if(highestEtParton->et() <= 20)       return false;
  if(secondHighestEtParton->et() <= 20) return false;

  return true;


}
