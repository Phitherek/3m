#ifndef _3M_EXCEPTIONS_H
#define _3M_EXCEPTIONS_H
#include <string>
#include <exception>
/// \file 3mExceptions.h
/// \brief A header file for all the exception classes for 3m
/// \author Phitherek_
/// \date 2013
/// \version 0.1

/// \namespace mmm
/// \brief A global namespace for 3m
namespace mmm {
/// \class FileException
/// \brief An exception to be thrown when file operation fails.
class FileException: public std::exception {
private:
	std::string _filename;
	std::string _mode;
	std::string _err;
public:
	FileException(std::string filename, std::string mode, std::string err); ///< \brief A constructor with parameters
	///< \param filename Name of the file that caused an error.
	///< \param mode A mode (reading/writing etc.) that the file was opened in.
	///< \param err Error message.
	~FileException() throw(); /// \brief A destructor, as needed by std::exception.
	const char* what() const throw(); ///< \brief A function, that returns the whole error.
	///< \return Whole error message.
};
/// \class ParseException
/// \brief An exception to be throw when parse error occurs.
class ParseException {
private:
	std::string _filename;
	std::string _err;
public:
	ParseException(std::string filename, std::string err); ///< \brief A constructor with parameters
	///< \param filename Name of the file that caused an error.
	///< \param err Error message.
	~ParseException() throw(); /// \brief A destructor, as needed by std::exception.
	const char* what() const throw(); ///< \brief A function, that returns the whole error.
	///< \return Whole error message.
};
}
#endif
