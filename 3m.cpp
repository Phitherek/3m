#include <iostream>
#include <cstdlib>
#include <sys/socket.h> // For sockets
#include <sys/types.h> // For getaddrinfo
#include <arpa/inet.h> // For inet addr conversion
#include <netdb.h> // For getaddrinfo
#include <cstring> // For memset
#include <cerrno>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

int socdesc; // Socket descriptor
string homedir, errmsg, req; // Home directory, error message, request

struct modlistdata {
string name;
string server;
string path;
};

struct rmodlistdata {
string name;
string server;
string modinfo;
};

struct rmodinfo {
string name;
string description;
int release;
vector<string> deps;
string repotype;
string repoaddr;
};

void *get_in_addr(sockaddr *sa) { // IP Address obtaining by protocol
if(sa->sa_family == AF_INET) {
return &(((sockaddr_in*)sa) -> sin_addr);
}
return &(((sockaddr_in6*)sa) -> sin6_addr);
}

string strip_endl(string s) { // Newline stripping from string
	int len = s.length();
	for(int i = 0; i < len; i++) {
		if(s[i] == '\n') {
			s.erase(i,1);
			len = s.length();
			i--;
		}
	}
	return s;

}
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
		if(tmpact == "description" || tmpact == "release" || tmpact == "deps" || tmpact == "repotype" || tmpact == "repoaddr") {
		action = tmpact;	
		} else {
			cerr << "Modinfo parse error: Found " << tmpact << " although description/release/deps/repotype/repoaddr was expected." << endl;
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
	} else if(action == "deps") {
		if(line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string or [ was expected." << endl;
			modinfo.close();
			return 1;
		} else if(line[0] == '[') {
			if(line[1] == 'd' && line[2] == 'e' && line[3] == 'p' && line[4] == 's' && line[5] == 'e' && line[6] == 'n' && line[7] == 'd' && line[8] == ']') {
			action = "parse";	
			} else {
			cerr << "Modinfo parse error: Found " << line << " although string or [depsend] was expected." << endl;	
			}
		} else {
		tmp.deps.push_back(line);	
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

int main(int argc, char **argv) {
homedir = getenv("HOME");	
string config;
stringstream sconfig;
sconfig << homedir << "/.3m/config";
config = sconfig.str();
int ps;
string localrepo, localml, localri;
if(argc < 2) {
cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
cout << "Usage: " << argv[0] << "[-S/I/R/Q/h/v] [options] modname1 modname2 ..." << endl;
} else {
	if(argv[1] == "-h") {
	cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
cout << "Usage: " << argv[0] << "[-S/I/R/Q/h/v] [options] modname1 modname2 ..." << endl;
cout << endl;
cout << "-S - Sync: gather information from remote modinfo files and update local modlist" << endl;
cout << "-I - Install: download and install mod(s)" << endl;
cout << "-R - Remove: remove installed mod(s)" << endl;
cout << "-Q - Query: Query local modlist for a modname(s)" << endl;
cout << "-h - Help: This message" << endl;
cout << "-v - Version: Print version" << endl;
	} else if(argv[1] == "-v") {
		cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
	} else {
ps = parseconfig(&localrepo, &localml, &localri, config);
if(ps == 1) {
cerr << "Parse error! Exiting..." << endl;
return EXIT_FAILURE;
}
cout << "Config parsed successfully!" << endl;
//---DEBUG START---
cout << "DEBUG: localrepo: " << localrepo << endl;
cout << "DEBUG: localml: " << localml << endl;
cout << "DEBUG: localri: " << localri << endl;
//---DEBUG END---
return EXIT_SUCCESS;
}
}
}
