// 
// Class: L1DTTFTrackConverter
//
// Info: This producer eats DTTF tracks (pre GMT) and matches them to 
//       L1TMuon::TriggerPrimitives. In the process of doing so it
//       converts the DTTF tracks into a collection L1TMuon::InternalTrack
//       each of which contains the track stubs it was matched to.
//
// Author: L. Gray (FNAL)
//

#include "DataFormats/L1TMuon/interface/MuonInternalTrackFwd.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrack.h"

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"

#include "DataFormats/L1TMuon/interface/MuonRegionalTracksFwd.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTTrackContainer.h"

#include "DataFormats/Common/interface/RefProd.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "L1Trigger/L1TMuonEndCap/interface/helpers.h"

// this magic file contains a DT TrackClass -> mode LUT
#include "L1Trigger/DTTrackFinder/src/L1MuDTTrackAssParam.h"

using namespace L1TMuon;

typedef edm::ParameterSet PSet;

class L1DTTFTrackConverter : public edm::EDProducer {
public:
  L1DTTFTrackConverter(const PSet&);
  ~L1DTTFTrackConverter() {}

  void produce(edm::Event&, const edm::EventSetup&);  
private:
  edm::InputTag _dtTrackSrc, _trigPrimSrc;
  const int min_bx, max_bx;
};

L1DTTFTrackConverter::L1DTTFTrackConverter(const PSet& ps):
  _dtTrackSrc(ps.getParameter<edm::InputTag>("DTTrackSrc")),
  _trigPrimSrc(ps.getParameter<edm::InputTag>("TriggerPrimitiveSrc")),
  min_bx(ps.getParameter<int>("BX_min")),
  max_bx(ps.getParameter<int>("BX_max")) {
  produces<DTTrackCollection>("input");
  produces<InternalTrackCollection>();
}

void L1DTTFTrackConverter::produce(edm::Event& ev, 
				    const edm::EventSetup& es) {
  std::auto_ptr<InternalTrackCollection> 
    convertedTracks (new InternalTrackCollection());
  std::auto_ptr<DTTrackCollection> inputTracks(new DTTrackCollection);

  // get the RefProd so we can make persistable references to
  // the track the converted track was made from
  edm::RefProd<DTTrackCollection> dttfProd = 
    ev.getRefBeforePut<DTTrackCollection>("input");

  edm::Handle<L1MuDTTrackContainer> dtTracks;
  ev.getByLabel(_dtTrackSrc,dtTracks);
  
  edm::Handle<TriggerPrimitiveCollection> trigPrims;
  ev.getByLabel(_trigPrimSrc,trigPrims);
    
  int wheel;
  // DT sector processors have wheels [-3,-2,-1,1,2,3] since 
  // wheel zero needs two SPs
  for( int sp_wheel = -3 ; sp_wheel <= 3; ++sp_wheel ) {
    if( sp_wheel == 0 ) continue;
    wheel = std::abs(sp_wheel)-1;    
    wheel = sp_wheel < 0 ? -wheel : wheel;
    for( int sector = 0; sector <= 11; ++sector ) {
      for( int bx = min_bx; bx <= max_bx; ++bx ) {
	for( int itrk = 1; itrk <=2; ++itrk ) {
	  std::unique_ptr<const L1MuDTTrackCand> dttrk;
	  if( itrk == 1 ) 
	    dttrk.reset(dtTracks->dtTrackCand1(sp_wheel,sector,bx));
	  else            
	    dttrk.reset(dtTracks->dtTrackCand2(sp_wheel,sector,bx));
	  
	  if( dttrk ) {
	    inputTracks->push_back(*dttrk);

	    InternalTrack trk(*dttrk);
	    DTTrackRef parentRef(dttfProd,inputTracks->size() - 1);
	    RegionalCandBaseRef parentBaseRef(parentRef);
	    trk.setParent(parentBaseRef);

	    std::vector<unsigned> addrs;
	    addrs.reserve(4);	     	   
	    for( int station = 1; station <= 4; ++ station ) {
	      addrs.push_back(dttrk->stNum(station));
	    }	

	    // in DTs the mode is encoded by the track class
	    // mode is a 4 bit word , the bit position indicates the station
	    // if the bit is 1 then the station was used in track building
	    const unsigned mode = tc2bitmap((TrackClass)dttrk->TCNum());
	    TriggerPrimitiveList tplist =
	      helpers::getPrimitivesByDTTriggerInfo(wheel,
						    sp_wheel,sector+1,
						    trigPrims,mode,
						    addrs);
	    
	    auto stub = tplist.cbegin();
	    auto stend = tplist.cend();
	    for( ; stub != stend; ++stub ) {
	      trk.addStub(*stub);      
	    }

	    convertedTracks->push_back(trk);
	  }
	  dttrk.release();
	}
      }
    }
  }
  ev.put(inputTracks,"input");
  ev.put(convertedTracks);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1DTTFTrackConverter);
