#pragma once
#include "../WindowManager.h"
#include <C_Utils/PointerList.h>

BEGIN_C_DECLARATIONS
extern crWindowManagerCallbacks WindowManagerCallbacks;
extern PointerList WindowList;
extern crWindowHandle MainWindowHandle;
END_C_DECLARATIONS
