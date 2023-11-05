#include <boost/filesystem.hpp>

#include "log.hpp"

std::ostream& Log::Info(const std::source_location& location)
{
    return FormatLocation(std::cout, location);
}

std::ostream& Log::Error(const std::source_location& location)
{
    return FormatLocation(std::cerr, location);
}

std::ostream& Log::FormatLocation(std::ostream& s, const std::source_location& location)
{
    namespace bf = boost::filesystem;

    return s << bf::path(location.file_name()).filename().string() << ':' << location.line() << " ["
             << location.function_name() << "]: ";
}

RuntimeError::RuntimeError(const std::string& what, bool fatal, const std::source_location& location)
    : std::runtime_error(what)
    , mLocation(location)
    , mFatal(fatal)
{
}

RuntimeError::RuntimeError(const char* what, bool fatal, const std::source_location& location)
    : std::runtime_error(what)
    , mLocation(location)
    , mFatal(fatal)
{
}
