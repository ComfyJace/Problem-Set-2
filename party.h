#pragma once
#include <iostream>

class Party {
    private:
        int partySize;
        int numDPS;
        int numHealer;
        int numTank;
        size_t partyClearTime;
        size_t ID;

    public:
        Party(int partySize, int numDPS, int numHealer, int numTank, size_t partyClearTime, size_t ID);
        void setClearTime(size_t clearTime);
        void printParty();
        size_t getID();
        size_t getClearTime();
};