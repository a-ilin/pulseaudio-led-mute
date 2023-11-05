#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <pulse/pulseaudio.h>

#include "main_loop.hpp"
#include "pa_callback_wrapper.hpp"
#include "pa_operation.hpp"
#include "pa_threaded_mainloop.hpp"

class IUpdater;

namespace PA {

struct ServerInfo {
    std::string mDefaultSinkName;
    std::string mDefaultSourceName;
};

class Context
{
public:
    Context(const std::shared_ptr<MainLoop>& mainLoop,
            const std::shared_ptr<ThreadedMainloop>& loop,
            const char* name);
    ~Context();

    void ConnectToServer();

    void SetUpdaters(std::vector<std::unique_ptr<IUpdater>>&& handlers);

    // Return server info
    ServerInfo GetServerInfo() const;

    // Return index and whether the sink is mute, or nullopt in case of no such sink found
    std::tuple<uint32_t, bool> GetSinkInfo(const std::string& name) const;

    // Return index and whether the source is mute, or nullopt in case of no such source found
    std::tuple<uint32_t, bool> GetSourceInfo(const std::string& name) const;

protected:
    void SubscribeToServerEvents();

    void OnStateChanged();
    void OnSubscriptionChanged(pa_subscription_event_type_t t, uint32_t idx);
    void OnServerChanged();

protected:
    std::shared_ptr<MainLoop> mMainLoop;
    std::shared_ptr<ThreadedMainloop> mLoop;
    pa_context* mCtx;

    std::vector<std::unique_ptr<IUpdater>> mUpdaters;

    using CBState = PA::CallbackWrapperVoid<1, pa_context*, void*>;
    CBState mCbState;

    using CBSubscribe = PA::CallbackWrapperVoid<3, pa_context*, pa_subscription_event_type_t, uint32_t, void*>;
    CBSubscribe mCbSubscribe;

private:
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
};

} // namespace PA
