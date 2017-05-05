#include "tools.h"

namespace wrTools {

bool compareEtPointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2) {
  if ( particle1->et() > particle2->et() ) return true;
  return false;
}
bool compareEt(reco::GenParticle particle1, reco::GenParticle particle2) {
  if ( particle1.et() > particle2.et() ) return true;
  return false;
}

}
