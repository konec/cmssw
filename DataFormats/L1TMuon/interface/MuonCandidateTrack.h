#ifndef __L1TMUON_CANDIDATETRACK_H__
#define __L1TMUON_CANDIDATETRACK_H__
// 
// Class: L1TMuon::CandidateTrack
//
// Info: This class represents (one of the) final tracks output by
//       L1ITMu after sorting. It is just a L1MuGMTCand with a few
//       extra frills.
//
// Author: L. Gray (FNAL)
//

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrackFwd.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrack.h"
#include "DataFormats/Common/interface/Ref.h"

namespace L1TMuon{
  
  class CandidateTrack : public L1MuGMTCand {
  public:
    CandidateTrack() {}
    ~CandidateTrack() {}

    CandidateTrack(const InternalTrackRef&);
    
    InternalTrackRef parent() const { return _parent; }
             
    const TriggerPrimitiveStationMap& getStubs() const 
      { return _parent->getStubs(); }

    unsigned long mode()     const { return _parent->mode(); }
    unsigned long dtMode()   const { return _parent->dtMode(); }
    unsigned long cscMode()  const { return _parent->cscMode(); }
    unsigned long rpcbMode() const { return _parent->rpcbMode(); }
    unsigned long rpcfMode() const { return _parent->rpcfMode(); }

  private:    
    InternalTrackRef _parent;
  };
}

#endif
