#include <iostream>
#include <cstdlib>
#include <string>
#include "../ConfigFile.h"
#include "../3mExceptions.h"
using namespace std;

int main() {
cout << "3m Tests: ConfigFile v. 0.1 (C) 2013 by Phitherek_" << endl;
cout << "Give me the settings path: ";
string path;
cin >> path;
try {
mmm::ConfigFile conf(path);
cout << "Config parsed successfully! Here are the results: " << endl << "localpath: " << conf.getLocalPath() << endl << "modlist: " << conf.getModList() << endl << "repoinfo: " << conf.getRepoInfo() << endl;
string localpath, modlist, repoinfo;
cout << "Enter new localpath: " << endl;
cin >> localpath;
cout << "modlist: " << endl;
cin >> modlist;
cout << "repoinfo: " << endl;
cin >> repoinfo;
conf.getLocalPath() = localpath;
conf.getModList() = modlist;
conf.getRepoInfo() = repoinfo;
cout << "Generating the config file..." << endl;
conf.write();
cout << "All OK! Thank you for testing!" << endl;
return EXIT_SUCCESS;
} catch(mmm::FileException& exc) {
cout << "File exception occured: " << exc.what() << endl;
return 1;
} catch(mmm::ParseException& exc) {
cout << "Parse exception occured: " << exc.what() << endl;
return 2;
}
}
