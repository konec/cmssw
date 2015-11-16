#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "OMTFProducerMix.h"
DEFINE_FWK_MODULE(OMTFProducerMix);

#include "L1TMuonOverlapTrackProducer.h"
DEFINE_FWK_MODULE(L1TMuonOverlapTrackProducer);

#include "OMTFPatternMaker.h"
DEFINE_FWK_MODULE(OMTFPatternMaker);

#include "L1TMTFOverlapParamsDBProducer.h"
DEFINE_FWK_MODULE(L1MTFOverlapParamsDBProducer);

#include "L1TMTFOverlapParamsESProducer.h"
DEFINE_FWK_EVENTSETUP_MODULE(L1TMTFOverlapParamsESProducer);
