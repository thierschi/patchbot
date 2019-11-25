#include "Pb_exceptions.h"

Map_format_exception::Map_format_exception(const string& m)
{
	message = m;
}

const char* Map_format_exception::what() const throw()
{
	return message.c_str();
}
