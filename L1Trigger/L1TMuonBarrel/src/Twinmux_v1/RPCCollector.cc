#include "L1Trigger/L1TMuonBarrel/src/Twinmux_v1/RPCCollector.h"
#include "DataFormats/RPCDigi/interface/RPCDigi.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

using namespace L1TwinMux;

RPCCollector::RPCCollector( )
{
}
/*
void RPCCollector::
extractPrimitives(const edm::Event& ev,
		  const edm::EventSetup& es,
		  std::vector<TriggerPrimitive>& out) const {
  edm::Handle<RPCDigiCollection> rpcDigis;
  edm::InputTag _src("simMuonRPCDigis");

  ev.getByLabel(_src,rpcDigis);

  auto chamber = rpcDigis->begin();
  auto chend  = rpcDigis->end();
  for( ; chamber != chend; ++chamber ) {
    auto digi = (*chamber).second.first;
    auto dend = (*chamber).second.second;
    for( ; digi != dend; ++digi ) {
      out.push_back(TriggerPrimitive((*chamber).first,
				     digi->strip(),
				     (*chamber).first.layer(),
				     digi->bx()));
    }
  }
}*/
void RPCCollector::
extractPrimitives(edm::Handle<RPCDigiCollection> rpcDigis,
		  std::vector<TriggerPrimitive>& out) const {

  auto chamber = rpcDigis->begin();
  auto chend  = rpcDigis->end();
  for( ; chamber != chend; ++chamber ) {
    auto digi = (*chamber).second.first;
    auto dend = (*chamber).second.second;
    for( ; digi != dend; ++digi ) {
      out.push_back(TriggerPrimitive((*chamber).first,
				     digi->strip(),
				     (*chamber).first.layer(),
				     digi->bx()));
    }
  }
}

//#include "L1Trigger/L1TMuonEndCap/interface/SubsystemCollectorFactory.h"
//DEFINE_EDM_PLUGIN( SubsystemCollectorFactory, RPCCollector, "RPCCollector");
