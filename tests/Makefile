CXXFLAGS=-Wall
all:
	${CXX} ${CXXFLAGS} -o ConfigFileTest ConfigFileTest.cpp ../ConfigFile.cpp ../3mExceptions.cpp
	${CXX} ${CXXFLAGS} -o ModDescriptionTest ModDescriptionTest.cpp ../ModDescription.cpp
debug:
	${CXX} ${CXXFLAGS} -o ConfigFileTest ConfigFileTest.cpp ../ConfigFile.cpp ../3mExceptions.cpp -g
	${CXX} ${CXXFLAGS} -o ModDescriptionTest ModDescriptionTest.cpp ../ModDescription.cpp -g
clean:
	rm -rf ConfigFileTest ModDescriptionTest
