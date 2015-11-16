#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include "L1Trigger/L1TMuonOverlap/interface/OMTFinputMaker.h"
#include "L1Trigger/L1TMuonOverlap/interface/OMTFinput.h"
#include "L1Trigger/L1TMuonOverlap/interface/OMTFConfiguration.h"
#include "L1Trigger/L1TMuonOverlap/interface/AngleConverter.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::OMTFinputMaker(){ 

  myInput = new OMTFinput();

  katownik.reset(new AngleConverter());
}
///////////////////////////////////////
///////////////////////////////////////
void OMTFinputMaker::initialize(const edm::EventSetup& es){ 

  katownik->checkAndUpdateGeometry(es);
}
///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::~OMTFinputMaker(){ 

  if(myInput) delete myInput;

}
///////////////////////////////////////
///////////////////////////////////////
bool  OMTFinputMaker::acceptDigi(uint32_t rawId,
				 unsigned int iProcessor,
				 l1t::tftype type){

  unsigned int aMin = OMTFConfiguration::barrelMin[iProcessor];
  unsigned int aMax = OMTFConfiguration::barrelMax[iProcessor];
  unsigned int aSector = 99;

  ///Clean up digis. Remove unconnected detectors
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical OMTFinputMaker") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId aId(rawId);
    
    ///Select RPC chambers connected to OMTF
    if(type==l1t::tftype::omtf_pos &&
       (aId.region()<0 ||
       (aId.region()==0 && aId.ring()!=2) ||
       (aId.region()==0 && aId.station()==4) ||
       (aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
       (aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
       (aId.region()==1 && aId.station()==4) ||
	///RPC RE1/2 temporarily not used (aId.region()==1 && aId.station()==1 && aId.ring()<2) ||
       (aId.region()==1 && aId.station()>0 && aId.ring()<3))
       ) return false;

    if(type==l1t::tftype::omtf_neg &&
        (aId.region()>0 ||
	(aId.region()==0 && aId.ring()!=-2) ||
	(aId.region()==0 && aId.station()==4) ||
	(aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
	(aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
	(aId.region()==-1 && aId.station()==4) ||
	 //RPC RE1/2 temporarily not used (aId.region()==1 && aId.station()==1 && aId.ring()<2) ||
	(aId.region()==-1 && aId.station()>0 && aId.ring()<3))
       ) return false;
    
    if(type==l1t::tftype::bmtf && aId.region()!=0) return false;
    
    if(type==l1t::tftype::emtf_pos &&
        (aId.region()<=0 ||
	(aId.station()==1 && aId.ring()==3))) return false;
    if(type==l1t::tftype::emtf_neg &&
        (aId.region()>=0 ||
	(aId.station()==1 && aId.ring()==3))) return false;
    ////////////////
    if(aId.region()==0) aSector = aId.sector();
    if(aId.region()!=0){
      aSector = (aId.sector()-1)*6+aId.subsector();
      aMin = OMTFConfiguration::endcap10DegMin[iProcessor];
      aMax = OMTFConfiguration::endcap10DegMax[iProcessor];
    }
   
    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);

    if(type==l1t::tftype::omtf_pos && dt.wheel()!=2) return false;
    if(type==l1t::tftype::omtf_neg && dt.wheel()!=-2) return false;
    if(type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) return false;
    
    aSector =  dt.sector();   	
    break;
  }
  case MuonSubdetId::CSC: {

    CSCDetId csc(rawId);    
    if(type==l1t::tftype::omtf_pos &&
       (csc.endcap()==2 || csc.ring()==1 || csc.station()==4)) return false;
    if(type==l1t::tftype::omtf_neg &&
       (csc.endcap()==1 || csc.ring()==1 || csc.station()==4)) return false;

    if(type==l1t::tftype::emtf_pos &&
       (csc.endcap()==2 || (csc.station()==1 && csc.ring()==3))
       ) return false;
    if(type==l1t::tftype::emtf_neg &&
       (csc.endcap()==1 || (csc.station()==1 && csc.ring()==3))
       ) return false;

    aSector =  csc.chamber();   	

    aMin = OMTFConfiguration::endcap10DegMin[iProcessor];
    aMax = OMTFConfiguration::endcap10DegMax[iProcessor];

    if( (type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) &&
	csc.station()>1 && csc.ring()==1){
      aMin = OMTFConfiguration::endcap20DegMin[iProcessor];
      aMax = OMTFConfiguration::endcap20DegMax[iProcessor];
    }
    break;
  }    
  }
  
  if(aMax>aMin && aSector>=aMin && aSector<=aMax) return true;
  if(aMax<aMin && (aSector>=aMin || aSector<=aMax)) return true;

  return false;
}
///////////////////////////////////////
///////////////////////////////////////
unsigned int OMTFinputMaker::getInputNumber(unsigned int rawId, 
					    unsigned int iProcessor,
					    l1t::tftype type){

  unsigned int iInput = 99;
  unsigned int aSector = 99;
  int aMin = OMTFConfiguration::barrelMin[iProcessor];
  int iRoll = 1;
  int nInputsPerSector = 2;

  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical OMTFinputMaker") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId rpc(rawId);
    if(rpc.region()==0){
      nInputsPerSector = 4;
      aSector = rpc.sector();
      ///on the 0-2pi border we need to add 1 30 deg sector
      ///to get the correct index
      if(iProcessor==5 && aSector<3) aMin = 0;
      //Use division into rolls
      iRoll = rpc.roll();
      ///Set roll number by hand to keep common input 
      ///number shift formula for all stations
      if(rpc.station()==2 && rpc.layer()==2 && rpc.roll()==2) iRoll = 1;
      if(rpc.station()==3) iRoll = 1;

      ///At the moment do not use RPC chambers splitting into rolls for bmtf part      
      if(type==l1t::tftype::bmtf)iRoll = 1;
    }
    if(rpc.region()!=0){
      aSector = (rpc.sector()-1)*6+rpc.subsector();
      aMin = OMTFConfiguration::endcap10DegMin[iProcessor];
      ///on the 0-2pi border we need to add 4 10 deg sectors
      ///to get the correct index
      if(iProcessor==5 && aSector<5) aMin = -3;
    }    
    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    aSector = dt.sector();
    ///on the 0-2pi border we need to add 1 30 deg sector
    ///to get the correct index
    if(iProcessor==5 && aSector<3) aMin = 0;
    break;
  }
  case MuonSubdetId::CSC: {   
    CSCDetId csc(rawId);    
    aSector = csc.chamber();    
    aMin = OMTFConfiguration::endcap10DegMin[iProcessor];       
    ///on the 0-2pi border we need to add 4 10deg sectors
    ///to get the correct index
    if(iProcessor==5 && aSector<5) aMin = -3;
    ///Endcap region covers alsgo 10 deg sectors
    ///on the 0-2pi border we need to add 2 20deg sectors
    ///to get the correct index
    if( (type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) &&
	csc.station()>1 && csc.ring()==1){
      aMin = OMTFConfiguration::endcap20DegMin[iProcessor];
      if(iProcessor==5 && aSector<3) aMin = -1;
    }
    break;
  }
  }

  ///Assume 2 hits per chamber
  iInput = (aSector - aMin)*nInputsPerSector;
  ///Chambers divided into two rolls have rolls number 1 and 3
  iInput+=iRoll-1;

  return iInput;
}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFinputMaker::processDT(const L1MuDTChambPhContainer *dtPhDigis,
	       const L1MuDTChambThContainer *dtThDigis,
	       unsigned int iProcessor,
	       l1t::tftype type){

  if(!dtPhDigis) return;

  for (const auto digiIt: *dtPhDigis->getContainer()) {

    DTChamberId detid(digiIt.whNum(),digiIt.stNum(),digiIt.scNum()+1);

    ///Check it the data fits into given processor input range
    if(!acceptDigi(detid.rawId(), iProcessor, type)) continue;
    ///Check Trigger primitive quality
    ///Ts2Tag() == 0 - take only first track from DT Trigger Server
    ///BxCnt()  == 0 - ??
    ///code()>=3     - take only double layer hits, HH, HL and LL
    if (digiIt.bxNum()!= 0 || digiIt.BxCnt()!= 0 || digiIt.Ts2Tag()!= 0 || digiIt.code()<4) continue;

    unsigned int hwNumber = OMTFConfiguration::getLayerNumber(detid.rawId());
    if(OMTFConfiguration::hwToLogicLayer.find(hwNumber)==OMTFConfiguration::hwToLogicLayer.end()) continue;
    
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];   
    //int iPhi =  katownik->getGlobalPhi(detid.rawId(), digiIt);
    int iPhi =  katownik->getProcessorPhi(iProcessor, type, digiIt);
    int iEta =  katownik->getGlobalEta(detid.rawId(), digiIt, dtThDigis);
    unsigned int iInput= getInputNumber(detid.rawId(), iProcessor, type);

    myInput->addLayerHit(iLayer,iInput,iPhi,iEta);
    myInput->addLayerHit(iLayer+1,iInput,digiIt.phiB(),iEta);
  }
  
}
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFinputMaker::processCSC(const CSCCorrelatedLCTDigiCollection *cscDigis,
	       unsigned int iProcessor,
	       l1t::tftype type){

  if(!cscDigis) return;

  auto chamber = cscDigis->begin();
  auto chend  = cscDigis->end();
  for( ; chamber != chend; ++chamber ) {

    unsigned int rawid = (*chamber).first;
    ///Check it the data fits into given processor input range
    if(!acceptDigi(rawid, iProcessor, type)) continue;

    auto digi = (*chamber).second.first;
    auto dend = (*chamber).second.second;    
    for( ; digi != dend; ++digi ) {

      ///Check Trigger primitive quality.
      ///CSC central BX is 6 for some reason.
      if (abs(digi->getBX()- 6)>0) continue;
      
      unsigned int hwNumber = OMTFConfiguration::getLayerNumber(rawid);
      if(OMTFConfiguration::hwToLogicLayer.find(hwNumber)==OMTFConfiguration::hwToLogicLayer.end()) continue;

      unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];      
      //int iPhi = katownik->getGlobalPhi(rawid, *digi);
      int iPhi = katownik->getProcessorPhi(iProcessor, type, CSCDetId(rawid), *digi);
      int iEta = katownik->getGlobalEta(rawid, *digi);
      ///Accept CSC digis only up to eta=1.26.
      ///The nominal OMTF range is up to 1.24, but cutting at 1.24
      ///kill efficnency at the edge. 1.26 is one eta bin above nominal.
      if(abs(iEta)>1.26/2.61*240) continue;
      unsigned int iInput= getInputNumber(rawid, iProcessor, type);      
      myInput->addLayerHit(iLayer,iInput,iPhi,iEta);     
    }
  }      
}
////////////////////////////////////////////
////////////////////////////////////////////
bool rpcPrimitiveCmp(const  RPCDigi &a,
		     const  RPCDigi &b) { return a.strip() < b.strip(); };
////////////////////////////////////////////
////////////////////////////////////////////
void OMTFinputMaker::processRPC(const RPCDigiCollection *rpcDigis,
				unsigned int iProcessor,
				l1t::tftype type){

  if(!rpcDigis) return;
  std::stringstream str;

  const RPCDigiCollection & rpcDigiCollection = *rpcDigis;
  for (auto rollDigis : rpcDigiCollection) {
    RPCDetId roll = rollDigis.first;
    unsigned int rawid = roll.rawId();

    if(!acceptDigi(rawid, iProcessor, type)) continue;
    ///Find clusters of consecutive fired strips.
    ///Have to copy the digis in chamber to sort them (not optimal).
    ///NOTE: when copying I select only digis with bx==0       //FIXME: find a better place/way to filtering digi against quality/BX etc.
    std::vector<RPCDigi> digisCopy;
    std::copy_if(rollDigis.second.first, rollDigis.second.second, std::back_inserter(digisCopy), [](const RPCDigi & aDigi){return (aDigi.bx()==0);});
    std::sort(digisCopy.begin(),digisCopy.end(),rpcPrimitiveCmp);
    typedef std::pair<RPCDigi & , RPCDigi & > Cluster;
    std::vector<Cluster> clusters;
    for(auto & digi: digisCopy) {
      if(clusters.empty()) clusters.push_back(Cluster(digi,digi));
      else if (digi.strip() - clusters.back().second.strip() == 1) clusters.back().second = digi;
      else if (digi.strip() - clusters.back().second.strip() > 1) clusters.push_back(Cluster(digi,digi));
    }

    for (auto & cluster: clusters) {
      int iPhiHalfStrip1 = katownik->getProcessorPhi(iProcessor, type, roll, cluster.first);
      int iPhiHalfStrip2 = katownik->getProcessorPhi(iProcessor, type, roll, cluster.second);
      int iPhi = (iPhiHalfStrip1+iPhiHalfStrip2)/2;
      int iEta =  katownik->getGlobalEta(rawid, cluster.first);
      unsigned int hwNumber = OMTFConfiguration::getLayerNumber(rawid);
      unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
      unsigned int iInput= getInputNumber(rawid, iProcessor, type);

      myInput->addLayerHit(iLayer,iInput,iPhi,iEta);

      str<<" RPC halfDigi "
           <<" begin: "<<cluster.first.strip()<<" end: "<<cluster.second.strip()
           <<" iPhi: "<<iPhi
           <<" iEta: "<<iEta
           <<" hwNumber: "<<hwNumber
           <<" iInput: "<<iInput
           <<" iLayer: "<<iLayer
           <<std::endl;
    }
  }

  edm::LogInfo("OMTFInputMaker")<<str.str();
}
////////////////////////////////////////////
////////////////////////////////////////////
const OMTFinput * OMTFinputMaker::buildInputForProcessor(const L1MuDTChambPhContainer *dtPhDigis,
							 const L1MuDTChambThContainer *dtThDigis,
							 const CSCCorrelatedLCTDigiCollection *cscDigis,
							 const RPCDigiCollection *rpcDigis,
							 unsigned int iProcessor,
							 l1t::tftype type){  
  myInput->clear();	

  processDT(dtPhDigis, dtThDigis, iProcessor, type);
  processCSC(cscDigis, iProcessor, type);
  processRPC(rpcDigis, iProcessor, type);

  return myInput;

}
////////////////////////////////////////////
////////////////////////////////////////////

