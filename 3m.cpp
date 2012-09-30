#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h> // For sockets
#include <sys/types.h> // For getaddrinfo
#include <sys/stat.h> // For mkdir etc.
#include <dirent.h>
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

string homedir; // Home directory

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

struct lmodlistdata {
string name;
string rmodlist;
string description;
int release;
vector<string> deps;
string repotype;
string repoaddr;
};

struct repoinfodata {
string name;
int release;
string path;
};

struct rmodinfo {
string name;
string description;
int release;
vector<string> deps;
string repotype;
string repoaddr;
};

struct modlist {
string name;
vector<rmodinfo> rmodinfos;
};

struct actlist {
string name;
string rmodlist;
int lmidx;
int installed;
};

modlistdata mldclear(modlistdata mld) {
mld.name = "";
mld.server = "";
mld.path = "";
return mld;
}

rmodlistdata rmldclear(rmodlistdata rmld) {
rmld.name = "";
rmld.server = "";
rmld.modinfo = "";
return rmld;
}

rmodinfo rmiclear(rmodinfo rmi) {
rmi.name = "";
rmi.description = "";
rmi.release = NULL;
rmi.deps.clear();
rmi.repotype = "";
rmi.repoaddr = "";
return rmi;
}

lmodlistdata lmldclear(lmodlistdata lmld) {
lmld.name = "";
lmld.rmodlist = "";
lmld.description = "";
lmld.release = NULL;
lmld.deps.clear();
lmld.repotype = "";
lmld.repoaddr = "";
return lmld;
}

repoinfodata ridclear(repoinfodata rid) {
rid.name = "";
rid.release = NULL;
rid.path = "";
return rid;
}

actlist alclear(actlist al) {
al.name = "";
al.rmodlist = "";
al.lmidx = -1;
al.installed = 0;
return al;
}

string strgetline(string *str, int* erased) {
string line = "";
int i;
for(i = 0; (*str)[i] != '\n'; i++) {
line += (*str)[i];
}
(*str).erase(0, i+1);
*erased = i;
return line;
}

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
vector<modlistdata> tmpv;
if(!rmfile) {
	cerr << "Remote modlist list parse error: Cannot open remote modlist file for reading" << endl;
}
string action = "detect";
modlistdata tmpmld;
tmpmld = mldclear(tmpmld);
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

int parselmodlist(vector<lmodlistdata> *lmodlist, string lmfn) {
	ifstream lmfile(lmfn.c_str());
	vector<lmodlistdata> tmpv;	
	if(!lmfile) {
		cerr << "Local modlist parse error: Could not open local modlist file for reading!" << endl;
		return 1;
	}
	string action = "detect";
lmodlistdata tmplmld;
tmplmld = lmldclear(tmplmld);
while(!lmfile.eof()) {
	string line = "";
	char c;
	do {
	lmfile.get(c);
	if(c != '\n');
	line += c;
	} while(c != '\n');
	if(lmfile) {
	if(action == "detect") {
		if(line[0] == '{') {
		string name = "";
		for(int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		tmplmld.name = name;
		action = "parse";
		} else {
			cerr << "Local modlist parse error: Found " << line[0] << " although { was expected." << endl;
			lmfile.close();
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(tmplmld.name != "" && tmplmld.rmodlist != "" && tmplmld.description != "" && !tmplmld.deps.empty() && tmplmld.repotype != "" && tmplmld.repoaddr != "") {
				tmpv.push_back(tmplmld);
				tmplmld = lmldclear(tmplmld);
				action = "detect";
			} else {
				cerr << "Local modlist parse error: Data error." << endl;
				lmfile.close();
				return 1;
			}
		action = "detect";
		} else {
			cerr << "Local modlist parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			lmfile.close();
			return 1;
		}
	} else if(line[0] == '[') {
		string tmpact = "";
		for(int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "rmodlist" || tmpact == "description" || tmpact == "release" || tmpact == "deps" || tmpact == "repotype" || tmpact == "repoaddr") {
		action = tmpact;	
		} else {
			cerr << "Local modlist parse error: Found " << tmpact << " although rmodlist/description/release/deps/repotype/repoaddr was expected." << endl;
			lmfile.close();
			return 1;
		}
	} else {
		cerr << "Local modlist parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			lmfile.close();
			return 1;
	}
	} else if(action == "rmodlist") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Local modlist parse error: Found " << line[0] << " although string was expected." << endl;
			lmfile.close();
			return 1;
		} else {
			tmplmld.rmodlist = line;
			action = "parse";
		}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Local modlist parse error: Found " << line[0] << " although string was expected." << endl;
			lmfile.close();
			return 1;
		} else {
			tmplmld.description = line;
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Local modlist parse error: Found " << line[0] << " although string was expected." << endl;
			lmfile.close();
			return 1;
		} else {
			tmplmld.release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "deps") {
		if(line[0] == '{') {
			cerr << "Local modlist parse error: Found " << line[0] << " although string or [ was expected." << endl;
			lmfile.close();
			return 1;
		} else if(line[0] == '[') {
			if(line[1] == 'd' && line[2] == 'e' && line[3] == 'p' && line[4] == 's' && line[5] == 'e' && line[6] == 'n' && line[7] == 'd' && line[8] == ']') {
				if(tmplmld.deps.empty()) {
				tmplmld.deps.push_back("none");	
				}
			action = "parse";	
			} else {
			cerr << "Local modlist parse error: Found " << line << " although string or [depsend] was expected." << endl;
			return 1;
			}
		} else {
		tmplmld.deps.push_back(line);	
		}
	} else if(action == "repotype") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Local modlist parse error: Found " << line[0] << " although string was expected." << endl;
			lmfile.close();
			return 1;
		} else {
			tmplmld.repotype = line;
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			cerr << "Local modlist parse error: Found " << line[0] << " although string was expected." << endl;
			lmfile.close();
			return 1;
		} else {
			tmplmld.repoaddr = line;
			action = "parse";
		}
	} else {
		cerr << "Local modlist parse error: The program should not reach this place!" << endl;
		lmfile.close();
		return 1;
	}
}
}
*lmodlist = tmpv;
lmfile.close();
return 0;
}

int parserepoinfo(vector<repoinfodata> *repoinfo, string rifn) {
ifstream rifile(rifn.c_str());
vector<repoinfodata> tmpv = *repoinfo;
if(!rifile) {
	cerr << "Local repoinfo parse error: Cannot open local repoinfo file for reading" << endl;
	return 1;
}
string action = "detect";
repoinfodata tmprid;
tmprid = ridclear(tmprid);
while(!rifile.eof()) {
	string line;
	rifile >> line;
	if(rifile) {
	if(action == "detect") {
		if(line[0] == '{') {
		string name = "";
		for(int i = 1; line[i] != '}' && i < line.length(); i++) {
			name += line[i];
		}
		tmprid.name = name;
		action = "parse";
		} else {
			cerr << "Local repoinfo parse error: Found " << line[0] << " although { was expected." << endl;
			rifile.close();
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(tmprid.name != "" && tmprid.path != "") {
				tmpv.push_back(tmprid);
			} else {
				cerr << "Local repoinfo parse error: Data error." << endl;
				rifile.close();
				return 1;
			}
		action = "detect";
		} else {
			cerr << "Local repoinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			rifile.close();
			return 1;
		}
	} else if(line[0] == '[') {
		string tmpact = "";
		for(int i = 1; line[i] != ']' && i < line.length(); i++) {
		tmpact += line[i];
		}
		if(tmpact == "release" || tmpact == "path") {
		action = tmpact;	
		} else {
			cerr << "Local repoinfo parse error: Found " << tmpact << " although release/path was expected." << endl;
			rifile.close();
			return 1;
		}
	} else {
		cerr << "Local repoinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			rifile.close();
			return 1;
	}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Local repoinfo parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmprid.release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "path") {
	if(line[0] == '[' || line[0] == '{') {
			cerr << "Local repoinfo parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmprid.path = line;
			action = "parse";
		}
	} else {
		cerr << "Local repoinfo parse error: The program should not reach this place!" << endl;
		return 1;
	}
}
}
*repoinfo = tmpv;
return 0;
}

int parsermodlist(vector<rmodlistdata> *rmodlist, string modlist, int size) {
vector<rmodlistdata> tmpv = *rmodlist;
string action = "detect";
rmodlistdata tmprmld;
tmprmld = rmldclear(tmprmld);
for(int i = 0; i < size; i++) {
	string line = "";
	int erased;
	line = strgetline(&modlist, &erased);
	size -= erased;
	if(line[0] != NULL && line[0] != ' ' && line[0] != '\n' && line[0] != '\r') {
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
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
			if(tmprmld.name != "" && tmprmld.server != "" && tmprmld.modinfo != "") {
				tmpv.push_back(tmprmld);
			} else {
				cerr << "Modlist parse error: Data error." << endl;
				return 1;
			}
		action = "detect";
		} else {
			cerr << "Modlist parse error: Found " << line << " although {end} or action in [] was expected." << endl;
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
			return 1;
		}
	} else {
		cerr << "Modlist parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			return 1;
	}
	} else if(action == "server") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modlist parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmprmld.server = line;
			action = "parse";
		}
	} else if(action == "modinfo") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modlist parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmprmld.modinfo = line;
			action = "parse";
		}
	} else {
		cerr << "Modlist parse error: The program should not reach this place!" << endl;
		return 1;
	}
}
}
*rmodlist = tmpv;
return 0;
}

int parsemodinfo(rmodinfo *mis, string modinfo, int size) {
rmodinfo tmp = *mis;
tmp = rmiclear(tmp);
string action = "detect";
for(int i = 0; i < size; i++) {
	string line = "";
	int erased;
	line = strgetline(&modinfo, &erased);
	size -= erased;
	if(line[0] != NULL && line[0] != ' ' && line[0] != '\n' && line[0] != '\r') {
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
			modinfo = "";
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
		action = "detect";
		} else {
			cerr << "Modinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			modinfo = "";
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
			modinfo = "";
			return 1;
		}
	} else {
		cerr << "Modinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
		modinfo = "";
			return 1;
	}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo = "";
			return 1;
		} else {
			tmp.description = line;
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo = "";
			return 1;
		} else {
			tmp.release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "deps") {
		if(line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string or [ was expected." << endl;
			modinfo = "";
			return 1;
		} else if(line[0] == '[') {
			if(line[1] == 'd' && line[2] == 'e' && line[3] == 'p' && line[4] == 's' && line[5] == 'e' && line[6] == 'n' && line[7] == 'd' && line[8] == ']') {
				if(tmp.deps.empty()) {
				tmp.deps.push_back("none");	
				}
			action = "parse";	
			} else {
			cerr << "Modinfo parse error: Found " << line << " although string or [depsend] was expected." << endl;
			modinfo = "";
			return 1;
			}
		} else {
		tmp.deps.push_back(line);	
		}
	} else if(action == "repotype") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo = "";
			return 1;
		} else {
			tmp.repotype = line;
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			modinfo = "";
			return 1;
		} else {
			tmp.repoaddr = line;
			action = "parse";
		}
	} else {
		cerr << "Modinfo parse error: The program should not reach this place!" << endl;
		modinfo = "";
		return 1;
	}
}
}
*mis = tmp;
modinfo = "";
return 0;
}

int getmodinfo(rmodinfo *rmi ,rmodlistdata mld) {
int remote;
int yes=1;
int status;
	char caddr[INET6_ADDRSTRLEN];
	char rcvbuf[1000000];
		for(int i=0; i<1000000; i++) {
		rcvbuf[i] = NULL;	
		}
	addrinfo hints;
	addrinfo *servinfo, *p;
	cout << "getmodinfo: Trying to get modinfo for: " << mld.name << endl;
	cout << "getmodinfo: Connecting to: " << mld.server << endl;
	memset(&hints, 0, sizeof(hints)); // We do not want any trash here...
	hints.ai_family = AF_UNSPEC; // Make it IPv4 or IPv6 - which one fits.
	hints.ai_socktype = SOCK_STREAM; // HTTP works on TCP, right?
	if((status = getaddrinfo(mld.server.c_str(), "http", &hints, &servinfo)) != 0) { // Trying to get servinfo - server network address etc.
		cerr << "Failed to connect to the server: " << gai_strerror(status) << endl;
		return 1; // 1 means GAI failed
	}
	for(p = servinfo; p != NULL; p = p->ai_next) { // For every GAI result try to create a socket, set socket options and, finally, connect
		remote = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
		if(remote == -1) {
			cerr << "Failed to create socket: " << strerror(errno) << endl;
		}
		if((setsockopt(remote, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)) {
			cerr << "Failed to set socket options: " << strerror(errno) << endl;
		}
		status = connect(remote, servinfo->ai_addr, servinfo->ai_addrlen);
		if(status == -1) {
			cerr << "Could not connect to this address: " << strerror(errno) << endl;
			continue;
		}
		break;
	}
	if(p == NULL) {
		cerr << "Could not connect to the server: " << strerror(errno) << endl;
		return 2; // 2 means connecting failed
	}
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), caddr, sizeof(caddr));
	freeaddrinfo(servinfo);
	cout << "Connection to " << caddr << " has been successfully established!" << endl;
	cout << "Sending HTTP GET request..." << endl;
	string req = "";
	stringstream sreq;
	sreq << "GET " << mld.modinfo << " HTTP/1.0\n" << "Host: " << mld.server << endl << endl;
	req = sreq.str();
	status = send(remote, req.c_str(), req.length(), 0);
	if(status == -1) {
		cerr << "Could not send HTTP GET request: " << strerror(errno) << endl;
		close(remote);
		return 3; // 3 means send failed
	}
	cout << status << " bytes sent" << endl;
	cout << "Receiving modinfo..." << endl;
	status = recv(remote, &rcvbuf, 1000000, 0);
	if(status == -1) {
		cerr << "Could not receive modinfo: " << strerror(errno) << endl;
		close(remote);
		return 4; // 4 means recv failed
	}
	cout << status << " bytes received" << endl;
	string protocol = "";
	int iter;
	for(int i=0; i<8;i++) {
	protocol += rcvbuf[i];
	iter = i;	
	}
	string response = "";
	for(int i=9; rcvbuf[i] != '\n'; i++) {
	response += rcvbuf[i];
	iter = i;	
	}
	string timestamp = "";
	iter += 7;
	for(int i=iter+1; rcvbuf[i] != '\n'; i++) {
	timestamp += rcvbuf[i];
	iter = i;	
	}
	string server = "";
	iter += 9;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	server += rcvbuf[i];
	iter = i;	
	}
	string addinfo = "";
	for(int i = iter+1; rcvbuf[i+1] != 'C' || rcvbuf[i+2] != 'o' || rcvbuf[i+3] != 'n'|| rcvbuf[i+4] != 't' || rcvbuf[i+5] != 'e'; i++) {
	addinfo += rcvbuf[i];
	iter = i;
	}
	string cl = "";
	if(rcvbuf[iter+5] == 't') {
	iter += 16;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	cl += rcvbuf[i];
	iter = i;	
	}
	} else {
	cl = "";	
	}
	string connection = "";
	iter += 13;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	connection += rcvbuf[i];
	iter = i;	
	}
	string ct = "";
	iter += 15;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	ct += rcvbuf[i];
	iter = i;	
	}
	string paddinfo = "";
	if(rcvbuf[iter+1] != '\n' || rcvbuf[iter+2] != '\r' || rcvbuf[iter+3] != '\n') {
	for(int i = iter+1; rcvbuf[i+1] != '\n' || rcvbuf[i+2] != '\r' || rcvbuf[i+3] != '\n';i++) {
		paddinfo += rcvbuf[i];
		iter = i;
	}
	} else {
	paddinfo = "";	
	}
	string content = "";
	iter += 3;
	int recvcon=0;
	for(int i = iter+1; rcvbuf[i] != '\000'; i++) {
	content += rcvbuf[i];
	iter = i;
	recvcon++; 	
	}
	if(response[0] != '2' || response[1] != '0' || response[2] != '0' || response[4] != 'O' || response[5] != 'K') {
		cerr << "HTTP Error! Response: " << response << endl;
		return 5; // 5 means HTTP Error
	}
	if(cl != "") {
	int icl;
	icl = atoi(cl.c_str());
	while(recvcon < icl) {
		for(int i=0; i<1000000; i++) {
		rcvbuf[i] = NULL;	
		}
	status = recv(remote, &rcvbuf, 1000000, 0);
	if(status == -1) {
		cerr << "Could not receive modinfo: " << strerror(errno) << endl;
		close(remote);
		return 4; // 4 means recv failed
	} else if(status == 0) {
	cout << "Remote end has closed the connection!" << endl;
	break;
	}
	cout << status << " bytes received" << endl;
	for(int i = 0; rcvbuf[i] != '\000'; i++) {
	content += rcvbuf[i];
	recvcon++; 	
	}
	}
	}
	close(remote);
	cout << "Received modinfo! Starting modinfo parser..." << endl;
	rmodinfo tmp;
	status = parsemodinfo(&tmp, content, recvcon);
	if(status == 1) {
	cout << "Modinfo parse error!" << endl;
	return 6; // 6 means parse error	
	}
	*rmi = tmp;
	return 0; // 0 means all OK
}

int getmodlist(modlist *ml, modlistdata mld) {
int remote;
int yes=1;
int status;
	char caddr[INET6_ADDRSTRLEN];
	char rcvbuf[1000000];
		for(int i=0; i<1000000; i++) {
		rcvbuf[i] = NULL;	
		}
	addrinfo hints;
	addrinfo *servinfo, *p;
	cout << "getmodlist: Trying to get modlist: " << mld.name << endl;
	cout << "getmodlist: Connecting to: " << mld.server << endl;
	memset(&hints, 0, sizeof(hints)); // We do not want any trash here...
	hints.ai_family = AF_UNSPEC; // Make it IPv4 or IPv6 - which one fits.
	hints.ai_socktype = SOCK_STREAM; // HTTP works on TCP, right?
	if((status = getaddrinfo(mld.server.c_str(), "http", &hints, &servinfo)) != 0) { // Trying to get servinfo - server network address etc.
		cerr << "Failed to connect to the server: " << gai_strerror(status) << endl;
		return 1; // 1 means GAI failed
	}
	for(p = servinfo; p != NULL; p = p->ai_next) { // For every GAI result try to create a socket, set socket options and, finally, connect
		remote = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
		if(remote == -1) {
			cerr << "Failed to create socket: " << strerror(errno) << endl;
		}
		if((setsockopt(remote, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)) {
			cerr << "Failed to set socket options: " << strerror(errno) << endl;
		}
		status = connect(remote, servinfo->ai_addr, servinfo->ai_addrlen);
		if(status == -1) {
			cerr << "Could not connect to this address: " << strerror(errno) << endl;
			continue;
		}
		break;
	}
	if(p == NULL) {
		cerr << "Could not connect to the server: " << strerror(errno) << endl;
		return 2; // 2 means connecting failed
	}
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), caddr, sizeof(caddr));
	freeaddrinfo(servinfo);
	cout << "Connection to " << caddr << " has been successfully established!" << endl;
	cout << "Sending HTTP GET request..." << endl;
	string req = "";
	stringstream sreq;
	sreq << "GET " << mld.path << " HTTP/1.0\n" << "Host: " << mld.server << endl << endl;
	req = sreq.str();
	status = send(remote, req.c_str(), req.length(), 0);
	if(status == -1) {
		cerr << "Could not send HTTP GET request: " << strerror(errno) << endl;
		close(remote);
		return 3; // 3 means send failed
	}
	cout << status << " bytes sent" << endl;
	cout << "Receiving modlist..." << endl;
	status = recv(remote, &rcvbuf, 1000000, 0);
	if(status == -1) {
		cerr << "Could not receive modlist: " << strerror(errno) << endl;
		close(remote);
		return 4; // 4 means recv failed
	}
	cout << status << " bytes received" << endl;
	string protocol = "";
	int iter;
	for(int i=0; i<8;i++) {
	protocol += rcvbuf[i];
	iter = i;	
	}
	string response = "";
	for(int i=9; rcvbuf[i] != '\n'; i++) {
	response += rcvbuf[i];
	iter = i;	
	}
	string timestamp = "";
	iter += 7;
	for(int i=iter+1; rcvbuf[i] != '\n'; i++) {
	timestamp += rcvbuf[i];
	iter = i;	
	}
	string server = "";
	iter += 9;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	server += rcvbuf[i];
	iter = i;	
	}
	string addinfo = "";
	for(int i = iter+1; rcvbuf[i+1] != 'C' || rcvbuf[i+2] != 'o' || rcvbuf[i+3] != 'n' || rcvbuf[i+4] != 't' || rcvbuf[i+5] != 'e'; i++) {
	addinfo += rcvbuf[i];
	iter = i;
	}
	string cl = "";
	if(rcvbuf[iter+5] == 't') {
	iter += 16;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	cl += rcvbuf[i];
	iter = i;	
	}
	} else {
	cl = "";	
	}
	string connection = "";
	iter += 13;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	connection += rcvbuf[i];
	iter = i;	
	}
	string ct = "";
	iter += 15;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	ct += rcvbuf[i];
	iter = i;	
	}
	string paddinfo = "";
	if(rcvbuf[iter+1] != '\n' || rcvbuf[iter+2] != '\r' || rcvbuf[iter+3] != '\n') {
	for(int i = iter+1; rcvbuf[i+1] != '\n' || rcvbuf[i+2] != '\r' || rcvbuf[i+3] != '\n';i++) {
		paddinfo += rcvbuf[i];
		iter = i;
	}
	} else {
	paddinfo = "";	
	}
	string content = "";
	iter += 3;
	int recvcon=0;
	for(int i = iter+1; rcvbuf[i] != '\000'; i++) {
	content += rcvbuf[i];
	iter = i;
	recvcon++; 	
	}
	if(response[0] != '2' || response[1] != '0' || response[2] != '0' || response[4] != 'O' || response[5] != 'K') {
		cerr << "HTTP Error! Response: " << response << endl;
		return 5; // 5 means HTTP Error
	}
	if(cl != "") {
	int icl;
	icl = atoi(cl.c_str());
	while(recvcon < icl) {
		for(int i=0; i<1000000; i++) {
		rcvbuf[i] = NULL;	
		}
	status = recv(remote, &rcvbuf, 1000000, 0);
	if(status == -1) {
		cerr << "Could not receive modlist: " << strerror(errno) << endl;
		close(remote);
		return 4; // 4 means recv failed
	} else if(status == 0) {
	cout << "Remote end has closed the connection!" << endl;
	break;	
	}
	cout << status << " bytes received" << endl;
	for(int i = 0; rcvbuf[i] != '\000'; i++) {
	content += rcvbuf[i];
	recvcon++; 	
	}
	}
	}
	close(remote);
	cout << "Received modlist! Starting modlist parser..." << endl;
	modlist tmp;
	vector<rmodlistdata> vtmp;
	status = parsermodlist(&vtmp, content, recvcon);
	if(status == 1) {
	cout << "Modlist parse error!" << endl;
	return 6; // 6 means parse error	
	}
	tmp.name = mld.name;
	for(int i = 0; i < vtmp.size(); i++) {
	rmodinfo rmi;
	status = getmodinfo(&rmi, vtmp[i]);
	if(status != 0) {
		cerr << "Getmodinfo error: " << status << endl;
	} else {
	tmp.rmodinfos.push_back(rmi);	
	}
	}
	*ml = tmp;
	return 0; // 0 means all OK	
}

int writelocalmodlist(vector<modlist> mlv, string lmlfn) {
	ofstream lml(lmlfn.c_str());
	if(!lml) {
		cout << "Error: Cannot open local modlist file for writing!" << endl;
		return 1; // 1 means file write error
	}
	for(int i = 0; i < mlv.size(); i++) {
		for(int j = 0; j < mlv[i].rmodinfos.size(); j++) {
			lml << "{" << mlv[i].rmodinfos[j].name << "}" << endl;
			lml << "[rmodlist]" << endl;
			lml << mlv[i].name << endl;
			lml << "[description]" << endl;
			lml << mlv[i].rmodinfos[j].description << endl;
			lml << "[release]" << endl;
			lml << mlv[i].rmodinfos[j].release << endl;
			lml << "[deps]" << endl;
			for(int k = 0; k < mlv[i].rmodinfos[j].deps.size(); k++) {
			lml << mlv[i].rmodinfos[j].deps[k] << endl;	
			}
			lml << "[depsend]" << endl;
			lml << "[repotype]" << endl;
			lml << mlv[i].rmodinfos[j].repotype << endl;
			lml << "[repoaddr]" << endl;
			lml << mlv[i].rmodinfos[j].repoaddr << endl;
			lml << "{end}" << endl;
		}
	}
	lml.close();
	return 0; // 0 means all OK.
}

void modarg_parse(string *modname, string *modlist, string arg) {
*modname = "";
*modlist = "";
string tmp;
for(int i = 0; i < arg.length(); i++) {
	if(arg[i] == '/') {
	*modlist = tmp;
	tmp = "";	
	} else {
	tmp += arg[i];	
	}
}
*modname = tmp;
return;
}

int checkdeps(int lmodlistidx, vector<lmodlistdata> lmodlist, vector<repoinfodata> repoinfo, vector<actlist> *pactionv) {
	vector<actlist> actionv = *pactionv;
	cout << "Checking dependencies for " << strip_endl(lmodlist[lmodlistidx].name) << "..." << endl;
		int depsok = 1;
		int deplmodlistidx = -1;
		for(int j = 0; j < lmodlist[lmodlistidx].deps.size(); j++) {
			int depignore = 0;
			for(int k = 0; k < actionv.size(); k++) {
				if(strip_endl(lmodlist[lmodlistidx].deps[j]) == strip_endl(actionv[k].name)) {
					cout << "Already installing: " << strip_endl(lmodlist[lmodlistidx].deps[j]) << " - ignoring..." << endl;
					depignore = 1;
					break;
				}
			}
				if(depignore == 0 && strip_endl(lmodlist[lmodlistidx].deps[j]) != "none" && strip_endl(lmodlist[lmodlistidx].deps[j]) != "default") {
			for(int k = 0; k < lmodlist.size(); k++) {
				if(strip_endl(lmodlist[lmodlistidx].deps[j]) == strip_endl(lmodlist[k].name)) {
				deplmodlistidx = k;
				break;
				}
			}
		if(deplmodlistidx == -1 && strip_endl(lmodlist[lmodlistidx].deps[j]) != "none" && strip_endl(lmodlist[lmodlistidx].deps[j]) != "default") {
		cerr << "Could not satisfy dependencies for " << strip_endl(lmodlist[lmodlistidx].name) << endl << "(-S (Sync) action may resolve this)!" << endl;
		depsok = 0;
		break;
		}
			}
				}
	if(depsok == 1) {
		for(int j = 0; j < lmodlist[lmodlistidx].deps.size(); j++) {
		actlist depal;
			depal = alclear(depal);
			int depignore = 0;
			for(int k = 0; k < actionv.size(); k++) {
				if(strip_endl(lmodlist[lmodlistidx].deps[j]) == strip_endl(actionv[k].name)) {
					cout << "Already installing: " << strip_endl(lmodlist[lmodlistidx].deps[j]) << " - ignoring..." << endl;
					depignore = 1;
					break;
				}	
	}
	if(depignore == 0 && strip_endl(lmodlist[lmodlistidx].deps[j]) != "none" && strip_endl(lmodlist[lmodlistidx].deps[j]) != "default") {
		depal.name = strip_endl(lmodlist[lmodlistidx].deps[j]);
		for(int k = 0; k < repoinfo.size(); k++) {
		if(strip_endl(repoinfo[k].name) == strip_endl(lmodlist[lmodlistidx].deps[j])) {
		cout << strip_endl(lmodlist[lmodlistidx].deps[k]) << " already installed, not installing..." << endl;
		depal.installed = 1;
		break;
		}
	}
	if(depal.installed == 0) {
		int depret;
		depret = checkdeps(deplmodlistidx, lmodlist, repoinfo, &actionv);
		if(depret == 1) {
			cout << "Dependency check failed!" << endl;
		return 1;	
		}
		cout << "Adding dependency " << strip_endl(lmodlist[lmodlistidx].deps[j]) << " for installation..." << endl;
		depal.rmodlist = "";
		actionv.push_back(depal);
	}
	}
		}
		*pactionv = actionv;
		return 0;
	} else {
	return 1;	
	}
}

int recursive_rmdir(string dirname) {
	if(dirname == "." || dirname == "..") {
	return 0;	
	}
dirent *d;
DIR *dir = opendir(dirname.c_str());
if(dir == NULL) {
	FILE *fp = fopen(dirname.c_str(), "r");
	if(!fp) {
	return 1;	
	} else {
	fclose(fp);
	closedir(dir);
	int ret = unlink(dirname.c_str());
	if(ret == -1) {
		cerr << "Unlink error: " << strerror(errno) << endl;
		return 1;
	} else {
	return 0;	
	}
	}
} else {
	int n = 0;
	while ((d = readdir(dir)) != NULL) {
		n++;
		if(n > 2) {
			if(n == 3) {
		int ret = chdir(dirname.c_str());
		if(ret == -1) {
			cerr << "Could not chdir: " << strerror(errno) << endl;
			return 1;
		}
			}
		int ret = recursive_rmdir(d -> d_name);
		if(ret == 1) {
		cerr << "recursive_rmdir failed!" << endl;
		return 1;
		}
		}
	}
	if(n > 2) {
	int ret = chdir("..");	
	if(ret == -1) {
		cerr << "Could not chdir: " << strerror(errno) << endl;
		return 1;
	}
	closedir(dir);
	ret = rmdir(dirname.c_str());
	if(ret == -1) {
		cerr << "Could not rmdir: " << strerror(errno) << endl;
		return 1;
	}
	return 0;
	}
}
}

int main(int argc, char **argv) {
homedir = getenv("HOME");	
string config, modlistsfn;
stringstream sconfig, smodlistsfn;
sconfig << homedir << "/.3m/config";
smodlistsfn << homedir << "/.3m/remote_modlists";
config = sconfig.str();
modlistsfn = smodlistsfn.str();
int ps;
string localrepo, localml, localri;
if(argc < 2 || argv[1][0] != '-') {
cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
cout << "Usage: " << argv[0] << " [-S/I/U/R/Q/h/v] [options] modname1 modname2 ..." << endl;
} else {
	if(argv[1][1] == 'h') {
	cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
cout << "Usage: " << argv[0] << " [-S/I/U/R/Q/h/v] [options] arg1 arg2 ..." << endl;
cout << endl;
cout << "-S - Sync: gather information from remote modinfo files and update local modlist" << endl;
cout << "-I - Install: download and install mod(s)" << endl;
cout << "-U - Update: update already installed mod(s)" << endl;
cout << "-R - Remove: remove installed mod(s)" << endl;
cout << "-Q - Query: Query local modlist for a string in modname or description. It takes only one argument. The --local option displays a list of installed mods" << endl;
cout << "-h - Help: This message" << endl;
cout << "-v - Version: Print version" << endl;
	} else if(argv[1][1] == 'v') {
		cout << "v. 0.1-indev" << endl;
	} else {
cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
ps = parseconfig(&localrepo, &localml, &localri, config);
if(ps == 1) {
cerr << "Parse error! Exiting..." << endl;
return EXIT_FAILURE;
}
cout << "Config parsed successfully!" << endl;
//---DEBUG START---
//cout << "DEBUG: localrepo: " << localrepo << endl;
//cout << "DEBUG: localml: " << localml << endl;
//cout << "DEBUG: localri: " << localri << endl;
//---DEBUG END---
if(argv[1][1] == 'S') {
	cout << "Syncing local modlist with information from remote modlists and modinfos..." << endl;
	cout << "Parsing remote modlist list..." << endl;
	vector<modlistdata> mldv;
	int ret;
	ret = parsemodlists(&mldv, modlistsfn);
	if(ret == 1) {
	cerr << "Remote modlist list parse error! Aborting..." << endl;
	return EXIT_FAILURE;	
	}
	vector<modlist> mlv;
	modlist tmp;
	for(int i = 0; i < mldv.size(); i++) {
		cout << "Getting modlist: " << mldv[i].name << endl;
		ret = getmodlist(&tmp, mldv[i]);
		if(ret != 0) {
			cerr << "Error in getting modlist: " << mldv[i].name << endl;
			continue;
		}
		mlv.push_back(tmp);
	}
	cout << "Got all modlists and modinfos, generating local modlist..." << endl;
	ret = writelocalmodlist(mlv, localml);
	if(ret == 1) {
	cerr << "Local modlist write error! Aborting..." << endl;
	return EXIT_FAILURE;	
	}
	cout << "Sync finished successfully!" << endl;
} else if(argv[1][1] == 'Q') {
	vector<lmodlistdata> lmodlist;
	vector<repoinfodata> repoinfo;
	int lmpr;
	int ripr;
	lmpr = parselmodlist(&lmodlist, localml);
	if(lmpr == 1) {
	cerr << "Local modlist parse error (did you remember to use the -S (Sync) option first?)! Aborting..." << endl;
	return EXIT_FAILURE;	
	}
	ripr = parserepoinfo(&repoinfo, localri);
	if(ripr == 1) {
	cout << "Local repoinfo parse error. This might mean no mod has been installed by 3m yet. Continuing..." << endl << endl;	
	}
	if(argc == 2) {
		for(int i = 0; i < lmodlist.size(); i++) {
			repoinfodata ritmp;
			ritmp = ridclear(ritmp);
			if(ripr == 0) {
				for(int j = 0; j < repoinfo.size(); j++) {
					if(strip_endl(repoinfo[j].name) == strip_endl(lmodlist[i].name)) {
					ritmp = repoinfo[j];
					break;
					}
				}
				cout << strip_endl(lmodlist[i].rmodlist) << "/" << strip_endl(lmodlist[i].name) << " (release: " << lmodlist[i].release << ")";
				if(ritmp.name != "") {
					cout << " [installed: release " << ritmp.release;
					if(ritmp.release < lmodlist[i].release) {
					cout << " (Out of date!)";
					}
					cout << "]";
				}
				cout << endl << strip_endl(lmodlist[i].description) << endl;
				if(strip_endl(lmodlist[i].deps[0]) != "none") {
					cout << "Depends on: ";
					for(int j = 0; j < lmodlist[i].deps.size(); j++) {
					cout << strip_endl(lmodlist[i].deps[j]) << " ";	
					}
					cout << endl;
				}
				cout << endl;
			} else {
				cout << strip_endl(lmodlist[i].rmodlist) << "/" << strip_endl(lmodlist[i].name) << " (release: " << lmodlist[i].release << ")" << endl;
				cout << strip_endl(lmodlist[i].description) << endl;
				if(strip_endl(lmodlist[i].deps[0]) != "none") {
					cout << "Depends on: ";
					for(int j = 0; j < lmodlist[i].deps.size(); j++) {
					cout << strip_endl(lmodlist[i].deps[j]) << " ";
					}
					cout << endl;
				}
				cout << endl;
			}
		}
	} else {
		if(argc > 3) {
		cout << "Too many arguments! Taking only 3..." << endl;	
		}
		if(argv[2][0] == '-' && argv[2][1] == '-' && argv[2][2] == 'l' && argv[2][3] == 'o' && argv[2][4] == 'c' && argv[2][5] == 'a' && argv[2][6] == 'l') {
			if(ripr == 0) {
			for(int i = 0; i < repoinfo.size(); i++) {
				cout << "localrepo/" << repoinfo[i].name << " (release: " << repoinfo[i].release << ")" << endl << "Installed at: " << repoinfo[i].path << endl << endl;
		}
	}
		} else {
			for(int i = 0; i < lmodlist.size(); i++) {
				if(lmodlist[i].name.find(argv[2]) != -1 || lmodlist[i].description.find(argv[2]) != -1) {
			repoinfodata ritmp;
			ritmp = ridclear(ritmp);
			if(ripr == 0) {
				for(int j = 0; j < repoinfo.size(); j++) {
					if(strip_endl(repoinfo[j].name) == strip_endl(lmodlist[i].name)) {
					ritmp = repoinfo[j];
					break;
					}
				}
				cout << strip_endl(lmodlist[i].rmodlist) << "/" << strip_endl(lmodlist[i].name) << " (release: " << lmodlist[i].release << ")";
				if(ritmp.name != "") {
					cout << " [installed: release " << ritmp.release;
					if(ritmp.release < lmodlist[i].release) {
					cout << " (Out of date!)";
					}
					cout << "]";
				}
				cout << endl << strip_endl(lmodlist[i].description) << endl;
				if(strip_endl(lmodlist[i].deps[0]) != "none") {
					cout << "Depends on: ";
					for(int j = 0; j < lmodlist[i].deps.size(); j++) {
					cout << strip_endl(lmodlist[i].deps[j]) << " ";	
					}
					cout << endl;
				}
				cout << endl;
			} else {
				cout << strip_endl(lmodlist[i].rmodlist) << "/" << strip_endl(lmodlist[i].name) << " (release: " << lmodlist[i].release << ")" << endl;
				cout << strip_endl(lmodlist[i].description) << endl;
				if(strip_endl(lmodlist[i].deps[0]) != "none") {
					cout << "Depends on: ";
					for(int j = 0; j < lmodlist[i].deps.size(); j++) {
					cout << strip_endl(lmodlist[i].deps[j]) << " ";
					}
					cout << endl;
				}
				cout << endl;
			}
		}
		}
		}
	}
} else if(argv[1][1] == 'I') {
	vector<lmodlistdata> lmodlist;
	vector<repoinfodata> repoinfo;
	int lmpr;
	int ripr;
	lmpr = parselmodlist(&lmodlist, localml);
	if(lmpr == 1) {
	cerr << "Local modlist parse error (did you remember to use the -S (Sync) option first?)! Aborting..." << endl;
	return EXIT_FAILURE;	
	}
	ripr = parserepoinfo(&repoinfo, localri);
	if(ripr == 1) {
	cout << "Local repoinfo parse error. This might mean no mod has been installed by 3m yet. Continuing..." << endl << endl;	
	}
	if(argc < 3) {
	cerr << "You must enter at least one mod name! Exiting..." << endl;
	return EXIT_FAILURE;	
	} else {
		vector<actlist> actionv;
		cout << "Checking mod existence in lmodlist and repoinfo..." << endl;
		for(int i = 2; i < argc; i++) {
			actlist tmpal;
			tmpal = alclear(tmpal);
			string modname, rmodlist;
			modarg_parse(&modname, &rmodlist, argv[i]);
			int lmodlistidx = -1;
			for(int j = 0; j < lmodlist.size(); j++) {
				if(rmodlist == "") {
				if(strip_endl(lmodlist[j].name) == modname) {
				lmodlistidx = j;
				break;
				}
				} else {
					if(strip_endl(lmodlist[j].name) == modname && strip_endl(lmodlist[j].rmodlist) == rmodlist) {
					lmodlistidx = j;
					break;
					}
				}
			}
			if(lmodlistidx == -1) {
			cerr << "Mod " << argv[i] << " not found in local modlist (-S (Sync) action may resolve this)!" << endl;
			} else {
			tmpal.name = modname;
			tmpal.rmodlist = rmodlist;
			tmpal.lmidx = lmodlistidx;
	if(ripr == 0) {
	for(int j = 0; j < repoinfo.size(); j++) {
		if(strip_endl(repoinfo[j].name) == modname) {
		cout << "Warning! " << argv[i] << " already installed, reinstalling..." << endl;
		tmpal.installed = 1;
		break;
		}
	}
	}
		}
		int depret;
		depret = checkdeps(lmodlistidx, lmodlist, repoinfo, &actionv);
		if(depret == 1) {
		cout << "Dependency check failed!" << endl;
		return EXIT_FAILURE;
		}
		actionv.push_back(tmpal);
		}
		for(int i = 0; i < actionv.size(); i++) {
			if(actionv[i].lmidx == -1) {
				if(actionv[i].rmodlist == "") {
				for(int j = 0; j < lmodlist.size(); j++) {
				if(strip_endl(lmodlist[j].name) == strip_endl(actionv[i].name)) {
				actionv[i].lmidx = j;
				actionv[i].rmodlist = strip_endl(lmodlist[j].rmodlist);
				break;
				}	
				}
				} else {
				for(int j = 0; j < lmodlist.size(); j++) {
				if(strip_endl(lmodlist[j].name) == strip_endl(actionv[i].name) && strip_endl(lmodlist[j].rmodlist) == strip_endl(actionv[i].rmodlist)) {
				actionv[i].lmidx = j;
				break;
				}	
				}	
				}
		}
		if(strip_endl(lmodlist[actionv[i].lmidx].repotype) == "archive") {
		int ret;
		ret = chdir(localrepo.c_str());
		if(ret == -1) {
			cerr << "Could not chdir to local repository: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
		ret = mkdir(actionv[i].name.c_str(), 0755);
		if(ret == -1 && errno != EEXIST) {
		cerr << "Could not make a directory for mod files!" << endl;
		return EXIT_FAILURE;
		}
		ofstream mp("modpack.txt");
		if(!mp) {
		cerr << "Could not create modpack.txt!" << endl;
		return EXIT_FAILURE;
		} else {
			mp << "This is a modpack file created by 3m for Minetest to include subdirectories of this directory." << endl;
			mp.close();
		}
		ret = chdir(actionv[i].name.c_str());
		if(ret == -1) {
			cerr << "Could not chdir to directory for mod files: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
		ofstream dmp("modpack.txt");
		if(!dmp) {
		cerr << "Could not create modpack.txt!" << endl;
		return EXIT_FAILURE;
		} else {
			dmp << "This is a modpack file created by 3m for Minetest to include subdirectories of this directory." << endl;
			dmp.close();
		}
		string cmd = "";;
		stringstream scmd;
		scmd << "wget -N " << lmodlist[actionv[i].lmidx].repoaddr;
		cmd = scmd.str();
		ret = system(cmd.c_str());
		if(ret == -1) {
		cerr << "Could not execute wget!" << endl;	
		} else {
		int status = WEXITSTATUS(ret);
		if(status != 0) {
			cerr << "wget failed: ";
		switch(status) {
		case 1: cerr << "Generic error code!" << endl; break;
		case 2: cerr << "Config parse error!" << endl; break;
		case 3: cerr << "File I/O error!" << endl; break;
		case 4: cerr << "Network failure!" << endl; break;
		case 5: cerr << "SSL verification failure!" << endl; break;
		case 6: cerr << "Username/password authentication failure!" << endl; break;
		case 7: cerr << "Protocol error!" << endl; break;
		case 8: cerr << "Server issued an error response!" << endl; break;
		default: cerr << "Something failed!" << endl; break;
		}
		return EXIT_FAILURE;
		}
		scmd.str("");
		scmd << "basename " << strip_endl(lmodlist[actionv[i].lmidx].repoaddr) << " > dlbn";
		cmd = scmd.str();
		ret = system(cmd.c_str());
		if(ret == -1) {
		cout << "Could not execute basename!" << endl;
		return EXIT_FAILURE;
		}
		ifstream dlbn("dlbn");
		if(!dlbn) {
		cout << "Could not open dlbn!" << endl;
		return EXIT_FAILURE;
		}
		string dlfn;
		while(!dlbn.eof()) {
		char c;
		c = dlbn.get();
		if(dlbn) {
		dlfn += c;	
		}
		}
		dlbn.close();
		dlfn = strip_endl(dlfn);
		scmd.str("");
		scmd << "7z x " << dlfn;
		cmd = scmd.str();
		ret = system(cmd.c_str());
		if(ret == -1) {
		cerr << "Could not execute 7z!" << endl;
		return EXIT_FAILURE;
		}
		status = WEXITSTATUS(ret);
		if(status == 2) {
			cerr << "7z failed: Fatal error!" << endl;
			return EXIT_FAILURE;
		} else if(status == 7) {
			cerr << "7z failed: Bad commandline arguments!" << endl;
			return EXIT_FAILURE;
		} else if(status == 8) {
			cerr << "7z failed: Not enough memory!" << endl;
			return EXIT_FAILURE;
		} else if(status == 255) {
			cerr << "7z failed: Aborted by user!" << endl;
			return EXIT_FAILURE;
		} else if(status == 1) {
			cerr << "7z reported warnings!" << endl;	
		} else {
			if(actionv[i].installed == 1) {
				for(int k = 0; k < repoinfo.size(); k++) {
					if(strip_endl(repoinfo[k].name) == strip_endl(actionv[i].name)) {
					string instpath;
					stringstream sinstpath;
					sinstpath << localrepo << strip_endl(actionv[i].name);
					instpath = sinstpath.str();
					repoinfo[k].path = instpath;
					repoinfo[k].release = lmodlist[actionv[i].lmidx].release;
					break;
					}
				}
			} else {
			repoinfodata tmprid;
			tmprid.name = actionv[i].name;
			tmprid.release = lmodlist[actionv[i].lmidx].release;
			string instpath;
			stringstream sinstpath;
			sinstpath << localrepo << strip_endl(actionv[i].name) << endl;
			instpath = sinstpath.str();
			tmprid.path = instpath;
			repoinfo.push_back(tmprid);
			ret = chdir("..");
		if(ret == -1) {
			cerr << "Could not chdir one level up: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
			}
			cout << actionv[i].name << " installed successfully!" << endl;
		}
		}
		} else if(strip_endl(lmodlist[actionv[i].lmidx].repotype) == "git") {
			int ret;
		ret = chdir(localrepo.c_str());
		if(ret == -1) {
			cerr << "Could not chdir to local repository: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
		ret = mkdir(actionv[i].name.c_str(), 0755);
		if(ret == -1 && errno != EEXIST) {
		cerr << "Could not make a directory for mod files!" << endl;
		return EXIT_FAILURE;
		}
		ofstream mp("modpack.txt");
		if(!mp) {
		cerr << "Could not create modpack.txt!" << endl;
		return EXIT_FAILURE;
		} else {
			mp << "This is a modpack file created by 3m for Minetest to include subdirectories of this directory." << endl;
			mp.close();
		}
		ret = chdir(actionv[i].name.c_str());
		if(ret == -1) {
			cerr << "Could not chdir to directory for mod files: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
		ofstream dmp("modpack.txt");
		if(!dmp) {
		cerr << "Could not create modpack.txt!" << endl;
		return EXIT_FAILURE;
		} else {
			dmp << "This is a modpack file created by 3m for Minetest to include subdirectories of this directory." << endl;
			dmp.close();
		}
		string cmd = "";
		stringstream scmd;
		scmd << "git clone " << strip_endl(lmodlist[actionv[i].lmidx].repoaddr);
		cmd = scmd.str();
		ret = system(cmd.c_str());
		if(ret == -1){
			cerr << "Could not execute git clone: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
		if(actionv[i].installed == 1) {
				for(int k = 0; k < repoinfo.size(); k++) {
					if(strip_endl(repoinfo[k].name) == strip_endl(actionv[i].name)) {
					string instpath;
					stringstream sinstpath;
					sinstpath << localrepo << strip_endl(actionv[i].name);
					instpath = sinstpath.str();
					repoinfo[k].path = instpath;
					repoinfo[k].release = lmodlist[actionv[i].lmidx].release;
					break;
					}
				}
			} else {
			repoinfodata tmprid;
			tmprid.name = actionv[i].name;
			tmprid.release = lmodlist[actionv[i].lmidx].release;
			string instpath;
			stringstream sinstpath;
			sinstpath << localrepo << strip_endl(actionv[i].name) << endl;
			instpath = sinstpath.str();
			tmprid.path = instpath;
			repoinfo.push_back(tmprid);
			ret = chdir("..");
		if(ret == -1) {
			cerr << "Could not chdir one level up: " << strerror(errno) << endl;
			return EXIT_FAILURE;
		}
			}
			cout << actionv[i].name << " installed successfully!" << endl;
		} else {
		cerr << "Bad repotype! Exiting..." << endl;
		return EXIT_FAILURE;
		}
		}
		ofstream ri(localri.c_str());
			if(!ri) {
			cerr << "Could not open local repoinfo file for writing!" << endl;
			return EXIT_FAILURE;
			}
			for(int k = 0; k < repoinfo.size(); k++) {
				ri << '{' << strip_endl(repoinfo[k].name) << '}' << endl << "[release]" << endl << repoinfo[k].release << endl << "[path]" << endl << strip_endl(repoinfo[k].path) << endl << "{end}" << endl;	
			}
			ri.close();
			cout << "Installation finished successfully!" << endl;
	}
} else if(argv[1][1] == 'R') {
	if(argc < 3) {
	cerr << "You must enter at least one mod name! Exiting..." << endl;
	return EXIT_FAILURE;	
	}
	int eri = 0;
	vector<repoinfodata> repoinfo;
	int ripr;
	ripr = parserepoinfo(&repoinfo, localri);
	if(ripr == 1) {
	cerr << "Local repoinfo parse error! Exiting..." << endl;
	return EXIT_FAILURE;	
	}
	for(int i = 2; i < argc; i++) {
	int ridx = -1;
	for(int j = 0; j < repoinfo.size(); j++) {
		if(strip_endl(repoinfo[j].name) == strip_endl(argv[i])) {
		ridx = j;	
		}
	}
	if(ridx == -1) {
	cerr << "Mod " << argv[i] << " not found in local repository! Skipping..." << endl;	
	} else {
	int ret = chdir(localrepo.c_str());
	if(ret == -1) {
		cerr << "Could not chdir to local repository: " << errno << endl;
		return EXIT_FAILURE;
	}
	ret = recursive_rmdir(repoinfo[ridx].name.c_str());
	if(ret == 1) {
	cerr << "Error in recursive_rmdir for " << argv[i] << "! Skipping..." << endl;	
	} else {
		vector<repoinfodata>::iterator it;
		int k;
		for(it = repoinfo.begin(), k = 0; it < repoinfo.end() && k <= ridx; k++, it++);
		it = repoinfo.erase(it);
		if(it == repoinfo.end()) {
		eri = 1;
		ret = unlink(localri.c_str());
		if(ret == -1) {
		cerr << "Could not delete empty repoinfo file: " << strerror(errno) << endl;
		return EXIT_FAILURE;
		}
		}
		cout << argv[i] << " successfully removed!" << endl;
	}
	}
}
if(eri == 0) {
ofstream ri(localri.c_str());
			if(!ri) {
			cerr << "Could not open local repoinfo file for writing!" << endl;
			return EXIT_FAILURE;
			}
			for(int k = 0; k < repoinfo.size(); k++) {
				ri << '{' << strip_endl(repoinfo[k].name) << '}' << endl << "[release]" << endl << repoinfo[k].release << endl << "[path]" << endl << strip_endl(repoinfo[k].path) << endl << "{end}" << endl;	
			}
			ri.close();
}
			cout << "Removal finished successfully!" << endl;
} else {
	cout << "No such action: " << argv[1] << endl << "Usage: " << argv[0] << " [-S/I/U/R/Q/h/v] [options] arg1 arg2 ..." << endl;	
}
return EXIT_SUCCESS;
}
}
}
