#include "pa_threaded_mainloop.hpp"

#include "log.hpp"

namespace PA {
ThreadedMainloop::ThreadedMainloop()
    : mLoop(pa_threaded_mainloop_new())
{
    if (!mLoop) {
        throw RuntimeError("Failed pa_threaded_mainloop_new");
    }

    if (int status = pa_threaded_mainloop_start(mLoop); status != 0) {
        pa_threaded_mainloop_free(mLoop);

        throw RuntimeError(std::format("Non-zero code in pa_threaded_mainloop_start: {}", status));
    }
}

ThreadedMainloop::~ThreadedMainloop()
{
    // This call must not be locked.
    pa_threaded_mainloop_stop(mLoop);
    pa_threaded_mainloop_free(mLoop);
}

pa_mainloop_api* ThreadedMainloop::Api() const
{
    pa_mainloop_api* mlapi = pa_threaded_mainloop_get_api(mLoop);
    if (!mlapi) {
        throw RuntimeError("Failed pa_threaded_mainloop_get_api");
    }
    return mlapi;
}
} // namespace PA
