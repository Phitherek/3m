#include "3m.h"
#include <fstream>
#include <cstdlib>
using namespace mmm;

LocalModList::LocalModList() {
	ConfigFile emptycf;
	_conf = emptycf;
	_modlistIterator = -1;
	_modlistAtEnd = true;
	_modlist.clear();
}

LocalModList::LocalModList(ConfigFile conf) {
	_conf = conf;
	_modlist.clear();
	std::ifstream lmfile(_conf.getModList().c_str());
		if(!lmfile) {
		throw FileException(_conf.getModList(), "reading", "Could not open file!");
	}
	std::string action = "detect";
LocalModDescription lmld;
while(!lmfile.eof()) {
	std::string line = "";
	char c;
	do {
	lmfile.get(c);
	if(c != '\n');
	line += c;
	} while(c != '\n');
	line = strip_endl(line);
	if(lmfile) {
	if(action == "detect") {
		if(line[0] == '{') {
		std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		lmld.setName(name);
		action = "parse";
		} else {
			std::string msg = "";
			msg = "Found ";
			msg += line[0];
			msg += " although { was expected.";
			lmfile.close();
			throw ParseException(_conf.getModList(), msg);
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(lmld.getName() != "" && lmld.getRemoteModlistName() != "" && lmld.getDescription() != "" && !lmld.dependenciesEmpty() && lmld.getRepositoryType() != "" && lmld.getRepositoryAddress() != "") {
				_modlist.push_back(lmld);
				if(_modlistAtEnd) {
					_modlistAtEnd = false;
				}
				lmld.clear();
				action = "detect";
			} else {
				lmfile.close();
				throw ParseException(_conf.getModList(),"Data error.");
			}
		action = "detect";
		} else {
			lmfile.close();
			std::string msg;
			msg = "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_conf.getModList(), msg); 
		}
	} else if(line[0] == '[') {
		std::string tmpact = "";
		for(unsigned int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "rmodlist" || tmpact == "description" || tmpact == "release" || tmpact == "deps" || tmpact == "repotype" || tmpact == "repoaddr") {
		action = tmpact;	
		} else {
			std::string msg;
			msg = "Found ";
			msg += tmpact;
			msg += " although rmodlist/description/release/deps/repotype/repoaddr was expected.";
			lmfile.close();
			throw ParseException(_conf.getModList(), msg);
		}
	} else {
		std::string msg;
		msg = "Found ";
		msg += line;
		msg += " although {end} or action in [] was expected.";
		lmfile.close();
		throw ParseException(_conf.getModList(), msg);
	}
	} else if(action == "rmodlist") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg;
			msg = "Found ";
			msg += line[0];
			msg += " although string was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
		} else {
			lmld.setRemoteModlistName(line);
			action = "parse";
		}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg;
			msg = "Found ";
			msg += line[0];
			msg += " although string was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
		} else {
			lmld.setDescription(line);
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg;
			msg = "Found ";
			msg += line[0];
			msg += " although string was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
		} else {
			lmld.setReleaseNr(atoi(line.c_str()));
			action = "parse";
		}
	} else if(action == "deps") {
		if(line[0] == '{') {
			std::string msg;
			msg = "Found ";
			msg += line[0];
			msg += " although string or [ was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
		} else if(line[0] == '[') {
			if(line[1] == 'd' && line[2] == 'e' && line[3] == 'p' && line[4] == 's' && line[5] == 'e' && line[6] == 'n' && line[7] == 'd' && line[8] == ']') {
				if(lmld.dependenciesEmpty()) {
				lmld.insertDependency("none");	
				}
			action = "parse";	
			} else {
			std::string msg;
			msg = "Found ";
			msg += line;
			msg += " although string or [depsend] was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
			}
		} else {
		lmld.insertDependency(line);	
		}
	} else if(action == "repotype") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg;
			msg = "Found ";
			msg += line[0];
			msg += " although string was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
		} else {
			lmld.setRepositoryType(line);
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			std::string msg;
			msg = "Found ";
			msg += line[0];
			msg += " although string was expected.";
			lmfile.close();
			throw(_conf.getModList(), msg);
		} else {
			lmld.setRepositoryAddress(line);
			action = "parse";
		}
	} else {
		lmfile.close();
		throw ParseException(_conf.getModList(), "The program should not reach this place!");
	}
}
}
lmfile.close();
_modlistIterator = -1;
_modlistAtEnd = true;
}

LocalModList::~LocalModList() {
	ConfigFile emptycf;
	_conf = emptycf;
	_modlist.clear();
	_modlistIterator = -1;
	_modlistAtEnd = true;
}

void LocalModList::addModDescription(LocalModDescription lmd) {
	_modlist.push_back(lmd);
	if(_modlistAtEnd) {
		_modlistAtEnd = false;
	}
}

LocalModDescription LocalModList::getNextModDescription() {
	if(_modlistIterator+1 < _modlist.size()) {
	_modlistIterator++;
	return _modlist[_modlistIterator];
	} else {
		_modlistAtEnd = true;
		LocalModDescription emptylmd;
		return emptylmd;
	}
}

LocalModDescription LocalModList::getModDescriptionByName(std::string name) {
	for(unsigned int i = 0; i < _modlist.size(); i++) {
		if(_modlist[i].getName() == name) {
			return _modlist[i];
		}
	}
	LocalModDescription emptylmd;
	return emptylmd;
}

void LocalModList::resetModDescriptionIterator() {
	if(_modlistAtEnd) {
		_modlistAtEnd = false;
	}
	_modlistIterator = -1;
}

bool LocalModList::modDescriptionsAtEnd() {
	return _modlistAtEnd;
}

void LocalModList::setConfigFile(ConfigFile conf) {
	_conf = conf;
}

void LocalModList::write() {
	std::ofstream lmfile(_conf.getModList().c_str());
	if(!lmfile) {
		lmfile.close();
		throw FileException(_conf.getModList(), "writing", "Could not open file!");
	}
	for(unsigned int i = 0; i < _modlist.size(); i++) {
		lmfile << "{" << _modlist[i].getName() << "}" << std::endl << "[rmodlist]" << std::endl << _modlist[i].getRemoteModlistName() << std::endl << "[description]" << std::endl << _modlist[i].getDescription() << std::endl << "[release]" << std::endl << _modlist[i].getReleaseNr() << std::endl << "[deps]" << std::endl;
		_modlist[i].resetDependencyIterator();
		while(!_modlist[i].dependenciesEnd()) {
			std::string dep = _modlist[i].getNextDependency();
			if(dep != "") {
				lmfile << dep << std::endl;
			}
		}
		lmfile << "[depsend]" << std::endl << "[repotype]" << std::endl << _modlist[i].getRepositoryType() << std::endl << "[repoaddr]" << std::endl << _modlist[i].getRepositoryAddress() << std::endl << "{end}" << std::endl;
	}
	lmfile.close();
}
