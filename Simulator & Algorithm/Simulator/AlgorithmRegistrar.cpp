// AlgorithmRegistrar.cpp
#include "AlgorithmRegistrar.h"
#include <dlfcn.h>
#include <iostream>

void AlgorithmRegistrar::DlCloser::operator()(void *dlhandle) const noexcept 
{
	std::cout << "Closing dl\n";
	dlclose(dlhandle);
}

AlgorithmRegistrar& AlgorithmRegistrar::getInstance()
{
	static AlgorithmRegistrar instance;
	return instance;
}

AlgorithmRegistrar::~AlgorithmRegistrar()
{
	// Remove all factories - before closing all handles!
	_factories.clear();
	_handles.clear();
}

bool AlgorithmRegistrar::loadAlgorithmFromFile(const char *file_path, std::string& error)
{
    	// Try to load given module
    	DlHandler algo_handle(dlopen(file_path, RTLD_LAZY));

    	// TODO: what if no algorithm was registerd???
	// TODO: more... 

    	// Check if dlopen succeeded
    	if (algo_handle != nullptr) {
	    	_handles[file_path] = std::move(algo_handle);
        	return true;
    	}
    
    	// On error - TODO: need to handle other cases..
    	const char *dlopen_error = dlerror();
	error = dlopen_error ? dlopen_error : "Failed to open shared object!";
    	return false;
}
