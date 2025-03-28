// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)
#include "dungeon.h"

Dungeon::Dungeon(size_t partyID, size_t ID):
    partyID(partyID), ID(ID), isOccupied(false), stopFlag(false), totalTimeServed(0), numPartiesServed(0)
    {
    std::cout << "Dungeon " << ID << " has been created." << std::endl;
}

Dungeon::~Dungeon() {
    std::cout << "Dungeon " << ID << " has been destroyed." << std::endl;
}

void Dungeon::setClearTime(int clearTime) {
    totalTimeServed += clearTime;
}

void Dungeon::assignParty(std::shared_ptr<Party> party) {
    // Lock the dungeon
    std::lock_guard<std::mutex> lock(dungeonMutex);

    // Assign the party to the dungeon
    currentParty = party;
    isOccupied = true;

    partyAvailable.notify_one();
}

void Dungeon::run() {
    // std::cout << "Dungeon " << ID << " is running." << std::endl;

    while (true) {
        // Lock the dungeon
        std::unique_lock<std::mutex> lock(dungeonMutex);

        // Wait until a party is assigned or stop signal is received
        partyAvailable.wait(lock, [this]() { return currentParty != nullptr || stopFlag; });

        if (stopFlag) {
            break;
        }

        // Check if Party is Assigned
        if (!isOccupied || currentParty == nullptr) continue;

        // Process party
        std::cout << "Dungeon " << ID << " is Active" << "." << std::endl;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(currentParty->getClearTime()));
        lock.lock();

        // Update dungeon stats
        setClearTime(currentParty->getClearTime());
        numPartiesServed++;

        // Reset the party
        currentParty = nullptr;
        isOccupied = false;

        std::cout << "Dungeon " << ID << " is now Empty." << std::endl;
    }
}

void Dungeon::stop() {
    {
        std::lock_guard<std::mutex> lock(dungeonMutex);
        stopFlag = true;
    }

    partyAvailable.notify_all();
}

void Dungeon::displayStats() {
    std::cout << "Dungeon " << ID << " has served " << static_cast<uint64_t>(numPartiesServed) << " parties for a total of " << static_cast<uint64_t>(totalTimeServed) << " seconds." << std::endl;
}
