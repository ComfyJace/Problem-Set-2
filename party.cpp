#include "party.h"


Party::Party(int partySize, int numDPS, int numHealer, int numTank, size_t partyClearTime,  size_t ID) :
    partySize(partySize),
    numDPS(numDPS),
    numHealer(numHealer),
    numTank(numTank),
    partyClearTime(partyClearTime),
    ID(ID) {

    }

void Party::setClearTime(long long unsigned int clearTime) { 
    this->partyClearTime = clearTime;
}

void Party::printParty() {
    std::cout << "Party Size: " << partySize << std::endl;
    std::cout << "DPS: " << numDPS << std::endl;
    std::cout << "Healer: " << numHealer << std::endl;
    std::cout << "Tank: " << numTank << std::endl;
    std::cout << "Clear Time: " << partyClearTime << std::endl;
}

size_t Party::getClearTime() {
    return partyClearTime;
}

size_t Party::getID() {
    return ID;
}