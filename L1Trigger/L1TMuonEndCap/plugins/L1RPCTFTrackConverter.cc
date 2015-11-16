// 
// Class: L1RPCTFTrackConverter
//
// Info: This producer eats RPCTF tracks (pre GMT) and matches them to 
//       L1ITMu::TriggerPrimitives. In the process of doing so it
//       converts the RPCTF tracks into a collection L1ITMu::InternalTrack
//       each of which contains the track stubs it was matched to.
//
// Author: L. Gray (FNAL)
//

#include "DataFormats/L1TMuon/interface/MuonInternalTrackFwd.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrack.h"

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"

#include "DataFormats/L1TMuon/interface/MuonRegionalTracksFwd.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCDigi/interface/RPCDigiL1Link.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"

using namespace L1TMuon;

typedef edm::ParameterSet PSet;

class L1RPCTFTrackConverter : public edm::EDProducer {
public:
  L1RPCTFTrackConverter(const PSet&);
  ~L1RPCTFTrackConverter() {}

  void produce(edm::Event&, const edm::EventSetup&);  
private:
  edm::InputTag _rpcTrackSrc, _rpcL1LinkSrc, _trigPrimSrc;
};

L1RPCTFTrackConverter::L1RPCTFTrackConverter(const PSet& ps):
  _rpcTrackSrc(ps.getParameter<edm::InputTag>("RPCTrackSrc")),
  _rpcL1LinkSrc(ps.getParameter<edm::InputTag>("RPCL1LinkSrc")),
  _trigPrimSrc(ps.getParameter<edm::InputTag>("TriggerPrimitiveSrc")) {
  produces<RegionalCandCollection>("input");
  produces<RPCL1LinkCollection>("input");
  produces<InternalTrackCollection>();
}

void L1RPCTFTrackConverter::produce(edm::Event& ev, 
				    const edm::EventSetup& es) {
  std::auto_ptr<InternalTrackCollection> 
    convertedTracks (new InternalTrackCollection());
  std::auto_ptr<RegionalCandCollection>
    inputTracks(new RegionalCandCollection);
  std::auto_ptr<RPCL1LinkCollection>
    inputLinks(new RPCL1LinkCollection);

  edm::RefProd<RegionalCandCollection> rpcpacProd = 
    ev.getRefBeforePut<RegionalCandCollection>("input");  

  edm::RefProd<RPCL1LinkCollection> rpclinkProd = 
    ev.getRefBeforePut<RPCL1LinkCollection>("input");  

  edm::Handle<std::vector<RPCDigiL1Link> > rpcLinks;
  ev.getByLabel(_rpcL1LinkSrc,rpcLinks);

  edm::Handle<RegionalCandCollection> rpcTracks;
  ev.getByLabel(_rpcTrackSrc,rpcTracks);

  edm::Handle<TriggerPrimitiveCollection> trigPrims;
  ev.getByLabel(_trigPrimSrc,trigPrims);

  assert(rpcLinks->size() == rpcTracks->size() && 
	 "link size is not the same as track size! wat!?");
  
  auto tpbeg = trigPrims->cbegin();
  auto tpend = trigPrims->cend();
  auto link = rpcLinks->cbegin();
  auto btrk = rpcTracks->cbegin();
  auto etrk = rpcTracks->cend();
  for( ; btrk != etrk; ++btrk ) {
    inputTracks->push_back(*btrk);
    inputLinks->push_back(*link);
    
    InternalTrack trk(*btrk,RPCL1LinkRef(rpclinkProd,inputLinks->size() - 1));
    RegionalCandRef parentRef(rpcpacProd,inputTracks->size() - 1);
    RegionalCandBaseRef parentBaseRef(parentRef);
    trk.setParent(parentBaseRef);
    
    auto tp = trigPrims->cbegin();
    for( ; tp != tpend; ++tp ) {
      if( tp->subsystem() != TriggerPrimitive::kRPC ) continue;
      for( unsigned ilayer = 1; ilayer <= link->nlayer(); ++ilayer ) {
	if( link->empty(ilayer) ) continue;
	RPCDetId linkId = RPCDetId( link->rawdetId(ilayer) );
	RPCDetId tpId   = tp->detId<RPCDetId>();
	if( linkId == tpId ) {
	  trk.addStub(TriggerPrimitiveRef(trigPrims,tp - tpbeg));
	}
      }
    }    

    ++link;
    convertedTracks->push_back(trk);
  }

  ev.put(inputLinks,"input");
  ev.put(inputTracks,"input");
  ev.put(convertedTracks);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1RPCTFTrackConverter);
