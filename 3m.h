#ifndef _3M_H
#define _3M_H
#include <string>
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
/// \fn std::string strip_endl(std::string s)
/// \brief A function that strips endline signs from the string.
/// \param s A string to strip endline signs from.
/// \return Stripped string.
std::string strip_endl(std::string s) {
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

/// \fn std::string strgetline(std::string *str)
/// \brief A function that gets line from the multiline string and erases it from this string.
/// \param[in,out] str A pointer to the string to get line from. The line is erased from the string.
/// \return Line from the string.
std::string strgetline(std::string *str) {
std::string line = "";
int i;
for(i = 0; (*str)[i] != '\n'; i++) {
line += (*str)[i];
}
return line;
}
}
#endif
