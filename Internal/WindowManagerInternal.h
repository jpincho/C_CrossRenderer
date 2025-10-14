#pragma once
#include "../WindowManagerDefinitions.h"
#include <C_Utils/pointer_list.h>
BEGIN_C_DECLARATIONS
extern crWindowManagerCallbacks WindowManagerCallbacks;
extern pointer_list WindowList;
extern crWindowHandle MainWindowHandle;
END_C_DECLARATIONS;
