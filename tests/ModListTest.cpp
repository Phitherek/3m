#include "../ModList.h"
#include "../ModInfo.h"
#include "NetSocket++/NetSocketPP.h"
#include "../ModListDescription.h"
#include "../ModInfoDescription.h"
#include "../ModDescription.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
char mode;
cout << "Select mode: Download, parse and list (r)emote modlist file, Open and parse (l)ocal modlist file, Create (n)ew local modlist file: ";
cin >> mode;
if(mode == 'r') {
	string server;
	string path;
	cout << "Enter server of remote modlist: ";
	cin >> server;
	cout << "Enter path to the remote modlist on the server: ";
	cin >> path;
	try {
	mmm::ModListDescription mld("test", server, path);
	mmm::ModList ml(mld);
	mld = ml.getModListDescription();
	cout << "Got modlist: " << mld.getName() << "!" << endl << "server: " << mld.getServer() << endl << "path: " << mld.getPath() << endl << "Content: " << endl;
	ml.resetModInfoIterator();
	while(!ml.modinfosEnd()) {
		mmm::ModInfo mi = ml.getNextModInfo();
		if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
			cout << "Modinfo: " << endl << "name: " << mi.getModInfoDescription().getName() << endl << "server: " << mi.getModInfoDescription().getServer() << endl << "path: " << mi.getModInfoDescription().getPath() << endl;
			if(mi.getName() != "" && mi.getDescription() != "" && mi.getReleaseNr() > 0 && mi.getRepositoryType() != "" && mi.getRepositoryAddress() != "") {
				cout << "Mod description: " << endl << "name: " << mi.getName() << endl << "description: " << mi.getDescription() << endl << "release: " << mi.getReleaseNr() << endl << "dependencies:" << endl;
				mi.resetDependencyIterator();
				while(!mi.dependenciesEnd()) {
					string dep = mi.getNextDependency();
					if(dep != "") {
						cout << dep << endl;
					}
				}
				cout << "repository type: " << mi.getRepositoryType() << endl << "repository address: " << mi.getRepositoryAddress() << endl;
			} else {
				cout << "No mod information!" << endl;
			}
		}
		cout << endl;
	}
	} catch(NetSocketPP::NetworkException &exc) {
		cerr << "NetworkException occured in NetSocket++: " << exc.what() << "! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(NetSocketPP::SocketException &exc) {
		cerr << "SocketException occured in NetSocket++: " << exc.what() << "! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(mmm::ParseException &exc) {
		cerr << "ParseException occured: " << exc.what() << "! That probably means you have badly formatted modlist file! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(mmm::BadResponseException &exc) {
		cerr << "BadResponseException occured: " << exc.what() << "! That probably means you have given wrong path to the file on the server! Exiting..." << endl;
	}
	cout << "All OK!" << endl;
} else if(mode == 'l') {
	string path;
	cout << "Enter path: ";
	cin >> path;
	try {
		mmm::ModList ml(path);
		char action;
		do {
			cout << "Select action: (l)ist modlist, (f)ind by name and list, (a)dd modinfo description, (d)elete modinfo description, (s)ave and exit, (q)uit without saving: ";
			cin >> action;
			if(action == 'l') {
				mmm::ModListDescription mld = ml.getModListDescription();
	cout << "Modlist name: " << mld.getName() << endl << "server: " << mld.getServer() << endl << "path: " << mld.getPath() << endl << "Content: " << endl;
	ml.resetModInfoIterator();
	while(!ml.modinfosEnd()) {
		mmm::ModInfo mi = ml.getNextModInfo();
		if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
			cout << "Modinfo: " << endl << "name: " << mi.getModInfoDescription().getName() << endl << "server: " << mi.getModInfoDescription().getServer() << endl << "path: " << mi.getModInfoDescription().getPath() << endl;
		}
		}
			} else if(action == 'f') {
				string name;
				cout << "Modinfo name: ";
				cin >> name;
				mmm::ModInfo mi = ml.getModInfoByName(name);
				if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
					cout << "name: " << mi.getModInfoDescription().getName() << endl << "server: " << mi.getModInfoDescription().getServer() << endl << "path: " << mi.getModInfoDescription().getPath() << endl;
				} else {
					cout << name << " not found!" << endl;
				}
			} else if(action == 'a') {
				string name, server, path;
				cout << "name: ";
				cin >> name;
				cout << "server: ";
				cin >> server;
				cout << "path: ";
				cin >> path;
				if(name != "" && server != "" && path != "") {
				mmm::ModInfoDescription mid(name, server, path);
				ml.insertModInfoDescription(mid);
				} else {
					cerr << "Cannot insert empty field!" << endl;
				}
			} else if(action == 'd') {
				string name;
				cout << "Modinfo name: ";
				cin >> name;
				mmm::ModInfo mi = ml.getModInfoByName(name);
				if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
					ml.deleteModInfo(name);
				} else {
					cout << name << " not found!" << endl;
				}
			} else if(action != 's' && action != 'q') {
				cerr << "Unknown action!" << endl;
			}
		} while(action != 's' && action != 'q');
		if(action == 's') {
			ml.write();
		}
		cout << "All OK!" << endl;
	} catch(mmm::FileException &exc) {
		cerr << "FileException occured: " << exc.what() << "! That probably means you entered wrong path! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(mmm::ParseException &exc) {
		cerr << "ParseException occured: " << exc.what() << "! That probably means your modinfo file is badly formatted! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(mmm::NonEditableException &exc) {
		cerr << "NonEditableException occured: " << exc.what() << "! Exiting..." << endl;
	}
} else if(mode == 'n') {
	string path;
	cout << "Enter path: ";
	cin >> path;
	try {
		mmm::ModList ml;
		ml.setPath(path);
		char action;
		do {
			cout << "Select action: (l)ist modlist, (f)ind by name and list, (a)dd modinfo description, (d)elete modinfo description, (s)ave and exit, (q)uit without saving: ";
			cin >> action;
			if(action == 'l') {
				mmm::ModListDescription mld = ml.getModListDescription();
	cout << "Modlist name: " << mld.getName() << endl << "server: " << mld.getServer() << endl << "path: " << mld.getPath() << endl << "Content: " << endl;
	ml.resetModInfoIterator();
	while(!ml.modinfosEnd()) {
		mmm::ModInfo mi = ml.getNextModInfo();
		if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
			cout << "Modinfo: " << endl << "name: " << mi.getModInfoDescription().getName() << endl << "server: " << mi.getModInfoDescription().getServer() << endl << "path: " << mi.getModInfoDescription().getPath() << endl;
		}
		}
			} else if(action == 'f') {
				string name;
				cout << "Modinfo name: ";
				cin >> name;
				mmm::ModInfo mi = ml.getModInfoByName(name);
				if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
					cout << "name: " << mi.getModInfoDescription().getName() << endl << "server: " << mi.getModInfoDescription().getServer() << endl << "path: " << mi.getModInfoDescription().getPath() << endl;
				} else {
					cout << name << " not found!" << endl;
				}
			} else if(action == 'a') {
				string name, server, path;
				cout << "name: ";
				cin >> name;
				cout << "server: ";
				cin >> server;
				cout << "path: ";
				cin >> path;
				if(name != "" && server != "" && path != "") {
				mmm::ModInfoDescription mid(name, server, path);
				ml.insertModInfoDescription(mid);
				} else {
					cerr << "Cannot insert empty field!" << endl;
				}
			} else if(action == 'd') {
				string name;
				cout << "Modinfo name: ";
				cin >> name;
				mmm::ModInfo mi = ml.getModInfoByName(name);
				if(mi.getModInfoDescription().getName() != "" && mi.getModInfoDescription().getServer() != "" && mi.getModInfoDescription().getPath() != "") {
					ml.deleteModInfo(name);
				} else {
					cout << name << "not found!" << endl;
				}
			} else if(action != 's' && action != 'q') {
				cerr << "Unknown action!" << endl;
			}
		} while(action != 's' && action != 'q');
		if(action == 's') {
			ml.write();
		}
		cout << "All OK!" << endl;
	} catch(mmm::FileException &exc) {
		cerr << "FileException occured: " << exc.what() << "! That probably means you entered wrong path! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(mmm::ParseException &exc) {
		cerr << "ParseException occured: " << exc.what() << "! That probably means your modinfo file is badly formatted! Exiting..." << endl;
		return EXIT_FAILURE;
	} catch(mmm::NonEditableException &exc) {
		cerr << "NonEditableException occured: " << exc.what() << "! Exiting..." << endl;
	}
} else {
	cerr << "Unknown mode: " << mode << "! Exiting..." << endl;
	return EXIT_FAILURE;
}
return EXIT_SUCCESS;	
}
