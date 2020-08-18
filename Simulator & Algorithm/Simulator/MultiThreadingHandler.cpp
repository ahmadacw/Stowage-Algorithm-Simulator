//
//  MultiThreadingHandler.h
//  multiThreading
//
//  Created by Raghd Zeidan on 07/06/2020.
//  Copyright © 2020 Majd Zeidan. All rights reserved.
//
#include <condition_variable>
#include <functional>
#include <iostream>
#include <vector>
#include "MultithreadingHandler.h"
#include <cstdlib>
#include "AlgorithmRegistrar.cpp"

void ThreadPool::enqueue(Simulator * sim)
    {
        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            mTasks.emplace(sim);
        }
        mEventVar.notify_one();
    }
 

void ThreadPool::start(std::size_t numThreads,std::condition_variable *statIndicator,std::mutex* myMutex)
    {
    
        for (unsigned int i = 0; i < numThreads; ++i)
        {
            //lock to wait for all the tasks to be taken
            mThreads.emplace_back([=,this] {
                while (true)
                {
                    Simulator* sim;
                    {
                        std::unique_lock<std::mutex> lock{mEventMutex};
                        mEventVar.wait(lock, [=,this] { return mStopping || !mTasks.empty(); });
                        if (mStopping && mTasks.empty()){
                            break;
                        }
                        sim= mTasks.front();
                        mTasks.pop();
						sim->setAlgorithm(std::move(getAlgorithm()));
                    }
                    sim->travel();
		{
                    std::unique_lock<std::mutex> lock{mEventMutex};
                    numberOfTasks--;
		   // cout<<"tasks remaining"<<numberOfTasks<<endl;
		}
                    if(numberOfTasks==0){
                    {
                        unique_lock<mutex> notifyLock{*myMutex};
                    }
                        statIndicator->notify_one();
                    }
                }
            });
        }
    }

shared_ptr<AbstractAlgorithm> ThreadPool::getAlgorithm() { 
    static int numberOfCurrentAlgorithm= 0;
    static int numberOfUniqueAlgortihms = 0;  
    string error="";
    static AlgorithmRegistrar& rar = AlgorithmRegistrar::getInstance();
    numberOfCurrentAlgorithm++;
    int indicator=0;
    if (numberOfCurrentAlgorithm == this->numberOfTravels+1) {
	numberOfCurrentAlgorithm--;
	//cout<<endl<<"registed: "<< numberOfCurrentAlgorithm<<" of algorithm at "<<AlgorithmPaths[numberOfUniqueAlgortihms]<<endl;
        numberOfCurrentAlgorithm = 0;
        numberOfUniqueAlgortihms++;
	indicator=1;
    }
   if(indicator)
	    rar.loadAlgorithmFromFile(AlgorithmPaths[numberOfUniqueAlgortihms].c_str(), error);
	cout<<error;
    return std::move((*(rar.begin()+numberOfUniqueAlgortihms))());

}

void ThreadPool::stop() 
{
    {
        std::unique_lock<std::mutex> lock{mEventMutex};
        mStopping = true;
    }

    mEventVar.notify_all();

    for (auto &thread : mThreads)
        thread.join();
	
}
