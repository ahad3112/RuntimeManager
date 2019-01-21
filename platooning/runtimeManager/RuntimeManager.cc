//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <iostream>
#include <iomanip>
#include "veins/modules/application/platooning/runtimeManager/RuntimeManager.h"

using namespace Veins;
Define_Module(RuntimeManager);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RuntimeManager Member function's implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


RuntimeManager::RuntimeManager() {
#if DEBUG_RM
    std::cout << "Module :: RuntimeManager created." << std::endl;
#endif
}

RuntimeManager::~RuntimeManager() {
    if(rmParam.rmEnabled) {
        cancelAndDelete(monitoringMsg);
        monitoringMsg = nullptr;
    }
}

void RuntimeManager::initialize(int stage) {
    BaseApplLayer::initialize(stage);

    if(stage == 0) {
        // Read all the RMParameters
        rmParam.rmEnabled               = par("rmEnabled").boolValue();

        if(rmParam.rmEnabled) {
            rmParam.rmMonitorInterval     = par("rmMonitorInterval").doubleValue();
            rmParam.expectedBeconInterval = par("expectedBeconInterval").doubleValue();

            rmParam.nPacketLossPoor     = par("nPacketLossPoor").intValue();
            rmParam.nPacketLossCritical = par("nPacketLossCritical").intValue();

            rmParam.upgradationEnabled = par("upgradationEnabled").boolValue();
            rmParam.degradationEnabled = par("degradationEnabled").boolValue();
            rmParam.gapControlEnabled  = par("gapControlEnabled").boolValue();

            rmParam.minSafetyDistance = par("minSafetyDistance").doubleValue();
            rmParam.maxDeceleration   = par("maxDeceleration").doubleValue();


            rmParam.actionOnTransitionEnabled  = par("actionOnTransitionEnabled").boolValue();

            rmParam.accHeadwaytimeGap   = par("accHeadwaytimeGap").doubleValue();
            rmParam.ploegHeadwayTimeGap = par("ploegHeadwayTimeGap").doubleValue();
            rmParam.caccConstantSpacing = par("caccConstantSpacing").doubleValue();


            rmParam.emergencyPloegHeadwayTimeGapFactor = par("emergencyPloegHeadwayTimeGapFactor").doubleValue();
            rmParam.emergencyCaccConstantSpacingFactor = par("emergencyCaccConstantSpacingFactor").doubleValue();
        }
    }

    if(stage == 1) {
        if(!rmParam.rmEnabled) return;

        // Get the pointer to the Plexe API
        mobility = Veins::TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        traciVehicle = mobility->getVehicleCommandInterface();
        positionHelper = FindModule<BasePositionHelper*>::findSubModule(getParentModule());



#if DEBUG_RM
            std::cout << std::setw(110) << std::setfill('#') << "" << std::setfill(' ') << std::endl;
            std::cout << std::setw(38) << ""
                      << "VEHICLE " << positionHelper->getId() << " :: SIMULATION PARAMETERS"
                      << std::setw(38) << ""
                      << std::endl;
            std::cout << std::setw(110) << std::setfill('#') << "" << std::setfill(' ') << std::endl;

            std::cout << std::left;
            std::cout << std::setw(40) << "rmMonitorInterval: " << std::setw(10) << rmParam.rmMonitorInterval << std::endl;
            std::cout << std::setw(40) << "expectedBeconInterval: " << std::setw(10) << rmParam.expectedBeconInterval << std::endl;
            std::cout << std::setw(40) << "nPacketLossPoor: " << std::setw(10) << rmParam.nPacketLossPoor << std::endl;
            std::cout << std::setw(40) << "nPacketLossCritical: " << std::setw(10) << rmParam.nPacketLossCritical << std::endl;
            std::cout << std::setw(40) << "upgradationEnabled: " << std::setw(10) << std::boolalpha << rmParam.upgradationEnabled << std::endl;
            std::cout << std::setw(40) << "degradationEnabled: " << std::setw(10) << std::boolalpha << rmParam.degradationEnabled << std::endl;
            std::cout << std::setw(40) << "gapControlEnabled: " << std::setw(10) << std::boolalpha << rmParam.gapControlEnabled << std::endl;
            std::cout << std::setw(40) << "minSafetyDistance: " << std::setw(10) << rmParam.minSafetyDistance << std::endl;
            std::cout << std::setw(40) << "maxDeceleration: " << std::setw(10) << rmParam.maxDeceleration << std::endl;
            std::cout << std::setw(40) << "actionOnTransitionEnabled: " << std::setw(10) << std::boolalpha << rmParam.actionOnTransitionEnabled << std::endl;
            std::cout << std::setw(40) << "accHeadwaytimeGap: " << std::setw(10) << rmParam.accHeadwaytimeGap << std::endl;
            std::cout << std::setw(40) << "ploegHeadwayTimeGap: " << std::setw(10) << rmParam.ploegHeadwayTimeGap << std::endl;
            std::cout << std::setw(40) << "caccConstantSpacing: " << std::setw(10) << rmParam.caccConstantSpacing << std::endl;
            std::cout << std::setw(40) << "emergencyCaccConstantSpacingFactor: "<< std::setw(10)  << rmParam.emergencyCaccConstantSpacingFactor << std::endl;
            std::cout << std::setw(40) << "emergencyPloegHeadwayTimeGapFactor: " << std::setw(10) << rmParam.emergencyPloegHeadwayTimeGapFactor << std::endl;
            std::cout << std::right;

            std::cout << std::setw(110) << std::setfill('-') << "" << std::setfill(' ') << std::endl;
#endif

#if DEBUG_RM && DEBUG_RM1
        if(positionHelper->isLeader()) {
            std::cout << "Leader: \n\tactiveController: " << std::get<0>(rmLog).activeController << std::endl;
        } else {
            std::cout << "VehicleId: " << positionHelper->getId() << "\n\tactiveController: " << std::get<0>(rmLog).activeController << std::endl;

        }
#endif

        // output file
        write2file = par("write2file").boolValue();
        outputFilename = par("outputFilename").stdstringValue() + std::to_string(positionHelper->getId()) + ".txt";
        if(write2file) fileWriter = std::make_shared<FileWriter>(positionHelper->getId(), outputFilename, write2file);


        // initialize contract list
        initializeContracts();

        // Initialize the Contracts + Guarantees
        contractGuarantees = std::make_shared<Contract_Guarantee>(this);

        // Schedule the monitoring self message
        monitoringMsg = new cMessage("monitoringMsg");
        SimTime callBackTime = simTime() + rmParam.rmMonitorInterval;
        scheduleAt(callBackTime, monitoringMsg);
    }
}


void RuntimeManager::handleSelfMsg(cMessage* msg) {
    if(msg == monitoringMsg) {
        EV << "Monitoring message has been arrived. Evaluation started..." << std::endl;
        // First we logged the ego vehicle
        ownLog();
        // Now we evaluate the current status of the vehicle
        evaluate();

        // Sanity Check for now (WIFIContract only) TODO: need to generalize this
        ASSERT(traciVehicle->getActiveController() == (std::get<0>(rmLog).contracts)->front()->getController());

#if DEBUG_RM && DEBUG_RM1
        if(traciVehicle->getActiveController() == Plexe::CACC) {
            std::cout << "Vehicle " << positionHelper->getId()
                      << ": \tActive Controller: " << traciVehicle->getActiveController()
                      << "\tConstantSpacing : "<< traciVehicle->getCACCConstantSpacing()
                      <<std::endl;
        } else {
            std::cout << "Vehicle " << positionHelper->getId()
                      << ": \tActive Controller: " << traciVehicle->getActiveController()
                      << "\tHeadwayTimeGap : " << traciVehicle->getACCHeadwayTime()
                      << std::endl;
        }
#endif

        // Reschedule the monitoring message. TODO make sure to reschedule the self message
        // after the current evaluation and transition (if there is any)
        SimTime callBackTime = simTime() + rmParam.rmMonitorInterval;
        scheduleAt(callBackTime, monitoringMsg);
    }
}

void RuntimeManager::ownLog() {
    Plexe::VEHICLE_DATA vdata;
    traciVehicle->getVehicleData(&vdata);
    RM::RMLog_Own &ego = std::get<0>(rmLog);
    ego.activeController = (Plexe::ACTIVE_CONTROLLER)traciVehicle->getActiveController();
    ego.time = vdata.time;
    ego.acceleration = vdata.acceleration;
    if(vdata.acceleration < 0.0 && vdata.acceleration < ego.maxDeceleration) {
        ego.maxDeceleration = vdata.acceleration;
    }
    // get the Radar measurements
    double distance, relativeSpeed;
    traciVehicle->getRadarMeasurements(distance, relativeSpeed);
    if(distance != -1 && relativeSpeed != 0) {
        ego.dist2pred = distance;
    } else {
        ego.crashed = traciVehicle->isCrashed();
    }
}

template <typename T> void RuntimeManager::commonLog(const PlatooningBeacon *pb, T &loggedVehicle, const SimTime currentTime) {
    if(!loggedVehicle.common.c2xInitiated) loggedVehicle.common.c2xInitiated = true;
    loggedVehicle.common.time = pb->getTime();
    loggedVehicle.common.lastBeaconArrivalTime = currentTime.dbl();
    loggedVehicle.common.acceleration = pb->getAcceleration();
    if(pb->getAcceleration() < 0.0 && pb->getAcceleration() < loggedVehicle.common.maxDeceleration) {
        loggedVehicle.common.maxDeceleration = pb->getAcceleration();
    }
    loggedVehicle.common.controllerAcceleration = pb->getControllerAcceleration();

}


void RuntimeManager::onPlatoonBeacon(const PlatooningBeacon *pb, const SimTime currentTime) {
    // We are only interested in storing log for front and leader vehicle
    if(pb->getVehicleId() == positionHelper->getFrontId()) {
        RM::RMLog_Front &frontLog = std::get<1>(rmLog);
        commonLog(pb, frontLog, currentTime);

        // TODO : log if there is any front specific log required
        // get front vehicle position
        Coord frontPosition(pb->getPositionX(), pb->getPositionY());
        // get my position
        Veins::TraCICoord traciPosition = mobility->getManager()->omnet2traci(mobility->getCurrentPosition());
        Coord position(traciPosition.x, traciPosition.y);
        // compute distance
        double distance = position.distance(frontPosition) - pb->getLength();
        frontLog.distance = distance; // Distance can be achieved during taking action

#if DEBUG_RM && DEBUG_RM1
        double distanceR, relativeSpeed;
        traciVehicle->getRadarMeasurements(distanceR, relativeSpeed);

        std::cout << "VehicleId : " << positionHelper->getId()
                  << "\n\tdistance: " <<  distance
                  << "\n\tdistanceR: " << distanceR
                  << std::endl;
#endif
    }

    // We need to perform both for front and leader subsequently, as the vehicle with id 0, got the same vehicle as leader and front
    // In that case we want to logged for both leader and the front vehicle
    // In case of other vehicles onley one of the condition will be satisfied
    if(pb->getVehicleId() == positionHelper->getLeaderId()) {
        RM::RMLog_Leader &leaderLog = std::get<2>(rmLog);
        commonLog(pb, leaderLog, currentTime);
    }
}



void RuntimeManager::initializeContracts() {
    RM::RMLog_Own &ego = std::get<0>(rmLog);
    ego.contracts  = std::make_shared<std::vector<std::shared_ptr<Contract>>>();
    (ego.contracts)->push_back(std::make_shared<WIFIContract>(CONTRACT_TYPE::WIFI, (Plexe::ACTIVE_CONTROLLER)traciVehicle->getActiveController(), C2F(), C2L()));

    // TODO other Contracts, if there is any

#if DEBUG_RM

    std::cout << std::setw(110) << std::setfill('#') << "" << std::setfill(' ') << std::endl;
    std::cout << std::setw(40) << ""
              << "VEHICLE " << positionHelper->getId() << " :: DEFAULT CONTRACTS"
              << std::setw(40) << ""
              << std::endl;
    std::cout << std::setw(110) << std::setfill('#') << "" << std::setfill(' ') << std::endl;

    // Currently we are having only only contractStart
    std::cout << *(std::static_pointer_cast<WIFIContract>((*ego.contracts)[0])) << std::endl;

    std::cout << std::setw(110) << std::setfill('-') << "" << std::setfill(' ') << std::endl;

#endif

}


void RuntimeManager::evaluate(bool onPlatoonBeacon, int index) {

    // Sanity Check
    ASSERT2(onPlatoonBeacon ? index >= 0 : index < 0, "Problem with default argument in evaluate() methods in RuntimeManager");

    RM::RMLog_Own &ego = std::get<0>(rmLog);
    for(auto it = (ego.contracts)->begin(); it != (ego.contracts)->end(); ++it) {
        if(onPlatoonBeacon) {
            (*it)->evaluate(rmParam, rmLog, onPlatoonBeacon, index);
        } else {
            (*it)->evaluate(rmParam, rmLog);
        }
    }

#if DEBUG_RM && DEBUG_RM1
    if(positionHelper->getId() == 7) {
        std::cerr << "Vehicle Id : " << positionHelper->getId() << "\n\t" << *(std::static_pointer_cast<WIFIContract>((*ego.contracts)[0])) << std::endl;
    }
#endif



    // Vehicles StateParameters related to the provided contract has been evaluated,
    // Now, we need to match the current state of the vehicle's contract with the Contracts-Guarantee
    // to ensure the appropriate Guarantee

    if(!onPlatoonBeacon) {
        contractGuarantees->evaluate(std::get<0>(rmLog));
    }
}


