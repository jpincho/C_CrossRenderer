#include "texture_descriptor.h"

void destroy_texture_descriptor(texture_descriptor_t *texture)
	{
	SAFE_DEL_C(texture->name);
	SAFE_DEL_C(texture->Data);
	}
