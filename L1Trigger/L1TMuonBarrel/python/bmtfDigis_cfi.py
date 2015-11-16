import FWCore.ParameterSet.Config as cms

bmtfDigis = cms.EDProducer("L1TMuonBarrelTrackProducer",
    Debug = cms.untracked.int32(0),
    DTDigi_Source = cms.InputTag("dtTriggerPrimitiveDigis"),
    DTDigi_Theta_Source = cms.InputTag("simDtTriggerPrimitiveDigis"),
)
