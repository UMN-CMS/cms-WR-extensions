#ifndef wrTools_h
#define wrTools_h



#include "tools.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <vector>

namespace wrTools {

 bool compareEtPointer(const reco::GenParticle* particle1, const reco::GenParticle* particle2); 
 bool compareEt(reco::GenParticle particle1, reco::GenParticle particle2); 




};

















#endif
