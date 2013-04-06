#include "../ModInfo.h"
#include "NetSocket++/NetSocketPP.h"
#include "../ModDescription.h"
#include "../ModInfoDescription.h"
#include "../3mExceptions.h"
#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
	char mode;
	cout << "Select mode: Download and parse (r)emote modinfo file, Open, parse and edit (l)ocal modinfo file, Create (n)ew modinfo file: ";
	cin >> mode;
	if(mode == 'r') {
		string server;
		string path;
		cout << "Enter server where modinfo is located: ";
		cin >> server;
		cout << "Enter path of the remote modinfo: ";
		cin >> path;
		mmm::ModInfoDescription midesc("", server, path);
		try {
		mmm::ModInfo mi(midesc);
		cout << "Got modinfo: " << mi.getName() << "!" << endl << "description: " << mi.getDescription() << endl << "release: " << mi.getReleaseNr() << endl << "dependencies:" << endl;
		mi.resetDependencyIterator();
		while(!mi.dependenciesEnd()) {
			std::string dep = mi.getNextDependency();
			if(dep != "") {
				cout << dep << endl;
			}
		}
		cout << "repository type: " << mi.getRepositoryType() << endl << "repository address: " << mi.getRepositoryAddress() << endl;
		cout << "All OK!" << endl;
		} catch (NetSocketPP::NetworkException &exc) {
			cerr << "NetSocket++ throwed NetworkException: " << exc.what() << "! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch (NetSocketPP::SocketException &exc) {
			cerr << "NetSocket++ throwed SocketException: " << exc.what() << "! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch (mmm::ParseException &exc) {
			cerr << "ParseException occured: " << exc.what() << ". That means your remote modinfo file is not formatted properly! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch (mmm::BadResponseException &exc) {
			cerr << "BadResponseException occured: " << exc.what() << ". That usually means you have given wrong modinfo server or path. Exiting..." << endl;
			return EXIT_FAILURE;
		}
	} else if(mode == 'l') {
		std::string path;
		cout << "Path to local modinfo file: ";
		cin >> path;
		try {
			mmm::ModInfo mi(path);
			char action;
			do {
			cout << "Select action: (l)ist modinfo, (i)ncrease modinfo release, (e)dit all modinfo fields, (s)ave and exit, (q)uit without saving: ";
			cin >> action;
			if(action == 'l') {
				cout << "name: " << mi.getName() << endl << "description: " << mi.getDescription() << endl << "release: " << mi.getReleaseNr() << endl << "dependencies:" << endl;
		mi.resetDependencyIterator();
		while(!mi.dependenciesEnd()) {
			std::string dep = mi.getNextDependency();
			if(dep != "") {
				cout << dep << endl;
			}
		}
		cout << "repository type: " << mi.getRepositoryType() << endl << "repository address: " << mi.getRepositoryAddress() << endl;
			} else if(action == 'i') {
				mi.releaseInc();
			} else if(action == 'e') {
				std::string name, desc, dep, repotype, repoaddr;
				int release;
				cout << "name: ";
				cin >> name;
				if(name != "") {
					mi.setName(name);
				}
				cout << "description: ";
				cin >> desc;
				if(desc != "") {
					mi.setDescription(desc);
				}
				cout << "release: ";
				cin >> release;
				if(release > 0) {
					mi.setReleaseNr(release);
				}
				cout << "dependencies ('end' to finish): ";
				do {
					cin >> dep;
					if(dep != "" && dep != "end") {
						mi.insertDependency(dep);
					}
				} while(dep != "end");
				cout << "repository type: ";
				cin >> repotype;
				if(repotype != "") {
					mi.setRepositoryType(repotype);
				}
				cout << "repository address: ";
				cin >> repoaddr;
				if(repoaddr != "") {
					mi.setRepositoryAddress(repoaddr);
				}
			} else if(action != 's' && action != 'q') {
				cout << "Unknown action!" << endl;
			}
			} while(action != 's' && action != 'q');
			if(action == 's') {
				mi.write();
			}
		cout << "All OK!" << endl;
		} catch(mmm::FileException &exc) {
			cerr << "FileException occured: " << exc.what() << "! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch(mmm::ParseException &exc) {
			cerr << "ParseException occured: " << exc.what() << ". That means your remote modinfo file is not formatted properly! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch(mmm::NonEditableException &exc) {
			cerr << "NonEditableException occured: " << exc.what() << "! Exiting..." << endl;
			return EXIT_FAILURE;
		}
	} else if(mode == 'n') {
		std::string path;
		cout << "Path to new modinfo file: ";
		cin >> path;
		try {
			mmm::ModInfo mi;
			mi.setPath(path);
			char action;
			do {
			cout << "Select action: (l)ist modinfo, (i)ncrease modinfo release, (e)dit all modinfo fields, (s)ave and exit, (q)uit without saving: ";
			cin >> action;
			if(action == 'l') {
				cout << "name: " << mi.getName() << endl << "description: " << mi.getDescription() << endl << "release: " << mi.getReleaseNr() << endl << "dependencies:" << endl;
		mi.resetDependencyIterator();
		while(!mi.dependenciesEnd()) {
			std::string dep = mi.getNextDependency();
			if(dep != "") {
				cout << dep << endl;
			}
		}
		cout << "repository type: " << mi.getRepositoryType() << endl << "repository address: " << mi.getRepositoryAddress() << endl;
			} else if(action == 'i') {
				mi.releaseInc();
			} else if(action == 'e') {
				std::string name, desc, dep, repotype, repoaddr;
				int release;
				cout << "name: ";
				cin >> name;
				if(name != "") {
					mi.setName(name);
				}
				cout << "description: ";
				cin >> desc;
				if(desc != "") {
					mi.setDescription(desc);
				}
				cout << "release: ";
				cin >> release;
				if(release > 0) {
					mi.setReleaseNr(release);
				}
				cout << "dependencies ('end' to finish): ";
				do {
					cin >> dep;
					if(dep != "" && dep != "end") {
						mi.insertDependency(dep);
					}
				} while(dep != "end");
				cout << "repository type: ";
				cin >> repotype;
				if(repotype != "") {
					mi.setRepositoryType(repotype);
				}
				cout << "repository address: ";
				cin >> repoaddr;
				if(repoaddr != "") {
					mi.setRepositoryAddress(repoaddr);
				}
			} else if(action != 's' && action != 'q') {
				cout << "Unknown action!" << endl;
			}
			} while(action != 's' && action != 'q');
			if(action == 's') {
				mi.write();
			}
		cout << "All OK!" << endl;
		} catch(mmm::FileException &exc) {
			cerr << "FileException occured: " << exc.what() << "! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch(mmm::ParseException &exc) {
			cerr << "ParseException occured: " << exc.what() << ". That means your remote modinfo file is not formatted properly! Exiting..." << endl;
			return EXIT_FAILURE;
		} catch(mmm::NonEditableException &exc) {
			cerr << "NonEditableException occured: " << exc.what() << "! Exiting..." << endl;
			return EXIT_FAILURE;
		}
	} else {
		cerr << "Unknown mode: " << mode << "! Exiting...";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
