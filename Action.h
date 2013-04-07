#ifndef _ACTION_H
#define _ACTION_H
#include <vector>
#include <string>
/// \file Action.h
/// \brief A base class for 3m actions.
/// \author Phitherek_
/// \date 2013
/// \version 0.1-pre

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \class Action
/// \brief A base class for 3m actions.
class Action {
protected:
	std::vector<std::string> parameters; ///< Action parameters.
	int parametersIterator; ///< Parameter list iterator.
	bool parametersEndIndicator; ///< Indicates if the parameter list iterator has reached end of the list.
public:
	Action(); ///< A constructor.
	~Action(); ///< A destructor.
	void insertParameter(std::string param); ///< \brief A function that inserts parameter to action' s parameter list.
	///< \param param Parameter to insert.
	std::string getNextParameter(); ///< \brief A function that gets next parameter from the parameter list.
	///< \return Next parameter from the parameter list.
	void resetParametersIterator(); ///< A function that resets iterator of parameter list.
	bool parametersEnd(); ///< \brief A function that returns if parameter list iterator is at the end of the list.
	///< \return True if parameter list iterator is at the end of the list, false otherwise.
	virtual void run() {} ///< A virtual function that executes the action.
};
}
#endif
