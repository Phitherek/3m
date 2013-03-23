#ifndef _LOCALMODDESCRIPTION_H
#define _LOCALMODDESCRIPTION_H
#include "ModDescription.h"
/// \file LocalModDescription.h
/// \brief A class describing a mod in a local modlist.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m
namespace mmm {
/// \class LocalModDescription
/// \brief A class describing a mod in a local modlist.
class LocalModDescription: public ModDescription {
private:
std::string _remoteModlist;
public:
LocalModDescription(); ///< A constructor.
~LocalModDescription(); ///< A destructor.
std::string getRemoteModlistName(); ///< \brief A function returning remote modlist name.
///< \return Remote modlist name.
void setRemoteModlistName(std::string remoteModlist); ///< \brief A function setting remote modlist name.
///< \param remoteModlist Remote modlist name
};
}
#endif
