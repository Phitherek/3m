#ifndef _MODLIST_H
#define _MODLIST_H
#include "ModListDescription.h"
#include "ModInfoDescription.h"
#include <string>
#include <vector>
/// \file ModList.h
/// \brief A class representing a modlist.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class ModList
/// \brief A class representing a modlist.
class ModList {
private:
	ModListDescription _desc;
	std::vector<ModInfoDescription> _modinfos;
	bool _edit;
	std::string _localPath;
	int _modinfosIterator;
	bool _modinfosAtEnd;
public:
	ModList(); ///< A constructor.
	ModList(ModListDescription mld); ///< \brief A constructor from ModListDescription.
	///< It tries to download and parse the remote modlist file. It throws ParseException or one of NetSocket++ exceptions.
	///< \param mld A ModListDescription object.
	ModList(std::string path); ///< \brief A constructor from std::string.
	///< It tries to open and parse locally stored modlist file. It throws FileException or ParseException.
	~ModList(); ///< A destructor.
	ModInfoDescription getNextModInfoDescription(); ///< \brief A function that returns next ModInfoDescription from the modlist.
	///< \return Next ModInfoDescription from the modlist or empty ModInfoDescription object if at the end of modlist.
	ModInfoDescription getModInfoDescriptionByName(std::string name); ///< \brief A function that searches for modinfo name and returns its ModInfoDescription.
	///< \param name Modinfo name.
	///< \return ModInfoDescription of the modinfo or empty ModInfoDescription object on failure.
	void insertModInfoDescription(ModInfoDescription mid); ///< \brief A function that inserts a ModInfoDescription to the modlist.
	///< \param mid A ModInfoDescription to be inserted.
	void deleteModInfoDescription(std::string name); ///< \brief A function that deletes ModInfoDescription of given name from the modlist.
	///< \param name Modinfo name.
	void resetModInfoDescriptionIterator(); ///< A function that resets modlist iterator.
	bool modInfoDescriptionsAtEnd(); ///< \brief A function, that returns if modlist iterator reached its end.
	///< \return True if modlist iterator reached its end, false otherwise.
	void write(); ///< A function that writes modlist to local file.
};
}
#endif
