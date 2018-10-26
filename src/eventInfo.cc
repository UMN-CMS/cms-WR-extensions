#include "ExoAnalysis/cmsWRextensions/interface/eventInfo.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>
#include <algorithm>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"
#include "DataFormats/VertexReco/interface/Vertex.h"




eventInfo::eventInfo () {

  std::string PU_Weights="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/puWeights_Jan11.root";
  TFile *lFile = TFile::Open(PU_Weights.c_str());
  fPUWeightHist  = (TH1F*) lFile->Get("puw");
  fPUWeightHist->SetDirectory(0);
  lFile->Close();

  std::string PU_Weights2="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/puWeights_All.root";
  TFile *lFile2 = TFile::Open(PU_Weights2.c_str());
  fPUWeightHist_up  = (TH1F*) lFile2->Get("puw_p");
  fPUWeightHist_up->SetDirectory(0);
  fPUWeightHist_down  = (TH1F*) lFile2->Get("puw_m");
  fPUWeightHist_down->SetDirectory(0);
  lFile2->Close();

  nPU = -1;
  nPUmean = -1;

  PVertex = NULL;

}

bool eventInfo::PVselection(edm::Handle<std::vector<reco::Vertex>> vertices) {

  bool isPV = false;
  // Let's check that we have at least one good vertex!
  std::cout << "Running vertex selection" << std::endl;
  std::vector<const reco::Vertex*> PVertices;
  std::vector<reco::Vertex>::const_iterator firstGoodVertex = vertices->end();

  int Nvtx = 0;

  for (std::vector<reco::Vertex>::const_iterator it=vertices->begin(); it!=vertices->end(); ++it) {
    if (!it->isFake() && it->ndof()>4 && it->position().Rho()<2. && std::abs(it->position().Z())<24.) {
      Nvtx++;
      if(firstGoodVertex == vertices->end()){
        firstGoodVertex = it;
        PVertices.push_back(&(*it));
      }
//      break;
    }
  }
  std::cout << "nVtx: " << Nvtx << std::endl;

  nVtx = Nvtx;
  if(firstGoodVertex == vertices->end()){
    std::cout<<"NO GOOD VERTEX" << std::endl;
    return isPV;
  }else{
    isPV = true;
  }

  PVertex = PVertices.at(0);

  return isPV;
}

std::vector<float> eventInfo::PUweight(edm::Handle< std::vector<PileupSummaryInfo> > hPileupInfoProduct){

  const std::vector<PileupSummaryInfo> *inPUInfos = hPileupInfoProduct.product();
  for (std::vector<PileupSummaryInfo>::const_iterator itPUInfo = inPUInfos->begin(); itPUInfo!=inPUInfos->end(); ++itPUInfo) {
    if(itPUInfo->getBunchCrossing()==0) {
      nPU      = itPUInfo->getPU_NumInteractions();
      nPUmean  = itPUInfo->getTrueNumInteractions();
    } /*else if(itPUInfo->getBunchCrossing()==-1) {
      evtInfo->nPUm     = itPUInfo->getPU_NumInteractions();
      evtInfo->nPUmeanm = itPUInfo->getTrueNumInteractions();
    } else if(itPUInfo->getBunchCrossing()==1) {
      evtInfo->nPUp     = itPUInfo->getPU_NumInteractions();
      evtInfo->nPUmeanp = itPUInfo->getTrueNumInteractions();
    }*/
  }

  float lNPVW = Float_t(fPUWeightHist->GetBinContent(fPUWeightHist->FindBin(nPUmean)));
  puweight_up = fPUWeightHist_up->GetBinContent(fPUWeightHist_up->FindBin(nPUmean));
  puweight_down = fPUWeightHist_down->GetBinContent(fPUWeightHist_down->FindBin(nPUmean));
  if(nPUmean > 49){
    lNPVW = Float_t(fPUWeightHist->GetBinContent(fPUWeightHist->FindBin(49)));
    puweight_up = fPUWeightHist_up->GetBinContent(fPUWeightHist_up->FindBin(49));
    puweight_down = fPUWeightHist_down->GetBinContent(fPUWeightHist_down->FindBin(49));
  }
  if(nPUmean <  1){
    lNPVW = Float_t(fPUWeightHist->GetBinContent(fPUWeightHist->FindBin(0)));
    puweight_up = fPUWeightHist_up->GetBinContent(fPUWeightHist_up->FindBin(0));
    puweight_down = fPUWeightHist_down->GetBinContent(fPUWeightHist_down->FindBin(0));
  }

  std::cout << "puweight_up: " << puweight_up << std::endl;
  std::cout << "puweight_down: " << puweight_down << std::endl;

  std::vector<float> puWeights;
  puWeights.push_back(lNPVW);
  puWeights.push_back(puweight_up);
  puWeights.push_back(puweight_down);

  return puWeights;

}
