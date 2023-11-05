#include "updater_named.hpp"

UpdaterNamed::UpdaterNamed(const std::weak_ptr<PA::Context>& paContext,
                           UpdaterType type,
                           const std::string& audioDeviceName,
                           const Led& led)
    : mPaContext(paContext)
    , mType(type)
    , mAudioDeviceName(audioDeviceName)
    , mLed(led)
{
}

void UpdaterNamed::OnServerConnected(const PA::ServerInfo&)
{
    UpdateAudioInfo();
}
void UpdaterNamed::OnDeviceChanged(UpdaterType type, uint32_t index)
{
    if (type == mType && index == mAudioDeviceIndex) {
        UpdateAudioInfo();
    }
}

void UpdaterNamed::UpdateAudioInfo()
{
    std::tuple<uint32_t, bool> deviceInfo;
    switch (mType) {
        case UpdaterType::Sink:
            if (const auto& ctx = mPaContext.lock()) {
                deviceInfo = ctx->GetSinkInfo(mAudioDeviceName);
            }
            break;
        case UpdaterType::Source:
            if (const auto& ctx = mPaContext.lock()) {
                deviceInfo = ctx->GetSourceInfo(mAudioDeviceName);
            }
            break;
        default:
            throw RuntimeError(std::format("Unknown updater type: {}", (int)mType));
    }

    mAudioDeviceIndex = std::get<0>(deviceInfo);
    auto isMute = std::get<1>(deviceInfo);

    mLed.Switch(isMute);
}
