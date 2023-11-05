#pragma once

#include <memory>
#include <string>

#include "led.hpp"
#include "pa_context.hpp"
#include "updater_named.hpp"

class UpdaterDefault : public UpdaterNamed
{
public:
    UpdaterDefault(const std::weak_ptr<PA::Context>& paContext, UpdaterType type, const Led& led);

    void OnServerConnected(const PA::ServerInfo& info) override;

protected:
    void UpdateAudioDeviceName(const std::string& name);
};
