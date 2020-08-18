#include <cstdlib>
#include <string.h>
#include <algorithm>
#include <list>
#include <set>
#include <math.h>
#include "SimulationBackEnd.h"
#include <sstream>
using std::list;
using std::set;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::filesystem::path;
/* compare vector function for the sorting of the results*/
Parser parser=Parser();
bool compareVector(vector<int> v1, vector<int> v2) {
	if (v1.size() == 0) return true;
	if (v2.size() == 0 && v1.size() == 0) return false;
	if (v2.size() == 0) return false;

	if (v1[v1.size() - 2] < v2[v2.size() - 2]) {
		return true;
	}
	if (v1[v1.size() - 2] == v2[v2.size() - 2])
		return v1[v1.size() - 3] < v2[v2.size() - 3];
	return false;
}
// counts the errors in the number ex per the expected format
int countErrors(int ex) {
	int out = 0;
	while (ex) {
		if (ex % 2)
			out++;
		ex /= 2;
	}
	return out;
}
//takes a path and returns the name of a file

// initlize the first line of the reustls folder
vector<string> initCSVFile(string& outputCsvFile, vector<string>& okayPile) {
	std::ofstream CSVfile(outputCsvFile);
	vector<string> spaceAti;
	if (CSVfile.is_open()) {
		CSVfile << "RESULTS,";

		spaceAti.push_back({});
		for (unsigned int i = 0; i < okayPile.size(); i++) {
			string name = okayPile.at(i);
			spaceAti.push_back({});
			for (unsigned int j = 0; j < getName(name).size(); j++) spaceAti[i] += " ";
			CSVfile << getName(name) << ",";


		}
		CSVfile << "totalInstructions,errors" << endl;

		for (unsigned int j = 0; j < 18; j++) spaceAti[spaceAti.size() - 1] += " ";
		spaceAti.push_back({});

		for (unsigned int j = 0; j < 10; j++) spaceAti[spaceAti.size() - 1] += " ";

		CSVfile.close();

	}
	else {
		return spaceAti;
	}
	return spaceAti;

}
int extractTravels(std::string& travelPath, std::ofstream& Errorfile, std::set<std::string>& badPaths, std::set<std::string>& tmp2, std::vector<std::string>& goodTravels, std::string& outputDirectory, bool& retflag);
int startSimulating(std::string& travelPath, std::string& algorithmPath, std::string& outputDirectory,int numThreads);


// for each travel checks if the travel is valid, one ship plan, one route and that they are openable
// any path that doesn't contain a vlaid travel is inserted to badPaths
set<string> checkTravelCorrectness(set<string>& paths,set<string>badPaths) {
	vector<string> erase = { };
	Parser parser = Parser();
	Cargo shipEmpty = Cargo();
	Cargo shipUnavailable = Cargo();
	vector<Port*> route;
	for (auto& path : paths) {
		if (ends(path, "..") || ends(path, ".ini"))continue;
		set<string> currentFiles = readFiles(path, ".ship_plan");
		//			stream.close();

		if (currentFiles.size() != 1) {
					badPaths.insert(path);
					continue;
				}
		ErrorHandler error = ErrorHandler();
				string ShipPlan = (*currentFiles.begin());
				vector<vector<int>> plan= parser.readShipPlanEx(ShipPlan);
				error = parser.getParserException();
				if (error.containsFatalShipPlanEx()) {
					badPaths.insert(path);
					continue;
				}
				int dimX = (plan[0][1]);
				int dimY = (plan[0][2]);
				int floorsNum = (plan[0][0]);
				Ship simShip = Ship(&plan, dimX, dimY, floorsNum, route, shipEmpty, shipUnavailable);
				error=error+simShip.getException();
				if (error.containsFatalShipPlanEx()) {
					badPaths.insert(path);
					continue;
					
				}
				currentFiles = readFiles(path, ".route");
				if (currentFiles.size() != 1) {
					badPaths.insert(path);
					continue;
				}

            if(parser.readShipRouteEx((*currentFiles.begin())).size()<2) {
					badPaths.insert(path);
					continue;
				}

		}
	for (auto& path : badPaths) {
		paths.erase(path);
	}
	return badPaths;
}

//prints the paths for the illegal travels
void printErrorPath(set<string> badPaths, ostream& Errorfile) {
	for (auto& path : badPaths) {
		Errorfile << "Bad Travel File " << path << std::endl;
	}
}

//prints the results to the CSVFIle, resulst[i]=result for algorithm results[i][0]
// this was done is such a format becuase we had to keep track of the result after it was sorted
void printResults(vector<string> algoNames, vector<vector<int>> results, string dest) {
	ofstream csvFile(dest, std::ios::app);
	string name="";
	for(auto& result: results){
		name=getName(algoNames[result[result.size()-1]]);
		csvFile << name << ",";
		cout << name << ",";
		for (unsigned int j = 0; j <result.size()-1; j++) {
			if (j != result.size() - 2) {
				string str = to_string(result[j]) += ",";
				csvFile << str << "";
				cout << str << "";

			}
			else {

				string str = to_string(result[j]);
				csvFile << str << "" << endl;
				cout << str << "" << endl;


			}
		}
	}
	csvFile.close();

}

// hash function from error code to string
string getErrorString(int i) {
	string str = "2^" + std::to_string(i) + "-";
	switch (i) {
	case(0):
		str += " position has an equal number of floors, or more, than the number of floorsprovided in the first line";
		return str;
	case(1):
		str += "a given position exceeds the X/Y ship limits";
		return str;
	case(2):
		str += "bad line format after first line or duplicate x,y appearance with same data";
		return str;
	case(3):
		str += "travel error - bad first line or file cannot be read altogether";
		return str;
	case(4):
		str += "travel error - duplicate x,y appearance with different data";
		return str;
	case(5):
		str += "a port appears twice or more consecutively";
		return str;

	case(6):
		str += "bad port symbol format";
		return str;

	case(7):
		str += "travel error - empty file or file cannot be read altogether";

		return str;

	case(8):
		str += "travel error - file with only a single valid port";
		return str;

	case(9):
		return string("");

	case(10):
		str += "duplicate ID on port";
		return str;

	case(11):
		return str + string("ID already on ship");

	case(12):
		return str + string("bad line format, missing or bad weight");

	case(13):
		return str + string("bad line format, missing or bad port dest");

	case(14):
		return str + string("bad line format, ID cannot be read");

	case(15):
		return str + string("illegal ID check ISO 6346");

	case(16):
		return str + string("file cannot be read altogether");

	case(17):
		return str + string("last port has waiting containers");

	case(18):
		return str + string("total containers amount exceeds ship capacity");


	}
	return str;

}

//initlize the file for the travel errors
void initTravelErrorFolder(string path, string name) {
	static set<string> initiated;
	if (initiated.contains(path)) return;
	initiated.insert(path);
	string line = "";
	string errorHeader = "";
	ofstream outputFile(path);
	for (int i = 0; i < 33; i++) line += "=";
	for (int i = 0; i < 4; i++) errorHeader += "";
	errorHeader += "start of error file for " + name;
	for (int i = 0; i < 4; i++) errorHeader += "";
	outputFile << line << endl << errorHeader << endl << line;
	outputFile.close();
}

// prints the algorithm errors to the right travel file
void printAlgorithmErrors(string algorithmErrorPath, string algoName,string name, int result) {
	string line = "";
	string errorHeader = "";
	initTravelErrorFolder(algorithmErrorPath, name);
	ofstream outputFile(algorithmErrorPath, std::ios::app);
	outputFile << endl;
	for (int i = 0; i < 33; i++) line += "*";
	errorHeader += "errors for algorithm" + algoName;
	outputFile << endl << line << endl << errorHeader << endl << line << endl;

	int i = 0;
	while (result != 0) {
		string currentErrorString;
		if (result % 2 != 0) {
			currentErrorString = getErrorString(i);
			if (currentErrorString.size() != 0) {
				outputFile << currentErrorString;

			}
		}
		if (result != 1 && result != 0 && result % 2 == 1) {
			outputFile << endl;
		}
		result /= 2;
		i++;
	}
	outputFile.close();
}

// deals with the logistecs for the validation of each travel file path
int extractTravels(std::string& travelPath, std::ofstream& Errorfile, std::set<std::string>& badPaths, std::vector<std::string>& goodTravels, std::string& outputDirectory, bool& retflag)
{
	retflag = true;
	std::set<std::string> tmp2;
	string st = travelPath;
	set<string> filesInTravelFolder = readFiles(st, ".*");
	if (filesInTravelFolder.size() == 0) {
		cout << "couldn't open travels folder or Folder is empty: " << st << std::endl;
		Errorfile << "couldn't open travels folder or Folder is empty" << std::endl;
		Errorfile.close();
		return EXIT_SUCCESS;
	}
	// for each Diretory which was found, check if it holds a valid Travel
	checkTravelCorrectness(filesInTravelFolder, badPaths);
	if (badPaths.size() != 0) {
		printErrorPath(badPaths, Errorfile);
	}

	if (filesInTravelFolder.size() == 0) {
		cout << "no legal Travels to run" << std::endl;
		Errorfile << "no legal Travels to run" << std::endl;

		Errorfile.close();
		return EXIT_SUCCESS;
	}

	for (auto& path : filesInTravelFolder) {
		if (!badPaths.count(path))
			tmp2.insert(path);
	}
	for (auto& path : tmp2) goodTravels.push_back(path);
	for (auto& str : goodTravels) {
		string path = outputDirectory + "errors/";
		path += getName(str) + "_TravelErrors";
	}
	retflag = false;
	return {};
}

static std::string setInstructionDirectoryPath(int i, std::string &instructionDirectory, vector<std::string> &okayPile, std::basic_string<char> &travelPath) {
    return instructionDirectory += getName(okayPile[i]) + "_" + getName(travelPath) + "_crane_instructions";
}

static std::string setAlgorithmErrorPath(std::string &algorithmErrorPath, std::basic_string<char> &travelPath) {
    return algorithmErrorPath += "errors/" + getName(travelPath) + "_.TravelErrors";
}

int startSimulating(std::string& travelPath, std::string& algorithmPath, std::string& outputDirectory, int numThreads)
{
	vector<vector<string>> names;
	vector<string> tmp;
	set<string> badPaths = {};
	vector<string> goodTravels;
	vector<int> result;
	string ShipRoute;
	set<string> rejectPile;
	vector<string> okayPile;
	string error;
	string outputErrorFile = outputDirectory + "simulation.errors";
	path output = outputDirectory;
	ofstream Errorfile(outputErrorFile);
	string tmp3 = outputDirectory + "errors";
	filesystem::create_directory(tmp3);
	filesystem::create_directory(output);
	string outputCsvFile = outputDirectory + "simulation.results";
	Errorfile << "start of error file\n";

	bool retflag;
	int retval = extractTravels(travelPath, Errorfile, badPaths, goodTravels, outputDirectory, retflag);
	if (retflag) return retval;

	set<string> AlgorithmFiles = readFiles(algorithmPath, ".so");
	if (AlgorithmFiles.size() == 0) {
		cout<<"inside here"<<endl;
		Errorfile << "Couldn't find any so files";
		Errorfile.close();
		return EXIT_SUCCESS;

	}
	{
	AlgorithmRegistrar& rar = AlgorithmRegistrar::getInstance();
	// spits out the so files that were not regestired
	for (auto& algorithmPath : AlgorithmFiles) {
		if (!rar.loadAlgorithmFromFile(algorithmPath.c_str(), error)) {
			rejectPile.insert(getName(algorithmPath));
			cout<<error<<endl;
		}
		else {
			okayPile.push_back(algorithmPath);
		}
	}
	}	
	if (okayPile.size() == 0) {
		Errorfile << "no Algorithm Registered";
		Errorfile.close();
		return EXIT_SUCCESS;
	}
	if (rejectPile.size() != 0) {
		for (auto& r : rejectPile) {
			Errorfile << r << " holds a path to a non registered so file" << endl;
		}
	}
	vector<string> spaces = initCSVFile(outputCsvFile, goodTravels); // need to intiate the first line of the file

	if (spaces.size() == 0) {
		Errorfile << "couldn't open csvFile" << endl;
		Errorfile.close();
		return EXIT_SUCCESS;
	}
	int numAlgo = 0;

	vector<string>ErrorPaths;
	numAlgo = 0;
	vector<Simulator> pairs = vector<Simulator>(okayPile.size() * goodTravels.size());
	int i = 0;
	std::mutex tmutex;
	std::condition_variable statIndicator;
	{	unique_lock<mutex> myLock{ tmutex };
		ThreadPool pool(numThreads, okayPile.size() * goodTravels.size(),goodTravels.size(), okayPile,&statIndicator, &tmutex);
		for(unsigned int l=0;l<okayPile.size();l++) {
			// I need to make a vector of SImulators and then enqueue each of them
			for (auto& travelPath : goodTravels) { //for each travel
				string instructionDirectory = outputDirectory;
				string algorithmErrorPath = outputDirectory;
				//nd the path for the ship plan file
				set<string> plan = readFiles(travelPath, ".ship_plan");
				setInstructionDirectoryPath(numAlgo, instructionDirectory, okayPile, travelPath);
				setAlgorithmErrorPath(algorithmErrorPath, travelPath);
				ErrorPaths.push_back(algorithmErrorPath);
				string shipPlanPath = *(plan.begin());
				filesystem::create_directory(instructionDirectory);
				// find the path for the route file
				string shipRoutePath = *readFiles(travelPath, ".route").begin(); // find in directory travelPath, a file which ends in .route'
				output = instructionDirectory;
				//this is were the simulation happens, result[0] is the number of operations, result[1] is the error code for the Algorithm
				pairs[i].initSimulator(shipPlanPath, shipRoutePath, travelPath,
					instructionDirectory);// add the ready Simulator to pairs
				pool.enqueue(&pairs[i]);
			
				i++;
			}
			numAlgo++;
		}
		statIndicator.wait(myLock);
	}
	vector<vector<int>> results;
	for (unsigned i = 0; i < okayPile.size(); i++) {
			int numErrorsForAlgorithm = 0;
			int numOperationsForAlgorithm = 0;
			results.push_back({});
			for (unsigned int j = 0; j < goodTravels.size(); j++) {
				result = pairs[j + i * goodTravels.size()].getResult();
               			numErrorsForAlgorithm += countErrors(result[1]); // updates total number of errors for algorithm
				results[i].push_back(result[0]);
				printAlgorithmErrors(ErrorPaths[j + i * goodTravels.size()], getName(okayPile[i]), getName(travelPath), result[1]);
				if (result[0] != -1)
					numOperationsForAlgorithm += result[0];
				if (pairs[j + i * goodTravels.size()].unusedCargoFiles)
					Errorfile << "cargo data file found but not used" << endl;
			}
			results[i].push_back(numOperationsForAlgorithm);
			results[i].push_back(numErrorsForAlgorithm);
		}
	for(unsigned int k=0;k<okayPile.size();k++){

		results[k].push_back(k);
	}
	sort(results.begin(), results.end(), compareVector);
	Errorfile.close();
	initCSVFile(outputCsvFile, goodTravels); // need to intiate the first line of the file
	printResults(okayPile, results, outputCsvFile);
	Errorfile.close();
	i = 0;
	string line;
	ifstream inputFile(outputErrorFile);

	while (getline(inputFile, line)) {
		i++;
	}
	inputFile.close();
	if (i==1){
		std::remove(outputErrorFile.c_str());
	}
	return EXIT_SUCCESS;
}

