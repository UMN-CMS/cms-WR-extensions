#include "tools.h"

namespace wrTools {

bool compareEt(const reco::GenParticle* particle1, const reco::GenParticle* particle2) {
  if ( particle1->et() > particle2->et() ) return true;
  return false;

}

}
