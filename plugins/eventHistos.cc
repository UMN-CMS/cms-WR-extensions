#include "eventHistos.h"









void eventHistos::book(TDirectory* histoFolder, uint16_t flavor) {

  //CREATED FOLDER IN HISTOFILE WITH FLAVOR SPECIFIED
  m_flavor = flavor;
  m_histoFolder = histoFolder;

  if (m_flavor == 1) {
  //MAKE GEN PLOTS
    


  } else if (m_flavor == 2) {
  //MAKE RECO PLOTS




  } else if (m_flavor == 3) {
  //MAKE GEN & RECO PLOTS


  } else {
  //NOTHING FOR NOW
  }


}
void eventHistos::fill(eventBits event) {
  if(m_flavor == 1) fillGen();
  else if(m_flavor == 2) fillReco();
  else if(m_flavor == 3) {
    fillGen();
    fillReco();
  }
  else return;
}

//SPECIFIC FILLER CLASSES
void eventHistos::fillGen() {


}
void eventHistos::fillReco() {



}
