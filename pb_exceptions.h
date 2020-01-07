#pragma once
#pragma once
#include <exception>

class map_format_exception : public std::exception {
private:
	const char* message;

public:
	explicit map_format_exception(const char* _message);
	virtual const char* what() const throw();

};