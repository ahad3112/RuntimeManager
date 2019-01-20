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

#include <map>
#include <string>
#include "AdjustGap2Front.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AdjustGap2Front's Member function's implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AdjustGap2Front::AdjustGap2Front(RuntimeManager *rm, GAP2FRONT gap2front) : Guarantees(rm), gap2front(gap2front){
    // TODO Auto-generated constructor stub

}

AdjustGap2Front::~AdjustGap2Front() {
    // TODO Auto-generated destructor stub
}

void AdjustGap2Front::operator()(std::shared_ptr<Contract> contract) const {
    if(traciVehicle->getActiveController() == Plexe::ACC){
        std::cerr << "Warning: " << __FILE__
                         << "\n\tLine: " << __LINE__
                         << "\n\tCompiled on: " << __DATE__
                         << " at " << __TIME__
                         << "\n\tfunction: " << __func__
                         << " ...Not Implemented yet!!!"
                         << std::endl;
    } else if (traciVehicle->getActiveController() == Plexe::PLOEG) {

        if(gap2front == GAP2FRONT::DEFAULT) {
            // go back to the default spacing
            traciVehicle->setParameter(CC_PAR_PLOEG_H, rmParam.ploegHeadwayTimeGap);
#if DEBUG_RM
            std::cout << "Vehicle " << positionHelper->getId() << "\n\t" << "performed "
                      << gap2front << " headwayTimeGap(" << rmParam.ploegHeadwayTimeGap << "s)"
                      << "\n\t" << "Contract Status: " << *(std::static_pointer_cast<WIFIContract>(contract))
                      << std::endl;
#endif

        } else if (gap2front == GAP2FRONT::INCREASE){
            // increase the gap
            traciVehicle->setParameter(CC_PAR_PLOEG_H, rmParam.ploegHeadwayTimeGap + rmParam.ploegHeadwayTimeGap * rmParam.emergencyPloegHeadwayTimeGapFactor);
#if DEBUG_RM
            std::cout << "Vehicle " << positionHelper->getId() << "\n\t" << "performed "
                      << gap2front << " headwayTimeGap(" << (rmParam.ploegHeadwayTimeGap + rmParam.ploegHeadwayTimeGap * rmParam.emergencyPloegHeadwayTimeGapFactor) << "s)"
                      << "\n\t" << "Contract Status: " << *(std::static_pointer_cast<WIFIContract>(contract))
                      << std::endl;
#endif

        } else {
            // TODO decrease the gap
        }

    } else if(traciVehicle->getActiveController() == Plexe::CACC) {
        if(gap2front == GAP2FRONT::DEFAULT) {
            // go back to the default spacing
            traciVehicle->setCACCConstantSpacing(rmParam.caccConstantSpacing);
#if DEBUG_RM
            std::cout << "Vehicle " << positionHelper->getId() << "\n\t" << "performed "
                      << gap2front << " distance(" << rmParam.caccConstantSpacing << "m)"
                      << "\n\t" << "Contract Status: " << *(std::static_pointer_cast<WIFIContract>(contract))
                      << std::endl;
#endif
        } else if (gap2front == GAP2FRONT::INCREASE){

            // increase the gap
            traciVehicle->setCACCConstantSpacing(rmParam.caccConstantSpacing + rmParam.caccConstantSpacing * rmParam.emergencyCaccConstantSpacingFactor);
#if DEBUG_RM
            std::cout << "Vehicle " << positionHelper->getId() << "\n\t" << "performed "
                      << gap2front << " distance(" << (rmParam.caccConstantSpacing + rmParam.caccConstantSpacing * rmParam.emergencyCaccConstantSpacingFactor) << "m)"
                      << "\n\t" << "Contract Status: " << *(std::static_pointer_cast<WIFIContract>(contract))
                      << std::endl;
#endif

        } else {
            // TODO decrease the gap. Not interest in increasing right now.
        }

    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GAP2FRONT enum's << operator implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &os, const GAP2FRONT gap2front) {
    std::map<GAP2FRONT, std::string> enum2string;

    if(enum2string.size() == 0) {
#define INSERT(gap2front) enum2string[gap2front] = #gap2front
        INSERT(GAP2FRONT::DEFAULT);
        INSERT(GAP2FRONT::DECREASE);
        INSERT(GAP2FRONT::INCREASE);
#undef INSERT
    }

    return os << enum2string[gap2front];
}
