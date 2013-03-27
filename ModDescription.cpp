#include "ModDescription.h"
using namespace mmm;

ModDescription::ModDescription() {
	_name = "";
	_description = "";
	_release = 0;
	_depsIterator = -1;
	_depsAtEnd = true;
	_repotype = "";
	_repoaddr = "";
	_deps.clear();
}

ModDescription::~ModDescription() {
	_name = "";
	_description = "";
	_release = 0;
	_deps.clear();
	_depsIterator = -1;
	_depsAtEnd = true;
	_repotype = "";
	_repoaddr = "";
	_deps.clear();
}

std::string ModDescription::getName() {
	return _name;
}

std::string ModDescription::getDescription() {
	return _description;
}

int ModDescription::getReleaseNr() {
	return _release;
}

std::string ModDescription::getNextDependency() {
	if(_depsIterator + 1 < _deps.size()) {
		_depsIterator++;
		return _deps[_depsIterator];
	} else {
		_depsAtEnd = true;
		return "";
	}
}

std::string ModDescription::getRepositoryType() {
	return _repotype;
}

std::string ModDescription::getRepositoryAddress() {
	return _repoaddr;
}

void ModDescription::setName(std::string name) {
	_name = name;
}

void ModDescription::setDescription(std::string description) {
	_description = description;
}

void ModDescription::setReleaseNr(int release) {
	_release = release;
}

void ModDescription::insertDependency(std::string dep) {
	_deps.push_back(dep);
	if(_depsAtEnd == true) {
		_depsAtEnd = false;
	}
}

void ModDescription::setRepositoryType(std::string repotype) {
	_repotype = repotype;
}

void ModDescription::setRepositoryAddress(std::string repoaddr) {
	_repoaddr = repoaddr;
}

void ModDescription::resetDependencyIterator() {
	_depsIterator = -1;
	if(_depsAtEnd == true) {
		_depsAtEnd = false;
	}
}

bool ModDescription::dependenciesEnd() {
	return _depsAtEnd;
}

bool ModDescription::dependenciesEmpty() {
	return _deps.empty();
}

void ModDescription::clear() {
	_name = "";
	_description = "";
	_release = 0;
	_depsIterator = -1;
	_depsAtEnd = true;
	_repotype = "";
	_repoaddr = "";
	_deps.clear();
}
