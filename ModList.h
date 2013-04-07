#ifndef _MODLIST_H
#define _MODLIST_H
#include "ModListDescription.h"
#include "ModInfoDescription.h"
#include "ModInfo.h"
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
	std::vector<ModInfo> _modinfos;
	bool _edit;
	std::string _localPath;
	int _modinfosIterator;
	bool _modinfosAtEnd;
public:
	ModList(); ///< A constructor.
	ModList(ModListDescription mld); ///< \brief A constructor from ModListDescription.
	///< It tries to download and parse the remote modlist file and the modinfos. It throws ParseException or one of NetSocket++ exceptions.
	///< \param mld A ModListDescription object.
	ModList(std::string path); ///< \brief A constructor from std::string.
	///< It tries to open and parse locally stored modlist file. It throws FileException or ParseException.
	~ModList(); ///< A destructor.
	ModInfo getNextModInfo(); ///< \brief A function that returns next ModInfo from the modlist.
	///< It throws ParseException or one of NetSocket++ exceptions.
	///< \return Next ModInfo from the modlist or empty ModInfo object if at the end of modlist.
	ModInfo getModInfoByName(std::string name); ///< \brief A function that searches for modinfo name and returns its ModInfo.
	///< \param name Modinfo name.
	///< \return ModInfo or empty ModInfo object on failure.
	void insertModInfoDescription(ModInfoDescription mid); ///< \brief A function that inserts a ModInfoDescription to the modlist.
	///< \param mid A ModInfoDescription to be inserted.
	void deleteModInfo(std::string name); ///< \brief A function that deletes ModInfo of given name from the modlist.
	///< \param name Modinfo name.
	void resetModInfoIterator(); ///< A function that resets modlist iterator.
	bool modinfosEnd(); ///< \brief A function, that returns if modlist iterator reached its end.
	///< \return True if modlist iterator reached its end, false otherwise.
	void write(); ///< A function that writes modlist to local file.
	void clear(); ///< A function that clears the object.
	ModListDescription getModListDescription(); ///< \brief A function that gets a ModListDescription object.
	///< \return A ModListDescription object.
	void setPath(std::string path); ///< \brief A function that sets path to local modlist file.
	///< \param path A path to local modlist file.
};
}
#endif
