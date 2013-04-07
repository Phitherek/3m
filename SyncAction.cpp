#include "SyncAction.h"
#include <iostream>
#include <cstdlib>
using namespace mmm;

SyncAction::SyncAction(): Action() {}

SyncAction::~SyncAction() {}

void SyncAction::run() {
	try {
		std::cout << "Starting Sync" << std::endl;
		std::string home;
		home = getenv("HOME");
		std::string confpath = home + "/.3m/config";
		mmm::ConfigFile config(confpath);
		std::cout << "Config read successfully!" << std::endl;
		mmm::ModListList rmodlists;
		std::cout << "Remote modlist list read successfully!" << std::endl;
		mmm::LocalModList lmodlist;
		lmodlist.setConfigFile(config);
		if(!parameters.empty()) {
			for(unsigned int i = 0; i < parameters.size(); i++) {
			try {
				mmm::ModListDescription modlistdesc = rmodlists.getModlistDescriptionByName(parameters[i]);
				if(modlistdesc.getName() == "" || modlistdesc.getServer() == "" || modlistdesc.getPath() == "") {
					throw BadParameterException("sync", parameters[i]);
				}
				mmm::ModList modlist(modlistdesc);
				while(!modlist.modinfosEnd()) {
					mmm::ModDescription mod = modlist.getNextModInfo();
				if(mod.getName() != "" && mod.getDescription() != "" && mod.getReleaseNr() != 0 && mod.getRepositoryType() != "" && mod.getRepositoryAddress() != "") {
					mmm::LocalModDescription localmod = mod;
					localmod.setRemoteModlistName(modlistdesc.getName());
					lmodlist.addModDescription(localmod);
				} else {
					std::cout << "Empty mod description or end of list, skipping..." << std::endl;
				}
				}
			} catch(BadParameterException &exc) {
				std::cerr << "BadParameterException occured: " << exc.what() << "! Skipping..." << std::endl;
			} catch(NetSocketPP::NetworkException &exc) {
				std::cerr << "NetworkException occured in NetSocket++: " << exc.what() << "! Skipping..." << std::endl;
			} catch(NetSocketPP::SocketException &exc) {
				std::cerr << "SocketException occured in NetSocket++: " << exc.what() << "! Skipping..." << std::endl;
			} catch(ParseException &exc) {
				std::cerr << "ParseException occured: " << exc.what() << "! Skipping..." << std::endl;
			} catch(BadResponseException &exc) {
				std::cerr << "BadResponseException occured: " << exc.what() << "! Skipping..." << std::endl;
			}
			}
		} else {
			while(!rmodlists.modlistDescriptionsAtEnd()) {
			try {
			mmm::ModListDescription modlistdesc = rmodlists.getNextModlistDescription();
				mmm::ModListDescription emptymld;
				if(modlistdesc.getName() != "" && modlistdesc.getServer() != "" && modlistdesc.getPath() != "") {
				mmm::ModList modlist(modlistdesc);
				while(!modlist.modinfosEnd()) {
					mmm::ModDescription mod = modlist.getNextModInfo();
				if(mod.getName() != "" && mod.getDescription() != "" && mod.getReleaseNr() != 0 && mod.getRepositoryType() != "" && mod.getRepositoryAddress() != "") {
					mmm::LocalModDescription localmod = mod;
					localmod.setRemoteModlistName(modlistdesc.getName());
					lmodlist.addModDescription(localmod);
				} else {
					std::cout << "Empty mod description or end of list, skipping..." << std::endl;
				}
				}
			} else {
				std::cout << "Empty modlist description or end of list, skipping..." << std::endl;
			}
			} catch(NetSocketPP::NetworkException &exc) {
				std::cerr << "NetworkException occured in NetSocket++: " << exc.what() << "! Skipping..." << std::endl;
			} catch(NetSocketPP::SocketException &exc) {
				std::cerr << "SocketException occured in NetSocket++: " << exc.what() << "! Skipping..." << std::endl;
			} catch(ParseException &exc) {
				std::cerr << "ParseException occured: " << exc.what() << "! Skipping..." << std::endl;
			} catch(BadResponseException &exc) {
				std::cerr << "BadResponseException occured: " << exc.what() << "! Skipping..." << std::endl;
			}
		}
	}
	lmodlist.write();
	std::cout << "Sync finished successfully!" << std::endl;
	} catch(ParseException &exc) {
		std::cerr << "ParseException occured: " << exc.what() << "! Exiting..." << std::endl;
		return;
	} catch(FileException &exc) {
		std::cerr << "FileException occured: " << exc.what() << "! Exiting..." << std::endl;
		return;
	}
}
