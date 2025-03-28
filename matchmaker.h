// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)
#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <random>
#include "threadpool.h"
#include "party.h"
#include "dungeon.h"

class Matchmaker {
    public:
        static Matchmaker& getInstance(std::unordered_map<std::string, long long int>* configValues = nullptr);

        // Delete the copy constructor and assignment operator
        Matchmaker(Matchmaker const&) = delete;
        Matchmaker& operator=(Matchmaker const&) = delete;

        // Deconstructor
        ~Matchmaker();
        
        void startMatchmaking();
       
    private:
        explicit Matchmaker(std::unordered_map<std::string, long long int>& configValues);
        
        ThreadPool& threadPool;
        
        long long unsigned int numDungeons;
        long long unsigned int dpsQueueSize;
        long long unsigned int healerQueueSize;
        long long unsigned int tankQueueSize;
        long long unsigned int minClearTime;
        long long unsigned int maxClearTime;

        std::queue<std::shared_ptr<Party>> partyQueue;
        std::vector<std::shared_ptr<Dungeon>> dungeonQueue;

        std::mutex partyMutex;
        std::condition_variable partyCreationComplete;
        std::mutex partyCreationMutex;

        bool partiesCreated = false;
    
        void makeParty();

        size_t getRandomClearTime(size_t min, size_t max);
};
