#ifndef _QUERYACTION_H
#define _QUERYACTION_H
#include "ConfigFile.h"
#include "3mExceptions.h"
#include "ModDescription.h"
#include "LocalModDescription.h"
#include "LocalModList.h"
#include "Action.h"
#include <string>
/// \file QueryAction.h
/// \brief A Query 3m action class.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class QueryAction
/// \brief A Query 3m action class.
class QueryAction: public Action {
private:
bool _local;
public:
	QueryAction(); ///< A constructor.
	QueryAction(bool local); ///< \brief A constructor with parameter.
	///< \param local Indicated if to search in local repository or remote repositories.
	~QueryAction(); ///< A destructor.
	void run(); ///< A virtual function that executes the action.
};
}
#endif
