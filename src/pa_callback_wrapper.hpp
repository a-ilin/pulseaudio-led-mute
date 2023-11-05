#pragma once

#include <functional>
#include <memory>

namespace PA {

template<size_t N, class... Args>
class CallbackWrapperVoid
{
public:
    using UserFunc = std::function<void(Args...)>;

public:
    CallbackWrapperVoid(const UserFunc& cb = UserFunc())
        : mCb(cb)
    {
    }

    void operator()(Args... args)
    {
        mCb(std::forward<Args>(args)...);
    }

    static void StaticFunc(Args... args)
    {
        void* userdata = std::get<N>(std::forward_as_tuple(args...));
        CallbackWrapperVoid* self = (CallbackWrapperVoid*)userdata;
        (*self)(std::forward<Args>(args)...);
    }

protected:
    UserFunc mCb;
};

} // namespace PA
