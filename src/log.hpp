#pragma once

#include <exception>
#include <iostream>
#include <source_location>
#include <string>

class Log
{
public:
    static std::ostream& Info(const std::source_location& location = std::source_location::current());
    static std::ostream& Error(const std::source_location& location = std::source_location::current());

protected:
    static std::ostream& FormatLocation(std::ostream& s, const std::source_location& location);
};

class RuntimeError : public std::runtime_error
{
public:
    RuntimeError(const std::string& what,
                 bool fatal = false,
                 const std::source_location& location = std::source_location::current());
    RuntimeError(const char* what,
                 bool fatal = false,
                 const std::source_location& location = std::source_location::current());

    const std::source_location& Location() const { return mLocation; }
    bool Fatal() const { return mFatal; }

protected:
    std::source_location mLocation;
    bool mFatal;
};
