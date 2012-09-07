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
using namespace std;

int socdesc; // Socket descriptor
string homedir, errmsg, req; // Home directory, error message, request

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
return 0;	
}
}
}
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
