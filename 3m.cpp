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

int parsermodlist(vector<rmodlistdata> *rmodlist, string modlist, int size) {
vector<rmodlistdata> tmpv = *rmodlist;
string action = "detect";
rmodlistdata tmprmld;
for(int i = 0; i < size; i++) {
	string line = "";
	int erased;
	line = strgetline(&modlist, &erased);
	size -= erased;
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
*rmodlist = tmpv;
return 0;
}

int parsemodinfo(rmodinfo *mis, string modinfo, int size) {
rmodinfo tmp = *mis;
string action = "detect";
for(int i = 0; i < size; i++) {
	string line = "";
	int erased;
	line = strgetline(&modinfo, &erased);
	size -= erased;
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
			return 1;
		}
	} else if(action == "parse") {
	if(line[0] == '{') {
		if(line[1] == 'e' && line[2] == 'n' && line[3] == 'd' && line[4] == '}') {
		action = "detect";
		} else {
			cerr << "Modinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
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
			return 1;
		}
	} else {
		cerr << "Modinfo parse error: Found " << line << " although {end} or action in [] was expected." << endl;
			return 1;
	}
	} else if(action == "description") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmp.description = line;
			action = "parse";
		}
	} else if(action == "release") {
		if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmp.release = atoi(line.c_str());
			action = "parse";
		}
	} else if(action == "deps") {
		if(line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string or [ was expected." << endl;
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
			return 1;
		} else {
			tmp.repotype = line;
			action = "parse";
		}
	} else if(action == "repoaddr") {
	if(line[0] == '[' || line[0] == '{') {
			cerr << "Modinfo parse error: Found " << line[0] << " although string was expected." << endl;
			return 1;
		} else {
			tmp.repoaddr = line;
			action = "parse";
		}
	} else {
		cerr << "Modinfo parse error: The program should not reach this place!" << endl;
		return 1;
	}
}
*mis = tmp;
return 0;
}

int getmodinfo(rmodinfo *rmi ,rmodlistdata mld) {
int remote;
int yes=1;
int status;
	char caddr[INET6_ADDRSTRLEN];
	char rcvbuf[1000000];
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
	sreq << "GET " << mld.modinfo << " HTTP/1.0\n\n";
	req = sreq.str();
	status = send(remote, req.c_str(), req.length(), 0);
	if(status == -1) {
		cerr << "Could not send HTTP GET request: " << strerror(errno) << endl;
		return 3; // 3 means send failed
	}
	cout << status << " bytes sent" << endl;
	cout << "Receiving modinfo..." << endl;
	status = recv(remote, &rcvbuf, 1000000, 0);
	if(status == -1) {
		cerr << "Could not receive modinfo: " << strerror(errno) << endl;
		return 4; // 4 means recv failed
	}
	string protocol;
	int iter;
	for(int i=0; i<8;i++) {
	protocol += rcvbuf[i];
	iter = i;	
	}
	string response;
	for(int i=9; rcvbuf[i] != '\n'; i++) {
	response += rcvbuf[i];
	iter = i;	
	}
	string timestamp;
	iter += 7;
	for(int i=iter+1; rcvbuf[i] != '\n'; i++) {
	timestamp += rcvbuf[i];
	iter = i;	
	}
	string server;
	iter += 9;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	server += rcvbuf[i];
	iter = i;	
	}
	string addinfo;
	for(int i = iter+1; rcvbuf[i+1] != 'C' || rcvbuf[i+2] != 'o' || rcvbuf[i+3] != 'n'; i++) {
	addinfo += rcvbuf[i];
	iter = i;
	}
	string cl;
	if(rcvbuf[iter+5] == 't') {
	iter += 16;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	cl += rcvbuf[i];
	iter = i;	
	}
	} else {
	cl = "";	
	}
	string connection;
	iter += 13;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	connection += rcvbuf[i];
	iter = i;	
	}
	string ct;
	iter += 15;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	ct += rcvbuf[i];
	iter = i;	
	}
	string paddinfo;
	if(rcvbuf[iter+1] != '\n' || rcvbuf[iter+2] != '\r' || rcvbuf[iter+3] != '\n') {
	for(int i = iter+1; rcvbuf[i+1] != '\n' || rcvbuf[i+2] != '\r' || rcvbuf[i+3] != '\n';i++) {
		paddinfo += rcvbuf[i];
		iter = i;
	}
	} else {
	paddinfo = "";	
	}
	string content;
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
		return 4; // 4 means recv failed
	}
	cout << status << " bytes received" << endl;
	for(int i = iter+1; rcvbuf[i] != '\000'; i++) {
	content += rcvbuf[i];
	iter = i;
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
	sreq << "GET " << mld.path << " HTTP/1.0\n\n";
	req = sreq.str();
	status = send(remote, req.c_str(), req.length(), 0);
	if(status == -1) {
		cerr << "Could not send HTTP GET request: " << strerror(errno) << endl;
		return 3; // 3 means send failed
	}
	cout << status << " bytes sent" << endl;
	cout << "Receiving modlist..." << endl;
	status = recv(remote, &rcvbuf, 1000000, 0);
	if(status == -1) {
		cerr << "Could not receive modlist: " << strerror(errno) << endl;
		return 4; // 4 means recv failed
	}
	string protocol;
	int iter;
	for(int i=0; i<8;i++) {
	protocol += rcvbuf[i];
	iter = i;	
	}
	string response;
	for(int i=9; rcvbuf[i] != '\n'; i++) {
	response += rcvbuf[i];
	iter = i;	
	}
	string timestamp;
	iter += 7;
	for(int i=iter+1; rcvbuf[i] != '\n'; i++) {
	timestamp += rcvbuf[i];
	iter = i;	
	}
	string server;
	iter += 9;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	server += rcvbuf[i];
	iter = i;	
	}
	string addinfo;
	for(int i = iter+1; rcvbuf[i+1] != 'C' || rcvbuf[i+2] != 'o' || rcvbuf[i+3] != 'n'; i++) {
	addinfo += rcvbuf[i];
	iter = i;
	}
	string cl;
	if(rcvbuf[iter+5] == 't') {
	iter += 16;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	cl += rcvbuf[i];
	iter = i;	
	}
	} else {
	cl = "";	
	}
	string connection;
	iter += 13;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	connection += rcvbuf[i];
	iter = i;	
	}
	string ct;
	iter += 15;
	for(int i = iter+1; rcvbuf[i] != '\n'; i++) {
	ct += rcvbuf[i];
	iter = i;	
	}
	string paddinfo;
	if(rcvbuf[iter+1] != '\n' || rcvbuf[iter+2] != '\r' || rcvbuf[iter+3] != '\n') {
	for(int i = iter+1; rcvbuf[i+1] != '\n' || rcvbuf[i+2] != '\r' || rcvbuf[i+3] != '\n';i++) {
		paddinfo += rcvbuf[i];
		iter = i;
	}
	} else {
	paddinfo = "";	
	}
	string content;
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
		return 4; // 4 means recv failed
	}
	cout << status << " bytes received" << endl;
	for(int i = iter+1; rcvbuf[i] != '\000'; i++) {
	content += rcvbuf[i];
	iter = i;
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
cout << "Usage: " << argv[0] << " [-S/I/R/Q/h/v] [options] modname1 modname2 ..." << endl;
} else {
	if(argv[1][1] == 'h') {
	cout << "3m - Minetest Mod Manager v. 0.1-indev (C) 2012 by Phitherek_" << endl;
cout << "Usage: " << argv[0] << " [-S/I/R/Q/h/v] [options] modname1 modname2 ..." << endl;
cout << endl;
cout << "-S - Sync: gather information from remote modinfo files and update local modlist" << endl;
cout << "-I - Install: download and install mod(s)" << endl;
cout << "-R - Remove: remove installed mod(s)" << endl;
cout << "-Q - Query: Query local modlist for a modname(s)" << endl;
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
cout << "DEBUG: localrepo: " << localrepo << endl;
cout << "DEBUG: localml: " << localml << endl;
cout << "DEBUG: localri: " << localri << endl;
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
} else {
	cout << "No such action: " << argv[1] << endl << "Usage: " << argv[0] << " [-S/I/R/Q/h/v] [options] modname1 modname2 ..." << endl;	
}
return EXIT_SUCCESS;
}
}
}
