#include <vector>
#include <map>
#include <filesystem>
#include "_314636663_b.h"
using std::vector;
typedef std::vector<Cargo*> vc;
#include <algorithm>
//==============================
//			functions
//==============================

//==============================
//			functions
//==============================

REGISTER_ALGORITHM(_314636663_b)


_314636663_b::_314636663_b() {
}
_314636663_b::~_314636663_b() {

}

void _314636663_b::resetAlgorithms() {
	parser = Parser();
	numOperations = 0;
	route = {};
	plan = { {} };
	shipEmpty = Cargo();
	shipUnavailable = Cargo();
	algoException = ErrorHandler();
	this->cantRun = 0;

}

int _314636663_b::readShipPlan(const std::string& full_path_and_file_name) {
	//cout<<endl<<"resetting"<<endl;
	resetAlgorithms(); // this will always be the first call to the algorithm class, after creating the object of course;
	//cout<<endl<<"resetting"<<endl;
	plan = parser.readShipPlanEx(full_path_and_file_name);
	//cout<<endl << "reading ship plan from: " << full_path_and_file_name<<endl;
	ErrorHandler myEx = parser.getParserException();
	if (myEx.containsFatalShipPlanEx()) {
		this->cantRun = 1;
		return myEx;
	}
	int dimX = (plan[0][1]);
	int dimY = (plan[0][2]);
	int floorsNum = (plan[0][0]);
	this->ship = std::move(make_shared<Ship>(&plan, dimX, dimY, floorsNum, route, shipEmpty, shipUnavailable));
	if (ship->getException().containsFatalShipPlanEx()) {
		this->cantRun = 1;
	}
	this->cantRun = 2;
	return myEx;
}
int _314636663_b::readShipRoute(const std::string& full_path_and_file_name) {
	if (this->cantRun != 2) return algoException;
	ErrorHandler myEx;
	route = parser.readShipRouteEx(full_path_and_file_name);
	myEx = parser.getParserException();
	ship->setShipRoute(route);
	if (myEx.containsFatalShipRouteEx()) {
		this->cantRun = 1;
	}
	this->cantRun = 3;
	myEx = myEx + ship->getException();
	return myEx;
}

int _314636663_b::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
	this->naive = calculator;
	return valid;
}



void emptyCargo(vc* cargo, Cargo* empty) {
	int i = 0, length = (int)cargo->size(), deleted = 0;
	while (i < length - deleted) {
		if (cargo->at(i) == empty) {
			cargo->erase(cargo->begin() + i);
			deleted++;
		}
		else {
			i++;
		}
	}
}

std::vector<Instruction>  _314636663_b::calcMovesToLoad(std::vector<int>* listOfIndx, vector<Instruction>& instruction) {
	vc* loadFrom = ship->getCurrentPort()->getPortCargos();
	mat* plan = ship->getPlan();
	Port* p = ship->getCurrentPort();
	vc* cargo = loadFrom;
	vc* hold = p->getHold();
	int y = (int)(*plan)[0][0].size(), x = (int)(*plan)[0].size(), height =
		(int)(*plan).size(), j = 0, k = 0;
	int loaded = 0, loadedFromHold = 0;
	bool broken = false;
	unsigned int cnt = 0;
	for (int i = 0; i < height && !broken; i++) {
		for (j = 0; j < y && !broken; j++) {
			for (k = 0; k < x && !broken; k++) {
				if (ship->checkSpot(i, k, j) != 1)
					continue;
				int indicatorHold = 0;
				int holdIndx = 0;

				if (hold->size() != 0) {
					holdIndx = hold->at(0)->forSort;
					indicatorHold = 1;
				}
				int indicatorCargo = 0;
				int CargoIndx = 0;
				int indx;
				if (listOfIndx->size() > cnt) {
					indx = (*listOfIndx).at(cnt);
					CargoIndx = loadFrom->at(indx)->forSort;
					indicatorCargo = 1;
				}
				if (indicatorHold == 0 || (indicatorCargo && !(holdIndx <= (CargoIndx) || ship->numOfemptySpots == hold->size()))) {
					if ((!indicatorCargo && cnt == listOfIndx->size())
						|| ship->numOfemptySpots == 0) {
						broken = true;
						break;
					}
					if (ship->onBoard((*loadFrom)[indx])) {
						(instruction).push_back(Instruction('R', (*loadFrom)[indx]->getId(), i, k, j, -1,
							-1, -1));
						algoException.addException(11);
						continue;
					}
					cnt++;
					loaded++;
					if (!ship->loadCargo((*loadFrom)[indx], i, k, j)) {
						continue;
					} //load to ship

					ship->addToSet((*loadFrom)[indx]);
					(instruction).push_back(Instruction('L', (*loadFrom)[indx]->getId(),
						i, k, j, -1, -1, -1));
					cargo->at(indx) = &ship->empty;
				}
				else {
					ship->loadCargo(hold->at(0), i, k, j); //load to ship
					(instruction).push_back(Instruction('L', hold->at(0)->getId(), i, k,
						j, -1, -1, -1));
					hold->erase(hold->begin());
					loadedFromHold++;
				}
			}
		}
	}
	if (cnt != listOfIndx->size()) {
		algoException.addException(
			ErrorHandler::Exceptions::TooManyContainersToLoad);
	}
	emptyCargo(cargo, &ship->empty);
	return instruction;
}


int  _314636663_b::moveContainer(int curHeight, int Curx, int Cury, vector<Instruction>& instructions) {
	mat* plan = ship->getPlan();
	int height = (int)plan->size();
	int x = (int)(*plan)[0].size();
	int y = (int)(*plan)[0][0].size();
	int Weight;
	vector<int> arr;
	Cargo* toMove = (*plan)[curHeight][Curx][Cury];
	myMap map = updateHash(*(ship->getRoute()));
	vector<int> lowestWeightVect;
	int lowestWeightForVect = INT_MAX;
	int myIndx = toMove->forSort = map.at(toMove->getDest());
	int notFound = 1;
	for (int k = 0; k < y && notFound; k++) {
		for (int j = 0; j < x && notFound; j++) {
			if (j == Curx && k == Cury) continue;
			Weight = INT_MAX;
			int l = 0;
			int foundEmpty = 0;
			while (l < height) {
				if (foundEmpty) break;

				if (ship->checkSpot(l, j, k) != 0) {
					if (ship->checkSpot(l, j, k) == 1) {
						if (Weight == INT_MAX) Weight = 0;
						foundEmpty = 1;
					}
					else {
						if (Weight == INT_MAX) Weight = 1;
						else if ((*plan)[l][j][k]->forSort < myIndx) {
							Weight++;
						}
					}
				}
				l++;
			}
			if (l == height + 1) continue;
			if (l != 0)l--;
			if (ship->checkSpot(l, j, k) == 1 && lowestWeightForVect > Weight) {
				lowestWeightForVect = Weight;
				lowestWeightVect = { l,j,k };
			}
			if (Weight == 0) notFound = 0;
		}
	}
	if (!lowestWeightVect.size()) return 0;
	ship->unloadCargo(curHeight, Curx, Cury);
	ship->loadCargo(toMove, lowestWeightVect[0], lowestWeightVect[1], lowestWeightVect[2]);
	instructions.push_back({ 'M',toMove->getId(),curHeight,Curx,Cury, lowestWeightVect[0], lowestWeightVect[1], lowestWeightVect[2] });
	return 1;
}

std::vector<Instruction>  _314636663_b::calcMovesTounLoad(vector<Instruction>& instruction) {
	std::vector<int> listOfIndx = std::vector<int>();
	mat* plan = ship->getPlan();
	int height = (int)plan->size();
	int x = (int)(*plan)[0].size();
	int y = (int)(*plan)[0][0].size();
	for (int i = height - 1; i > -1; i--) {
		for (int k = 0; k < y; k++) {
			for (int j = 0; j < x; j++) {
				if (ship->checkSpot(i, j, k) != 2)
					continue;
				if ((*plan)[i][j][k]->getDest()
					== ship->getCurrentPort()->getPortname()) { //unload this container
					int l = i;
					if (height != 1) {
						while (l < height - 1 && ship->checkSpot(l + 1, j, k) == 2)
							l++;
					}
					while (l > i) { //move containers on top to hold//
						Cargo* toHold = (*plan)[l][j][k]; // Container to move to hold
						if (moveContainer(l, j, k, instruction)) {
						}
						else {// couldnt move the contianer, means that the ship is full
							(instruction).push_back(
								Instruction('U', toHold->getId(), l, j, k)); // add instruction

							ship->unloadCargo(l, j, k);        //get the container off
							ship->getCurrentPort()->addCargoToHold(toHold); //add the container to the hold
							listOfIndx.push_back(0); // the container was added to index 0

						}
						l--;
					}
					Cargo* toPort = (*plan)[l][j][k]; // container to be moved to port
					ship->RemoveFromSet(toPort);
					(instruction).push_back(Instruction('U', toPort->getId(), l, j, k)); //add instruction
					ship->unloadCargo(l, j, k);	// remove cargo from ship
					ship->getCurrentPort()->addCargoToPort(toPort); //add cargo to port
				}
			}
		}
	}
	return instruction;

}
std::vector<Instruction>  _314636663_b::calcStowage() {
	std::vector<Instruction> instruction;
	instruction = std::vector<Instruction>(0, Instruction());
	Port* port = ship->getCurrentPort();
	vc* allCargo = port->getPortCargos();
	calcMovesTounLoad(instruction);

	std::set<string> inRoute = std::set<string>();
	std::vector<Port*> Route = *(ship->getRoute());
	std::vector<int> listOfIndx = std::vector<int>();
	int indic = 0;
	for (std::vector<Port*>::iterator it = Route.begin(); it != Route.end();
		++it) {
		if (indic == 0) {
			indic = 1;
		}
		else inRoute.insert((*it)->getPortname());
	}
	vector<Instruction> rejects;
	updateLoad(allCargo, *ship->getRoute());
	updateLoad(port->getHold(), *ship->getRoute());
	unsigned int i = 0;
	for (; i < allCargo->size() && i < (unsigned int)ship->numOfemptySpots;
		i++) {
		if (inRoute.find((*allCargo)[i]->getDest()) == inRoute.end()) {
			continue;
		}
		else {
			if (ship->onBoard((*allCargo)[i])) {
				algoException.addException(ErrorHandler::Exceptions::IDOnShip);
			}

			else
				listOfIndx.push_back(i);
		}
	}
	if (i < allCargo->size()) {
		algoException.addException(ErrorHandler::Exceptions::TooManyContainersToLoad);
	}
	std::vector<Cargo*>* vc = port->getPortCargos();
	calcMovesToLoad(&listOfIndx, instruction);

	for (unsigned int i = 0; i < vc->size(); i++) {
		instruction.push_back(Instruction('R', vc->at(i)->getId()));
	}

	instruction.insert(instruction.end(), rejects.begin(), rejects.end());

	return instruction;
}

int _314636663_b::getInstructionsForCargo(
	const std::string& input_full_path_and_file_name,
	const std::string& output_full_path_and_file_name) {
	algoException = ErrorHandler();
	if (this->cantRun != 3) {
		ofstream instructionsForCargoFile(output_full_path_and_file_name);
		instructionsForCargoFile.close();
		return this->algoException;
	}

	ofstream instructionsForCargoFile(output_full_path_and_file_name);
	ErrorHandler ex = ErrorHandler();
	set<string> rejectSet = set<string>();
	vector<Cargo*> cargos = parser.ParseContainers(input_full_path_and_file_name, &rejectSet);
	//cout<<endl<<"cargo size: " <<cargos.size()<<endl;
	algoException = algoException + parser.getParserException();
	if (ship->lastPort() && cargos.size()+rejectSet.size() != 0) {
		ex.addException(ErrorHandler::Exceptions::LastPortHasContainers);
		cargos.clear();
	}
	ship->getCurrentPort()->setPortCargos((&cargos));
	vector<Instruction> instructions = calcStowage();
	//cout <<endl<<"Instruction size:"<<instructions.size()<<endl;
	for (auto& container : rejectSet) {
		instructions.push_back({ 'R',container });
	}
	vector<Instruction>* p = &instructions;
	parser.writeInstructionsToFile(p, instructionsForCargoFile);
	ex = ex + ship->getException();
	ex = ex + algoException;
	if (ship->lastPort() || algoException.containsIlligalAlgoritmErrors()) {
		resetAlgorithms();
		this->cantRun = 1;
	}
	if (ship->lastPort()) {
		this->cantRun = 1;
	}
	else ship->SailToNextPort();
	//for (Instruction ins: instructions) {
	//	cout << endl << ins << endl;
	//}
	return ex;
}
