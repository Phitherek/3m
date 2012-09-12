#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct modlistdata {
string name;
string server;
string path;
};

int parseconfig(string *localpath, string *modlist, string *repoinfo, string config) { // Parse config file and obtain variables modifiable by user
ifstream conf(config.c_str());
if(!conf) {
cerr << "Config parse error: Cannot open config file for reading: " << config << endl;
conf.close();
return 1;
}
string action = "parse";
while(!conf.eof()) {
string line = "";
conf >> line;
if(conf) {
if(action == "parse") {
if(line[0] == '[') {
int i = 1;
string sa = "";
while(line[i] != ']') {
if(i >= line.length()-1 && line[i] != ']') {
cerr << "Config parse error: Found " << line[i] << " although ] was expected." << endl;
conf.close();
return 1;
}
sa += line[i];
i++;
}
if(sa == "localpath") {
action = "localpath";
} else if(sa == "modlist") {
action = "modlist";
} else if(sa == "repoinfo") {
action = "repoinfo";
} else if(sa == "end") {
action = "end";
} else {
cerr << "Config parse error: Found " << sa << " although localpath, modlist or repoinfo was expected." << endl;
conf.close();
return 1;
}
} else {
cerr << "Config parse error: Found " << line[0] << " although [ was expected." << endl;
conf.close();
return 1;
}
} else if(action == "localpath") {
if(line[0] == '[') {
cerr << "Config parse error: Found [ although string and not option was expected." << endl;
conf.close();
return 1;
}
*localpath = line;
action = "parse";
} else if(action == "modlist") {
if(line[0] == '[') {
cerr << "Config parse error: Found [ although string and not option was expected." << endl;
conf.close();
return 1;
}
*modlist = line;
action = "parse";
} else if(action == "repoinfo") {
if(line[0] == '[') {
cerr << "Config parse error: Found [ although string and not option was expected." << endl;
conf.close();
return 1;
}
*repoinfo = line;
action = "parse";
} else if(action == "end") {
conf.close();
return 0;	
}
}
}
return 0;
}

int parsemodlists(vector<modlistdata> *pmodlists, string rmfn) {
ifstream rmfile(rmfn.c_str());
vector<modlistdata> tmpv = *pmodlists;
if(!rmfile) {
	cerr << "Remote modlist list parse error: Cannot open remote modlist file for reading" << endl;
}
string action = "detect";
modlistdata tmpmld;
while(!rmfile.eof()) {
	string line;
	rmfile >> line;
	if(rmfile) {
	if(action == "detect") {
		if(line[0] == '{') {
		string name = "";
		for(int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		tmpmld.name = name;
		action = "parse";
		} else {
			cerr << "Remote modlist list parse error: Found " << line[0] << " although { was expected." << endl;
			rmfile.close();
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(tmpmld.name != "" && tmpmld.server != "" && tmpmld.path != "") {
				tmpv.push_back(tmpmld);
			} else {
				cerr << "Remote modlist list parse error: Data error." << endl;
				rmfile.close();
				return 1;
			}
		action = "detect";
		} else {
			cerr << "Remote modlist list parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			rmfile.close();
			return 1;
		}
	} else if(line[0] == '[') {
		string tmpact = "";
		for(int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "server" || tmpact == "path") {
		action = tmpact;	
		} else {
			cerr << "Remote modlist list parse error: Found " << tmpact << " although server/path was expected." << endl;
			rmfile.close();
			return 1;
		}
	} else {
		cerr << "Remote modlist list parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			rmfile.close();
			return 1;
	}
	} else if(action == "server") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Remote modlist list parse error: Found " << line[0] << " although string was expected." << endl;
			rmfile.close();
			return 1;
		} else {
			tmpmld.server = line;
			action = "parse";
		}
	} else if(action == "path") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Remote modlist list parse error: Found " << line[0] << " although string was expected." << endl;
			rmfile.close();
			return 1;
		} else {
			tmpmld.path = line;
			action = "parse";
		}
	} else {
		cerr << "Remote modlist list parse error: The program should not reach this place!" << endl;
		rmfile.close();
		return 1;
	}
}
}
*pmodlists = tmpv;
return 0;
}

int main(int argc, char **argv) {
string homedir;
homedir = getenv("HOME");	
string config, rmfn;
stringstream sconfig, srmfn;
sconfig << homedir << "/.3m/config";
config = sconfig.str();
srmfn << homedir << "/.3m/remote_modlists";
rmfn = srmfn.str();
int ps;
string localrepo, localml, localri;
ps = parseconfig(&localrepo, &localml, &localri, config);
if(ps == 1) {
cerr << "Parse error! Your config file is not in the format for 3m!" << endl;
return EXIT_FAILURE;
}
cout << "Config parsed successfully!" << endl << "Your configuration: " << endl;
cout << "localrepo (Local mod repository): " << localrepo << endl;
cout << "localml (Local modlist): " << localml << endl;
cout << "localri (Local repoinfo): " << localri << endl;
int mlf;
vector<modlistdata> mld;
mlf = parsemodlists(&mld, rmfn);
if(mlf == 1) {
cerr << "Parse error! Your remote modlist list is not in the format for 3m!" << endl;
return EXIT_FAILURE;
}
cout << "Remote modlist list parsed successfully!" << endl << "Modlist data: " << endl;
for(int i = 0; i < mld.size(); i++) {
	cout << "mld[" << i << "] = {name: " << mld[i].name << ", server: " << mld[i].server << ", path: " << mld[i].path << "}" << endl;	
}
return EXIT_SUCCESS;
}
