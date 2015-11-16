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
#ifndef L1MuBMTrackContainer_H
#define L1MuBMTrackContainer_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "DataFormats/L1TMuon/interface/BMTrackCand.h"

//----------------------
// Base Class Headers --
//----------------------
#include <vector>

//---------------
// C++ Headers --
//---------------

//              ---------------------
//              -- Class Interface --
//              ---------------------


class BMTrackContainer {

 public:

  typedef std::vector<BMTrackCand>    TrackContainer;
  typedef TrackContainer::const_iterator  Trackiterator;
  typedef TrackContainer::iterator        TrackIterator;

  //  Constructors
  BMTrackContainer();

  //  Destructor
  ~BMTrackContainer();

  void setContainer(const TrackContainer& inputTracks);

  TrackContainer const* getContainer() const;

  bool bxEmpty(int step) const;

  int bxSize(int step1, int step2) const;

  BMTrackCand const* dtTrackCand1(int wheel, int sect, int bx) const;

  BMTrackCand const* dtTrackCand2(int wheel, int sect, int bx) const;


 private:

  TrackContainer dtTracks;

};

#endif
