//-------------------------------------------------
//
//   Class BMTrackContainer
//
//   Description: output data for BMTF trigger
//
//
//   Author List: Jorge Troconiz  UAM Madrid
//
//
//--------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "DataFormats/L1TMuon/interface/BMTrackContainer.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//---------------
// C++ Headers --
//---------------
using namespace std;

//-------------------
// Initializations --
//-------------------


//----------------
// Constructors --
//----------------
BMTrackContainer::BMTrackContainer() {}

//--------------
// Destructor --
//--------------
BMTrackContainer::~BMTrackContainer() {}

//--------------
// Operations --
//--------------
void BMTrackContainer::setContainer(const TrackContainer& inputTracks) {

  dtTracks = inputTracks;
}

BMTrackContainer::TrackContainer const* BMTrackContainer::getContainer() const {
  return &dtTracks;
}

bool BMTrackContainer::bxEmpty(int step) const {

  bool empty = true;

  for ( Trackiterator i  = dtTracks.begin();
                      i != dtTracks.end();
                      i++ ) {
    if  (step == i->bx()) empty = false;
  }

  return(empty);
}

int BMTrackContainer::bxSize(int step1, int step2) const {

  int size = 0;

  for ( Trackiterator i  = dtTracks.begin();
                      i != dtTracks.end();
                      i++ ) {
    if  (step1 <= i->bx() && step2 >= i->bx()
//      && i->quality_packed() != 0) size++;
      && i->hwQual() != 0) size++;
  }

  return(size);
}

BMTrackCand const* BMTrackContainer::dtTrackCand1(int wheel, int sect, int step) const {

  BMTrackCand const* rT=0;

  for ( Trackiterator i  = dtTracks.begin();
                      i != dtTracks.end();
                      i++ ) {
    if  (step == i->bx() && wheel == i->whNum() && sect == i->scNum()
      && i->TrkTag() == 0)
      rT = &(*i);
  }

  return(rT);
}

BMTrackCand const* BMTrackContainer::dtTrackCand2(int wheel, int sect, int step) const {

  BMTrackCand const* rT=0;

  for ( Trackiterator i  = dtTracks.begin();
                      i != dtTracks.end();
                      i++ ) {
    if  (step == i->bx() && wheel == i->whNum() && sect == i->scNum()
      && i->TrkTag() == 1)
      rT = &(*i);
  }

  return(rT);
}
