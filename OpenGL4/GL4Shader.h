#pragma once
#include "../Shader.h"

crShaderHandle crGL4CreateShader ( struct crShaderCode NewCode );
bool crGL4DeleteShader ( const crShaderHandle Handle );
const struct crShaderInformation *crGL4GetShaderInformation ( const crShaderHandle Handle );
