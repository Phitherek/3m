#include "Action.h"
using namespace mmm;

Action::Action() {
	parameters.clear();
	parametersIterator = -1;
	parametersEndIndicator = true;
}

Action::~Action() {
	parameters.clear();
	parametersIterator = -1;
	parametersEndIndicator = true;
}

void Action::insertParameter(std::string param) {
	parameters.push_back(param);
	if(parametersEndIndicator) {
		parametersEndIndicator = false;
	}
}

std::string Action::getNextParameter() {
	if(parametersIterator+1 < parameters.size()) {
		parametersIterator++;
		return parameters[parametersIterator];
	} else {
		parametersEndIndicator = true;
		return "";
	}
}

void Action::resetParametersIterator() {
	parametersIterator = -1;
	if(parametersEndIndicator) {
		parametersEndIndicator = false;
	}
}

bool Action::parametersEnd() {
	return parametersEndIndicator;
}
