/*
 * SimpleRuntimeManager.cc
 *
 *  Created on: Oct 30, 2018
 *      Author: ahad
 */

#include "veins/modules/application/platooning/apps/BaseApp.h"
#include "SimpleRuntimeManager.h"


//=================================================================================================================================//
// Constructor
//=================================================================================================================================//
SimpleRuntimeManager::SimpleRuntimeManager(BaseApp *app) : BaseRuntimeManager(app) {
    // TODO Auto-generated constructor stub

}


//=================================================================================================================================//
// Destructor
//=================================================================================================================================//
SimpleRuntimeManager::~SimpleRuntimeManager() {
    // TODO Auto-generated destructor stub
    // Nothing to delete here. BaseRuntimeManager will delete the allocated memory
    // As this derived class does not have any dynamically allocated memory
}

//=================================================================================================================================//
// override virtual mehtods
//=================================================================================================================================//
void SimpleRuntimeManager::monitor() {
    // If there is no connection neither to front or leader vehicle,
    // there is no need to call StateManager to check for connection
    // Leader vehicle's rtState will always be CAR2FRONT_CAR2LEADER_DISENGAGED
    // and the controller will be ACC. Hence,
    // Leader vehicle does not need to check the connection
    if(rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_DISENGAGED) return;

    switch(traciVehicle->getActiveController()) {
    case Plexe::ACC:
        stateManager->accStateManager();
        break;
    case Plexe::PLOEG:
        stateManager->ploegStateManager();
        break;
    case Plexe::CACC:
        stateManager->caccStateManager();
        break;
    default:
        std::cerr << "Error : Unrecognizable Active Controller +/ not considered yet in : "
                  << "\n\tFile: "
                  << __FILE__
                  << "\n\tFunction: "
                  << __func__
                  << "\n\tLine: "
                  << __LINE__
                  << std::endl;
        break;
    }
}

void SimpleRuntimeManager::record(const int sourceVehicleId, simtime_t currentSimTime){

    updateSafetyRecords(sourceVehicleId, currentSimTime);

    if (sourceVehicleId == positionHelper->getLeaderId() || sourceVehicleId == positionHelper->getFrontId()) {
        // this is the leader or the front vehicle
        updateStateMachine(sourceVehicleId, currentSimTime);
        // after updating the StateMachine and the stored information, call the monitor() to monitor the check state stability
        monitor();
    }
}


void SimpleRuntimeManager::updateStateMachine(const int sourceVehicleId, const simtime_t currentSimTime) {
    switch(traciVehicle->getActiveController()) {
    case Plexe::ACC:
        // sanity check
        ASSERT(rtState != RTStateMachine::CAR2FRONT_ENGAGED);
        ASSERT(rtState != RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED);

        // Update the RTStateMachine
        if(rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_DISENGAGED) {
            if (sourceVehicleId == positionHelper->getLeaderId()) {
                rtState = (positionHelper->getLeaderId() == positionHelper->getFrontId()) ? RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED :
                        RTStateMachine::CAR2LEADER_ENGAGED;
            } else if(sourceVehicleId == positionHelper->getFrontId()) {
                rtState = (positionHelper->getLeaderId() == positionHelper->getFrontId()) ? RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED :
                                        RTStateMachine::CAR2FRONT_ENGAGED;
            } else {
                std::cerr << "Error : wrong Vehicle Id"
                          << "\n\tFile: "
                          << __FILE__
                          << "\n\tFunction: "
                          << __func__
                          << "\n\tLine: "
                          << __LINE__
                          << std::endl;
            }
        } else if(rtState == RTStateMachine::CAR2LEADER_ENGAGED) {
            // Sanity check
            ASSERT(positionHelper->getFrontId() != positionHelper->getLeaderId());

            if (sourceVehicleId == positionHelper->getLeaderId()) return;

            if(sourceVehicleId == positionHelper->getFrontId()) {
                rtState = RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED;
            } else {
                std::cerr << "Error : wrong Vehicle Id"
                          << "\n\tFile: "
                          << __FILE__
                          << "\n\tFunction: "
                          << __func__
                          << "\n\tLine: "
                          << __LINE__
                          << std::endl;
            }

        } else {
            std::cerr << "Error : wrong rtState"
                      << "\n\tFile: "
                      << __FILE__
                      << "\n\tFunction: "
                      << __func__
                      << "\n\tLine: "
                      << __LINE__
                      << std::endl;
        }
        break;
    case Plexe::PLOEG:
        // Sanity Check
        ASSERT(rtState != RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED);
        ASSERT((rtState != RTStateMachine::CAR2LEADER_ENGAGED));

        if(rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_DISENGAGED) {
            if(sourceVehicleId == positionHelper->getLeaderId()) {
                rtState = (positionHelper->getLeaderId() == positionHelper->getFrontId()) ? RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED :
                                        RTStateMachine::CAR2LEADER_ENGAGED;
            } else if(sourceVehicleId == positionHelper->getFrontId()) {
                rtState = (positionHelper->getLeaderId() == positionHelper->getFrontId()) ? RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED :
                                                        RTStateMachine::CAR2FRONT_ENGAGED;
            } else {
                std::cerr << "Error : wrong Vehicle Id"
                                          << "\n\tFile: "
                                          << __FILE__
                                          << "\n\tFunction: "
                                          << __func__
                                          << "\n\tLine: "
                                          << __LINE__
                                          << std::endl;
            }

//            if(positionHelper->getLeaderId() == positionHelper->getFrontId()) {
//                rtState = RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED;
//            } else {
//                // Sanity check
//                ASSERT(sourceVehicleId != positionHelper->getLeaderId());
//
//                rtState = RTStateMachine::CAR2FRONT_ENGAGED;
//            }

        } else if(rtState == RTStateMachine::CAR2FRONT_ENGAGED) {
            // Sanity check
            ASSERT(positionHelper->getFrontId() != positionHelper->getLeaderId());

            if(sourceVehicleId == positionHelper->getFrontId()) return;

            if (sourceVehicleId == positionHelper->getLeaderId()) {
                rtState = RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED;
            } else {
                std::cerr << "Error : wrong Vehicle Id"
                          << "\n\tFile: "
                          << __FILE__
                          << "\n\tFunction: "
                          << __func__
                          << "\n\tLine: "
                          << __LINE__
                          << std::endl;
            }

        } else {
            std::cerr << "Error : wrong rtState"
                                 << "\n\tFile: "
                                 << __FILE__
                                 << "\n\tFunction: "
                                 << __func__
                                 << "\n\tLine: "
                                 << __LINE__
                                 << std::endl;
        }

//        else if(rtState == RTStateMachine::CAR2LEADER_ENGAGED) {
//                    // Sanity check
//                    ASSERT(positionHelper->getFrontId() != positionHelper->getLeaderId());
//                    ASSERT(sourceVehicleId != positionHelper->getLeaderId());
//
//                    rtState = RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED;
//                }

        break;
    case Plexe::CACC:
        // Sanity check
        ASSERT(rtState != RTStateMachine::CAR2FRONT_ENGAGED);
        ASSERT(rtState != RTStateMachine::CAR2LEADER_ENGAGED);

        if (rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED) return;

        if(rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_DISENGAGED) {
            if(sourceVehicleId == positionHelper->getLeaderId()) {
                rtState = (positionHelper->getLeaderId() == positionHelper->getFrontId()) ? RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED :
                                        RTStateMachine::CAR2LEADER_ENGAGED;
            } else if(sourceVehicleId == positionHelper->getFrontId()) {
                rtState = (positionHelper->getLeaderId() == positionHelper->getFrontId()) ? RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED :
                                                        RTStateMachine::CAR2FRONT_ENGAGED;
            } else {
                std::cerr << "Error : wrong Vehicle Id"
                                          << "\n\tFile: "
                                          << __FILE__
                                          << "\n\tFunction: "
                                          << __func__
                                          << "\n\tLine: "
                                          << __LINE__
                                          << std::endl;
            }
        } else {
            std::cerr << "Error : wrong rtState"
                      << "\n\tFile: "
                      << __FILE__
                      << "\n\tFunction: "
                      << __func__
                      << "\n\tLine: "
                      << __LINE__
                      << std::endl;
        }


//        if(rtState == RTStateMachine::CAR2FRONT_ENGAGED) {
//                   // Sanity check
//                   ASSERT(positionHelper->getFrontId() != positionHelper->getLeaderId());
//                   if (sourceVehicleId == positionHelper->getFrontId()) return;
//
//                   rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED;
//               } else if(rtState == RTStateMachine::CAR2LEADER_ENGAGED) {
//                   // Sanity check
//                   ASSERT(positionHelper->getFrontId() != positionHelper->getLeaderId());
//                   if (sourceVehicleId == positionHelper->getLeaderId())return;
//
//                   rtState == RTStateMachine::CAR2FRONT_CAR2LEADER_ENGAGED;
//               }
        break;
    default:
        std::cerr << "Error : Unrecognizable Active Controller +/ not considered yet in : "
                  << "\n\tFile: "
                  << __FILE__
                  << "\n\tFunction: "
                  << __func__
                  << "\n\tLine: "
                  << __LINE__
                  << std::endl;
        break;

    }
}
void SimpleRuntimeManager::updateSafetyRecords(const int key, simtime_t currentSimTime) {
    if(safetyRecords.find(key) == safetyRecords.end()) {
        // This is the first time called of this method for this key during the simulation
        // or after connection to key is lost
        // create a new records for this key and insert it to the safetyRecords
        // TODO ADD ADDITIONAL SAFETY DATA IF REQUIRED
        BaseRuntimeManager::SafetyRecords safetyData;
        safetyData.lastBeaconArrivalTime = currentSimTime;
        safetyData.firstBeaconArrivalTime = currentSimTime;
        safetyData.nbeaconReceived = 1;
        safetyRecords.insert({key, safetyData});
    } else {
        // The record for the key is already exist
        auto safetyData = safetyRecords.find(key);
        safetyData->second.timeIntervalBetweenBeacon = currentSimTime - safetyData->second.lastBeaconArrivalTime;
        safetyData->second.lastBeaconArrivalTime = currentSimTime;
        safetyData->second.nbeaconReceived += 1;
        safetyData->second.avgBeaconInterval = (currentSimTime.dbl() - safetyData->second.firstBeaconArrivalTime.dbl()) / (double)(safetyData->second.nbeaconReceived - 1);


#ifdef DEBUG_RUNTIMEMANAGER
//        if(positionHelper->getId() == 1 && key == 0)
//        std::cout << "vehicle_" << positionHelper->getId() << " from vehicle_"
//                         << key << ":\n\tlastBeaconArrivalTime: " << safetyData->second.lastBeaconArrivalTime.dbl()
//                         << "\n\ttimeIntervalBetweenBeacon: "<< safetyData->second.timeIntervalBetweenBeacon.dbl()
//                         << "\n\tnbeaconReceived: " << safetyData->second.nbeaconReceived
//                         << "\n\tavgBeaconInterval: " << safetyData->second.avgBeaconInterval
//                         << std::endl;
#endif

    }

//#ifdef DEBUG_RUNTIMEMANAGER
//        std::cout << "Total No. of vehicle in recordData: " << safetyRecords.size() << std::endl;
//#endif

}








