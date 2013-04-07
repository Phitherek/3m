#ifndef _SYNCACTION_H
#define _SYNCACTION_H
#include "ConfigFile.h"
#include "3mExceptions.h"
#include "ModListList.h"
#include "ModListDescription.h"
#include "ModInfoDescription.h"
#include "ModDescription.h"
#include "LocalModDescription.h"
#include "ModList.h"
#include "ModInfo.h"
#include "NetSocket++/NetSocketPP.h"
#include "Action.h"
#include <string>
/// \file SyncAction.h
/// \brief A Sync 3m action class.
/// \author Phitherek_
/// \date 2013
/// \version 0.1

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class SyncAction
/// \brief A Sync 3m action class.
class SyncAction: public Action {
public:	
	SyncAction(); ///< A constructor.
	~SyncAction(); ///< A destructor.
	void run(); ///< A virtual function that executes the action.
};
}
#endif
