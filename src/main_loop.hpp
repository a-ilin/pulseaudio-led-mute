#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind/bind.hpp>

#include <functional>
#include <iostream>

#include "log.hpp"

class MainLoop
{
public:
    MainLoop()
        : mIoContext()
        , mWorkGuard(boost::asio::make_work_guard(mIoContext))
        , mSignals(mIoContext, SIGINT, SIGTERM)
        , mFatal(false)
    {
        mSignals.async_wait([this](auto, auto) { Stop(); });
    }

    void Post(const std::function<void()>& func)
    {
        // This method may be called from different threads
        boost::asio::post(mIoContext, boost::bind(&MainLoop::Execute, this, func));
    }

    bool Run()
    {
        mIoContext.run();
        return !mFatal;
    }

protected:
    void Execute(const std::function<void()>& func)
    {
        try {
            func();
        } catch (const RuntimeError& e) {
            Log::Error(e.Location()) << (e.Fatal() ? "FATAL: " : "") << e.what() << std::endl;
            if (e.Fatal()) {
                mFatal = true;
                Stop();
            }
        } catch (const std::exception& e) {
            Log::Error() << e.what() << std::endl;
        }
    }

    void Stop()
    {
        mSignals.cancel();
        mWorkGuard.reset();
    }

protected:
    boost::asio::io_context mIoContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> mWorkGuard;
    boost::asio::signal_set mSignals;
    bool mFatal;
};
