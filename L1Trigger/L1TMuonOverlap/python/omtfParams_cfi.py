import FWCore.ParameterSet.Config as cms

###OMTF ESProducer. Fills CondFormats from XML files.
omtfParamsSource = cms.ESSource(
    "EmptyESSource",
    recordName = cms.string('L1TMTFOverlapParamsRcd'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
)


omtfParams = cms.ESProducer(
    "L1TMTFOverlapParamsESProducer",
    configFromXML = cms.bool(False),   
    patternsXMLFiles = cms.VPSet(
        cms.PSet(patternsXMLFile = cms.FileInPath("L1Trigger/L1TMuonOverlap/data/Patterns_ipt6_31_750_4x.xml")),
    ),
    configXMLFile = cms.FileInPath("L1Trigger/L1TMuonOverlap/data/hwToLogicLayer_750.xml"),
)




