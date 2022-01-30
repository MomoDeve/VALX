#include "api/Context.h"

namespace VALX
{
    static Context* currentContext = nullptr;

    Context* SetCurrentContext(Context* context)
    {
        Context* oldContext = currentContext;
        currentContext = context;
        return oldContext;
    }

    Context* GetCurrentContext()
    {
        return currentContext;
    }
}