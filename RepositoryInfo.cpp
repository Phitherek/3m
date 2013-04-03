#include "RepositoryInfo.h"
#include "3mExceptions.h"
#include <fstream>
#include <cstdlib>
using namespace mmm;

RepositoryInfo::RepositoryInfo() {
	ConfigFile emptyconf;
	_conf = emptyconf;
	_repoinfo.clear();
	_repoinfoIterator = -1;
	_repoinfoAtEnd = true;
}

RepositoryInfo::RepositoryInfo(ConfigFile conf) {
_conf = conf;
std::string rifn = _conf.getRepoInfo();
std::ifstream rifile(rifn.c_str());
if(!rifile) {
	throw FileException(rifn, "reading", "Could not open file!");
}
std::string action = "detect";
RepositoryModDescription tmprid;
while(!rifile.eof()) {
	std::string line;
	rifile >> line;
	if(rifile) {
	if(action == "detect") {
		if(line[0] == '{') {
		std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		tmprid.setName(name);
		action = "parse";
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although { was expected.";
			rifile.close();
			throw ParseException(rifn, msg);
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(tmprid.getName() != "" && tmprid.getPath() != "") {
				_repoinfo.push_back(tmprid);
				tmprid.clear();
			} else {
				rifile.close();
				throw ParseException(rifn, "Data error.");
			}
		action = "detect";
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			rifile.close();
			throw ParseException(rifn, msg);
		}
	} else if(line[0] == '[') {
		std::string tmpact = "";
		for(unsigned int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "release" || tmpact == "path") {
		action = tmpact;	
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += tmpact;
			msg += " although release/path was expected.";
			rifile.close();
			throw ParseException(rifn, msg);
		}
	} else {
		std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			rifile.close();
			throw ParseException(rifn, msg);
	}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			rifile.close();
			throw ParseException(rifn, msg);
		} else {
			tmprid.setReleaseNr(atoi(line.c_str()));
			action = "parse";
		}
	} else if(action == "path") {
	if(line[0] == '[' || line[0] == '{') {
				std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			rifile.close();
			throw ParseException(rifn, msg);
		} else {
			tmprid.setPath(line);
			action = "parse";
		}
	} else {
		rifile.close();
		throw ParseException(rifn, "The program should not reach this place!");
	}
}
}
_repoinfoIterator = -1;
_repoinfoAtEnd = false;
}

RepositoryInfo::~RepositoryInfo() {
	ConfigFile emptyconf;
	_conf = emptyconf;
	_repoinfo.clear();
	_repoinfoIterator = -1;
	_repoinfoAtEnd = true;
}

RepositoryModDescription RepositoryInfo::getNextModDescription() {
	if(_repoinfoIterator+1 < _repoinfo.size()) {
		_repoinfoIterator++;
		return _repoinfo[_repoinfoIterator];
	} else {
		static RepositoryModDescription emptyrmd;
		_repoinfoAtEnd = true;
		return emptyrmd;
	}
}

RepositoryModDescription RepositoryInfo::getModDescriptionByName(std::string name) {
	for(unsigned int i = 0; i < _repoinfo.size(); i++) {
		if(_repoinfo[i].getName() == name) {
			return _repoinfo[i];
		}
	}
	static RepositoryModDescription emptyrmd;
	return emptyrmd;
}

void RepositoryInfo::insertModDescription(RepositoryModDescription rmd) {
	_repoinfo.push_back(rmd);
	if(_repoinfoAtEnd) {
		_repoinfoAtEnd = false;
	}
}

void RepositoryInfo::deleteModDescription(std::string name) {
	for(std::vector<RepositoryModDescription>::iterator i = _repoinfo.begin(); i < _repoinfo.end(); i++) {
		if(i -> getName() == name) {
			_repoinfo.erase(i);
		}
	}
}

void RepositoryInfo::resetModDescriptionIterator() {
	_repoinfoIterator = -1;
	if(_repoinfoAtEnd) {
		_repoinfoAtEnd = false;
	}
}

bool RepositoryInfo::modDescriptionsAtEnd() {
	return _repoinfoAtEnd;
}

void RepositoryInfo::setConfigFile(ConfigFile conf) {
	_conf = conf;
}

void RepositoryInfo::write() {
	std::string rifn = _conf.getRepoInfo();
std::ofstream rifile(rifn.c_str());
if(!rifile) {
	throw FileException(rifn, "writing", "Could not open file!");
}
for(unsigned int i = 0; i < _repoinfo.size(); i++) {
	rifile << "{" << _repoinfo[i].getName() << "}" << std::endl << "[release]" << std::endl << _repoinfo[i].getReleaseNr() << std::endl << "[path]" << std::endl << _repoinfo[i].getPath() << std::endl << "{end}" << std::endl;
}
rifile.close();
}
