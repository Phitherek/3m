#include "../RepositoryInfo.h"
#include "../3mExceptions.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
	string home = getenv("HOME");
	string confpath = home + "/.3m/config";
	mmm::ConfigFile conf;
	try {
		mmm::ConfigFile tmpconf(confpath);
		conf = tmpconf;
	} catch(mmm::FileException &exc) {
		cerr << "FileException occured: " << exc.what() << endl;
		return EXIT_FAILURE;
	} catch(mmm::ParseException &exc) {
		cerr << "ParseException occured: " << exc.what() << endl;
		return EXIT_FAILURE;
	}
	mmm::RepositoryInfo ri;
	bool newri = true;
	try {
		mmm::RepositoryInfo tmpri(conf);
		newri = false;
		ri = tmpri;
	} catch(mmm::FileException &exc) {
		cerr << "FileException occured: " << exc.what() << endl;
		cout << "Exception occurred, creating empty repository info..." << endl;
		
	} catch(mmm::ParseException &exc) {
		cerr << "ParseException occured: " << exc.what() << endl;
		cout << "Exception occurred, creating empty repository info..." << endl;
	}
	if(newri) {
		ri.setConfigFile(conf);
		char action;
		do {
		cout << "Choose action: (l)ist all mods, (f)ind by name and list, (a)dd a mod, (r)emove mod by name, (s)ave and exit, (q)uit without saving" << endl;
		cin >> action;
		if(action == 'l') {
			ri.resetModDescriptionIterator();
			while(!ri.modDescriptionsAtEnd()) {
				mmm::RepositoryModDescription rmd = ri.getNextModDescription();
				if(rmd.getName() != "" && rmd.getReleaseNr() != 0 && rmd.getPath() != "") {
					cout << "name: " << rmd.getName() << endl << "release: " << rmd.getReleaseNr() << endl << "path: " << rmd.getPath() << endl << endl;
				}
			}
		} else if(action == 'f') {
			std::string name;
			cout << "Name of the mod: ";
			cin >> name;
			mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(name);
				if(rmd.getName() != "" && rmd.getReleaseNr() != 0 && rmd.getPath() != "") {
					cout << "name: " << rmd.getName() << endl << "release: " << rmd.getReleaseNr() << endl << "path: " << rmd.getPath() << endl << endl;
				} else {
					cout << name << " not found!" << endl;
				}
		} else if(action == 'a') {
			mmm::RepositoryModDescription rmd;
			std::string name, path;
			int release;
			cout << "name: ";
			cin >> name;
			cout << "release: ";
			cin >> release;
			cout << "path: ";
			cin >> path;
			rmd.setName(name);
			rmd.setReleaseNr(release);
			rmd.setPath(path);
			ri.insertModDescription(rmd);
		} else if(action == 'r') {
			std::string name;
			cout << "Name of the mod: ";
			cin >> name;
			mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(name);
				if(rmd.getName() != "" && rmd.getReleaseNr() != 0 && rmd.getPath() != "") {
					ri.deleteModDescription(name);
				} else {
					cout << name << " not found!" << endl;
				}
		}
		} while(action != 's' && action != 'q');
		if(action == 's') {
			ri.write();
		}
	} else {
		char action;
		do {
		cout << "Choose action: (l)ist all mods, (f)ind by name and list, (a)dd a mod, (r)emove mod by name, (s)ave and exit, (q)uit without saving" << endl;
		cin >> action;
		if(action == 'l') {
			ri.resetModDescriptionIterator();
			while(!ri.modDescriptionsAtEnd()) {
				mmm::RepositoryModDescription rmd = ri.getNextModDescription();
				if(rmd.getName() != "" && rmd.getReleaseNr() != 0 && rmd.getPath() != "") {
					cout << "name: " << rmd.getName() << endl << "release: " << rmd.getReleaseNr() << endl << "path: " << rmd.getPath() << endl << endl;
				}
			}
		} else if(action == 'f') {
			std::string name;
			cout << "Name of the mod: ";
			cin >> name;
			mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(name);
				if(rmd.getName() != "" && rmd.getReleaseNr() != 0 && rmd.getPath() != "") {
					cout << "name: " << rmd.getName() << endl << "release: " << rmd.getReleaseNr() << endl << "path: " << rmd.getPath() << endl << endl;
				} else {
					cout << name << " not found!" << endl;
				}
		} else if(action == 'a') {
			mmm::RepositoryModDescription rmd;
			std::string name, path;
			int release;
			cout << "name: ";
			cin >> name;
			cout << "release: ";
			cin >> release;
			cout << "path: ";
			cin >> path;
			rmd.setName(name);
			rmd.setReleaseNr(release);
			rmd.setPath(path);
			ri.insertModDescription(rmd);
		} else if(action == 'r') {
			std::string name;
			cout << "Name of the mod: ";
			cin >> name;
			mmm::RepositoryModDescription rmd = ri.getModDescriptionByName(name);
				if(rmd.getName() != "" && rmd.getReleaseNr() != 0 && rmd.getPath() != "") {
					ri.deleteModDescription(name);
				} else {
					cout << name << " not found!" << endl;
				}
		}
		} while(action != 's' && action != 'q');
		if(action == 's') {
			ri.write();
		}
	}
	return EXIT_SUCCESS;
}
