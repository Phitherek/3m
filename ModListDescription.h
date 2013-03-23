#ifndef _MODLISTDESCRIPTION_H
#define _MODLISTDESCRIPTION_H
#include <string>
/// \file ModListDescription.h
/// \brief A class describing a remote modlist.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class ModListDescription
/// \brief A class describing a remote modlist.
class ModListDescription {
private:
	std::string _name;
	std::string _server;
	std::string _path;
public:
	ModListDescription(); ///< A constructor.
	ModListDescription(std::string name, std::string server, std::string path); ///< \brief A constructor with parameters.
	///< \param name Remote modlist name.
	///< \param server Server with remote modlist.
	///< \param path Remote modlist path on server.
	~ModListDescription(); ///< A destructor.
	std::string getName(); ///< \brief A function returning remote modlist name.
	///< \return Remote modlist name.
	std::string getServer(); ///< \brief A function returning server with remote modlist.
	///< \return A server with remote modlist.
	std::string getPath(); ///< \brief A function returning remote modlist path on server.
	///< \return Remote modlist path on server.
	void setName(std::string name); ///< \brief A function setting remote modlist name.
	///< \param name Remote modlist name.
	void setServer(std::string server); ///< \brief A function setting server with remote modlist.
	///< \param server Server with remote modlist.
	void setPath(std::string path); ///< \brief A function setting remote modlist path on server.
	///< \param path Remote modlist path on server.
};
}
#endif
