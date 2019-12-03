#include "pb_exceptions.h"

map_format_exception::map_format_exception(const char* _message) :
	message(_message)
{
}

const char* map_format_exception::what() const throw()
{
	return message;
}
