#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct rmodlistdata {
string name;
string server;
string modinfo;
};

int parsermodlist(vector<rmodlistdata> *rmodlist, string mfn) {
ifstream modlist(mfn.c_str());
vector<rmodlistdata> tmpv = *rmodlist;
if(!modlist) {
	cerr << "Modlist parse error: Cannot open modlist file for reading." << endl;
}
string action = "detect";
rmodlistdata tmprmld;
while(!modlist.eof()) {
	string line;
	modlist >> line;
	if(modlist) {
	if(action == "detect") {
		if(line[0] == '{') {
		string name = "";
		for(int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		tmprmld.name = name;
		action = "parse";
		} else {
			cerr << "Modlist parse error: Found " << line[0] << " although { was expected." << endl;
			modlist.close();
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(tmprmld.name != "" && tmprmld.server != "" && tmprmld.modinfo != "") {
				tmpv.push_back(tmprmld);
			} else {
				cerr << "Modlist parse error: Data error." << endl;
				modlist.close();
				return 1;
			}
		action = "detect";
		} else {
			cerr << "Modlist parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			modlist.close();
			return 1;
		}
	} else if(line[0] == '[') {
		string tmpact = "";
		for(int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "server" || tmpact == "modinfo") {
		action = tmpact;	
		} else {
			cerr << "Modlist parse error: Found " << tmpact << " although server/modinfo was expected." << endl;
			modlist.close();
			return 1;
		}
	} else {
		cerr << "Modlist parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			modlist.close();
			return 1;
	}
	} else if(action == "server") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modlist parse error: Found " << line[0] << " although string was expected." << endl;
			modlist.close();
			return 1;
		} else {
			tmprmld.server = line;
			action = "parse";
		}
	} else if(action == "modinfo") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modlist parse error: Found " << line[0] << " although string was expected." << endl;
			modlist.close();
			return 1;
		} else {
			tmprmld.modinfo = line;
			action = "parse";
		}
	} else {
		cerr << "Modlist parse error: The program should not reach this place!" << endl;
		modlist.close();
		return 1;
	}
}
}
*rmodlist = tmpv;
return 0;
}

int main(int argc, char** argv) {
string path;
if(argc != 2) {
cout << "Enter path to your remote modlist to parse: " << endl;
cin >> path;
} else {
path = argv[1];	
}
vector<rmodlistdata> mods;
int pr;
pr = parsermodlist(&mods, path);
if(pr == 1) {
	cerr << "Parse error! Your modlist is in the wrong format!" << endl;
	return EXIT_FAILURE;	
}
cout << "Modlist parsed successfully! Read contents: " << endl;
for(int i = 0; i < mods.size(); i++) {
	cout << "mods[" << i << "] = {name: " << mods[i].name << ", server: " << mods[i].server << ", modinfo: " << mods[i].modinfo << "}" << endl;	
}
return EXIT_SUCCESS;	
}
