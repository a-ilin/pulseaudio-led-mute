#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <yaml-cpp/yaml.h>

#include "led.hpp"
#include "log.hpp"
#include "main_loop.hpp"
#include "pa_context.hpp"
#include "pa_threaded_mainloop.hpp"
#include "updater_default.hpp"
#include "updater_named.hpp"

struct Args
{
    std::string configFileName;
};

namespace po = boost::program_options;

static bool ParseArgs(Args& args, int argc, char* argv[])
{
    po::options_description desc("pulseaudio-led-mute - synchronizes mute status of PulseAudio with LED indicators\n\n"
                                 "Usage");

    // clang-format off
    desc.add_options()
        ("help", "produce help message")
        ("config", po::value<std::string>(), "path to config file");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return false;
    }

    try {
        if (vm.count("config")) {
            args.configFileName = vm["config"].as<std::string>();
        } else {
            args.configFileName = CONFIG_FILE;
        }
    } catch (const std::exception&) {
        Log::Error() << desc << std::endl;
        return false;
    }

    return true;
}

static std::vector<std::unique_ptr<IUpdater>> CreateUpdaters(const YAML::Node& devices,
                                                             const std::weak_ptr<PA::Context>& paContext)
{
    std::vector<std::unique_ptr<IUpdater>> updaters;
    updaters.reserve(devices.size());

    for (YAML::const_iterator it = devices.begin(); it != devices.end(); ++it) {
        YAML::Node node = *it;

        try {
            // audio type
            IUpdater::UpdaterType updaterType;
            std::string type;
            try {
                type = node["audio-type"].as<std::string>();
            } catch (const std::exception& e) {
                throw RuntimeError(std::format("Field 'audio-type': {}", e.what()), true);
            }

            if (type == "sink") {
                updaterType = IUpdater::UpdaterType::Sink;
            } else if (type == "source") {
                updaterType = IUpdater::UpdaterType::Source;
            } else {
                throw RuntimeError(std::format("Wrong audio device type: {}", type), true);
            }

            // audio default
            bool isDefault = false;
            try {
                isDefault = node["audio-default"].as<bool>(bool{});
            } catch (const YAML::KeyNotFound&) {
            } catch (const std::exception& e) {
                throw RuntimeError(std::format("Field 'audio-default': {}", e.what()), true);
            }

            // audio device
            std::string audioDevice;
            try {
                audioDevice = node["audio-device"].as<std::string>(std::string{});
            } catch (const YAML::KeyNotFound&) {
            } catch (const std::exception& e) {
                throw RuntimeError(std::format("Field 'audio-device': {}", e.what()), true);
            }

            // sanity check
            if (isDefault && !audioDevice.empty()) {
                throw RuntimeError("Name for default audio device is not supported", true);
            }

            if (!isDefault && audioDevice.empty()) {
                throw RuntimeError("Audio device must be default or name provided", true);
            }

            // LED device
            std::string ledDevicePath;
            try {
                ledDevicePath = node["led-device"].as<std::string>();
            } catch (const std::exception& e) {
                throw RuntimeError(std::format("Field 'led-device': {}", e.what()), true);
            }
            Led ledDevice(ledDevicePath);

            // add to list of devices
            std::unique_ptr<IUpdater> updater;
            if (isDefault) {
                updater = std::make_unique<UpdaterDefault>(paContext, updaterType, ledDevice);
            } else {
                updater = std::make_unique<UpdaterNamed>(paContext, updaterType, audioDevice, ledDevice);
            }
            updaters.push_back(std::move(updater));
        } catch (const std::exception& e) {
            throw RuntimeError(
              std::format("Device #{}/{}: {}", std::distance(devices.begin(), it) + 1, devices.size(), e.what()), true);
        }
    }

    return updaters;
}

int main(int argc, char* argv[])
try {
    Args args;
    if (!ParseArgs(args, argc, argv)) {
        // no need to print message here
        return 1;
    }

    YAML::Node config;
    try {
        config = YAML::LoadFile(args.configFileName);
        if (config.IsNull()) {
            throw RuntimeError("Config file is empty");
        }
    } catch (const std::exception& e) {
        throw RuntimeError(std::format("Cannot load config: {}", e.what()), true);
    }

    auto mainLoop = std::make_shared<MainLoop>();
    auto paLoop = std::make_shared<PA::ThreadedMainloop>();
    auto paContext = std::make_shared<PA::Context>(mainLoop, paLoop, "pulseaudio-led-mute");

    std::vector<std::unique_ptr<IUpdater>> updaters = CreateUpdaters(config["devices"], paContext);
    paContext->SetUpdaters(std::move(updaters));

    mainLoop->Post([&paContext]() { paContext->ConnectToServer(); });

    return mainLoop->Run() ? 0 : -1;
} catch (const std::exception& e) {
    Log::Error() << e.what() << std::endl;
    return -2;
}
