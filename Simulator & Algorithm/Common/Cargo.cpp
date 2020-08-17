//==============================
//			Includes
//==============================
#include "Cargo.h"
#include <limits.h>
//==============================
//			functions
//==============================

Cargo::Cargo() :
	weight(0), id("") {
	this->dest = "";
	this->forSort = INT_MAX;
}

Cargo::Cargo(int _weight, string _dest, string _id) :
	weight(_weight), dest(_dest), id(_id) {
    this->forSort = INT_MAX;
}

bool Cargo::operator <(const Cargo& first) const {
	return (forSort < first.forSort);
}

int Cargo::getWeight() {
	return weight;
}

string Cargo::getDest() const {
	return dest;
}

string Cargo::getId() const {
	return id;
}


