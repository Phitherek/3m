#include <iostream>
#include <cstdlib>
#include "../LocalModList.h"
#include "../3mExceptions.h"
using namespace std;

int main() {
string home = getenv("HOME");
string conffn = home + "/.3m/config";
try {
mmm::ConfigFile conf(conffn);
mmm::LocalModList lml(conf);
char action;
do {
	cout << "Select action: (l)ist modlist, (f)ind and list mod by name, (s)ave and exit, (q)uit without saving: ";
	cin >> action;
	if(action == 'l') {
		lml.resetModDescriptionIterator();
		while(!lml.modDescriptionsAtEnd()) {
			mmm::LocalModDescription temp;
			temp = lml.getNextModDescription();
			if(temp.getName() != "" && temp.getRemoteModlistName() != "" && temp.getDescription() != "" && !temp.dependenciesEmpty() && temp.getRepositoryType() != "" && temp.getRepositoryAddress() != "") {
				cout << "name: " << temp.getName() << endl << "remote modlist name: " << temp.getRemoteModlistName() << endl << "description: " << temp.getDescription() << endl << "release: " << temp.getReleaseNr() << endl << "dependencies: " << endl;
				temp.resetDependencyIterator();
				while(!temp.dependenciesEnd()) {
					string dep = temp.getNextDependency();
					if(dep != "") {
						cout << dep << endl;
					}
				}
				cout << "repository type: " << temp.getRepositoryType() << endl << "repository address: " << temp.getRepositoryAddress() << endl;
			}
		}
		cout << endl;
	} else if(action == 'f') {
		cout << "Enter name: ";
		string name;
		cin >> name;
		mmm::LocalModDescription temp;
			temp = lml.getModDescriptionByName(name);
		if(temp.getName() != "" && temp.getRemoteModlistName() != "" && temp.getDescription() != "" && !temp.dependenciesEmpty() && temp.getRepositoryType() != "" && temp.getRepositoryAddress() != "") {
			cout << "name: " << temp.getName() << endl << "remote modlist name: " << temp.getRemoteModlistName() << endl << "description: " << temp.getDescription() << endl << "release: " << temp.getReleaseNr() << endl << "dependencies: " << endl;
			temp.resetDependencyIterator();
			while(!temp.dependenciesEnd()) {
				string dep = temp.getNextDependency();
				if(dep != "") {
					cout << dep << endl;
				}
			}
			cout << "repository type: " << temp.getRepositoryType() << endl << "repository address: " << temp.getRepositoryAddress() << endl;
		} else {
			cout << name << " not found!" << endl;
		}
	}
} while(action != 'q' && action != 's');
if(action == 's') {
	lml.write();
}
} catch(mmm::FileException &exc) {
	cerr << "FileException occured: " << exc.what() << endl;
	return EXIT_FAILURE;
} catch(mmm::ParseException &exc) {
	cerr << "ParseException occured: " << exc.what() << endl;
	return EXIT_FAILURE;
}
return EXIT_SUCCESS;
}
