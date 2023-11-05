#pragma once

#include <format>
#include <memory>
#include <stdexcept>

#include <pulse/pulseaudio.h>

namespace PA {
class ThreadedMainloop
{
public:
    class Locker;

public:
    ThreadedMainloop();
    ~ThreadedMainloop();

    pa_mainloop_api* Api() const;

public:
    pa_threaded_mainloop* mLoop;

private:
    ThreadedMainloop(const ThreadedMainloop&) = delete;
    ThreadedMainloop& operator=(const ThreadedMainloop&) = delete;
};

class ThreadedMainloop::Locker
{
public:
    Locker(const std::shared_ptr<ThreadedMainloop>& loop)
        : mLoop(loop)
        , mNeedLock(pa_threaded_mainloop_in_thread(mLoop->mLoop) == 0)
    {
        if (mNeedLock) {
            pa_threaded_mainloop_lock(mLoop->mLoop);
        }
    }

    ~Locker()
    {
        if (mNeedLock) {
            pa_threaded_mainloop_unlock(mLoop->mLoop);
        }
    }

    bool Locked() const { return mNeedLock; }

protected:
    std::shared_ptr<ThreadedMainloop> mLoop;
    bool mNeedLock;
};

} // namespace PA
