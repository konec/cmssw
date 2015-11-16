#ifndef __L1TMUON_CANDIDATETRACKFWD_H__
#define __L1TMUON_CANDIDATETRACKFWD_H__

#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/Ptr.h"
namespace L1TMuon {
  class CandidateTrack;

  typedef std::vector<CandidateTrack> CandidateTrackCollection;
  typedef edm::Ref<CandidateTrackCollection> CandidateTrackRef;
  typedef edm::Ptr<CandidateTrack> CandidateTrackPtr;
}

#endif
