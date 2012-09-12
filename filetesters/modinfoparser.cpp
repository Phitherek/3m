#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

struct rmodinfo {
string name;
string description;
int release;
string repotype;
string repoaddr;
};

int parsemodinfo(rmodinfo *mis, string mfn) {
ifstream modinfo(mfn.c_str());
rmodinfo tmp = *mis;
if(!modinfo) {
	cerr << "Modinfo parse error: Cannot open modinfo file for reading." << endl;
}
string action = "detect";
while(!modinfo.eof()) {
	string line = "";
	char ch;
	modinfo.get(ch);
	while(ch != '\n') { 
	line += ch;
	modinfo.get(ch);	
	}
	if(modinfo) {
	if(action == "detect") {
		if(line[0] == '{') {
		string name = "";
		for(int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		tmp.name = name;
		action = "parse";
		} else {
			cerr << "Modinfo parse error: Found " << line[0] << " although { was expected." << endl;
			modinfo.close();
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
		action = "detect";
		} else {
			cerr << "Modinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			modinfo.close();
			return 1;
		}
	} else if(line[0] == '[') {
		string tmpact = "";
		for(int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "description" || tmpact == "release" || tmpact == "repotype" || tmpact == "repoaddr") {
		action = tmpact;	
		} else {
			cerr << "Modinfo parse error: Found " << tmpact << " although description/release/repotype/repoaddr was expected." << endl;
			modinfo.close();
			return 1;
		}
	} else {
		cerr << "Modinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			modinfo.close();
			return 1;
	}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo.close();
			return 1;
		} else {
			tmp.description = line;
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo.close();
			return 1;
		} else {
			tmp.release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "repotype") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo.close();
			return 1;
		} else {
			tmp.repotype = line;
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo.close();
			return 1;
		} else {
			tmp.repoaddr = line;
			action = "parse";
		}
	} else {
		cerr << "Modinfo parse error: The program should not reach this place!" << endl;
		modinfo.close();
		return 1;
	}
}
}
*mis = tmp;
return 0;
}

int main(int argc, char** argv) {
string path;
if(argc != 2) {
cout << "Enter path to your modinfo to parse: " << endl;
cin >> path;
} else {
path = argv[1];	
}
rmodinfo rmi;
int pr;
pr = parsemodinfo(&rmi, path);
if(pr == 1) {
	cerr << "Parse error! Your modinfo is in the wrong format!" << endl;
	return EXIT_FAILURE;	
}
cout << "Modinfo parsed successfully! This is how your mod will be displayed like: " << endl;
cout << "modlist/" << rmi.name << " (release: " << rmi.release << ")" << endl << rmi.description << endl;
cout << "and this is what will be also read by 3m:" << endl;
cout << "repotype: " << rmi.repotype << ", repoaddr: " << rmi.repoaddr << endl;
return EXIT_SUCCESS;	
}
