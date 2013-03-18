#include "ConfigFile.h"
#include "3mExceptions.h"
#include <fstream>
using namespace mmm;

ConfigFile::ConfigFile(std::string path) {
std::ifstream conf(path.c_str());
if(!conf) {
conf.close();
throw FileException(path, "reading", "Could not open file!");
}
_path = path;
string action = "parse";
while(!conf.eof()) {
std::string line = "";
conf >> line;
if(conf) {
if(action == "parse") {
if(line[0] == '[') {
int i = 1;
string sa = "";
while(line[i] != ']') {
if(i >= line.length()-1 && line[i] != ']') {
std::string err = "";
err += "Found ";
err += line[i]; 
err += " although ] was expected.";
conf.close();
throw ParseException(_path, err);
}
sa += line[i];
i++;
}
if(sa == "localpath") {
action = "localpath";
} else if(sa == "modlist") {
action = "modlist";
} else if(sa == "repoinfo") {
action = "repoinfo";
} else if(sa == "end") {
action = "end";
} else {
conf.close();
std::string err = "";
err += "Found ";
err += sa;
err += " although localpath, modlist or repoinfo was expected.";
throw ParseException(_path, err);
}
} else {
conf.close();
std::string err;
err += "Found ";
err += line[0];
err += " although [ was expected.";
throw ParseException(_path, err);
}
} else if(action == "localpath") {
if(line[0] == '[') {
conf.close();
throw ParseException(_path, "Found [ although string and not option was expected.");
}
_localpath = line;
action = "parse";
} else if(action == "modlist") {
if(line[0] == '[') {
conf.close();
throw ParseException(_path, "Found [ although string and not option was expected.");
}
_modlist = line;
action = "parse";
} else if(action == "repoinfo") {
if(line[0] == '[') {
conf.close();
throw ParseException(_path, "Found [ although string and not option was expected.");
}
_repoinfo = line;
action = "parse";
} else if(action == "end") {
conf.close();	
}
}
}
}

std::string& ConfigFile::getLocalPath() {
	return _localpath;	
}

std::string& ConfigFile::getModList() {
	return _modlist;	
}

std::string& ConfigFile::getRepoInfo() {
	return _repoinfo;
}

void ConfigFile::write() {
	std::ofstream conf(_path.c_str());
	if(!conf) {
		conf.close();
		throw FileException(_path, "writing", "Could not open file!");	
	}
	conf << "[localpath]" << std::endl << _localpath << std::endl << "[modlist]" << std::endl << _modlist << std::endl << "[repoinfo]" << std::endl << _repoinfo << std::endl << "[end]" << std::endl;
	conf.close();
}
