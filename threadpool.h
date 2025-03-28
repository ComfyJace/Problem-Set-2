// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)
#pragma once
#include <iostream>
#include <functional>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

class ThreadPool {
    public:
        static ThreadPool& getInstance(long long unsigned int numThreads); 
        // Delete the copy constructor and assignment operator
        ThreadPool(ThreadPool const&) = delete;
        ThreadPool& operator=(ThreadPool const&) = delete;

        // Add a task to the thread pool
        void enqueue(std::function<void()> task);

        // Wait for all tasks to complete
        void wait();
        // Deconstructor
        ~ThreadPool();
    private:
        explicit ThreadPool(size_t numThreads);

        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::condition_variable finished;
        std::atomic<bool> stop;
        std::atomic<size_t> activeTasks;     
};