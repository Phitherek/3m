#include "ModList.h"
#include "NetSocket++/NetSocketPP.h"
#include "3mExceptions.h"
using namespace mmm;

ModList::ModList() {
	ModListDescription emptymld;
	_desc = emptymld;
	_modinfos.clear();
	_edit = false;
	_localPath = "";
	_modinfosIterator = -1;
	_modinfosAtEnd = true;
}

ModList::ModList(ModListDescription mld) {
	_desc = mld;
	_modinfos.clear();
	_edit = false;
	_localPath = "";
	_modinforIterator = -1;
	_modinfosAtEnd = false;
	NetSocketPP::HTTPReply data;
	try {
	NetSocketPP::HTTPClientSocket conn(_desc.getServer(), "http", _desc.getPath());
	std::cout << "Connection to " << conn.getIP() << " successful!" << std::endl;
	data = conn.getReply();
	} catch (NetSocketPP::SocketException &exc) {
		std::cerr << "SocketException occured while getting remote modlist: " << exc.what() << std::endl;
		throw(exc);
	} catch (NetSocketPP::NetworkException &exc) {
		std::cerr << "NetworkException occured while getting remote modlist: " << exc.what() << std::endl;
		throw(exc);
	}
	if(data.getResponse() != "200 OK") {
		throw BadResponseException(data.getResponse());
	}
	std::string content = data.getContent();
	
}
