#ifndef eventInfo_h
#define eventInfo_h

//CMSSW CLASSES
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

//ROOT CLASSES
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

class eventInfo {
  public:
  eventInfo();
  bool PVselection(edm::Handle<std::vector<reco::Vertex>> vertices);
  std::vector<float> PUweight(edm::Handle< std::vector<PileupSummaryInfo> > hPileupInfoProduct);

  int 	 nVtx		;
  int    nPU            ;
  int    nPUmean        ;

  const reco::Vertex*               PVertex;

  float puweight_up	;
  float puweight_down	;

  private:

  TH1F* fPUWeightHist	;
  TH1F* fPUWeightHist_up;
  TH1F* fPUWeightHist_down;

};

#endif
