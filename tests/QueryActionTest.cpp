#include "../QueryAction.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char** argv) {
	mmm::QueryAction act;
	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
			std::string arg = "";
			arg += argv[i];
			act.insertParameter(arg);
		}
	}
	act.run();
	return EXIT_SUCCESS;
}
