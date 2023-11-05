#pragma once

#include <stdint.h>

#include "pa_context.hpp"

class IUpdater
{
public:
    enum class UpdaterType
    {
        Sink,
        Source,
    };

public:
    virtual ~IUpdater() = default;
    virtual void OnServerConnected(const PA::ServerInfo& info) = 0;
    virtual void OnDeviceChanged(UpdaterType type, uint32_t index) = 0;
};
