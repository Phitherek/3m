#ifndef _3M_H
#define _3M_H
#include <string>
#include <vector>
#include "ConfigFile.h"
#include "3mExceptions.h"
#include "LocalModDescription.h"
#include "ModDescription.h"
#include "ModInfoDescription.h"
#include "ModListDescription.h"
#include "ModListList.h"
#include "RepositoryModDescription.h"
#include "LocalModList.h"
#include "RepositoryModDescription.h"
#include "NetSocket++/NetSocketPP.h"
/// \file 3m.h
/// \brief A common header for 3m and definitions of helper functions.
/// \author Phitherek_
/// \date 2013
/// \version 0.1

/// \namespace mmm
/// \brief A global namespace for 3m.
namespace mmm {
/// \fn inline std::string strip_endl(std::string s)
/// \brief A function that strips endline signs from the string.
/// \param s A string to strip endline signs from.
/// \return Stripped string.
inline std::string strip_endl(std::string s) {
	int len = s.length();
	for(int i = 0; i < len; i++) {
		if(s[i] == '\n') {
			s.erase(i,1);
			len = s.length();
			i--;
		}
	}
	return s;
}

/// \fn inline std::vector<std::string> strtovec(std::string *str)
/// \brief A function that converts a multiline string to a vector of strings.
/// \param str A string to convert to vector.
/// \return Vector of strings, each one a line from original string
inline std::vector<std::string>& strtovec(std::string str) {
std::string line = "";
static std::vector<std::string> vec;
vec.clear();
for(unsigned int i = 0; i < str.length(); i++) {
	if(str[i] == '\n' || str[i] == '\r') {
		if(line != "") {
			vec.push_back(line);
			line = "";
		}
	} else {
		line += str[i];
	}
}
return vec;
}
}
#endif
