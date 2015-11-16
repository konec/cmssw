#ifndef __L1TMUON_INTERNALTRACKFWD_H__
#define __L1TMUON_INTERNALTRACKFWD_H__

#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/Ptr.h"
namespace L1TMuon {
  class InternalTrack;

  typedef std::vector<InternalTrack> InternalTrackCollection;
  typedef edm::Ref<InternalTrackCollection> InternalTrackRef;
  typedef edm::Ptr<InternalTrack> InternalTrackPtr;
}

#endif
