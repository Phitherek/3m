#ifndef _MODLISTLIST_H
#define _MODLISTLIST_H
#include "ModListDescription.h"
#include <vector>
/// \file ModListList.h
/// \brief A class representing a modlist list.
/// \author Phitherek_
/// \date 2013
/// \version 0.1

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class ModListList
/// \brief A class representing a modlist list.
class ModListList {
private:
	std::string _rmfn;
	std::vector<ModListDescription> _modlists;
	int _modlistsIterator;
	bool _modlistsAtEnd;
public:
	ModListList(); ///< A constructor.
	~ModListList(); ///< A destructor.
	ModListDescription getNextModlistDescription(); ///< \brief A function returning next modlist description from the list.
	///< \return Next modlist description from the list as the ModListDescription object.
	ModListDescription& getModlistDescriptionByName(std::string name); ///< \brief A function returning a modlist description by modlist name.
	///< \param name Name of the modlist.
	///< \return A reference to ModListDescription object.
	void addModlistDescription(ModListDescription mld); ///< \brief A function adding new modlist description to the end of the list.
	///< \param mld A ModListDescription object.
	void deleteModlistDescription(std::string name); ///< \brief A function removing modlist description from the list.
	///< \param name Name of the modlist.
	void resetModlistDescriptionIterator(); ///< A function resetting modlist list iterator.
	void write(); ///< A function that writes back the changes to the file.
	bool modlistDescriptionsAtEnd(); ///< \brief A function returning if the modlist list iterator reached its end.
	///< \return True if the modlist list iterator reached its end, false otherwise.
};
}
#endif
