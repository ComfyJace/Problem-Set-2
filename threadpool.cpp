// Created by: Jason Erwin Clyde V. Dimalanta (ID: 12198331)

#include "threadpool.h"

// Singleton instance
ThreadPool& ThreadPool::getInstance(long long unsigned int numThreads) {
    static ThreadPool instance(numThreads);
    return instance;
}

// Constructor
ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0) {
    std::cout << "Initializing ThreadPool with " << numThreads << " threads." << std::endl;
    for(size_t i = 0; i < numThreads; i++) {
        workers.emplace_back([this](){
            while (true){
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) return;
                    task = tasks.front();
                    tasks.pop();
                    ++activeTasks;
                }

                task(); // execute the task

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    --activeTasks;
                    if (tasks.empty() && activeTasks == 0) {
                        finished.notify_all();
                    }
                }
            }
        });
    }
}

// Queue a task to the thread pool
void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

// Wait for all tasks to complete
void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMutex);
    finished.wait(lock, [this] { return tasks.empty() && activeTasks == 0; });
}

// Destructor
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for(std::thread &worker : workers) {
        if(worker.joinable()){
            worker.join();
        }
    }
}
