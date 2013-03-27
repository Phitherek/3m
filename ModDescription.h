#ifndef _MODDESCRIPTION_H
#define _MODDESCRIPTION_H
#include <string>
#include <vector>
/// \file ModDescription.h
/// \brief A class describing a mod.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m
namespace mmm {
/// \class ModDescription
/// \brief A class describing a mod.
class ModDescription {
protected:
	std::string _name; ///< A short name (ID) of the mod.
	std::string _description; ///< A short description of the mod.
	int _release; ///< Release of the 3m package.
	std::vector<std::string> _deps; ///< Mod dependency list.
	int _depsIterator; ///< Iterator for dependency list;
	bool _depsAtEnd; ///< A boolean indicating if depsIterator is at the end of the dependency list.
	std::string _repotype; ///< Mod repository type.
	std::string _repoaddr; ///< Mod repository address.
public:
	ModDescription(); ///< A constructor.
	~ModDescription(); ///< A destructor.
	std::string getName(); ///< \brief A function returning mod name.
	///< \return Mod name.
	std::string getDescription(); ///< \brief A function returning mod description.
	///< \return Mod description.
	int getReleaseNr(); ///< \brief A function returning 3m package release number.
	///< \return 3m package release number.
	std::string getNextDependency(); ///< \brief A function returning next dependency from the dependency list.
	///< \return Next dependency from the dependency list.
	std::string getRepositoryType(); ///< \brief A function returning mod repository type.
	///< \return Mod repository type.
	std::string getRepositoryAddress(); ///< \brief A function returning mod repository address.
	///< \return Mod repository address.
	void setName(std::string name); ///< \brief Function setting mod name.
	///< \param name Mod name.
	void setDescription(std::string desc); ///< \brief Function setting mod description.
	///< \param desc Mod description.
	void setReleaseNr(int release); ///< \brief Function setting 3m package release number.
	///< \param release 3m package release number.
	void insertDependency(std::string dep); ///< \brief Function inserting a dependency to the dependency list.
	/// \param dep A dependency.
	void setRepositoryType(std::string repotype); ///< \brief Function setting mod repository type.
	///< \param repotype Mod repository type.
	void setRepositoryAddress(std::string repoaddr); ///< \brief Function setting mod repository address.
	///< \param repoaddr Mod repository address.
	void resetDependencyIterator(); ///< A function resetting the iterator of the dependency list.
	bool dependenciesEnd(); ///< \brief A function returning if dependency list is at its end.
	///< \return True if dependency list iterator reached its end, false otherwise.
	bool dependenciesEmpty(); ///< \brief A function returning if dependency list is empty.
	///< \return True if dependency list is empty, false otherwise.
	void clear(); ///< A function that clears the object.
};
}
#endif
