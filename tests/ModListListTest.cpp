#include <iostream>
#include <cstdlib>
#include "../ModListList.h"
#include "../3mExceptions.h"
using namespace std;

int main() {
mmm::ModListList mll;
char act;
do {
cout << "Choose action: (l)ist all modlists, (f)ind by name and list, (d)elete by name, (a)dd new modlist, (s)ave and quit, (q)uit without saving: ";
cin >> act;
if(act == 'l') {
mll.resetModlistDescriptionIterator();
while(!mll.modlistDescriptionsAtEnd()) {
	mmm::ModListDescription mld = mll.getNextModlistDescription();
	if(mld.getName() != "" && mld.getServer() != "" && mld.getPath() != "") {
	cout << "name: " << mld.getName() << endl << "server: " << mld.getServer() << endl << "path: " << mld.getPath() << endl << endl;
	}
}
} else if(act == 'f') {
	cout << "Enter name: ";
	string name;
	cin >> name;
	mmm::ModListDescription mld = mll.getModlistDescriptionByName(name);
	if(mld.getName() != "" && mld.getServer() != "" && mld.getPath() != "") {
		cout << "name: " << mld.getName() << endl << "server: " << mld.getServer() << endl << "path: " << mld.getPath() << endl;
	} else {
		cout << name << " not found!" << endl;
	}
} else if(act == 'd') {
	cout << "Enter name: ";
	string name;
	cin >> name;
	mll.deleteModlistDescription(name);
} else if(act == 'a') {
	mmm::ModListDescription mld;
	cout << "Enter name: ";
	string name;
	cin >> name;
	cout << "Enter server: ";
	string server;
	cin >> server;
	cout << "Enter path: ";
	string path;
	cin >> path;
	mld.setName(name);
	mld.setPath(path);
	mld.setServer(server);
	if(mld.getName() != "" && mld.getPath() != "" && mld.getServer() != "") {
	mll.addModlistDescription(mld);
	} else {
	cerr << "You must fill all fields!" << endl;	
	}
}
} while(act != 'q' && act != 's');
if(act == 's') {
try {	
mll.write();
} catch(mmm::FileException& exc) {
cerr << "File exception occured: " << exc.what() << endl;
return EXIT_FAILURE;
}
}
cout << "All done! Thank you for testing!" << endl;
return EXIT_SUCCESS;
}
