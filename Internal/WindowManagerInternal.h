#pragma once
#include "../WindowManagerDefinitions.h"
#include <C_Utils/PointerList.h>

BEGIN_C_DECLARATIONS
extern crWindowManagerCallbacks WindowManagerCallbacks;
extern PointerList WindowList;
extern crWindowHandle MainWindowHandle;

bool crInitializeWindowBackend ( void );

END_C_DECLARATIONS
