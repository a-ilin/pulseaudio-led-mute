#include "updater_default.hpp"

UpdaterDefault::UpdaterDefault(const std::weak_ptr<PA::Context>& paContext, UpdaterType type, const Led& led)
    : UpdaterNamed(paContext, type, std::string{}, led)
{
}

void UpdaterDefault::OnServerConnected(const PA::ServerInfo& info)
{
    switch (mType) {
        case UpdaterType::Sink:
            UpdateAudioDeviceName(info.mDefaultSinkName);
            break;
        case UpdaterType::Source:
            UpdateAudioDeviceName(info.mDefaultSourceName);
            break;
        default:
            throw RuntimeError(std::format("Unknown updater type: {}", (int)mType));
    }
}

void UpdaterDefault::UpdateAudioDeviceName(const std::string& audioDeviceName)
{
    if (audioDeviceName != mAudioDeviceName) {
        mAudioDeviceName = audioDeviceName;
        UpdateAudioInfo();
    }
}
