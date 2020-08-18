// main function only handles the input from the command line and calls the function SImulator if all is well

#include "SimulationBackEnd.cpp"
int main(int argc, char** argv) {
	string travelPath;
	string algorithmPath = ".";
	string outputDirectory = "";
	int numThreads = 1;
	bool pathsFound[4] = { false,false,false,false };
	for (int i = 1; i < argc - 1; i++) {
		if (strcmp(argv[i], "-travel_path") == 0) {
			travelPath = argv[i + 1];
			i++;
			pathsFound[0] = true;

		}
		if (strcmp(argv[i], "-algorithm_path") == 0) {
			algorithmPath = argv[i + 1];
			i++;
			pathsFound[1] = true;
		}
		if (strcmp(argv[i], "-output") == 0) {
			outputDirectory = argv[i + 1];
			outputDirectory += "/";
			pathsFound[2] = true;
			i++;

		}
		if (strcmp(argv[i], "-num_threads") == 0) {
			try {
				numThreads = std::stoi(argv[i + 1]);
				pathsFound[3] = true;
			}
			catch (std::exception& e) {
				numThreads = -1;
			}
		}
	}
	if (numThreads == 0) {
		std::cout << std::endl << "num Threads is 0... bro ?" << std::endl;
		return 0;
	}
	if (numThreads == -1) {
		std::cout << std::endl << "something is wrong with the numThreads param" << std::endl;
	}
	if (!pathsFound[1]) {
		algorithmPath = "./";
	}
	if (!pathsFound[2])
		outputDirectory += "./";
	string outputErrorFile = outputDirectory + "/simulation.errors";
	std::filesystem::path output = outputDirectory;
	try {
		std::filesystem::create_directory(output);
	}
	catch (std::filesystem::filesystem_error& e) {
		std::cout << std::endl << "couldn't create Directory at path: " << output << std::endl;
	}

	if (!pathsFound[0]) {
		std::ofstream ErrorFile(outputErrorFile);
		ErrorFile << "Missing Argument -travel_path [path]";
		ErrorFile.close();
		return EXIT_FAILURE;
	}

	try {
		std::filesystem::create_directory(outputDirectory);
	}
	catch (std::filesystem::filesystem_error& e) {
		std::cout << std::endl << "couldn't create Directory at path: " << "./" << std::endl;
	}
	return startSimulating(travelPath, algorithmPath, outputDirectory, numThreads);
}
