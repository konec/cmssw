#ifndef __L1TMUON_RPCCOLLECTOR_H__
#define __L1TMUON_RPCCOLLECTOR_H__
//
// Class: L1TMuon::RPCCollector
//
// Info: Processes RPC digis into L1TMuon trigger primitives.
//       Positional information is not assigned here.
//
// Author: L. Gray (FNAL)
//
#include <vector>
//#include "L1Trigger/L1TMuonBarrel/src/Twinmux_v1/SubsystemCollector.h"
#include "L1Trigger/L1TMuonBarrel/src/Twinmux_v1/MuonTriggerPrimitive.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/RPCDigi/interface/RPCDigi.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/Common/interface/Handle.h"

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

namespace L1TwinMux {

  class RPCCollector {
  public:
    RPCCollector();
    ~RPCCollector() {}

    //virtual void extractPrimitives(const edm::Event&, const edm::EventSetup&,
	//			   std::vector<TriggerPrimitive>&) const;
	   virtual void extractPrimitives(edm::Handle<RPCDigiCollection> rpcDigis,
				   std::vector<TriggerPrimitive>&) const;
  private:
  };
}

#endif
