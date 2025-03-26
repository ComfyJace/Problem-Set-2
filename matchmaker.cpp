#include "matchmaker.h"

// Singleton instance
Matchmaker& Matchmaker::getInstance(std::unordered_map<std::string, long long int>* configValues) {
    static Matchmaker* instance = nullptr;
    if (instance == nullptr) {
        if (configValues == nullptr) {
            throw std::runtime_error("Config values cannot be null");
        }
        
        instance = new Matchmaker(*configValues);
    }
    return *instance;
}

// Set the config values
Matchmaker::Matchmaker(std::unordered_map<std::string, long long int>& configValues) : 
    threadPool(ThreadPool::getInstance(configValues["n"]+1)),
    numDungeons(configValues["n"]),
    dpsQueueSize(configValues["d"]),
    healerQueueSize(configValues["h"]),
    tankQueueSize(configValues["t"]),
    minClearTime(configValues["t1"]),
    maxClearTime(configValues["t2"]) {

    std::cout << "Initializing Matchmaker with config values." << std::endl;
    for (size_t i = 0; i < numDungeons; i++) {
        dungeonQueue.emplace_back(std::make_shared<Dungeon>(i, i));
    }
    
}

// Create a parties
void Matchmaker::makeParty() {
    // Constants for party creation
    const int partySize = 5;
    const int numDPS = 3;
    const int numHealer = 1;
    const int numTank = 1;

    // counter for party id
    size_t id = 0;
    
    // Create a party
    while (dpsQueueSize >= 3 && healerQueueSize > 0 && tankQueueSize > 0) {
        // Create a party
        size_t clearTime = getRandomClearTime(minClearTime, maxClearTime);
        auto party = std::make_shared<Party>(partySize, numDPS, numHealer, numTank, clearTime, id);
        {
            std::lock_guard<std::mutex> lock(partyMutex);
            partyQueue.push(party);
        }
        
        dpsQueueSize -= 3;
        healerQueueSize -= 1;
        tankQueueSize -= 1;
        id++;
    }
    std::cout << "Party creation complete. Created " << id << " parties" << std::endl;

    // Set the parties created flag
    {
        std::lock_guard<std::mutex> lock(partyCreationMutex);
        partiesCreated = true;
    }

    // Notify the main thread that party creation is complete
    partyCreationComplete.notify_one();
}

// Start the matchmaking process
void Matchmaker::startMatchmaking() {
    // Create the parties
    // "this" is a pointer to the current instance of matchmaker
    // "this" is needed access the makeParty function
    threadPool.enqueue([this]() { this->makeParty(); });

    // Wait for the parties to be created
    {
        std::unique_lock<std::mutex> lock(partyCreationMutex);
        partyCreationComplete.wait(lock, [this]() { return partiesCreated; });
    }

    // Start dungeon threads
    for (auto& dungeon : dungeonQueue) {
        threadPool.enqueue([dungeon]() { dungeon->run(); });
    }

    // Matchmake the parties
    while (partyQueue.size() > 0) {
        std::shared_ptr<Party> party;

        // Assign the party to a dungeon
        for (auto& dungeon : dungeonQueue) {
            if (!dungeon->isOccupied) {
                {
                    std::lock_guard<std::mutex> lock(partyMutex);
                    party = partyQueue.front();
                    partyQueue.pop();
                    dungeon->assignParty(party);
                    
                    std::cout << "Party assigned to dungeon allegedly" << std::endl;
                    std::cout << "Party queue is now: " << partyQueue.size() << std::endl;
                }
                break;
            }  
        }  
    }

    std::cout << "Matchmaking after while loop" << std::endl;
    bool allStopped = false;
    while (!allStopped) {
        allStopped = true;
        for (auto& dungeon : dungeonQueue) {
            if (dungeon->isOccupied) {
                allStopped = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Prevent busy waiting
            }
        }
    }
    
    // Now stop remaining dungeons
    for (auto& dungeon : dungeonQueue) {
        std::cout << "Stopping occupied dungeon " << std::endl;
        dungeon->stop();
    }
}

size_t Matchmaker::getRandomClearTime(size_t min, size_t max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(min, max);
    return dis(gen);
}
