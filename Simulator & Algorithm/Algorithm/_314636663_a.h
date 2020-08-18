//==============================
//			Includes
//==============================
#pragma once

#include<vector>
#include<tuple>
#include <string>
#include <vector>
#include "../Common/Parser.cpp"
#include "../Common/AbstractAlgorithm.h"
#include "../Common/AlgorithmRegistration.h"
#include "../Common/WeightBalanceCalculator.cpp"

//==============================
//			functions
//==============================
class _314636663_a: public AbstractAlgorithm{
private:
    Parser parser;
 	void resetAlgorithms();
	std::vector<Instruction> calcMovesToLoad(std::vector<int>* listOfIndx, vector<Instruction>& instruction);
	int moveContainer(int curHeight, int Curx, int Cury, vector<Instruction>& instructions);
	std::vector<Instruction> calcMovesTounLoad(vector<Instruction>& instruction);
	std::vector<Instruction> calcStowage();
public:
	_314636663_a();
	~_314636663_a();
	public:
	shared_ptr<Ship> ship;
	vector<vector<int>> plan;
	vector<Port*> route;
	Cargo shipEmpty; 
	Cargo shipUnavailable;
	WeightBalanceCalculator naive;
	int numOperations;
	ErrorHandler algoException=ErrorHandler();
	int cantRun=0;
	//read the ship plan from the given path
	int readShipPlan(const std::string& full_path_and_file_name);
	//read the ship route from the given path
	int readShipRoute(const std::string& full_path_and_file_name);
	int setWeightBalanceCalculator(WeightBalanceCalculator& calculator);
	//calculate the instruction set for cargo in the input for and output the instructions to the output path
	int getInstructionsForCargo(
				const std::string& input_full_path_and_file_name,
				const std::string& output_full_path_and_file_name);


};



