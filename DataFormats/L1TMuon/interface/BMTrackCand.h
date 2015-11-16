//-------------------------------------------------
//
//   Class BMTrackCand
//
//   Description: output data for BMTF trigger
//
//
//   Author List: Jorge Troconiz  UAM Madrid
//
//
//--------------------------------------------------
#ifndef L1MuBMTrackCand_H
#define L1MuBMTrackCand_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------


//----------------------
// Base Class Headers --
//----------------------

//#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"

//---------------
// C++ Headers --
//---------------

//              ---------------------
//              -- Class Interface --
//              ---------------------

class BMTrackCand: public l1t::RegionalMuonCand {

 public:

  //  Constructors
  BMTrackCand();

  //BMTrackCand( unsigned dataword, int bx, int uwh, int usc, int utag,
  //               int adr1, int adr2, int adr3, int adr4, int utc );

  //BMTrackCand( unsigned type_idx, unsigned phi, unsigned eta, unsigned pt, unsigned charge,
  //		   unsigned ch_valid, unsigned finehalo, unsigned quality, int bx,
  //                 int uwh, int usc, int utag, int adr1, int adr2, int adr3, int adr4 );
BMTrackCand( int pt, int phi, int eta, int charge, int quality, int bx,
                                  int uwh, int usc, int utag,
                                  int adr1, int adr2, int adr3, int adr4, int utc ) ;
  //  Destructor
  ~BMTrackCand();

  // Operations
  int whNum()        const;
  int scNum()        const;
  int stNum(int ust) const;
  int TCNum()        const;
  int TrkTag()       const;
  int bx()           const;

  void setTC();
  void setAdd(int ust);
  void setBx(int bx) {m_bx = bx;}

 private:

  int wheel;
  int sector;
  int TrkTagCode;
  int TClassCode;
  int TrkAdd[4];
  int m_bx;

};

#endif
