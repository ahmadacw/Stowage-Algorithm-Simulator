//==============================
//			Includes
//==============================
#pragma once
#pragma once
#define valid 1;
#define invalid 0;
#include <string>
#include <tuple>

using std::string;

//==============================
//			functions
//==============================

class Cargo {
	/* Weight in kgs (int) */
	int weight;
	/* Destination port (5 english letters of seaport code) */
	string dest;
	/* Unique identifier (as described by ISO 6346) */
	string id;

public:
	
	int forSort;// used to sort a vector of containers
	Cargo();

	Cargo(int _weight, string _dest, string _id);

	int getWeight();

	string getDest() const;

	string getId() const;

	bool operator <(const Cargo& first) const;
};
