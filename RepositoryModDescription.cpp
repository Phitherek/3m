#include "RepositoryModDescription.h"
using namespace mmm;

RepositoryModDescription::RepositoryModDescription() {
	_path = "";
}

RepositoryModDescription::~RepositoryModDescription() {
	_path = "";
}

std::string RepositoryModDescription::getPath() {
	return _path;
}

void RepositoryModDescription::setPath(std::string path) {
	_path = path;
}

void RepositoryModDescription::clear() {
	_name = "";
	_release = 0;
	_path = "";
}
