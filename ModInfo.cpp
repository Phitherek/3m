#include "ModInfo.h"
#include "NetSocket++/NetSocketPP.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace mmm;

ModInfo::ModInfo() {
	ModInfoDescription emptymid;
	_desc = emptymid;
	_edit = false;
}

ModInfo::ModInfo(ModInfoDescription mid) {
	_desc = mid;
	_edit = false;
	NetSocketPP::HTTPReply data;
	try {
	NetSocketPP::HTTPClientSocket conn(_desc.getServer(), "http", _desc.getPath());
	std::cout << "Connection to " << conn.getIP() << " successful!" << std::endl;
	data = conn.getReply();
	} catch (NetSocketPP::SocketException &exc) {
		std::cerr << "SocketException occured while getting remote modinfo: " << exc.what() << std::endl;
		throw(exc);
	} catch (NetSocketPP::NetworkException &exc) {
		std::cerr << "NetworkException occured while getting remote modinfo: " << exc.what() << std::endl;
		throw(exc);
	}
	if(data.getResponse() != "200 OK") {
		throw BadResponseException(data.getResponse());
	}
	std::string smodinfo = data.getContent();
	std::vector<std::string> modinfo = strtovec(smodinfo);
	std::string action = "detect";
for(unsigned int i = 0; i < modinfo.size(); i++) {
	std::string line = "";
	line = modinfo[i];
	if(line[0] != NULL && line[0] != ' ' && line[0] != '\n' && line[0] != '\r') {
	if(action == "detect") {
		if(line[0] == '{') {
			std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		_name = name;
		_desc.setName(name);
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
		if(tmpact == "description" || tmpact == "release" || tmpact == "deps" || tmpact == "repotype" || tmpact == "repoaddr") {
		action = tmpact;	
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += tmpact;
			msg += " although description/release/deps/repotype/repoaddr was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		}
	} else {
		std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
	}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_description = line;
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "deps") {
		if(line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string or [ was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else if(line[0] == '[') {
			if(line[1] == 'd' && line[2] == 'e' && line[3] == 'p' && line[4] == 's' && line[5] == 'e' && line[6] == 'n' && line[7] == 'd' && line[8] == ']') {
				if(_deps.empty()) {
				_deps.push_back("none");	
				}
			action = "parse";	
			} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although string or [depsend] was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
			}
		} else {
		_deps.push_back(line);	
		}
	} else if(action == "repotype") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_repotype = line;
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_repoaddr = line;
			action = "parse";
		}
	} else {
		throw ParseException(_desc.getServer() + _desc.getPath(), "The program should not reach this place!");
	}
}
}
}

ModInfo::ModInfo(std::string path) {
	_edit = true;
	ModInfoDescription emptymid;
	_desc = emptymid;
	_localPath = path;
	std::ifstream modinfo(_localPath.c_str());
	if(!modinfo) {
		throw FileException(_localPath, "reading", "Could not open file!");
	}
	std::string action = "detect";
	while(!modinfo.eof()) {
	std::string line = "";
	char c;
	do {
	modinfo.get(c);
	if(c != '\n');
	line += c;
	} while(c != '\n');
	line = strip_endl(line);
	if(modinfo) {
		if(line[0] != NULL && line[0] != ' ' && line[0] != '\n' && line[0] != '\r') {
	if(action == "detect") {
		if(line[0] == '{') {
			std::string name = "";
		for(unsigned int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		_name = name;
		_desc.setName(name);
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
		if(tmpact == "description" || tmpact == "release" || tmpact == "deps" || tmpact == "repotype" || tmpact == "repoaddr") {
		action = tmpact;	
		} else {
			std::string msg = "";
			msg += "Found ";
			msg += tmpact;
			msg += " although description/release/deps/repotype/repoaddr was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		}
	} else {
		std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although {end} or action in [] was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
	}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_description = line;
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "deps") {
		if(line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string or [ was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else if(line[0] == '[') {
			if(line[1] == 'd' && line[2] == 'e' && line[3] == 'p' && line[4] == 's' && line[5] == 'e' && line[6] == 'n' && line[7] == 'd' && line[8] == ']') {
				if(_deps.empty()) {
				_deps.push_back("none");	
				}
			action = "parse";	
			} else {
			std::string msg = "";
			msg += "Found ";
			msg += line;
			msg += " although string or [depsend] was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
			}
		} else {
		_deps.push_back(line);	
		}
	} else if(action == "repotype") {
		if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_repotype = line;
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			std::string msg = "";
			msg += "Found ";
			msg += line[0];
			msg += " although string was expected.";
			throw ParseException(_desc.getServer() + _desc.getPath(), msg);
		} else {
			_repoaddr = line;
			action = "parse";
		}
	} else {
		throw ParseException(_desc.getServer() + _desc.getPath(), "The program should not reach this place!");
	}
}
	}
	}
}

void ModInfo::setPath(std::string path) {
	_edit = true;
	_localPath = path;
}

void ModInfo::write() {
	if(!_edit) {
		throw NonEditableException("Tried to write back remotely obtained modinfo file!");
	}
	std::ofstream modinfo(_localPath.c_str());
	if(!modinfo) {
		throw FileException(_localPath, "writing", "Could not open file!");
	}
	modinfo << "{" << _name << "}" << std::endl << "[description]" << std::endl << _description << std::endl << "[release]" << std::endl << _release << std::endl << "[deps]" << std::endl;
	if(_deps.size() == 0) {
		modinfo << "none" << std::endl;
	} else {
		for(unsigned int i = 0; i < _deps.size(); i++) {
			modinfo << _deps[i] << std::endl;
		}
	}
	modinfo << "[depsend]" << std::endl << "[repotype]" << std::endl << _repotype << std::endl << "[repoaddr]" << std::endl << _repoaddr << std::endl << "{end}" << std::endl;
	modinfo.close();
}

void ModInfo::releaseInc() {
	_release++;
}

ModInfo::~ModInfo() {
	ModInfoDescription emptymid;
	_desc = emptymid;
	_edit = false;
	_localPath = "";
}

void ModInfo::clear() {
	_name = "";
	_description = "";
	_release = 0;
	_depsIterator = -1;
	_depsAtEnd = true;
	_repotype = "";
	_repoaddr = "";
	_deps.clear();
	ModInfoDescription emptymid;
	_desc = emptymid;
	_edit = false;
	_localPath = "";
}
