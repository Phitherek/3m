#include "3mExceptions.h"
using namespace mmm;

FileException::FileException(std::string filename, std::string mode, std::string err) {
	_filename = filename;
	_mode = mode;
	_err = err;
}

FileException::~FileException() throw() {
	_filename = "";
	_mode = "";
	_err = "";	
}

const char* FileException::what() const throw() {
	std::string msg = "";
	msg += _filename;
	msg += " (";
	msg += _mode;
	msg += "): ";
	msg += _err;
	return msg.c_str();	
}

ParseException::ParseException(std::string filename, std::string err) {
	_filename = filename;
	_err = err;
}

ParseException::~ParseException() throw() {
	_filename = "";
	_err = "";	
}

const char* ParseException::what() const throw() {
	std::string msg = "";
	msg += _filename;
	msg += ": ";
	msg += _err;
	return msg.c_str();	
}

OutOfBoundsException::OutOfBoundsException(std::string what) {
	_what = what;
}

OutOfBoundsException::~OutOfBoundsException() throw() {
	_what = "";
}

const char* OutOfBoundsException::what() const throw() {
	return _what.c_str();
}
