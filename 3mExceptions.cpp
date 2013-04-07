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

BadResponseException::BadResponseException(std::string response) {
	_response = response;
}

BadResponseException::~BadResponseException() throw() {
	_response = "";
}

const char* BadResponseException::what() const throw() {
	std::string what = "Got HTTP response: ";
	what += _response;
	return what.c_str();
}

NonEditableException::NonEditableException(std::string what) {
	_what = what;
}

NonEditableException::~NonEditableException() throw() {
	_what = "";
}

const char* NonEditableException::what() const throw() {
	return _what.c_str();
}

BadParameterException::BadParameterException(std::string parameter) {
	_action = "global";
	_parameter = parameter;
}

BadParameterException::BadParameterException(std::string action, std::string parameter) {
	_action = action;
	_parameter = parameter;
}

BadParameterException::~BadParameterException() throw() {
	_action = "";
	_parameter = "";
}

const char* BadParameterException::what() const throw() {
	std::string swhat = "";
	swhat += "(";
	swhat += _action;
	swhat += ") Invalid parameter: ";
	swhat += _parameter;
	return swhat.c_str();
}
