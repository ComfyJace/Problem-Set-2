// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)
#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "party.h"

class Dungeon {
    public:
        Dungeon(size_t partyID, size_t ID);
        ~Dungeon();

        // Run the dungeon
        void run();
        void assignParty(std::shared_ptr<Party> party);
        void stop();
        std::atomic<bool> isOccupied;
        void displayStats();

    private:
        size_t partyID;
        size_t ID;
        size_t totalTimeServed;
        size_t numPartiesServed;

        std::mutex dungeonMutex;
        std::condition_variable partyAvailable;
        std::shared_ptr<Party> currentParty;
        std::atomic<bool> stopFlag;

        // Add a clear time to the dungeon
        void setClearTime(int clearTime);

};
