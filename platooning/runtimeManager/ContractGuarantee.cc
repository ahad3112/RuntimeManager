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
#include "ContractGuarantee.h"

#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Contracts's Member function's implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Contract_Guarantee::Contract_Guarantee(RuntimeManager *rm) : wifiCG(std::make_shared<contract_guarantee_type>()){
    // TODO Auto-generated constructor stub
    initContractList(rm);
}

Contract_Guarantee::~Contract_Guarantee() {
    // TODO Auto-generated destructor stub
}

// This is for checking TODO will extend later
void Contract_Guarantee::evaluate(RMLog_Own &state) {
    // Iterate through every single contract of the state and perform the associate guarantees
    for(auto start = (state.contracts)->begin(); start != (state.contracts)->end(); ++start) {
        if(auto contract = dynamic_cast<WIFIContract *>(*start)) {
            // match should be unique
            auto match = wifiCG->find(*contract);
            if(match != wifiCG->end()) {
                // Match found and call the provideGuarantee method of the found Guarantee
                Guarantees guarantee = match->second;
                guarantee.provideGuarantee(*start);
            } else {
                std::cout << "Not match contract found. No action needs to be taken...." << std::endl;
            }
            // each contract of a vehicle should be exclusive to only one particular Contract_Guarantee list
            continue;
        }

        // TODO check in other Contract_Gurantee list
    }
}

void Contract_Guarantee::initContractList(RuntimeManager *rm) {
    // Creating the WIFIContract-Guarantee list

    // StateParameters
    C2X ok_c2f(QUALITY::OK, ROLE::FRONT);
    C2X critical_c2f(QUALITY::CRITICAL, ROLE::FRONT);

    C2X ok_c2l(QUALITY::OK, ROLE::LEADER);
    C2X critical_c2l(QUALITY::CRITICAL, ROLE::LEADER);

    // Guarantees
    Guarantees g2acc(rm, true, Plexe::ACTIVE_CONTROLLER::ACC);
    Guarantees g2ploeg(rm, true, Plexe::ACTIVE_CONTROLLER::PLOEG);
    Guarantees g2cacc(rm, true, Plexe::ACTIVE_CONTROLLER::CACC);


    // WIFIContract for ACC controller
    // Upgrade
    WIFIContract acc2cacc(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::ACC, ok_c2f, ok_c2l);
    WIFIContract acc2ploeg(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::ACC, ok_c2f, critical_c2l);

    // WIFIContract for PLOEG controller
    // Upgrade
    WIFIContract ploeg2cacc(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::PLOEG, ok_c2f, ok_c2l);
    // degrade
    WIFIContract ploeg2acc1(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::PLOEG, critical_c2f, ok_c2l);
    WIFIContract ploeg2acc2(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::PLOEG, critical_c2f, critical_c2l);

    //WIFIContract for CACC
    // Degrade
    WIFIContract cacc2ploeg(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::CACC, ok_c2f, critical_c2l);
    WIFIContract cacc2acc1(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::CACC, critical_c2f, ok_c2l);
    WIFIContract cacc2acc2(CONTRACT_TYPE::WIFI, Plexe::ACTIVE_CONTROLLER::CACC, critical_c2f, critical_c2l);

    // mapping between WIFIContract and Guarantee
    wifiCG->insert(std::make_pair(acc2cacc, g2cacc));
    wifiCG->insert(std::make_pair(acc2ploeg, g2ploeg));

    wifiCG->insert(std::make_pair(ploeg2cacc, g2cacc));
    wifiCG->insert(std::make_pair(ploeg2acc1, g2acc));
    wifiCG->insert(std::make_pair(ploeg2acc2, g2acc));

    wifiCG->insert(std::make_pair(cacc2ploeg, g2ploeg));
    wifiCG->insert(std::make_pair(cacc2acc1, g2acc));
    wifiCG->insert(std::make_pair(cacc2acc2, g2acc));

//    contract_guarantee_type ::size_type size = wifiCG->size();
//
//    std::cout << "  " <<std::endl;

}
