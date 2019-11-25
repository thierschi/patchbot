#pragma once
#include <exception>
#include <string>

using namespace std;

/*
	Header file for custom exceptions
*/

class Map_format_exception : public exception {
private:
	string message;

public:
	explicit Map_format_exception(const string& m);
	virtual const char* what() const throw();
	
};

