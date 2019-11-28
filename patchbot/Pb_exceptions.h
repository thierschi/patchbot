#pragma once
#include <exception>

class Map_format_exception : public std::exception {
private:
	const char* message;

public:
	explicit Map_format_exception(const char* _message);
	virtual const char* what() const throw();

};