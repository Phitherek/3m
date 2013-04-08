#include "QueryAction.h"
#include <iostream>
#include <cstdlib>
using namespace mmm;

QueryAction::QueryAction(): Action() {
	_local = false;
}

QueryAction::QueryAction(bool local): Action() {
	_local = local;
}

QueryAction::~QueryAction() {}

void QueryAction::run() {
	std::cout << "Starting Query" << std::endl;
	try {
		std::string home = getenv("HOME");
		std::string confpath = home + "/.3m/config";
		mmm::ConfigFile config(confpath);
		std::cout << "Config file loaded successfully!" << std::endl;
		mmm::LocalModList lml(config);
		std::cout << "Local modlist file loaded successfully!" << std::endl;
		bool repoinfook = true;
		mmm::RepositoryInfo ri;
		try {
		mmm::RepositoryInfo tmpri(config);
		std::cout << "Local repository info file loaded successfully!" << std::endl;
		ri = tmpri;
		} catch(mmm::FileException &exc) {
			std::cout << "FileException occured: " << exc.what() << ". That probably means no mod has been installed by 3m yet. Continuing..." << std::endl;
			repoinfook = false;
		} catch(mmm::ParseException &exc) {
			throw exc;
		}
	if(_local) {
		if(!repoinfook) {
			std::cerr << "Error: local repoinfo file not loaded properly and local search requested! Exiting..." << std::endl;
			return;
		} else {
		if(parameters.empty()) {
			ri.resetModDescriptionIterator();
			while(!ri.modDescriptionsAtEnd()) {
				mmm::RepositoryModDescription rmd = ri.getNextModDescription();
				if(rmd.getName() != "" && rmd.getReleaseNr() > 0) {
				std::cout << "LocalRepository/" << rmd.getName() << " (release: " << rmd.getReleaseNr() << ") [installed]" << std::endl;
			}
			}
		} else {
			for(unsigned int i = 0; i < parameters.size(); i++) {
				mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(parameters[i]);
					if(rmd.getName() != "" && rmd.getReleaseNr() > 0) {
				std::cout << "LocalRepository/" << rmd.getName() << " (release: " << rmd.getReleaseNr() << ") [installed]" << std::endl;
			} else {
				std::cout << parameters[i] << " not found!" << std::endl;
			}
		}
		}
		}
	} else {
		if(parameters.empty()) {
			while(!lml.modDescriptionsAtEnd()) {
				mmm::LocalModDescription lmd = lml.getNextModDescription();
				if(lmd.getName() != "" && lmd.getReleaseNr() > 0 && lmd.getDescription() != "" && lmd.getRemoteModlistName() != "") {
				std::cout << lmd.getRemoteModlistName() << "/" << lmd.getName() << " (release: " << lmd.getReleaseNr() << ")";
				if(repoinfook) {
				mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(lmd.getName());
				if(rmd.getName() != "") {
					std::cout << " [installed: " << rmd.getReleaseNr() << "]";
				}
				}
				std::cout << std::endl << lmd.getDescription() << std::endl;
				if(!lmd.dependenciesEmpty()) {
					lmd.resetDependencyIterator();
						std::string dep = lmd.getNextDependency();
						if(dep != "" && dep != "none") {
				std::cout << "Depends on: ";
				while(!lmd.dependenciesEnd()) {
					if(dep != "") {
					std::cout << dep << " ";
					dep = lmd.getNextDependency();
					}
				}
				std::cout << std::endl;
				}
				}
			}
			}
		} else {
			for(unsigned int i = 0; i < parameters.size(); i++) {
				mmm::LocalModDescription lmd = lml.getModDescriptionByName(parameters[i]);
					if(lmd.getName() != "" && lmd.getReleaseNr() > 0 && lmd.getDescription() != "" && lmd.getRemoteModlistName() != "") {
				std::cout << lmd.getRemoteModlistName() << "/" << lmd.getName() << " (release: " << lmd.getReleaseNr() << ")";
				if(repoinfook) {
				mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(lmd.getName());
				if(rmd.getName() != "") {
					std::cout << " [installed: " << rmd.getReleaseNr() << "]";
				}
				}
				std::cout << std::endl << lmd.getDescription() << std::endl;
				if(!lmd.dependenciesEmpty()) {
					lmd.resetDependencyIterator();
						std::string dep = lmd.getNextDependency();
						if(dep != "" && dep != "none") {
				std::cout << "Depends on: ";
				while(!lmd.dependenciesEnd()) {
					if(dep != "") {
					std::cout << dep << " ";
					dep = "";
					dep = lmd.getNextDependency();
					}
				}
				std::cout << std::endl;
				}
				}
			} else {
				std::cout << parameters[i] << " not found!" << std::endl;
			}
			}
		}
	}
	} catch(FileException &exc) {
		std::cerr << "FileException occured: " << exc.what() << "! Exiting..." << std::endl;
		return;
	} catch(ParseException &exc) {
		std::cerr << "ParseException occured: " << exc.what() << "! Exiting..." << std::endl;
		return;
	}
}
