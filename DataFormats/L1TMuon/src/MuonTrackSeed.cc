#include "DataFormats/L1TMuon/interface/MuonTrackSeed.h"

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTTrackCand.h"
#include "DataFormats/L1CSCTrackFinder/interface/L1Track.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

using namespace L1TMuon;

TrackSeed::TrackSeed( const TriggerPrimitiveRef& tp) {
  switch( tp->subsystem() ) {
  case TriggerPrimitive::kCSC:
    _type = kCSCOnly;
    break;
  case TriggerPrimitive::kDT:
    _type = kDTOnly;
    break;
  case TriggerPrimitive::kRPC: // one hit seeds from RPCs not allowed    
  default:
    throw cms::Exception("Invalid Subsytem") 
      << "The specified subsystem for this track stub is out of range"
      << std::endl;
  }
  addStub(tp);  
}

TrackSeed::TrackSeed( const TriggerPrimitiveRef& tp1,
		      const TriggerPrimitiveRef& tp2 ) {
  if( ( tp1->subsystem() == TriggerPrimitive::kCSC &&
	tp2->subsystem() == TriggerPrimitive::kRPC ) ||
      ( tp1->subsystem() == TriggerPrimitive::kRPC &&
	tp2->subsystem() == TriggerPrimitive::kCSC )    ) {
    _type = kCSCRPC;
  } else if( ( tp1->subsystem() == TriggerPrimitive::kDT &&
	       tp2->subsystem() == TriggerPrimitive::kRPC ) ||
	     ( tp1->subsystem() == TriggerPrimitive::kRPC &&
	       tp2->subsystem() == TriggerPrimitive::kCSC )    ) {
    _type = kDTRPC;
  } else if( tp1->subsystem() == TriggerPrimitive::kRPC &&
	     tp2->subsystem() == TriggerPrimitive::kRPC    ) {
    _type = kRPCRPC;
  } else {
    throw cms::Exception("Invalid Subsytem") 
      << "The specified subsystem for this track stub is out of range"
      << std::endl;
  }
  addStub(tp1);
  addStub(tp2);
}

TrackSeed::seed_type TrackSeed::type_idx() const {  
  return _type;
}

void TrackSeed::addStub(const TriggerPrimitiveRef& stub) { 
  unsigned station;
  subsystem_offset offset;
  TriggerPrimitive::subsystem_type type = stub->subsystem();
  switch(type){
  case TriggerPrimitive::kCSC:    
    offset = kCSC;
    station = stub->detId<CSCDetId>().station();
    break;
  case TriggerPrimitive::kDT:    
    offset = kDT;
    station = stub->detId<DTChamberId>().station();
    break;
  case TriggerPrimitive::kRPC:    
    offset = kRPCb;
    if(stub->detId<RPCDetId>().region() != 0) 
      offset = kRPCf;
    station = stub->detId<RPCDetId>().station(); 
    break;
  default:
    throw cms::Exception("Invalid Subsytem") 
      << "The specified subsystem for this track stub is out of range"
      << std::endl;
  }  

  const unsigned shift = 4*offset + station - 1;
  const unsigned bit = 1 << shift;
   // add this track to the mode
  _mode = _mode | bit;
  if( _associatedStubs.count(shift) == 0 ) {
    _associatedStubs[shift] = TriggerPrimitiveList();
  }   
  _associatedStubs[shift].push_back(stub);
}

void TrackSeed::print(std::ostream& out) const {
  std::cout << "Track Seed -- endcap: " << std::dec << _endcap
	    << " wheel: " << _wheel 
	    << " sector: " << _sector << std::endl;
}
