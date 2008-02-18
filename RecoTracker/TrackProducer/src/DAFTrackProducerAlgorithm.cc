#include "RecoTracker/TrackProducer/interface/DAFTrackProducerAlgorithm.h"
#include "RecoTracker/SiTrackerMRHTools/interface/SiTrackerMultiRecHitUpdator.h"
#include "RecoTracker/SiTrackerMRHTools/interface/MultiRecHitCollector.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"

#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "TrackingTools/PatternTools/interface/TrajectoryFitter.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/InvalidTransientRecHit.h"
#include "TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateWithArbitraryError.h"

#include "Utilities/General/interface/CMSexception.h"

void DAFTrackProducerAlgorithm::runWithCandidate(const TrackingGeometry * theG,
					         const MagneticField * theMF,
					         const TrackCandidateCollection& theTCCollection,
					         const TrajectoryFitter * theFitter,
					         const TransientTrackingRecHitBuilder* builder,
						 const MultiRecHitCollector* measurementCollector,
						 const SiTrackerMultiRecHitUpdator* updator,
					         AlgoProductCollection& algoResults) const
{
  edm::LogInfo("TrackProducer") << "Number of TrackCandidates: " << theTCCollection.size() << "\n";

  int cont = 0;
  for (TrackCandidateCollection::const_iterator i=theTCCollection.begin(); i!=theTCCollection.end();i++){
    
      
      const TrackCandidate * theTC = &(*i);
      PTrajectoryStateOnDet state = theTC->trajectoryStateOnDet();
      const TrackCandidate::range& recHitVec=theTC->recHits();
      const TrajectorySeed& seed = theTC->seed();

      TrajectoryStateTransform transformer;
  
      DetId  detId(state.detId());
      TrajectoryStateOnSurface theTSOS = transformer.transientState( state,
								     &(theG->idToDet(detId)->surface()), 
								     theMF);

      LogDebug("TrackProducer") << "Initial TSOS\n" << theTSOS << "\n";
      
      //convert the TrackingRecHit vector to a TransientTrackingRecHit vector
      TransientTrackingRecHit::RecHitContainer hits;
      
      float ndof=0;
      LogDebug("DAFTrackProducerAlgorithm") << "about to build the rechits for the first round...."; 	    
      for (edm::OwnVector<TrackingRecHit>::const_iterator i=recHitVec.first;
	   i!=recHitVec.second; i++){
	hits.push_back(builder->build(&(*i) ));
      }
      LogDebug("DAFTrackProducerAlgorithm") << "done" << std::endl;
      //first of all do a fit-smooth round to get the trajectory
      LogDebug("DAFTrackProducerAlgorithm") << "About to build tha trajectory for the first round....";  	
      //theMRHChi2Estimator->setAnnealingFactor(1);
      std::vector<Trajectory> vtraj = theFitter->fit(seed, hits, theTSOS);
      LogDebug("DAFTrackProducerAlgorithm") << "done" << std::endl;	
      LogDebug("DAFTrackProducerAlgorithm") << "after the first round found " << vtraj.size() << " trajectories"  << std::endl; 

      if (vtraj.size() != 0){
	//bool isFirstIteration=true;
	std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> hits = collectHits(vtraj, measurementCollector);
	fit(hits, theFitter, vtraj);

	for (std::vector<double>::const_iterator ian = updator->getAnnealingProgram().begin(); ian != updator->getAnnealingProgram().end(); ian++){
		if (vtraj.size()){
			LogDebug("DAFTrackProducerAlgorithm") << "Seed direction is " << vtraj.front().seed().direction() 
							      << "  Traj direction is " << vtraj.front().direction(); 
			std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> curiterationhits = updateHits(vtraj,updator,*ian);
			fit(curiterationhits, theFitter, vtraj);
				
			LogDebug("DAFTrackProducerAlgorithm") << "done annealing value "  <<  (*ian) << " with " << vtraj.size() << " trajectories";
		} else break;
		LogDebug("DAFTrackProducerAlgorithm") << "number of trajectories after annealing value " << (*ian) << " annealing step " << vtraj.size();
	}
	LogDebug("DAFTrackProducerAlgorithm") << "Ended annealing program with " << vtraj.size() << " trajectories" << std::endl;

	//check the trajectory to see that the number of valid hits with 
	//reasonable weight (>1e-6) is higher than the minimum set in the DAFTrackProducer.
	//This is a kind of filter to remove tracks with too many outliers.
	//Outliers are mrhits with all components with weight less than 1e-6 
  
	std::vector<Trajectory> filtered;
	filter(theFitter, vtraj, conf_.getParameter<int>("MinHits"), filtered);				
	ndof = calculateNdof(filtered);	
        bool ok = buildTrack(filtered, algoResults, ndof);
        if(ok) cont++;

      }
  }
  edm::LogInfo("TrackProducer") << "Number of Tracks found: " << cont << "\n";
}

std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface> 
DAFTrackProducerAlgorithm::collectHits(const std::vector<Trajectory>& vtraj, 
    				       const MultiRecHitCollector* measurementCollector) const{
	TransientTrackingRecHit::RecHitContainer hits;
	std::vector<TrajectoryMeasurement> collectedmeas = measurementCollector->recHits(vtraj.front());
        if (collectedmeas.empty()) return std::make_pair(TransientTrackingRecHit::RecHitContainer(), TrajectoryStateOnSurface());
        for (std::vector<TrajectoryMeasurement>::const_iterator iter = collectedmeas.begin(); iter!=collectedmeas.end(); iter++){
        	hits.push_back(iter->recHit());
        }
        return std::make_pair(hits,TrajectoryStateWithArbitraryError()(collectedmeas.front().predictedState()));	
}

std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface>
DAFTrackProducerAlgorithm::updateHits(const std::vector<Trajectory>& vtraj,
				      const SiTrackerMultiRecHitUpdator* updator,
				      double annealing) const {
	TransientTrackingRecHit::RecHitContainer hits;
	std::vector<TrajectoryMeasurement> vmeas = vtraj.front().measurements();
        std::vector<TrajectoryMeasurement>::reverse_iterator imeas;
        for (imeas = vmeas.rbegin(); imeas != vmeas.rend(); imeas++){
                TransientTrackingRecHit::RecHitPointer updated = updator->update(imeas->recHit(), imeas->updatedState(), annealing);
                hits.push_back(updated);
        }
        return std::make_pair(hits,TrajectoryStateWithArbitraryError()(vmeas.back().updatedState()));
}

void DAFTrackProducerAlgorithm::fit(const std::pair<TransientTrackingRecHit::RecHitContainer, TrajectoryStateOnSurface>& hits, 
				    const TrajectoryFitter * theFitter,
				    std::vector<Trajectory>& vtraj) const {
        std::vector<Trajectory> newVec = theFitter->fit(TrajectorySeed(PTrajectoryStateOnDet(),
                                                                       BasicTrajectorySeed::recHitContainer(),
                                                                       vtraj.front().seed().direction()),
                                                        hits.first,
                                                        hits.second);
        vtraj.reserve(newVec.size());
        vtraj.swap(newVec);
        LogDebug("DAFTrackProducerAlgorithm") << "swapped!" << std::endl;
}


bool DAFTrackProducerAlgorithm::buildTrack (const std::vector<Trajectory>& vtraj,
					    AlgoProductCollection& algoResults,
					    float ndof) const{
  //variable declarations
  reco::Track * theTrack;
  Trajectory * theTraj; 
      
  //perform the fit: the result's size is 1 if it succeded, 0 if fails
  
  
  //LogDebug("TrackProducer") <<" FITTER FOUND "<< vtraj.size() << " TRAJECTORIES" <<"\n";
  LogDebug("DAFTrackProducerAlgorithm") <<" FITTER FOUND "<< vtraj.size() << " TRAJECTORIES" << std::endl;;
  TrajectoryStateOnSurface innertsos;
  
  if (vtraj.size() != 0){

    theTraj = new Trajectory( vtraj.front() );
    
    if (theTraj->direction() == alongMomentum) {
    //if (theTraj->direction() == oppositeToMomentum) {
      innertsos = theTraj->firstMeasurement().updatedState();
      //std::cout << "Inner momentum " << innertsos.globalParameters().momentum().mag() << std::endl;	
    } else { 
      innertsos = theTraj->lastMeasurement().updatedState();
    }
    
    
    TSCPBuilderNoMaterial tscpBuilder;

    TrajectoryStateClosestToPoint tscp = tscpBuilder(*(innertsos.freeState()),
						     GlobalPoint(0,0,0) );//FIXME Correct?
    
    GlobalPoint v = tscp.theState().position();
    math::XYZPoint  pos( v.x(), v.y(), v.z() );
    GlobalVector p = tscp.theState().momentum();
    math::XYZVector mom( p.x(), p.y(), p.z() );

    LogDebug("TrackProducer") <<v<<p<<std::endl;

    theTrack = new reco::Track(theTraj->chiSquared(),
			       ndof, //in the DAF the ndof is not-integer
			       pos, mom, tscp.charge(), tscp.theState().curvilinearError());


    LogDebug("TrackProducer") <<"track done\n";

    AlgoProduct aProduct(theTraj,std::make_pair(theTrack,theTraj->direction()));
    LogDebug("TrackProducer") <<"track done1\n";
    algoResults.push_back(aProduct);
    LogDebug("TrackProducer") <<"track done2\n";
    
    return true;
  } 
  else  return false;
}

void  DAFTrackProducerAlgorithm::filter(const TrajectoryFitter* fitter, std::vector<Trajectory>& input, int minhits, std::vector<Trajectory>& output) const {
	if (input.empty()) return;

	int ngoodhits = 0;

	std::vector<TrajectoryMeasurement> vtm = input[0].measurements();	

	TransientTrackingRecHit::RecHitContainer hits;

	//count the number of non-outlier and non-invalid hits	
	for (std::vector<TrajectoryMeasurement>::reverse_iterator tm=vtm.rbegin(); tm!=vtm.rend();tm++){
		//if the rechit is valid
		if (tm->recHit()->isValid()) {
			TransientTrackingRecHit::ConstRecHitContainer components = tm->recHit()->transientHits();
			bool isGood = false;
			for (TransientTrackingRecHit::ConstRecHitContainer::iterator rechit = components.begin(); rechit != components.end(); rechit++){
				//if there is at least one component with weight higher than 1e-6 then the hit is not an outlier
				if ((*rechit)->weight()>1e-6) {ngoodhits++; isGood = true; break;}
			}
			if (isGood) hits.push_back(tm->recHit()->clone(tm->updatedState()));
                        else hits.push_back(InvalidTransientRecHit::build(tm->recHit()->det(), TrackingRecHit::missing));
		} else {
			hits.push_back(tm->recHit()->clone(tm->updatedState()));
		}
	}


	LogDebug("DAFTrackProducerAlgorithm") << "Original number of valid hits " << input[0].foundHits() << "; after filtering " << ngoodhits;
	//debug
	if (ngoodhits>input[0].foundHits()) edm::LogError("DAFTrackProducerAlgorithm") << "Something wrong: the number of good hits from DAFTrackProducerAlgorithm::filter " << ngoodhits << " is higher than the original one " << input[0].foundHits();
	
	if (ngoodhits < minhits) return;	

	TrajectoryStateOnSurface curstartingTSOS = input.front().lastMeasurement().updatedState();
	LogDebug("DAFTrackProducerAlgorithm") << "starting tsos for final refitting " << curstartingTSOS ;
        //curstartingTSOS.rescaleError(100);

	output = fitter->fit(TrajectorySeed(PTrajectoryStateOnDet(),
                                                BasicTrajectorySeed::recHitContainer(),
                                                input.front().seed().direction()),
                                hits,
                                TrajectoryStateWithArbitraryError()(curstartingTSOS));
	LogDebug("DAFTrackProducerAlgorithm") << "After filtering " << output.size() << " trajectories";

}

float DAFTrackProducerAlgorithm::calculateNdof(const std::vector<Trajectory>& vtraj) const {
	if (vtraj.empty()) return 0;
	float ndof = 0;
	const std::vector<TrajectoryMeasurement>& meas = vtraj.front().measurements();
	for (std::vector<TrajectoryMeasurement>::const_iterator iter = meas.begin(); iter != meas.end(); iter++){
		if (iter->recHit()->isValid()){
			TransientTrackingRecHit::ConstRecHitContainer components = iter->recHit()->transientHits();
                        TransientTrackingRecHit::ConstRecHitContainer::const_iterator iter2;
                        for (iter2 = components.begin(); iter2 != components.end(); iter2++){
                                if ((*iter2)->isValid())ndof+=((*iter2)->dimension())*(*iter2)->weight();
                        }
		}
	}
	return ndof-5;
}
