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
