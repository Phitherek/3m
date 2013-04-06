CXXFLAGS=-Wall
all:
	${CXX} ${CXXFLAGS} -o ConfigFileTest ConfigFileTest.cpp ../ConfigFile.cpp ../3mExceptions.cpp
	${CXX} ${CXXFLAGS} -o ModDescriptionTest ModDescriptionTest.cpp ../ModDescription.cpp
	${CXX} ${CXXFLAGS} -o LocalModDescriptionTest LocalModDescriptionTest.cpp ../LocalModDescription.cpp ../ModDescription.cpp
	${CXX} ${CXXFLAGS} -o ModListListTest ModListListTest.cpp ../ModListList.cpp ../ModListDescription.cpp ../3mExceptions.cpp
	${CXX} ${CXXFLAGS} -o LocalModListTest LocalModListTest.cpp ../LocalModList.cpp ../LocalModDescription.cpp ../ModDescription.cpp ../3mExceptions.cpp ../ConfigFile.cpp
	${CXX} ${CXXFLAGS} -o RepositoryInfoTest RepositoryInfoTest.cpp ../RepositoryModDescription.cpp ../ModDescription.cpp ../3mExceptions.cpp ../ConfigFile.cpp ../RepositoryInfo.cpp
	${CXX} ${CXXFLAGS} -o ModInfoTest ModInfoTest.cpp ../ModInfo.cpp ../ModInfoDescription.cpp ../ModDescription.cpp ../3mExceptions.cpp -lnetsocketpp
debug:
	${CXX} ${CXXFLAGS} -o ConfigFileTest ConfigFileTest.cpp ../ConfigFile.cpp ../3mExceptions.cpp -g
	${CXX} ${CXXFLAGS} -o ModDescriptionTest ModDescriptionTest.cpp ../ModDescription.cpp -g
	${CXX} ${CXXFLAGS} -o LocalModDescriptionTest LocalModDescriptionTest.cpp ../LocalModDescription.cpp ../ModDescription.cpp
	${CXX} ${CXXFLAGS} -o ModListListTest ModListListTest.cpp ../ModListList.cpp ../ModListDescription.cpp ../3mExceptions.cpp -g
	${CXX} ${CXXFLAGS} -o LocalModListTest LocalModListTest.cpp ../LocalModList.cpp ../LocalModDescription.cpp ../ModDescription.cpp ../3mExceptions.cpp ../ConfigFile.cpp -g
	${CXX} ${CXXFLAGS} -o RepositoryInfoTest RepositoryInfoTest.cpp ../RepositoryModDescription.cpp ../ModDescription.cpp ../3mExceptions.cpp ../ConfigFile.cpp ../RepositoryInfo.cpp -g
	${CXX} ${CXXFLAGS} -o ModInfoTest ModInfoTest.cpp ../ModInfo.cpp ../ModInfoDescription.cpp ../ModDescription.cpp ../3mExceptions.cpp -lnetsocketpp -g
clean:
	rm -rf ConfigFileTest ModDescriptionTest LocalModDescriptionTest ModListListTest LocalModListTest RepositoryInfoTest ModInfoTest
