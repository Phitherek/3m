#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H
#include <string>
/// \file ConfigFile.h
/// \brief A class that parses and stores 3m configuration.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m
namespace mmm {

/// \class ConfigFile
/// \brief A class that parses and stores 3m configuration.
class ConfigFile {
private:
	std::string _path;
	std::string _localpath;
	std::string _modlist;
	std::string _repoinfo;
public:
	ConfigFile(std::string path); ///< \brief A constructor from file path.
	///< Tries to open the config file and parse it. It throws FileException or ParseException.
	/// \param path Path to the config file.
	std::string& getLocalPath(); ///< \brief A function returning a reference to local 3m repository path (e. g. path to the Minetest mod directory)
	/// \return Local 3m repository path.
	std::string& getModList(); ///< \brief A function returning a reference to local modlist file path.
	///< \return A reference to local modlist file path.
	std::string& getRepoInfo(); ///< \brief A function returning a reference to local repoinfo file path.
	///< \return A reference to local repoinfo file path
	void write(); ///< A function that writes the changes to config file.
};
}
#endif
