#pragma once

#include <memory>

#include <pulse/pulseaudio.h>

namespace PA {

class Operation
{
public:
    Operation(pa_operation* op = {})
        : mOp(op)
    {
    }

    void Cancel()
    {
        if (mOp) {
            pa_operation_cancel(mOp.get());
        }
    }

protected:
    struct OperationDeleter
    {
        void operator()(pa_operation* op)
        {
            if (op) {
                pa_operation_cancel(op);
                pa_operation_unref(op);
            }
        }
    };

private:
    std::unique_ptr<pa_operation, OperationDeleter> mOp;
};

} // namespace PA
