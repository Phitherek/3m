#include "ModList.h"
#include "NetSocket++/NetSocketPP.h"
#include "3mExceptions.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace mmm;

ModList::ModList() {
	ModListDescription emptymld;
	_desc = emptymld;
	_modinfos.clear();
	_edit = false;
	_localPath = "";
	_modinfosIterator = -1;
	_modinfosAtEnd = true;
}

ModList::ModList(ModListDescription mld) {
	_desc = mld;
	_modinfos.clear();
	_edit = false;
	_localPath = "";
	_modinfosIterator = -1;
	_modinfosAtEnd = false;
	NetSocketPP::HTTPReply data;
	try {
	NetSocketPP::HTTPClientSocket conn(_desc.getServer(), "http", _desc.getPath());
	std::cout << "Connection to " << conn.getIP() << " successful!" << std::endl;
	data = conn.getReply();
	} catch (NetSocketPP::SocketException &exc) {
		std::cerr << "SocketException occured while getting remote modlist: " << exc.what() << std::endl;
		throw(exc);
	} catch (NetSocketPP::NetworkException &exc) {
		std::cerr << "NetworkException occured while getting remote modlist: " << exc.what() << std::endl;
		throw(exc);
	}
	if(data.getResponse() != "200 OK") {
		throw BadResponseException(data.getResponse());
	}
	std::string content = data.getContent();
	std::vector<std::string> modlist = strtovec(content);
		std::string action = "detect";
		ModInfoDescription mid;
	for(unsigned int i = 0; i < modlist.size(); i++) {
	std::string line = "";
	line = modlist[i];
	if(line[0] != NULL && line[0] != ' ' && line[0] != '\n' && line[0] != '\r') {
	if(action == "detect") {
		if(line[0] == '{') {
			std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		mid.setName(name);
		action = "parse";
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although { was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(mid.getName() != "" && mid.getServer() != "" && mid.getPath() != "") {
				ModInfo mi;
				mi.setModInfoDescription(mid);
				_modinfos.push_back(mi);
			} else {
				throw ParseException(_desc.getServer() + _desc.getPath(), "Data error.");
			}
		action = "detect";
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		}
	} else if(line[0] == '[') {
		std::string tmpact = "";
		for(unsigned int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "server" || tmpact == "modinfo") {
		action = tmpact;	
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += tmpact;
			msg += " although server/modinfo was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		}
	} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
	}
	} else if(action == "server") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			mid.setServer(line);
			action = "parse";
		}
	} else if(action == "modinfo") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			mid.setPath(line);
			action = "parse";
		}
	} else {
		throw ParseException(_desc.getServer() + _desc.getPath(), "The program should not reach this place!");
	}
}
}
std::cout << "Got all modinfo descriptions from " << _desc.getName() << ", downloading and parsing modinfos..." << std::endl;
for(unsigned int i = 0; i < _modinfos.size(); i++) {
	try {
		ModInfo mi(_modinfos[i].getModInfoDescription());
		std::cout << "Successfully downloaded and parsed: " << mi.getName() << std::endl;
		_modinfos[i] = mi;
		sleep(2); // This is to prevent DoS-ing rubenwardy' s hosting...
	} catch(NetSocketPP::NetworkException &exc) {
		std::cerr << "NetworkException occured in NetSocket++: " << exc.what() << std::endl;
	} catch(NetSocketPP::SocketException &exc) {
		std::cerr << "SocketException occured in NetSocket++: " << exc.what() << std::endl;
	} catch(ParseException &exc) {
		std::cerr << "ParseException occured: " << exc.what() << std::endl;
	} catch(BadResponseException &exc) {
		std::cerr << "BadResponseException occured: " << exc.what() << std::endl;
	}
}
}

ModList::ModList(std::string path) {
	ModListDescription emptymld;
	_desc = emptymld;
	_modinfos.clear();
	_edit = true;
	_localPath = path;
	_modinfosIterator = -1;
	_modinfosAtEnd = false;
	std::ifstream modlist(_localPath.c_str());
	if(!modlist) {
		throw FileException(_localPath, "reading", "Could not open file!");
	}
		std::string action = "detect";
		ModInfoDescription mid;
	while(!modlist.eof()) {
	std::string line = "";
	char c;
	do {
	modlist.get(c);
	if(c != '\n');
	line += c;
	} while(c != '\n');
	line = strip_endl(line);
	if(modlist) {
	if(line[0] != NULL && line[0] != ' ' && line[0] != '\n' && line[0] != '\r') {
	if(action == "detect") {
		if(line[0] == '{') {
			std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		mid.setName(name);
		action = "parse";
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although { was expected.";
			throw ParseException(_localPath, msg);
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(mid.getName() != "" && mid.getServer() != "" && mid.getPath() != "") {
				ModInfo mi;
				mi.setModInfoDescription(mid);
				_modinfos.push_back(mi);
			} else {
				throw ParseException(_localPath, "Data error.");
			}
		action = "detect";
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_localPath, msg);
		}
	} else if(line[0] == '[') {
		std::string tmpact = "";
		for(unsigned int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "server" || tmpact == "modinfo") {
		action = tmpact;	
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += tmpact;
			msg += " although server/modinfo was expected.";
			throw ParseException(_localPath, msg);
		}
	} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_localPath, msg);
	}
	} else if(action == "server") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_localPath, msg);
		} else {
			mid.setServer(line);
			action = "parse";
		}
	} else if(action == "modinfo") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_localPath, msg);
		} else {
			mid.setPath(line);
			action = "parse";
		}
	} else {
		throw ParseException(_localPath, "The program should not reach this place!");
	}
}
}
}
modlist.close();
}

ModList::~ModList() {
	ModListDescription emptymld;
	_desc = emptymld;
	_modinfos.clear();
	_edit = false;
	_localPath = "";
	_modinfosIterator = -1;
	_modinfosAtEnd = true;
}

void ModList::clear() {
	ModListDescription emptymld;
	_desc = emptymld;
	_modinfos.clear();
	_edit = false;
	_localPath = "";
	_modinfosIterator = -1;
	_modinfosAtEnd = true;
}

ModInfo ModList::getNextModInfo() {
	if(_modinfosIterator+1 < _modinfos.size()) {
		_modinfosIterator++;
		return _modinfos[_modinfosIterator];
	} else {
		static ModInfo emptymi;
		_modinfosAtEnd = true;
		return emptymi;
	}
}

ModInfo ModList::getModInfoByName(std::string name) {
	for(unsigned int i = 0; i < _modinfos.size(); i++) {
		if(_modinfos[i].getModInfoDescription().getName() == name) {
			return _modinfos[i];
		}
	}
	static ModInfo emptymi;
	return emptymi;
}

void ModList::insertModInfoDescription(ModInfoDescription mid) {
	ModInfo mi;
	mi.setModInfoDescription(mid);
	_modinfos.push_back(mi);
	if(_modinfosAtEnd) {
		_modinfosAtEnd = false;
	}
}

void ModList::deleteModInfo(std::string name) {
	for(std::vector<ModInfo>::iterator i = _modinfos.begin(); i != _modinfos.end(); i++) {
		mmm::ModInfoDescription mid = i -> getModInfoDescription();
		if(mid.getName() == name) {
			_modinfos.erase(i);
			return;
		}
	}
}

void ModList::resetModInfoIterator() {
	_modinfosIterator = -1;
	if(_modinfosAtEnd) {
		_modinfosAtEnd = false;
	}
}

bool ModList::modinfosEnd() {
	return _modinfosAtEnd;
}

void ModList::write() {
	if(!_edit) {
		throw NonEditableException("Tried to write back remotely obtained modlist file!");
	}
	std::ofstream modlist(_localPath.c_str());
	if(!modlist) {
		throw FileException(_localPath, "writing", "Could not open file!");
	}
	for(unsigned int i = 0; i < _modinfos.size(); i++) {
		ModInfoDescription mid = _modinfos[i].getModInfoDescription();
		modlist << "{" << mid.getName() << "}" << std::endl << "[server]" << std::endl << mid.getServer() << std::endl << "[modinfo]" << std::endl << mid.getPath() << std::endl << "{end}" << std::endl;
	}
	modlist.close();
}

ModListDescription ModList::getModListDescription() {
	return _desc;
}

void ModList::setPath(std::string path) {
	_localPath = path;
	_edit = true;
}
