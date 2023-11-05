#pragma once

#include <string>

enum class LedBrightness
{
    Off = 0,
    On = 1,
};

class Led
{
public:
    Led(const std::string& devicePath);

    void Switch(bool on);

private:
    std::string mDevicePath;
};
