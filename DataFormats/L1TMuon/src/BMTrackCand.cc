//-------------------------------------------------
//
//   Class BMTrackCand
//
//   Description: ouput data for BMTF trigger
//
//
//   Author List: Jorge Troconiz  UAM Madrid
//
//
//--------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "DataFormats/L1TMuon/interface/BMTrackCand.h"
//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//---------------
// C++ Headers --
//---------------
using namespace std;

//-------------------
// Initializations --
//-------------------


//----------------
// Constructors --
//----------------
BMTrackCand::BMTrackCand() : l1t::RegionalMuonCand() {

  wheel           = 0;
  sector          = 0;
  TrkTagCode      = -1;
  TrkAdd[0]       =  3;
  TrkAdd[1]       = 15;
  TrkAdd[2]       = 15;
  TrkAdd[3]       = 15;
  TClassCode      = 11;
}



BMTrackCand::BMTrackCand( int pt, int phi, int eta, int charge, int quality, int bx,
                                  int uwh, int usc, int utag,
                                  int adr1, int adr2, int adr3, int adr4, int utc )  {

  setHwPhi(phi);
  setHwEta(eta);
  setHwPt(pt);
  setHwSign(charge);
  setHwSignValid(1);
  setHwQual(quality);
  setBx(bx);

 setTFIdentifiers(usc,l1t::tftype::bmtf );


  wheel           = uwh;
  sector          = usc;
  TrkTagCode      = utag;
  TrkAdd[0]       = adr1;
  TrkAdd[1]       = adr2;
  TrkAdd[2]       = adr3;
  TrkAdd[3]       = adr4;

  setAdd(1);
  setAdd(2);
  setAdd(3);
  setAdd(4);

  // setHwTrackAddress(TrkAdd[0]*1000000 + TrkAdd[1]*10000 + TrkAdd[2]*100 + TrkAdd[3] );

  setTrackSubAddress(RegionalMuonCand::kWheel, uwh);
  setTrackSubAddress(RegionalMuonCand::kStat1, adr1);
  setTrackSubAddress(RegionalMuonCand::kStat2, adr2);
  setTrackSubAddress(RegionalMuonCand::kStat3, adr3);
  setTrackSubAddress(RegionalMuonCand::kStat4, adr4);

  TClassCode      = utc;
  setTC();
}


//--------------
// Destructor --
//--------------
BMTrackCand::~BMTrackCand() {
}

//--------------
// Operations --
//--------------
int BMTrackCand::whNum() const {
  return wheel;
}

int BMTrackCand::bx() const {
  return m_bx;
}

int BMTrackCand::scNum() const {
  return sector;
}

int BMTrackCand::stNum(int ust) const {
  return TrkAdd[ust-1];
}

int BMTrackCand::TCNum() const {
  return TClassCode;
}

int BMTrackCand::TrkTag() const {
  return TrkTagCode;
}

void BMTrackCand::setTC() {
  unsigned int uqua = hwQual(); //quality_packed();

  switch (uqua) {
    case 7:  { TClassCode =  0; break; }
    case 6:  { TClassCode =  1;
               if ( TrkAdd[3] != 15 ) TClassCode = 2;
               break; }
    case 5:  { TClassCode =  3; break; }
    case 4:  { TClassCode =  4; break; }
    case 3:  { TClassCode =  5;
               if ( TrkAdd[3] != 15 ) TClassCode = 6;
               if ( TrkAdd[2] != 15 ) TClassCode = 7;
               break; }
    case 2:  { TClassCode =  8;
               if ( TrkAdd[2] != 15 ) TClassCode = 9;
               break; }
    case 1:  { TClassCode = 10; break; }
    default: { TClassCode = 11; break; }
  }
}

void BMTrackCand::setAdd(int ust) {
  unsigned int uadd = stNum(ust);

  switch (uadd) {
    case  0:  { TrkAdd[ust-1] =   8; break; }
    case  1:  { TrkAdd[ust-1] =   9; break; }
    case  2:  { TrkAdd[ust-1] =   0; break; }
    case  3:  { TrkAdd[ust-1] =   1; break; }
    case  4:  { TrkAdd[ust-1] =  10; break; }
    case  5:  { TrkAdd[ust-1] =  11; break; }
    case  6:  { TrkAdd[ust-1] =   2; break; }
    case  7:  { TrkAdd[ust-1] =   3; break; }
    case  8:  { TrkAdd[ust-1] =  12; break; }
    case  9:  { TrkAdd[ust-1] =  13; break; }
    case 10:  { TrkAdd[ust-1] =   4; break; }
    case 11:  { TrkAdd[ust-1] =   5; break; }
    case 15:  { TrkAdd[ust-1] =  15; break; }
    default:  { TrkAdd[ust-1] =  15; break; }
  }

  if (ust!=1) return;

  switch (uadd) {
    case  0:  { TrkAdd[0] =  2; break; }
    case  1:  { TrkAdd[0] =  1; break; }
    case 15:  { TrkAdd[0] =  3; break; }
    default:  { TrkAdd[0] =  3; break; }
  }
}
