#ifndef __L1TMUON_TRACKSEED_H__
#define __L1TMUON_TRACKSEED_H__
// 
// Class: L1TMuon::TrackSeed 
//
// Info: This track represents a DT(1 station), CSC (1 station), 
//       CSC+RPC (2 station), DT+RPC (2 station), or RPC+RPC (2 station)
//       based track seed, from which a full multi-station track can be
//       built.
//
// Author: L. Gray (FNAL)
//

#include <iostream>

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1TMuon/interface/MuonRegionalTracksFwd.h"
#include "DataFormats/Common/interface/RefToBase.h"

class L1MuDTTrackCand;
namespace csc {
  class L1Track;
}

namespace L1TMuon{
  class TrackSeed {   

  public:
    enum seed_type{ kDTOnly, kCSCOnly, kCSCRPC, kDTRPC, kRPCRPC, kNSeedTypes };
    enum subsystem_offset{ kDT, kRPCb, kCSC, kRPCf };
    TrackSeed():_endcap(0),_wheel(0),_sector(0),_type(kNSeedTypes),_mode(0) {}
      
    TrackSeed( const TriggerPrimitiveRef& );
    TrackSeed( const TriggerPrimitiveRef&,
	       const TriggerPrimitiveRef& );
    
    ~TrackSeed() {}

    seed_type type_idx() const;
    
    const TriggerPrimitiveStationMap& getStubs() const 
      { return _associatedStubs; }

    unsigned long mode()     const { return (_mode & 0xffff); }
    unsigned long dtMode()   const { return (_mode & 0xf<<4*kDT )>>4*kDT; }
    unsigned long cscMode()  const { return (_mode & 0xf<<4*kCSC)>>4*kCSC; }
    unsigned long rpcbMode() const { return (_mode & 0xf<<4*kRPCb)>>4*kRPCb; }
    unsigned long rpcfMode() const { return (_mode & 0xf<<4*kRPCf)>>4*kRPCf; }

    void print(std::ostream&) const;

  private:
    void addStub(const TriggerPrimitiveRef& stub);
    TriggerPrimitiveStationMap _associatedStubs;
    int _endcap, _wheel, _sector;
    seed_type _type;
    // this represents the mode considering all available muon detector types
    // 0 DT 4 bits | RPCb 4 bits | CSC 4 bits | RPC f 4 bits
    // using an unsigned long since we may want to add GEMs later
    // so cscMode() will return only the CSC part of the tracks contributing
    // to a CSC track (if this track was built from one)
    unsigned long _mode; 
  };
}

#endif
