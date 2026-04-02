#include "Texture.h"
#include <string.h>
#include <assert.h>

const crTextureBindSettings crDefaultTextureBindSettings = { 0, {crTextureWrapMode_Repeat, crTextureWrapMode_Repeat}, {crTextureFilter_Linear, crTextureFilter_Linear} };

void crDestroyTextureDescriptor ( crTextureDescriptor *Descriptor )
	{
	SAFE_DEL_C ( Descriptor->Data );
	SAFE_DEL_C ( Descriptor->TextureName );
	}

void crCopyTextureDescriptor ( crTextureDescriptor *Output, const crTextureDescriptor *Input )
	{
	memcpy ( Output, Input, sizeof ( crTextureDescriptor ) );
	Output->Data = malloc ( Input->Dimensions.x * Input->Dimensions.y * BytesPerPixel ( Input->Format ) );
	assert ( Output->Data != NULL );
	if ( Output->Data == NULL )
		return;
	memcpy ( Output->Data, Input->Data, Input->Dimensions.x * Input->Dimensions.y * BytesPerPixel ( Input->Format ) );
	Output->TextureName = strdup ( Input->TextureName );
	}