#include <iostream>
#include <cstdlib>
#include "../LocalModDescription.h"
using namespace std;

int main() {
mmm::LocalModDescription md;
md.setName("test");
md.setDescription("Test ModDescription entry");
md.setReleaseNr(1);
md.setRemoteModlistName("test");
md.setRepositoryType("archive");
md.setRepositoryAddress("http://example.tld/mod.zip");
md.insertDependency("test2");
md.insertDependency("test3");
cout << "Remote modlist name: " << md.getRemoteModlistName() << endl << "Name: " << md.getName() << endl << "Description: " << md.getDescription() << endl << "Release number: " << md.getReleaseNr() << endl << "Depends on: ";
while(!md.dependenciesEnd()) {
	cout << md.getNextDependency() << " ";
}
cout << endl << "Dependencies again: ";
md.resetDependencyIterator();
while(!md.dependenciesEnd()) {
	cout << md.getNextDependency() << " ";
}
cout << endl << "Repository type: " << md.getRepositoryType() << endl << "Repository address: " << md.getRepositoryAddress() << endl;
return EXIT_SUCCESS;
}
