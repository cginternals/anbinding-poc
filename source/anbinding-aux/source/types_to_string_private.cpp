
#include "types_to_string_private.h"

#include <sstream>


namespace anbinding { namespace aux
{


std::string wrapString(const char * value)
{
    std::stringstream ss;
    ss << "\"";
    ss << (value == nullptr ? "nullptr" : value);
    ss << "\"";
    return ss.str();
}


} } // namespace anbinding::aux
