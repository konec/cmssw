// 
// Class: L1ITMuSimpleDeltaEtaHitMatcher
//
// Info: This producer takes generated muons and finds those hits in a wide
//       delta-eta window about the muon, creating a track out of them.
//       This kind of track-building really only works in the most simple of
//       cases (like single muon gun MC) and is meant as a simple mock-up.
//
// Author: L. Gray (FNAL)
//

#include <memory>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "L1Trigger/L1TMuonEndCap/interface/GeometryTranslator.h"

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"

#include "DataFormats/L1TMuon/interface/MuonInternalTrack.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrackFwd.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

using namespace L1TMuon;

typedef edm::ParameterSet PSet;

class L1TMuonSimpleDeltaEtaHitMatcher : public edm::EDProducer {
public:
  L1TMuonSimpleDeltaEtaHitMatcher(const PSet&);
  ~L1TMuonSimpleDeltaEtaHitMatcher() {}

  void produce(edm::Event&, const edm::EventSetup&);  
private:
  edm::InputTag _trigPrimSrc, _genSrc;
  double _detaWindow;
};

L1TMuonSimpleDeltaEtaHitMatcher::L1TMuonSimpleDeltaEtaHitMatcher(const PSet& p) :
  _trigPrimSrc(p.getParameter<edm::InputTag>("TriggerPrimitiveSrc")),
  _genSrc(p.getParameter<edm::InputTag>("genSrc")),
  _detaWindow(p.getParameter<double>("DetaWindowSize")) {  
  produces<InternalTrackCollection>();
}

void L1TMuonSimpleDeltaEtaHitMatcher::produce(edm::Event& ev, 
					     const edm::EventSetup& es) {
  std::auto_ptr<InternalTrackCollection> 
    out(new InternalTrackCollection);

  edm::Handle<reco::GenParticleCollection> genps;
  ev.getByLabel(_genSrc,genps);

  edm::Handle<TriggerPrimitiveCollection> tps;
  ev.getByLabel(_trigPrimSrc, tps);

  auto bgen = genps->cbegin();
  auto egen = genps->cend();
  for( ; bgen != egen; ++bgen ) {
    if( std::abs(bgen->pdgId()) == 13 ) {
      InternalTrack trk;
      trk.setType(4);
      //std::cout << trk.type_idx() << std::endl;
      /*
      std::cout << "Generated Muon pdgId:" << bgen->pdgId() 
		<< " pt: " << bgen->pt()
		<< " eta: " << bgen->eta() 
		<< " phi: " << bgen->phi() << std::endl;
      */
      
      auto tp = tps->cbegin();
      auto tpbeg = tps->cbegin();
      auto tpend = tps->cend();
      for( ; tp != tpend; ++tp ) {
	TriggerPrimitiveRef tpref(tps,tp - tpbeg);
	double deta = std::abs(bgen->eta() - tp->getCMSGlobalEta());
	if( deta < _detaWindow ) {
	  trk.addStub(tpref);
	}
      }
      if(trk.mode() != 0x0) out->push_back(trk);
    }
  }  
  ev.put(out);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1TMuonSimpleDeltaEtaHitMatcher);
