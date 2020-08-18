//==============================
//			Includes
//==============================

#include <sstream>
#include <string>
#include <iostream>
#include <math.h>
#include "Simulation.h"
#include <mutex>
#define MAX_X 100
#define MAX_Y 100
#define MAX_Z 10
using std::vector;
using std::tuple;
using std::get;
using std::ofstream;
std::mutex outputLock;
//==============================
//			functions
//==============================

string getName(string path, char deli ) {
    if (path == "./") return path;
    vector<string>tmp;
    tmp = split(path, deli);
    string curr = tmp[tmp.size() - 1];
    tmp = split(curr, '.');
    return tmp[0];
}

void Simulator::initSimulator(string _shipPlanFileName, string _shipRouteFileName, string _portsFiles, string _InstructionsOutPutFile){
    /*cout <<endl<<"Algorithm Travel pair created with:"<<endl;
       cout<<"shipPlanPath: "<<_shipPlanFileName<<endl;
       cout<<"shipRoutePath: "<< _shipRouteFileName<<endl;
       cout<<"cargo file location: "<<_portsFiles<<endl;
       cout<<"instruction outputfile: "<<_InstructionsOutPutFile<<endl;
    */shipPlanFileName=_shipPlanFileName;
    shipRouteFileName=_shipRouteFileName;
    portsFiles=_portsFiles ;
    InstructionsOutPutFile=_InstructionsOutPutFile ;
	unusedCargoFiles = 0;
    
}
void createPaths(std::string& outLocation, std::string& InstructionsOutPutFile, Port* currPort, std::string& cargoLocation, int currPortI)
{
	outLocation += InstructionsOutPutFile;
	outLocation += "/";
	outLocation += currPort->getPortname();
	cargoLocation += currPort->getPortname();
	outLocation += "_";
	cargoLocation += "_";
	cargoLocation += std::to_string(currPortI);
	outLocation += std::to_string(currPortI);
	cargoLocation += ".cargo_data";
	outLocation += ".instruction";
}






// freeup the memory


int loadInstruction(Instruction& ins, Ship& simShip, set<string>& routeNames, vector<Cargo*>* cargo) {
	string id = ins.Id;
	int x = ins.x;
	int y = ins.y;
	int z = ins.z;
	
	for (auto& container : *cargo) {
		//cout<<endl<<"container"<<container->getId()<<endl;
		if (!container->getId().compare(id)) {
			//cout<<endl<<"IDokay"<<endl;
			if (!(simShip.onBoard(container))) {
				//cout<<endl<<"ID not onBoard"<<endl;
				if (routeNames.count(container->getDest())) {
					//cout<<endl<<"ID not In route"<<endl;
					if (simShip.loadCargo(container, z, x, y)) {
						//cout<<endl<<"loaded"<<endl;
						simShip.addToSet(container);
						return valid;
					}
					else
						return invalid;
				}
			}
		}
	}
	return invalid;
}

int unloadInstruction(Instruction ins,vector<Cargo*>* cargo,Ship& simShip, unsigned int i, vector<Instruction>& instructions) {
	string id = ins.Id;
	int x = ins.x;
	int y = ins.y;
	int z = ins.z;
	Cargo* container = (*simShip.getPlan())[z][x][y];
	if (!(container->getId().compare(id))) {
		if (!simShip.getCurrentPort()->getPortname().compare(container->getDest())) {
			if (!(simShip.unloadCargo(z, x, y))) {
				return invalid;
			}
			simShip.RemoveFromSet(container);
			return valid;

		}
		else {
			for (; i < instructions.size(); i++) {
				if (instructions[i].Id == id && instructions[i].op == 'L') {
					cargo->push_back(container);
					simShip.RemoveFromSet(container);
					if (simShip.unloadCargo(z, x, y)) {
						return 2;
					}
					else {
						return invalid;
					}

				}
			}
		}
	}
	return invalid;

}




void emptyOutputFile(std::string& InstructionsOutPutFile)
{
	ofstream instructionsForCargoFile(InstructionsOutPutFile); //empty the intended output file
	instructionsForCargoFile.close();
}



void initPiles(Ship& simShip,lst* cargo, set<string>& rejectPile, set<string>& containersAtPort, set<string>& route)
{
	unsigned int limit = simShip.numOfemptySpots;
	for (unsigned int j = 0; j < limit && j < cargo->size(); j++) {// prioritizing closer Ports
		if (simShip.onBoard((*cargo)[j]) || !route.count(cargo->at(j)->getDest())) {
			rejectPile.insert((*cargo)[j]->getId());
			continue;
		}
		else
			containersAtPort.insert((*cargo)[j]->getId());
	}
	for (unsigned int j = limit; j < cargo->size(); j++) {
		rejectPile.insert((*cargo)[j]->getId());
	}
}

vector<set<string>> getContainersToUnload(Ship& simShip) {
	mat* plan = (simShip.getPlan());
	int y = (int) (*plan)[0][0].size();
	int x = (int) (*plan)[0].size();
	int z = (int) (*plan).size();
	vector<set<string>> output;
	output.resize(2);
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			for (int k = 0; k < z; k++) {
				if ((*plan)[k][i][j]->getDest() == simShip.getCurrentPort()->getPortname()) {
					if (simShip.getCurrentPort()->contains((*plan)[k][i][j]->getId())) {
						output.at(0).insert((*plan)[k][i][j]->getId());
					}
					else {
						output.at(1).insert((*plan)[k][i][j]->getId());
					}
				}
			}
		}
	}
	return output;
}


int moveInstruction(Instruction ins, Ship& simShip) {
	string id = ins.Id;
	int x = ins.x;
	int y = ins.y;
	int z = ins.z;
	Cargo* container = (*simShip.getPlan())[z][x][y];
	if (!(container->getId().compare(id))) {
		if (!simShip.unloadCargo(z, x, y))	return invalid;
		return simShip.loadCargo(container, ins.z1, ins.x1, ins.y1);
	}
	return 0;
}

vector<int> evaluateInstructions(vector<Instruction> instructions, Ship& simShip, set<string>& emptySet) {
	set<string> routeNames;
	set<string> rejectPile=emptySet;
	int num = 0;
	vector<Cargo*>* cargo = simShip.getCurrentPort()->getPortCargos();
	updateLoad(cargo,* simShip.getRoute());

	vector<set<string>> containtersToUnload = getContainersToUnload(simShip);

	set<string> unload = containtersToUnload[1];
	set<string> containersAtPort;
	int loaded = 0;
	int indic = 0;
	for (auto& p : (*simShip.getRoute())) {
		if (indic == 0) {
			indic = 1;
			continue;
		}
		routeNames.insert(p->getPortname());
	}
	int i = 0;
	int done=0;
	for (auto& ins : instructions) {
		if(ins.Id== "") continue;
		//cout << "printed";
		if (ins.op == 'L') {
			if (!done) {
				initPiles(simShip, cargo, rejectPile, containersAtPort, routeNames);
				done = 1;
			}
            num+=5;
			if (loadInstruction(ins, simShip, routeNames, cargo)) {
				//cout<<endl<<"loaded"<<endl;
				loaded = 1;
				containersAtPort.erase(ins.Id);
			}
		}
		else if (ins.op == 'U') {
			if (loaded) return { 0 ,-1 };
            num+=5;
			int tmp = unloadInstruction(ins, cargo,simShip, i, instructions);
			if (tmp == 2) {
				unload.erase(ins.Id);
				containersAtPort.insert(ins.Id);
			}
			else if (tmp) {
				unload.erase(ins.Id);
			}
			else {
				return { 0 };

			}
		}
		else if (ins.op == 'R') {
			if(!done) 	initPiles(simShip, cargo, rejectPile, containersAtPort, routeNames);
			done = 1;
			loaded = 1;
			if (rejectPile.count(ins.Id)) {
				rejectPile.erase(ins.Id);
			}
			else {
				return { 0 };

			}
		}
		else {
			if (ins.op == 'M') {
                num+=3;
				if (moveInstruction(ins, simShip)) {
				}
				else return { 0 };
			}
		}
		i++;
	}
	//cout<<endl<<"load: "<<containersAtPort.size()<<" rej: "<<rejectPile.size()<<" unload :"<<unload.size()<<endl;
	if (containersAtPort.size() || rejectPile.size() || unload.size()) return { 0,-1 };
	return { 1,num };
}
void createEmptyFile(std::string& InstructionsOutPutFile)
{
	ofstream instructionsForCargoFile(InstructionsOutPutFile + "/emptyFile.cargo_data"); //empty the intended output file
	instructionsForCargoFile.close();
}
bool endsWith2(const std::string& mainStr, const std::string& toMatch)
{
    if (mainStr.size() >= toMatch.size() &&
        mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
        return true;
    else
        return false;
}
int num=0;
void Simulator::travel() {
    	Parser parser=Parser();
	//cout<<endl<<"algo adress: "<<algo<<endl;
	int currPortI = -1;
	list<string> files;
	vector<Instruction> instruction;
	Port* currPort;
	ErrorHandler simException = ErrorHandler();
	ErrorHandler algoException;
	Cargo shipEmpty = Cargo();
	Cargo shipUnavailable = Cargo();
	vector<vector<int>> vecForShipPlan;
    std::vector<std::vector<int>> plan;
    plan = parser.readShipPlanEx(shipPlanFileName);
   	simException =parser.getParserException()  ;

	
	if (simException.containsFatalShipPlanEx()) {
		algoException = algoException + ErrorHandler((algo)->readShipPlan(shipPlanFileName));

		returnResult={ -1,algoException };
		return;
	}
	int dimX = (plan[0][1]);
	int dimY = (plan[0][2]);

	int floorsNum = (plan[0][0]);
	//cout<<endl<<"creating ship in Simulation"<<endl;
	Ship simShip = Ship(&plan, dimX, dimY, floorsNum, route, shipEmpty, shipUnavailable);
    	//cout<<endl<<"ship created"<<endl;
    algoException = algoException + ErrorHandler((algo)->readShipPlan(shipPlanFileName));

	//cout << endl << "read plan2123123123" << endl;

	if (algoException.containsFatalShipPlanEx()) {
		//cout << endl << "sim exception " << simException << " algorithm: " << algoException;
		returnResult = { -1,algoException };
		return;
	}
	
    route = parser.readShipRouteEx(shipRouteFileName);
    simException=simException+parser.getParserException();
	simShip.setShipRoute(route);

	algoException = algoException + ErrorHandler((algo)->readShipRoute(shipRouteFileName));
   
	if (algoException.containsFatalShipRouteEx()) {
		returnResult= {-1,algoException };
		return;



	}

	simException = simException + simShip.getException();

	if (algoException.containsFatalSimEx()) {
		returnResult= { -1,algoException };
		return;

	}

	createEmptyFile(InstructionsOutPutFile);

	map<string, int> ports;
	for (Port* port : route) {
		ports[port->getPortname()] = 1;
	}
	int numInstructions = 0;
	set<string> paths;
    
    	//cout<<endl<<"reading all cargo_data files"<<endl;
	for (const auto& entry : std::filesystem:: directory_iterator(portsFiles)) {
		string s = entry.path().generic_string();
		if (endsWith2(s,".cargo_data"))
			paths.insert(s);
	}
        //cout<<endl<<"all cargo_data files read "<<endl;

	Instruction empty = { {} };
	int counter = 0;
	for (Port* port : route) {
		string cargoLocation;
		string outLocation;
		port = simShip.getCurrentPort();
		currPortI = ports[port->getPortname()];
		ports[port->getPortname()]++;
		currPort = port;
		cargoLocation = portsFiles + "/";
		createPaths(outLocation, InstructionsOutPutFile, currPort, cargoLocation, currPortI);
		paths.erase(cargoLocation);
		set<string> rejectSet=set<string>();
		vector<Cargo*> cargo;
		std::filesystem::path path = cargoLocation.c_str();
		if (!filesystem::exists(path)) {
			cargoLocation = InstructionsOutPutFile + "/emptyFile.cargo_data";			
		}
		cargo = parser.ParseContainers(cargoLocation, &rejectSet);
		if (simShip.lastPort()) {
			if (cargo.size() + rejectSet.size() > 0) {
				cout << "last port Hash containters" << endl;
				cargo.clear();
			}
		}
		if (counter > 10) {
			cout << "stuck inside loop" << endl;
		}
		counter++;
		port->setPortCargos(&cargo);
		algoException = algoException + ErrorHandler((algo)->getInstructionsForCargo(cargoLocation, outLocation));

        vector<Instruction> instructions = ReadInstructionsFromFile(outLocation,  (int)(*simShip.getPlan()).size(),(int)(*simShip.getPlan())[0].size(), (int)(*simShip.getPlan())[0][0].size());
		if (instructions.size() && instructions[0] == empty) {
			cout << endl<<"bad instruction format" << endl;
			returnResult ={ -1, 0};// maybe we need to update this.
			return;
		}
		vector<int>tmp = evaluateInstructions(instructions, simShip, rejectSet);
		if (!tmp[0]) {
			cout << endl<<"failed eval" << endl;
			returnResult= { -1,0 };// maybe we need to update this.
			return;

		}
		if(tmp.size()>=2)
			numInstructions += tmp[1];
		simShip.SailToNextPort();

	}
    unique_lock<mutex> {outputLock};
  // cout<<endl<<"finished Simulation: "<<num<<endl;
   //num++;
	if (paths.size() != 0) unusedCargoFiles = 1;
    returnResult.emplace_back(numInstructions);
    returnResult.emplace_back(algoException);
}
