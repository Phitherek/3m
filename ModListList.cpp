#include "ModListList.h"
#include "3mExceptions.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace mmm;

ModListList::ModListList() {
	std::string home = getenv("HOME");
	_rmfn = home + "/.3m/remote_modlists";
	try {
	std::ifstream rmfile(_rmfn.c_str());
	if(!rmfile) {
		throw FileException(_rmfn, "reading", "Could not open file!");
	}
	std::string action = "detect";
	ModListDescription mld;
	while(!rmfile.eof()) {
	std::string line;
	rmfile >> line;
	if(rmfile) {
	if(action == "detect") {
		if(line[0] == '{') {
		std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		mld.setName(name);
		action = "parse";
		} else {
			rmfile.close();
			std::string msg;
			msg += "Found ";
			msg += line[0];
			msg += " although { was expected.";
			throw ParseException(_rmfn, msg);
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(mld.getName() != "" && mld.getServer() != "" && mld.getPath() != "") {
				addModlistDescription(mld);
				mld.clear();
			} else {
				rmfile.close();
				throw ParseException(_rmfn, "Data error.");
			}
		action = "detect";
		} else {
			rmfile.close();
			throw ParseException(_rmfn, "Found " + line + " although {end} or action in [] was expected.");
		}
	} else if(line[0] == '[') {
		std::string tmpact = "";
		for(unsigned int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "server" || tmpact == "path") {
		action = tmpact;	
		} else {
			rmfile.close();
			throw ParseException(_rmfn, "Found " + tmpact + " although server/path was expected.");
		}
	} else {
			rmfile.close();
			throw ParseException(_rmfn, "Found " + line + " although {end} or action in [] was expected.");
	}
	} else if(action == "server") {
		if(line[0] == '[' || line[0] == '{') {
			rmfile.close();
			std::string msg;
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_rmfn, msg);
		} else {
			mld.setServer(line);
			action = "parse";
		}
	} else if(action == "path") {
		if(line[0] == '[' || line[0] == '{') {
			rmfile.close();
			std::string msg;
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_rmfn, msg);
		} else {
			mld.setPath(line);
			action = "parse";
		}
	} else {
		rmfile.close();
		throw ParseException(_rmfn, "The program should not reach this place!");
	}
}	
	}
	_modlistsIterator = -1;
	_modlistsAtEnd = false;
} catch(FileException& exc) {
	std::cerr << "File exception occured: " << exc.what() << std::endl;
	_modlistsIterator = -1;
	_modlistsAtEnd = true;
} catch(ParseException& exc) {
	std::cerr << "Parse exception occured: " << exc.what() << std::endl;
	_modlistsIterator = -1;
	_modlistsAtEnd = true;
}
}

ModListList::~ModListList() {
	_modlists.clear();
}

ModListDescription ModListList::getNextModlistDescription() {
	if(_modlistsIterator+1 < _modlists.size()) {
		_modlistsIterator++;
		return _modlists[_modlistsIterator];
	} else {
		_modlistsAtEnd = true;
		ModListDescription mld;
		return mld;
	}
}

ModListDescription& ModListList::getModlistDescriptionByName(std::string name) {
	for(unsigned int i = 0; i < _modlists.size(); i++) {
		if(_modlists[i].getName() == name) {
			return _modlists[i];
		}
	}
	static ModListDescription mld;
	return mld;
}

void ModListList::addModlistDescription(ModListDescription mld) {
	_modlists.push_back(mld);
	_modlistsAtEnd = false;
}

void ModListList::deleteModlistDescription(std::string name) {
	for(std::vector<ModListDescription>::iterator i = _modlists.begin(); i < _modlists.end(); i++) {
		if(i -> getName() == name) {
			_modlists.erase(i);
		}
	}
}

void ModListList::resetModlistDescriptionIterator() {
	_modlistsIterator = -1;
	_modlistsAtEnd = false;
}

void ModListList::write() {
	std::string home = getenv("HOME");
	_rmfn = home + "/.3m/remote_modlists";
	std::ofstream rmfile(_rmfn.c_str());
	if(!rmfile) {
		throw FileException(_rmfn, "writing", "Could not open file!");
	}
	for(unsigned int i = 0; i < _modlists.size(); i++) {
		rmfile << "{" << _modlists[i].getName() << "}" << std::endl << "[server]" << std::endl << _modlists[i].getServer() << std::endl << "[path]" << std::endl << _modlists[i].getPath() << std::endl << "{end}" << std::endl;
	}
	rmfile.close();
}

bool ModListList::modlistDescriptionsAtEnd() {
	return _modlistsAtEnd;
}
