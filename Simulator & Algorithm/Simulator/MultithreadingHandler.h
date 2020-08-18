//
//  MultithreadingHandler.h
//  multiThreading
//
//  Created by Raghd Zeidan on 07/06/2020.
//  Copyright Â© 2020 Majd Zeidan. All rights reserved.
//

#ifndef MultithreadingHandler_h
#define MultithreadingHandler_h
#include "Simulation.cpp"
#include <queue>
#include <functional>
#include <thread>
class ThreadPool {
public:
    using Task = std::function<void()>;
    void start(std::size_t numThreads, std::condition_variable* statIndicator, std::mutex* myMutex);
    void stop();
    void enqueue(Simulator* sim);

    explicit ThreadPool(std::size_t numThreads, int _numTasks,int _numberOfTravels ,vector<string>& _algorithmPaths, std::condition_variable* statIndicator, std::mutex* myMutex)
    { 	numberOfTravels=  std::min(_numberOfTravels,_numTasks);
		if (numberOfTravels < 1) numberOfTravels=1;
        AlgorithmPaths = _algorithmPaths;
        numberOfTasks = _numTasks;
        start(numThreads, statIndicator, myMutex);
    }

    ~ThreadPool()
    {
        stop();
    }
private:
    shared_ptr<AbstractAlgorithm> getAlgorithm();

    int numberOfTasks;
    std::vector<std::thread> mThreads;
    std::condition_variable mEventVar;
    std::mutex mEventMutex;
    bool mStopping = false;
    vector<string> AlgorithmPaths;
    int numberOfTravels;
    int currNumberOfAlgo;
    std::queue<Simulator*> mTasks;
};

#endif /* MultithreadingHandler_h */
