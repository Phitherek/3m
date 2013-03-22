#ifndef _MODINFODESCRIPTION_H
#define _MODINFODESCRIPTION_H
#include <string>
/// \file ModInfoDescription.h
/// \brief A class describing a modinfo file.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m
namespace mmm {
/// \class ModInfoDescription
/// \brief A class describing a modinfo file.
class ModInfoDescription {
private:
	std::string _name;
	std::string _server;
	std::string _path;
public:
	ModInfoDescription(); ///< A constructor.
	ModInfoDescription(std::string name, std::string server, std::string path); ///< \brief A constructor with parameters.
	///< \param name Modinfo name.
	///< \param server Server with modinfo.
	///< \param path Modinfo path on server.
	std::string getName(); ///< \brief A function returning modinfo name.
	///< \return Modinfo name.
	std::string getServer(); ///< \brief A function returning server with modinfo.
	///< \return A server with modinfo.
	std::string getPath(); ///< \brief A function returning modinfo path on server.
	///< \return Modinfo path on server.
	void setName(std::string name); ///< \brief A function setting modinfo name.
	///< \param name Modinfo name.
	void setServer(std::string server); ///< \brief A function setting server with modinfo.
	///< \param server Server with modinfo.
	void setPath(std::string path); ///< \brief A function setting modinfo path on server.
	///< \param path Modinfo path on server.
}
}
