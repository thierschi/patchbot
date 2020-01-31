#include "pb_exceptions.h"

map_format_exception::map_format_exception(const char* message_) :
	message(message_)
{
}

const char* map_format_exception::what() const throw()
{
	return message;
}
