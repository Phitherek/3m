#include "ModInfoDescription.h"
using namespace mmm;

ModInfoDescription::ModInfoDescription() {
_name = "";
_server = "";
_path = "";
}

ModInfoDescription::ModInfoDescription(std::string name, std::string server, std::string path) {
_name = name;
_server = server;
_path = path;
}

std::string ModInfoDescription::getName() {
return _name;	
}

std::string ModInfoDescription::getServer() {
return _server;	
}

std::string ModInfoDescription::getPath() {
return _path;	
}

void ModInfoDescription::setName(std::string name) {
_name = name;	
}

void ModInfoDescription::setServer(std::string server) {
_server = server;	
}

void ModInfoDescription::setPath(std::string path) {
_path = path;	
}

void ModInfoDescription::clear() {
_name = "";
_server = "";
_path = "";
}
