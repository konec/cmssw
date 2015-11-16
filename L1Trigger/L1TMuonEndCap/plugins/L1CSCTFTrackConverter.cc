// 
// Class: L1CSCTFTrackConverter
//
// Info: This producer eats CSCTF tracks (pre GMT) and matches them to 
//       L1TMuon::TriggerPrimitives. In the process of doing so it
//       converts the CSCTF tracks into a collection L1TMuon::InternalTrack
//       each of which contains the track stubs it was matched to.
//
// Author: L. Gray (FNAL)
//

#include "DataFormats/L1TMuon/interface/MuonInternalTrackFwd.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrack.h"

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"

#include "DataFormats/L1TMuon/interface/MuonRegionalTracksFwd.h"
#include "DataFormats/L1CSCTrackFinder/interface/L1CSCTrackCollection.h"

#include "DataFormats/Common/interface/RefProd.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "L1Trigger/L1TMuonEndCap/interface/helpers.h"

using namespace L1TMuon;

typedef edm::ParameterSet PSet;

class L1CSCTFTrackConverter : public edm::EDProducer {
public:
  L1CSCTFTrackConverter(const PSet&);
  ~L1CSCTFTrackConverter() {}

  void produce(edm::Event&, const edm::EventSetup&);  
private:
  edm::InputTag _cscTrackSrc, _trigPrimSrc;
};

L1CSCTFTrackConverter::L1CSCTFTrackConverter(const PSet& ps):
  _cscTrackSrc(ps.getParameter<edm::InputTag>("CSCTrackSrc")),
  _trigPrimSrc(ps.getParameter<edm::InputTag>("TriggerPrimitiveSrc")) {
  produces<CSCTrackCollection>("input");
  produces<InternalTrackCollection>();  
}

void L1CSCTFTrackConverter::produce(edm::Event& ev, 
				    const edm::EventSetup& es) {
  std::auto_ptr<InternalTrackCollection> 
    convertedTracks(new InternalTrackCollection());
  std::auto_ptr<CSCTrackCollection> inputTracks(new CSCTrackCollection); 

  // get the RefProd so we can make persistable references to
  // the track the converted track was made from
  edm::RefProd<CSCTrackCollection> csctfProd = 
    ev.getRefBeforePut<CSCTrackCollection>("input");  

  edm::Handle<L1CSCTrackCollection> cscTracks;
  ev.getByLabel(_cscTrackSrc,cscTracks);
  
  edm::Handle<TriggerPrimitiveCollection> trigPrims;
  ev.getByLabel(_trigPrimSrc,trigPrims);
  
  auto btrk = cscTracks->cbegin();  
  auto etrk = cscTracks->cend();
  for( ; btrk != etrk; ++btrk ) {
    inputTracks->push_back(btrk->first);
    
    // set the global position and pt bits for this CSC track
    unsigned global_phi = ( inputTracks->back().localPhi() + 
			    ((inputTracks->back().sector() - 1)*24) + 6 );
    unsigned eta_sign = ( inputTracks->back().endcap() == 1 ? 0 : 1 );
    unsigned global_eta = ( inputTracks->back().eta_packed() +
			    (eta_sign << (L1MuRegionalCand::ETA_LENGTH -1)) );
    unsigned rank = inputTracks->back().rank();
    unsigned qual = rank >> L1MuRegionalCand::PT_LENGTH;
    unsigned pt   = rank & ( ( 1 << L1MuRegionalCand::PT_LENGTH ) - 1 );
    if(global_phi > 143) global_phi -= 143;	
    inputTracks->back().setPhiPacked( global_phi & 0xff );
    inputTracks->back().setEtaPacked( global_eta & 0x3f );    
    inputTracks->back().setQualityPacked(qual);
    inputTracks->back().setPtPacked(pt);

    InternalTrack trk(inputTracks->back());
    CSCTrackRef parentRef(csctfProd,inputTracks->size() - 1);
    RegionalCandBaseRef parentBaseRef(parentRef);
    trk.setParent(parentBaseRef);

    std::vector<unsigned> trkNmbs;
    trkNmbs.reserve(5);
    trkNmbs.push_back(btrk->first.me1ID());
    trkNmbs.push_back(btrk->first.me2ID());
    trkNmbs.push_back(btrk->first.me3ID());
    trkNmbs.push_back(btrk->first.me4ID());
    trkNmbs.push_back(btrk->first.mb1ID());

    TriggerPrimitiveList tplist =
      helpers::getPrimitivesByCSCTriggerInfo(btrk->first.endcap(),
					     btrk->first.sector(),
					     trigPrims,
					     trkNmbs);
    auto stub = tplist.cbegin();
    auto stend = tplist.cend();
    for( ; stub != stend; ++stub ) {
      trk.addStub(*stub);      
    }
    convertedTracks->push_back(trk);
  }
  ev.put(inputTracks,"input");
  ev.put(convertedTracks);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1CSCTFTrackConverter);
