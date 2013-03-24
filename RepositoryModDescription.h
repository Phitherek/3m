#ifndef _REPOSITORYMODDESCRIPTION_H
#define _REPOSITORYMODDESCRIPTION_H
#include "ModDescription.h"
/// \file RepositoryModDescription.h
/// \brief A class describing a mod in the local repository.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class RepositoryModDescription
/// \brief A class describing a mod in the local repository.
class RepositoryModDescription: public ModDescription {
private:
	std::string _path;
public:
	RepositoryModDescription(); ///< A constructor.
	~RepositoryModDescription(); ///< A destructor.
	std::string getPath(); ///< \brief A function returning installation path of the mod.
	///< \return Installation path of the mod.
	void setPath(std::string path); ///< \brief A function that sets installation path of the mod.
	///< \param path Installation path of the mod.
};
}
#endif
