#pragma once
#include "../Shader.h"

BEGIN_C_DECLARATIONS
crShaderHandle crGL4CreateShader ( crShaderCode NewCode );
bool crGL4DeleteShader ( const crShaderHandle Handle );
const crShaderInformation *crGL4GetShaderInformation ( const crShaderHandle Handle );
END_C_DECLARATIONS
