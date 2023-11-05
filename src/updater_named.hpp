#pragma once

#include <memory>
#include <stdint.h>
#include <string>

#include "iupdater.hpp"
#include "led.hpp"
#include "pa_context.hpp"

class UpdaterNamed : public IUpdater
{
public:
    UpdaterNamed(const std::weak_ptr<PA::Context>& paContext,
                 UpdaterType type,
                 const std::string& audioDeviceName,
                 const Led& led);

    void OnServerConnected(const PA::ServerInfo& info) override;
    void OnDeviceChanged(UpdaterType type, uint32_t index) override;

protected:
    void UpdateAudioInfo();

protected:
    std::weak_ptr<PA::Context> mPaContext;
    UpdaterType mType;
    std::string mAudioDeviceName;
    Led mLed;
    uint32_t mAudioDeviceIndex;
};
