#include <future>
#include <iostream>

#include "iupdater.hpp"
#include "log.hpp"
#include "pa_context.hpp"

namespace PA {

Context::Context(const std::shared_ptr<MainLoop>& mainLoop,
                 const std::shared_ptr<ThreadedMainloop>& loop,
                 const char* name)
    : mMainLoop(mainLoop)
    , mLoop(loop)
    , mCbState(CBState([this](pa_context*, void*) { OnStateChanged(); }))
    , mCbSubscribe(CBSubscribe(
        [this](pa_context*, pa_subscription_event_type_t t, uint32_t idx, void*) { OnSubscriptionChanged(t, idx); }))
{
    { // Create context
        PA::ThreadedMainloop::Locker loopLocker(mLoop);
        pa_mainloop_api* api = mLoop->Api();
        mCtx = pa_context_new(api, name);
        if (!mCtx) {
            throw RuntimeError("Failed pa_context_new");
        }
    }

    { // Subscribe to context connection state changes
        PA::ThreadedMainloop::Locker loopLocker(mLoop);
        pa_context_set_state_callback(mCtx, &CBState::StaticFunc, &mCbState);
    }
}

Context::~Context()
{
    PA::ThreadedMainloop::Locker loopLocker(mLoop);
    pa_context_set_state_callback(mCtx, nullptr, nullptr);
    pa_context_set_subscribe_callback(mCtx, nullptr, nullptr);
    pa_context_disconnect(mCtx);
    pa_context_unref(mCtx);
}

void Context::ConnectToServer()
{
    PA::ThreadedMainloop::Locker locker(mLoop);
    assert(locker.Locked()); // must not be called from PulseAudio thread

    Log::Info() << "Connecting to server..." << std::endl;

    int status = pa_context_connect(mCtx, NULL, pa_context_flags_t(PA_CONTEXT_NOAUTOSPAWN | PA_CONTEXT_NOFAIL), NULL);
    if (status != 0) {
        // Errors reported by pa_context_connect are fatal
        throw RuntimeError(
          std::format("Failed to connect context to server: error {}: {}", status, pa_strerror(pa_context_errno(mCtx))),
          true);
    }
}

void Context::SetUpdaters(std::vector<std::unique_ptr<IUpdater>>&& updaters)
{
    mUpdaters.swap(updaters);
}

void Context::SubscribeToServerEvents()
{
    std::promise<void> p;
    auto f = p.get_future();
    Operation op;

    using CBSuccess = PA::CallbackWrapperVoid<2, pa_context*, int, void*>;
    auto cbSuccess = CBSuccess([&p, &op](pa_context* c, int success, void*) {
        op.Cancel();
        if (0 == success) {
            std::string err = std::format("Failed subscribe to context: success: {}", pa_strerror(pa_context_errno(c)));
            p.set_exception(std::make_exception_ptr(RuntimeError(err)));
        } else {
            p.set_value();
        }
    });

    {
        PA::ThreadedMainloop::Locker locker(mLoop);
        pa_context_set_subscribe_callback(mCtx, CBSubscribe::StaticFunc, &mCbSubscribe);

        assert(locker.Locked()); // must not be called from PulseAudio thread

        // clang-format off
        auto mask = pa_subscription_mask_t(
            PA_SUBSCRIPTION_MASK_SINK
            | PA_SUBSCRIPTION_MASK_SOURCE
            | PA_SUBSCRIPTION_MASK_SERVER   // events related to new audio configuration
        );
        // clang-format on

        op = Operation(pa_context_subscribe(mCtx, mask, CBSuccess::StaticFunc, &cbSuccess));
    }

    return f.get();
}

ServerInfo Context::GetServerInfo() const
{
    std::promise<ServerInfo> p;
    auto f = p.get_future();
    Operation op;

    using CBGetServerInfo = PA::CallbackWrapperVoid<2, pa_context*, const pa_server_info*, void*>;
    auto cbServerInfo = CBGetServerInfo([&p, &op](pa_context* c, const pa_server_info* info, void*) {
        if (info) {
            op.Cancel();
            p.set_value(ServerInfo{
              .mDefaultSinkName{ info->default_sink_name },
              .mDefaultSourceName{ info->default_source_name },
            });
        } else {
            op.Cancel();
            std::string err = std::format("Failed to get server information: {}", pa_strerror(pa_context_errno(c)));
            p.set_exception(std::make_exception_ptr(RuntimeError(err)));
        }
    });

    {
        PA::ThreadedMainloop::Locker locker(mLoop);
        assert(locker.Locked()); // must not be called from PulseAudio thread?
        op = Operation(pa_context_get_server_info(mCtx, CBGetServerInfo::StaticFunc, &cbServerInfo));
    }

    return f.get();
}

std::tuple<uint32_t, bool> Context::GetSinkInfo(const std::string& name) const
{
    std::promise<std::tuple<uint32_t, bool>> p;
    auto f = p.get_future();
    Operation op;

    using CBSinkInfo = PA::CallbackWrapperVoid<3, pa_context*, const pa_sink_info*, int, void*>;
    auto cbSinkInfo = CBSinkInfo([&p, &op, &name](pa_context* c, const pa_sink_info* info, int eol, void*) {
        if (!eol) {
            if (info) {
                if (std::string_view(info->name) == name) {
                    op.Cancel();
                    p.set_value(std::make_tuple(info->index, info->mute != 0));
                }
            } else {
                op.Cancel();
                std::string err = std::format("Failed to get sink information: {}", pa_strerror(pa_context_errno(c)));
                p.set_exception(std::make_exception_ptr(RuntimeError(err)));
            }
        } else {
            // no more sinks
            op.Cancel();
            std::string err = std::format("No sinks available. Context status: {}", pa_strerror(pa_context_errno(c)));
            p.set_exception(std::make_exception_ptr(RuntimeError(err)));
        }
    });

    {
        PA::ThreadedMainloop::Locker locker(mLoop);
        assert(locker.Locked()); // must not be called from PulseAudio thread
        op = Operation(pa_context_get_sink_info_by_name(mCtx, name.c_str(), CBSinkInfo::StaticFunc, &cbSinkInfo));
    }

    return f.get();
}

std::tuple<uint32_t, bool> Context::GetSourceInfo(const std::string& name) const
{
    std::promise<std::tuple<uint32_t, bool>> p;
    auto f = p.get_future();
    Operation op;

    using CBSourceInfo = PA::CallbackWrapperVoid<3, pa_context*, const pa_source_info*, int, void*>;
    auto cbSourceInfo = CBSourceInfo([&p, &op, &name](pa_context* c, const pa_source_info* info, int eol, void*) {
        if (!eol) {
            if (info) {
                if (std::string_view(info->name) == name) {
                    op.Cancel();
                    p.set_value(std::make_tuple(info->index, info->mute != 0));
                }
            } else {
                op.Cancel();
                std::string err = std::format("Failed to get source information: {}", pa_strerror(pa_context_errno(c)));
                p.set_exception(std::make_exception_ptr(RuntimeError(err)));
            }
        } else {
            // no more sinks
            op.Cancel();
            std::string err = std::format("No sources available. Context status: {}", pa_strerror(pa_context_errno(c)));
            p.set_exception(std::make_exception_ptr(RuntimeError(err)));
        }
    });

    {
        PA::ThreadedMainloop::Locker locker(mLoop);
        assert(locker.Locked()); // must not be called from PulseAudio thread
        op = Operation(pa_context_get_source_info_by_name(mCtx, name.c_str(), CBSourceInfo::StaticFunc, &cbSourceInfo));
    }

    return f.get();
}

void Context::OnStateChanged()
{
    // This method is called from PA thread

    pa_context_state_t state = pa_context_get_state(mCtx);

    if (state == PA_CONTEXT_READY) {
        Log::Info() << "Connected to server" << std::endl;
        mMainLoop->Post(boost::bind(&Context::SubscribeToServerEvents, this));
        OnServerChanged();

    } else if (state == PA_CONTEXT_FAILED) {
        // It is not possible to reconnect existing context. The service must be restarted.
        mMainLoop->Post([this]() {
            throw RuntimeError(
              std::format("Connection to server failed. Error: {}", pa_strerror(pa_context_errno(mCtx))), true);
        });
    }
}

void Context::OnSubscriptionChanged(pa_subscription_event_type_t t, uint32_t idx)
{
    // This method is called from PA thread

    if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
        switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
            case PA_SUBSCRIPTION_EVENT_SERVER:
                OnServerChanged();
                break;
            case PA_SUBSCRIPTION_EVENT_SINK:
                mMainLoop->Post([this, idx]() {
                    for (auto& updater : mUpdaters) {
                        updater->OnDeviceChanged(IUpdater::UpdaterType::Sink, idx);
                    }
                });
                break;
            case PA_SUBSCRIPTION_EVENT_SOURCE:
                mMainLoop->Post([this, idx]() {
                    for (auto& updater : mUpdaters) {
                        updater->OnDeviceChanged(IUpdater::UpdaterType::Source, idx);
                    }
                });
                break;
            default:
                break;
        }
    }
}

void Context::OnServerChanged()
{
    // This method is called from PA thread

    mMainLoop->Post([this]() {
        ServerInfo info = GetServerInfo();
        for (auto& updater : mUpdaters) {
            mMainLoop->Post(std::bind(&IUpdater::OnServerConnected, &(*updater), info));
        }
    });
}

} // namespace PA
