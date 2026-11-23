#include "Texture.h"

const crTextureBindSettings crDefaultTextureBindSettings = { 0, {crTextureWrapMode_Repeat, crTextureWrapMode_Repeat}, {crTextureFilter_Linear, crTextureFilter_Linear} };

void crDestroyTextureDescriptor ( crTextureDescriptor *Descriptor )
	{
	SAFE_DEL_C ( Descriptor->Data );
	SAFE_DEL_C ( Descriptor->TextureName );
	}

void crCopyTextureDescriptor ( crTextureDescriptor *Output, const crTextureDescriptor *Input )
	{
	memcpy ( Output, Input, sizeof ( crTextureDescriptor ) );
	const unsigned DataSize = crPixelFormatDataSize ( Input->Format, Input->Dimensions.x, Input->Dimensions.y );
	Output->Data = malloc ( DataSize );
	assert ( Output->Data != NULL );
	if ( Output->Data == NULL )
		return;
	memcpy ( Output->Data, Input->Data, DataSize );
	if ( Input->TextureName )
		Output->TextureName = strdup ( Input->TextureName );
	}