#include "QueryAction.h"
#include <iostream>
#include <cstdlib>
using namespace mmm;

QueryAction::QueryAction(): Action() {
	_local = false;
}

QueryAction::QueryAction(bool local): Action() {
	_local = local;
}

QueryAction::~QueryAction() {}

void QueryAction::run() {
	std::cout << "Starting Query" << std::endl;
	if(_local) {
		
	} else {
		
	}
}
