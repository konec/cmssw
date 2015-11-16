// 
// Class: L1TMuonInternalTrackPlotter
//
// Info: Processes a track into histograms of delta-phis and such
//
// Author: L. Gray (FNAL)
//

#include <memory>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"

#include "DataFormats/L1TMuon/interface/MuonInternalTrack.h"
#include "DataFormats/L1TMuon/interface/MuonInternalTrackFwd.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include <map>

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

// detector ID types
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"

using namespace L1TMuon;

typedef edm::ParameterSet PSet;

class L1TMuonInternalTrackPlotter : public edm::EDAnalyzer {  
  typedef TH1F* hist1dfp;
  typedef TH2F* hist2dfp;
  typedef std::map<std::string,hist1dfp> hist1dmap;
  typedef std::map<std::string,hist2dfp> hist2dmap;
  typedef std::map<std::string,double> deltas_map;
  
public:
  L1TMuonInternalTrackPlotter(const PSet&);
  ~L1TMuonInternalTrackPlotter() {}

  void analyze(const edm::Event&, const edm::EventSetup&);  
private:
  std::string convertStubsToName(const TriggerPrimitive&,
				 const TriggerPrimitive&) const;
  deltas_map makeCombinations(const InternalTrack&,double );
  TriggerPrimitiveRef getBestTriggerPrimitive(const TriggerPrimitiveList&,
					      unsigned) const;
  edm::Service<TFileService> _fs;
  bool _dogen;
  edm::InputTag _geninput;
  std::vector<edm::InputTag> _trkInput;
  // gen histograms
  hist1dfp gen_eta, gen_phi, gen_pt;  
  // track histograms
  hist2dmap detahists;
  hist2dmap dphihists;
};

L1TMuonInternalTrackPlotter::L1TMuonInternalTrackPlotter(const PSet& p) {
  if( (_dogen = p.getUntrackedParameter<bool>("doGen",false)) ) {
    _geninput = p.getParameter<edm::InputTag>("genSrc");
  }
  _trkInput = p.getParameter<std::vector<edm::InputTag> >("trackSrcs");

  if( _dogen ) {
    gen_eta = _fs->make<TH1F>("hgen_eta","Generated Eta",100,-2.5,2.5);
    gen_phi = _fs->make<TH1F>("hgen_phi","Generated Eta",100,-M_PI,M_PI);
    gen_pt = _fs->make<TH1F>("hgen_pt","Generated Eta",400,0,200);
  }
  
}

void L1TMuonInternalTrackPlotter::analyze(const edm::Event& ev, 
					const edm::EventSetup& es) {
  //dump the generated muons in the event (if requested)
  
  if( _dogen ) {
    edm::Handle<reco::GenParticleCollection> genps;
    ev.getByLabel(_geninput,genps);
    auto bgen = genps->cbegin();
    auto egen = genps->cend();
    for( ; bgen != egen; ++bgen ) {
      if( std::abs(bgen->pdgId()) == 13 ) {
	gen_eta->Fill(bgen->eta());
	gen_phi->Fill(bgen->phi());
	gen_pt->Fill(bgen->pt());

	auto src = _trkInput.cbegin();
	auto send = _trkInput.cend();
	for( ; src != send; ++src ) {
	  edm::Handle<InternalTrackCollection> trks;
	  ev.getByLabel(*src,trks);
	  
	  auto trk = trks->cbegin();
	  auto tend = trks->cend();
	  for( ; trk != tend; ++trk ) {
	    makeCombinations(*trk, bgen->pt());
	  }
	}

      }
    }    
  }  
}

// take a pair of positions within a mode and make a name!
std::string L1TMuonInternalTrackPlotter::
convertStubsToName(const TriggerPrimitive& tp1,
		   const TriggerPrimitive& tp2) const {
  std::string name1,name2;
  unsigned station1=0,station2=0;
  TriggerPrimitive::subsystem_type type1, type2;
  CSCDetId cscid;
  RPCDetId rpcid;
  DTChamberId dtid;

  type1 = tp1.subsystem();
  type2 = tp2.subsystem();
  
  switch(type1) {
  case TriggerPrimitive::kDT:
    name1 = std::string("DT");
    dtid = tp1.detId<DTChamberId>();    
    station1 = dtid.station();
    break;
  case TriggerPrimitive::kCSC:
    name1 = std::string("CSC");
    cscid = tp1.detId<CSCDetId>();
    station1 = cscid.station();
    break;
  case TriggerPrimitive::kRPC:
    rpcid = tp1.detId<RPCDetId>();
    name1 = (rpcid.region() == 0 ? std::string("RPCb") : std::string("RPCf"));
    station1 = rpcid.station();
    break;
  default:
    break;
  }

  switch(type2) {
  case TriggerPrimitive::kDT:
    name2 = std::string("DT");
    dtid = tp2.detId<DTChamberId>();
    station2 = dtid.station();
    break;
  case TriggerPrimitive::kCSC:
    name2 = std::string("CSC");
    cscid = tp2.detId<CSCDetId>();
    station2 = cscid.station();
    break;
  case TriggerPrimitive::kRPC:
    rpcid = tp2.detId<RPCDetId>();
    name2 = (rpcid.region() == 0 ? std::string("RPCb") : std::string("RPCf"));
    station2 = rpcid.station();
    break;
  default:
    break;
  }

  return std::string(Form("%s%i_%s%i",
			  name1.c_str(),station1,
			  name2.c_str(),station2));
}

// this function takes the list of used 
std::map<std::string,double> L1TMuonInternalTrackPlotter::
makeCombinations(const InternalTrack& track, double pt) {
  unsigned station1, station2, subsystem1, subsystem2;
  //int bx1, bx2;  
  TriggerPrimitiveStationMap stubs = track.getStubs();
  for( station1 = 1; station1 <= 4; ++station1 ) {
    for( subsystem1 = 0; subsystem1 <= 3; ++subsystem1 ) {
      const unsigned idx1 = 4*subsystem1+station1-1;
      if( !stubs.count(idx1) ) continue;
      TriggerPrimitiveList tps1 = stubs[idx1];
      for( station2 = 0; station2 <= 4; ++station2 ) {
	for( subsystem2 = 0; subsystem2 <=3; ++subsystem2 ) {
	  if( subsystem1 == subsystem2 && station1 == station2 ) continue;
	  const unsigned idx2 = 4*subsystem2+station2-1;
	    if( !stubs.count(idx2) ) continue;	    
	    TriggerPrimitiveList tps2 = stubs[idx2];

	    TriggerPrimitiveRef one = 
	      getBestTriggerPrimitive(tps1, subsystem1);
	    TriggerPrimitiveRef two = 
	      getBestTriggerPrimitive(tps2, subsystem2);

	    if( one.isNonnull() && two.isNonnull() ) {
	      std::string name =  convertStubsToName(*one,*two);
	      double phi1, phi2, eta1, eta2;
	      phi1 = one->getCMSGlobalPhi();
	      phi2 = two->getCMSGlobalPhi();
	      eta1 = one->getCMSGlobalEta();
	      eta2 = two->getCMSGlobalEta();

	      if( !detahists.count(name) ) {
		detahists[name] = 
		  _fs->make<TH2F>(Form("h%s_deta",name.c_str()),
				  Form("%s #Delta#eta vs. 1/p_{T}; 1/p_{T}^{True} GeV^{-1}; #Delta#eta",
				       name.c_str()),
				  1000,0,1,
				  500,-0.5,0.5);
	      } 

	      if( !dphihists.count(name) ) {
		dphihists[name] = 
		  _fs->make<TH2F>(Form("h%s_dphi",name.c_str()),
				  Form("%s #Delta#phi vs. 1/p_{T}; 1/p_{T}^{True} GeV^{-1}; #Delta#phi (rad)",
				       name.c_str()),
				  5000,0,1,
				  500,-0.5,0.5);
	      }

	      detahists[name]->Fill(1.0/pt,eta2-eta1);
	      dphihists[name]->Fill(1.0/pt,phi2-phi1);

	      //std::cout << "dphi: "<< (phi2 - phi1) << ' ' 
	      //<< "deta: " << (eta2 - eta1) << std::endl;
	      //one->print(std::cout);
	      //two->print(std::cout);
	    }
	       
	}// loop over subsystem in outer station
      }// loop over outer station
    }// loop over subsystem in inner station
  }// loop over inner station
  return deltas_map();
}

// this can return null if we're using a quality zero stub
TriggerPrimitiveRef L1TMuonInternalTrackPlotter::
getBestTriggerPrimitive(const TriggerPrimitiveList& list, 
			unsigned subsystem) const {
  TriggerPrimitiveRef result;
  unsigned bestquality = 0, qualtemp; // for CSCs / DTs
  float phiavg, bestdphi, lsize; // average strip for RPCS
  auto tp = list.cbegin();
  auto tpend = list.cend();
  
  switch( subsystem ) {
  case 0: // DTs
    for( ; tp != tpend; ++tp ) {
      qualtemp = 0;
      if( (*tp)->getDTData().qualityCode != -1 ) {
	qualtemp += (~((*tp)->getDTData().qualityCode)&0x7) << 1;	
      }
      if( (*tp)->getDTData().theta_quality != -1 ) {
	qualtemp += (~((*tp)->getDTData().theta_quality)&0x1);
      }
      if( qualtemp > bestquality ) {
	bestquality = qualtemp;
	result = *tp;
      }
    }
      break;
  case 2: // CSCs
    for( ; tp != tpend; ++tp ) {
      qualtemp = (*tp)->getCSCData().quality;      
      if ( qualtemp > bestquality ) {
	bestquality = qualtemp;
	result = *tp;
      }
    }
    break;
  case 1:
  case 3: // RPCb/f
    phiavg = 0;
    lsize = list.size();
    for( ; tp != tpend; ++tp ) {
      phiavg += (*tp)->getCMSGlobalPhi();
    }
    phiavg = phiavg/lsize;    
    tp = list.cbegin();
    bestdphi = 100;
    for( ; tp != tpend; ++tp ) {      
      if( std::abs((*tp)->getCMSGlobalPhi() - phiavg) < bestdphi ) {
	result = *tp;
	bestdphi = std::abs((*tp)->getCMSGlobalPhi() - phiavg);
      }
    }
    break;
  default:
    break;
  }
  return result;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1TMuonInternalTrackPlotter);
