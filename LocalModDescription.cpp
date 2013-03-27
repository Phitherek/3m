#include "LocalModDescription.h"
using namespace mmm;

LocalModDescription::LocalModDescription() {
	_remoteModlist = "";
}

LocalModDescription::~LocalModDescription() {
	_remoteModlist = "";
}

std::string LocalModDescription::getRemoteModlistName() {
	return _remoteModlist;
}

void LocalModDescription::setRemoteModlistName(std::string remoteModlist) {
	_remoteModlist = remoteModlist;
}

void LocalModDescription::clear() {
	_name = "";
	_description = "";
	_release = 0;
	_depsIterator = -1;
	_depsAtEnd = true;
	_repotype = "";
	_repoaddr = "";
	_deps.clear();
	_remoteModlist = "";
}
