#include "CommonLoaders.h"
#include <Platform/Logger.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "../Shader.h"
#include "../Texture.h"

crShaderHandle LoadShader(const char *VertexFile, const char *GeometryFile, const char *FragmentFile)
    {
    crShaderCode ShaderCode;
    crShaderHandle ShaderHandle = crShaderHandle_Invalid;
    ShaderCode.FragmentShader = ShaderCode.GeometryShader = ShaderCode.VertexShader = NULL;

    if (LoadFileContents(VertexFile, &ShaderCode.VertexShader) == false)
        {
        LOG_ERROR("Unable to load vertex shader from '%s'", VertexFile);
        goto OnError;
        }
    if (LoadFileContents(FragmentFile, &ShaderCode.FragmentShader) == false)
        {
        LOG_ERROR("Unable to load fragment shader from '%s'", FragmentFile);
        goto OnError;
        }
    if ((GeometryFile != NULL) && (LoadFileContents(GeometryFile, &ShaderCode.GeometryShader) == false))
        {
        LOG_ERROR("Unable to load geometry shader from '%s'", GeometryFile);
        goto OnError;
        }

    ShaderHandle = crCreateShader(ShaderCode);
    if (ShaderHandle == crShaderHandle_Invalid)
        LOG_ERROR("Unable to build shader from '%s' and '%s'", VertexFile, FragmentFile);

OnError:
    SAFE_DEL_C(ShaderCode.FragmentShader);
    SAFE_DEL_C(ShaderCode.VertexShader);
    SAFE_DEL_C(ShaderCode.GeometryShader);
    return ShaderHandle;
    }

bool LoadFileContents(const char *Filename, char **Contents)
    {
    FILE *FileHandle = NULL;
#if defined (PLATFORM_COMPILER_MSVC)
    if (fopen_s(&FileHandle, Filename, "rt") != 0)
        return false;
#else
    FileHandle = fopen(Filename, "rt");
#endif
    if (FileHandle == NULL)
        return false;

    fseek(FileHandle, 0, SEEK_END);
    size_t FileSize = ftell(FileHandle);
    fseek(FileHandle, 0, SEEK_SET);

    *Contents = calloc(FileSize + 1, sizeof(char));
    if (*Contents == NULL)
        {
        fclose(FileHandle);
        return false;
        }
    fread(*Contents, sizeof(char), FileSize, FileHandle);
    fclose(FileHandle);
    return true;
    }

crTextureHandle LoadTexture(const char *ImageFile, const bool Flip)
    {
    ivec2 ImageSize;
    int Channels;
    crPixelFormat ImageFormat;

    stbi_set_flip_vertically_on_load(Flip ? 1 : 0);
    stbi_uc *Image = stbi_load(ImageFile, &ImageSize.x, &ImageSize.y, &Channels, 0);
    if (!Image)
        {
        LOG_ERROR("Unable to load texture from '%s'", ImageFile);
        return crTextureHandle_Invalid;
        }
    switch (Channels)
        {
        case 3:
            ImageFormat = crPixelFormat_RedGreenBlue888;
            break;
        case 4:
            ImageFormat = crPixelFormat_RedGreenBlueAlpha8888;
            break;
        default:
            stbi_image_free(Image);
            return crTextureHandle_Invalid;
        }

    crTextureDescriptor TextureDescriptor = { 0 };
    TextureDescriptor.Type = crTextureType_Texture2D;
    TextureDescriptor.Dimensions.x = ImageSize.x;
    TextureDescriptor.Dimensions.y = ImageSize.y;
    TextureDescriptor.Format = ImageFormat;
    TextureDescriptor.Mipmapped = true;
    TextureDescriptor.Data = Image;
    crTextureHandle Texture = crCreateTexture(TextureDescriptor);
    stbi_image_free(Image);
    return Texture;
    }

crTextureHandle LoadCubemapTexture(const char *ImageFile[6], const bool Flip)
    {
    ivec2 ImageSize;
    int Channels;
    crPixelFormat ImageFormat;

    stbi_set_flip_vertically_on_load(Flip ? 1 : 0);

    stbi_uc *Images[6] = { 0 };
    for (unsigned Face = 0; Face < 6; ++Face)
        {
        Images[Face] = stbi_load(ImageFile[Face], &ImageSize.x, &ImageSize.y, &Channels, 0);
        if (!Images[Face])
            {
            LOG_ERROR("Unable to load texture from '%s'", ImageFile[Face]);
            goto OnError;
            }

        switch (Channels)
            {
            case 3:
                ImageFormat = crPixelFormat_RedGreenBlue888;
                break;
            case 4:
                ImageFormat = crPixelFormat_RedGreenBlueAlpha8888;
                break;
            default:
                goto OnError;
            }
        }

    crTextureDescriptor TextureDescriptor = { 0 };
    TextureDescriptor.Type = crTextureType_TextureCubeMap;
    TextureDescriptor.Dimensions.x = ImageSize.x;
    TextureDescriptor.Dimensions.y = ImageSize.y;
    TextureDescriptor.Format = ImageFormat;
    TextureDescriptor.Mipmapped = true;
    TextureDescriptor.Data = NULL;
    crTextureHandle Texture = crCreateTexture(TextureDescriptor);

    void *VoidPointer[6];
    for (unsigned Face = 0; Face < 6; ++Face)
        VoidPointer[Face] = Images[Face];
    if (crLoadCubeMapTextureData(Texture, ImageFormat, VoidPointer) == false)
        goto OnError;

    for (unsigned Face = 0; Face < 6; ++Face)
        stbi_image_free(Images[Face]);
    return Texture;
OnError:
    for (unsigned Face = 0; Face < 6; ++Face)
        stbi_image_free(Images[Face]);
    return crTextureHandle_Invalid;
    }