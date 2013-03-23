#include "ModListDescription.h"
using namespace mmm;

ModListDescription::ModListDescription() {
_name = "";
_server = "";
_path = "";
}

ModListDescription::ModListDescription(std::string name, std::string server, std::string path) {
_name = name;
_server = server;
_path = path;
}

std::string ModListDescription::getName() {
return _name;	
}

std::string ModListDescription::getServer() {
return _server;	
}

std::string ModListDescription::getPath() {
return _path;	
}

void ModListDescription::setName(std::string name) {
_name = name;	
}

void ModListDescription::setServer(std::string server) {
_server = server;	
}

void ModListDescription::setPath(std::string path) {
_path = path;	
}
