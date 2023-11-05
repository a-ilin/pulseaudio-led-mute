#include <fstream>
#include <iostream>

#include "led.hpp"
#include "log.hpp"

Led::Led(const std::string& devicePath)
    : mDevicePath(devicePath)
{
}

void Led::Switch(bool on)
{
    int maxBrightness = (int)LedBrightness::On;

    // Attempt to get maximum brightness
    try {
        std::ifstream maxBrightnessFile(mDevicePath + "/max_brightness");
        while (maxBrightnessFile >> maxBrightness) {
            break;
        }
    } catch (const std::exception& e) {
        Log::Error() << "Cannot get max_brightness for LED " << mDevicePath << ": " << e.what() << std::endl;
    }

    std::ofstream brightnessFile(mDevicePath + "/brightness");
    if (on) {
        brightnessFile << maxBrightness << std::endl;
    } else {
        brightnessFile << (int)LedBrightness::Off << std::endl;
    }
}
