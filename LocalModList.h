#ifndef _LOCALMODLIST_H
#define _LOCALMODLIST_H
#include <string>
#include <vector>
#include "LocalModDescription.h"
#include "ConfigFile.h"
/// \file LocalModList.h
/// \brief A class representing a local modlist.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class LocalModList
/// \brief A class representing a local modlist.
class LocalModList {
private:
	ConfigFile _conf;
	std::vector<LocalModDescription> _modlist;
	int _modlistIterator;
	bool _modlistAtEnd;
public:
	LocalModList(); ///< A constructor.
	LocalModList(ConfigFile conf); ///< \brief A constructor with parameter.
	///< This constructor tries to parse the modlist in the path given by the ConfigFile object.
	///< \param conf A ConfigFile object.
	~LocalModList(); ///< A destructor.
	void addModDescription(LocalModDescription lmd); ///< \brief A function that adds a mod description to the modlist.
	/// \param lmd A mod description to be added.
	LocalModDescription getNextModDescription(); ///< \brief A function that returns next mod description from the modlist.
	///< \return Next mod description from the modlist.
	LocalModDescription getModDescriptionByName(std::string name); ///< \brief A function, that searches a mod name and returns its description.
	///< \param name A mod name.
	///< \return Description of the mod.
	LocalModDescription getModDescriptionByNameFrom(std::string name, std::string remoteModlist); ///< \brief A function, that searches a mod name in given remote modlist and returns its description.
	///< \param name A mod name.
	///< \param remoteModlist A name of remote modlist to search in.
	///< \return Description of the mod.
	void resetModDescriptionIterator(); ///< A function that resets iterator of the modlist.
	bool modDescriptionsAtEnd(); ///< \brief A function that returns if the modlist iterator has reached its end.
	///< \return True if modlist iterator is at end, false otherwise.
	void setConfigFile(ConfigFile conf); ///< \brief A function that sets the used config file.
	///< \param conf A ConfigFile object.
	void write(); ///< A function that writes the changes to the modlist file.
};
}

#endif
