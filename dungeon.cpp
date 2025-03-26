#include "dungeon.h"

Dungeon::Dungeon(size_t partyID, size_t ID): partyID(partyID), ID(ID), isOccupied(false), stopFlag(false) {
    std::cout << "Dungeon " << ID << " has been created." << std::endl;
}

Dungeon::~Dungeon() {
    std::cout << "Dungeon " << ID << " has been destroyed." << std::endl;
}

void Dungeon::setClearTime(int clearTime) {
    std::cout << "Dungeon " << ID << " has been cleared in " << clearTime << " seconds." << std::endl;
    totalTimeServed += clearTime;
}

void Dungeon::assignParty(std::shared_ptr<Party> party) {
    // Lock the dungeon
    std::cout << "Dungeon " << ID << " is assigning party " << party->getID() << "." << std::endl;
    std::lock_guard<std::mutex> lock(dungeonMutex);

    // Assign the party to the dungeon
    currentParty = party;
    isOccupied = true;

    partyAvailable.notify_one();
}

void Dungeon::run() {
    std::cout << "Dungeon " << ID << " is running." << std::endl;

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
        std::cout << "Dungeon " << ID << " is processing party " << currentParty->getID() << "." << std::endl;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(currentParty->getClearTime()));
        lock.lock();

        // Reset the party
        currentParty = nullptr;
        isOccupied = false;

        std::cout << "Dungeon " << ID << " is now available." << std::endl;
    }
}

void Dungeon::stop() {
    {
        std::lock_guard<std::mutex> lock(dungeonMutex);
        stopFlag = true;
    }
    std::cout << "Dungeon " << ID << " has been stopped. Allegedly" << std::endl;
    partyAvailable.notify_all();
}
