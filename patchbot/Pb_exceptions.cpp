#include "pb_exceptions.h"

Map_format_exception::Map_format_exception(const char* _message)
{
	message = _message;
}

const char* Map_format_exception::what() const throw()
{
	return message;
}
