#pragma once
#include "../Shader.h"

crShaderHandle crGL4CreateShader ( crShaderCode NewCode );
bool crGL4DeleteShader ( const crShaderHandle Handle );
const crShaderInformation *crGL4GetShaderInformation ( const crShaderHandle Handle );
