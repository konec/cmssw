import FWCore.ParameterSet.Config as cms

###OMTF emulator configuration
omtfEmulator = cms.EDProducer("L1TMuonOverlapTrackProducer",
                              srcDTPh = cms.InputTag('simDtTriggerPrimitiveDigis'),
                              srcDTTh = cms.InputTag('simDtTriggerPrimitiveDigis'),
                              srcCSC = cms.InputTag('simCscTriggerPrimitiveDigis','MPCSORTED'),
                              srcRPC = cms.InputTag('simMuonRPCDigis'),                              
                              dumpResultToXML = cms.bool(False),
                              dumpDetailedResultToXML = cms.bool(False),
                              XMLDumpFileName = cms.string("TestEvents.xml"),                                     
                              dumpGPToXML = cms.bool(False),  
                              readEventsFromXML = cms.bool(False),
                              eventsXMLFiles = cms.vstring("TestEvents.xml"),
                              dropRPCPrimitives = cms.bool(False),                                    
                              dropDTPrimitives = cms.bool(False),                                    
                              dropCSCPrimitives = cms.bool(False),   
                              omtf = cms.PSet(
                                  configFromXML = cms.bool(False),   
                                  patternsXMLFiles = cms.VPSet(
                                       cms.PSet(patternsXMLFile = cms.FileInPath("L1Trigger/L1TMuonOverlap/data/Patterns_ipt6_31_750_4x.xml")),
                                      ),
                                  configXMLFile = cms.FileInPath("L1Trigger/L1TMuonOverlap/data/hwToLogicLayer_750.xml"),
                              )
)


