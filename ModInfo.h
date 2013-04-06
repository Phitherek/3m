#ifndef _MODINFO_H
#define _MODINFO_H
#include "3m.h"
#include <string>
/// \file ModInfo.h
/// \brief A class that represents remote modinfo file.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class ModInfo
/// \brief A class that represents remote modinfo file
class ModInfo: public ModDescription {
private:
ModInfoDescription _desc;
bool _edit;
std::string _localPath;
public:
ModInfo(); ///< A constructor.
ModInfo(ModInfoDescription mid); ///< \brief A constructor from ModInfoDescription object.
///< Tries to download and parse remote modinfo file.
///< \param mid A ModInfoDescription object.
ModInfo(std::string path); ///< \brief A construtor from the path to local modinfo file.
///< It tries to open and parse local modinfo file, opening it for editing.
///< \param path A path to local modinfo file.
void setPath(std::string path); ///< \brief A function that sets local modinfo file to edit.
///< \param path A path to local modinfo file.
void setModInfoDescription(ModInfoDescription mid); ///< \brief A function that sets ModInfoDescription in the object.
///< \param mid A ModInfoDescription object.
void write(); ///< A function that writes the changes to local modinfo file.
void releaseInc(); ///< A function that increases modinfo release.
~ModInfo(); ///< A destructor.
void clear(); ///< A function that clears the object.
};
}
#endif
