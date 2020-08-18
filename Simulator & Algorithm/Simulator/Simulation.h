//==============================
//			Includes
//==============================

#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "../Common/WeightBalanceCalculator.cpp"
#include "../Common/AbstractAlgorithm.h"
#include "../Common/Parser.cpp"
#include <memory>
using std::vector;
using std::string;
using std::tuple;
//==============================
//			functions
//==============================
class Simulator {
    vector<int> results;
 private:
        string shipPlanFileName;
        string shipRouteFileName;
        string portsFiles;
        string InstructionsOutPutFile;
    shared_ptr<AbstractAlgorithm> algo;
    vector<Port*> route;
    vector<int> returnResult;


    public:
        int unusedCargoFiles;

        void travel(); 
    Simulator(){};
    void initSimulator(string shipPlanFileName, string shipRouteFileName, string portsFiles, string InstructionsOutPutFile);
    void setAlgorithm(shared_ptr<AbstractAlgorithm> _algo){
        algo=std::move(_algo);
    }
    vector<int> getResult(){
        return returnResult;
    }
    
        
};
string getName(string path, char deli = '/');
/*vector<int> travel(string shipPlanFileName, string shipRouteFileName,
	string portsFiles, string InstructionsOutPutFile,std::shared_ptr<AbstractAlgorithm> algo);*/





