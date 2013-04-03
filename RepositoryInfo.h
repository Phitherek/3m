#ifndef _REPOSITORYINFO_H
#define _REPOSITORYINFO_H
#include "ConfigFile.h"
#include "RepositoryModDescription.h"
#include <vector>
#include <string>
/// \file RepositoryInfo.h
/// \brief A class representing a local repository info file.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class RepositoryInfo
/// \brief A class representing a local repository info file.
class RepositoryInfo {
private:
	ConfigFile _conf;
	std::vector<RepositoryModDescription> _repoinfo;
	int _repoinfoIterator;
	bool _repoinfoAtEnd;
public:
	RepositoryInfo(); ///< A constructor.
	RepositoryInfo(ConfigFile conf); ///< \brief A constructor with parameter.
	///< Tries to open and parse local repository info file from given ConfigFile.
	///< \param conf A ConfigFile object.
	~RepositoryInfo(); ///< A destructor.
	RepositoryModDescription getNextModDescription(); ///< \brief A function that returns next RepositoryModDescription from the list.
	///< \return Next RepositoryModDescription from the list or empty RepositoryModDescription object if at end.
	RepositoryModDescription getModDescriptionByName(std::string name); ///< \brief A function that searches for a mod name and returns its RepositoryModDescription.
	///< \param name A mod name.
	///< \return A RepositoryModDescription of the mod or empty RepositoryModDescription object on failure.
	void insertModDescription(RepositoryModDescription rmd); ///< \brief A function that inserts a RepositoryModDescription to the repository info file.
	///< \param rmd A RepositoryModDescription to insert.
	void deleteModDescription(std::string name); ///< \brief A function that removes a RepositoryModDescription from local repository info file.
	///< \param name A name of a mod to be removed.
	void resetModDescriptionIterator(); ///< A function that resets repository info iterator.
	bool modDescriptionsAtEnd(); ///< \brief A function that returns if repository info iterator is at its end.
	///< \return True if iterator is at end, false otherwise.
	void setConfigFile(ConfigFile conf); ///< \brief A function that sets ConfigFile in the object.
	///< \param conf A ConfigFile object.
	void write(); ///< A function that writes back the repository info file.
	void clear(); ///< A function that clears the object.
};
}
