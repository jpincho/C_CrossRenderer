#include "GL4Shader.h"
#include "GL4Internals.h"
#include <string.h>
#include <assert.h>
#include <malloc.h>

#if 0
bool ParseErrorMessages(const std::string &Code, const std::string &Infolog, std::vector <std::string> &Output);
bool DetectUniformsAndAttributes(GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes);

ShaderObjectHandle CreateShaderObject(const ShaderObjectType Type)
    {
    GLenum GLShaderTypes[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };

    ShaderObjectInfo NewShaderObject;
    NewShaderObject.OpenGLID = glCreateShader(GLShaderTypes[(int)Type]);
    NewShaderObject.Type = Type;
    ShaderObjectHandle NewHandle(ShaderObjects.GetFreeIndex());
    ShaderObjects[NewHandle.GetKey()] = NewShaderObject;
    return NewHandle;
    }

void DeleteShaderObject(const ShaderObjectHandle Handle)
    {
    ShaderObjectInfo *ShaderObjectInformation = &ShaderObjects[Handle.GetKey()];

    glDeleteShader(ShaderObjectInformation->OpenGLID);
    ShaderObjects.ReleaseIndex(Handle.GetKey());
    }

bool BuildShaderObject(const ShaderObjectHandle Handle, const std::string &Code)
    {
    ShaderObjectInfo *ShaderObjectInformation = &ShaderObjects[Handle.GetKey()];
    LOG_DEBUG("Compiling shader object");

    // Loads the shader source code
    ShaderObjectInformation->Code = Code;
    const char *Sources[1];
    Sources[0] = ShaderObjectInformation->Code.c_str();

    int CompileStatus = 0;
    glShaderSource(ShaderObjectInformation->OpenGLID, (GLsizei)1, Sources, NULL);
    glCompileShader(ShaderObjectInformation->OpenGLID);
    glGetShaderiv(ShaderObjectInformation->OpenGLID, GL_COMPILE_STATUS, &CompileStatus);

    if ((CheckError() == false) || (CompileStatus != GL_TRUE))
        {
        std::string Infolog;
        int InfoLogLength;
        glGetShaderiv(ShaderObjectInformation->OpenGLID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength)
            {
            Infolog.resize(InfoLogLength);
            glGetShaderInfoLog(ShaderObjectInformation->OpenGLID, InfoLogLength, NULL, (GLchar *)Infolog.c_str());
            }

        std::vector <std::string> ErrorStrings;
        if ((Infolog.length() > 0) && (ParseErrorMessages(Code, Infolog, ErrorStrings)))
            {
            LOG_ERROR("OpenGL error during shader object compilation");
            for (const auto &ErrorStringIterator : ErrorStrings)
                LOG_RAW("%s", ErrorStringIterator.c_str());
            return false;
            }
        else
            {
            LOG_ERROR("OpenGL error during shader object compilation. Unable to parse log. '%s'", Infolog.c_str());
            LOG_ERROR("%s", Code.c_str());
            }
        }

    LOG_DEBUG("Shader object successfully compiled");
    return true;
    }

bool ParseErrorMessages(const std::string &Code, const std::string &Infolog, std::vector <std::string> &Output)
    {
    std::string TextColorReset, TextColorError;
    ConsoleTextColor::TextColor(TextColorError, ConsoleTextColor::Color::Red);
    ConsoleTextColor::TextColorReset(TextColorReset);

    std::vector <std::string> ErrorStrings;
    std::vector <std::string> SourceLines;
    StringUtils::Split(Code, '\n', SourceLines);
    StringUtils::Split(Infolog, '\n', ErrorStrings);
    size_t LastPrintedLine = 0;
    if (ErrorStrings.size() == 0)
        return false;
    for (const auto &ErrorStringIterator : ErrorStrings)
        {
        unsigned long Column, Line;
#if defined (CROSS_RENDERER_TARGET_PLATFORM_WINDOWS)
        if (sscanf_s(ErrorStringIterator.c_str(), "ERROR: %lu:%lu", &Column, &Line) == 2)
#elif defined (CROSS_RENDERER_TARGET_PLATFORM_LINUX)
        if (sscanf(ErrorStringIterator.c_str(), "ERROR: %lu:%lu", &Column, &Line) == 2)
#endif
            {
            for (; LastPrintedLine < Line; ++LastPrintedLine)
                Output.push_back(SourceLines[LastPrintedLine]);
            Output.push_back(TextColorError + ErrorStringIterator + TextColorReset);
            }
        else
            Output.push_back(TextColorError + ErrorStringIterator + TextColorReset);
        }
    for (; LastPrintedLine < SourceLines.size(); ++LastPrintedLine)
        Output.push_back(SourceLines[LastPrintedLine]);
    return true;
    }

ShaderHandle CreateShader(void)
    {
    ShaderInfo NewShader;
    NewShader.OpenGLID = glCreateProgram();
    if (CheckError() == false)
        return ShaderHandle::Invalid;
    ShaderHandle NewHandle(Shaders.GetFreeIndex());
    Shaders[NewHandle.GetKey()] = NewShader;
    return NewHandle;
    }

ShaderHandle CreateShader(const ShaderCode &NewCode)
    {
    ShaderObjectHandle VertexShader, GeometryShader, FragmentShader;
    ShaderHandle NewHandle = CreateShader();

    std::vector <ShaderObjectHandle> ShaderObjectsToLink;

    if (!NewCode.VertexShader.empty())
        {
        VertexShader = CreateShaderObject(ShaderObjectType::Vertex);
        if (BuildShaderObject(VertexShader, NewCode.VertexShader) == false)
            goto OnError;
        ShaderObjectsToLink.push_back(VertexShader);
        }
    if (!NewCode.GeometryShader.empty())
        {
        GeometryShader = CreateShaderObject(ShaderObjectType::Geometry);
        if (BuildShaderObject(GeometryShader, NewCode.GeometryShader) == false)
            goto OnError;
        ShaderObjectsToLink.push_back(GeometryShader);
        }
    if (!NewCode.FragmentShader.empty())
        {
        FragmentShader = CreateShaderObject(ShaderObjectType::Fragment);
        if (BuildShaderObject(FragmentShader, NewCode.FragmentShader) == false)
            goto OnError;
        ShaderObjectsToLink.push_back(FragmentShader);
        }
    if (LinkShader(NewHandle, ShaderObjectsToLink) == false)
        goto OnError;

    return NewHandle;
OnError:
    if (VertexShader)
        DeleteShaderObject(VertexShader);
    if (FragmentShader)
        DeleteShaderObject(FragmentShader);
    if (GeometryShader)
        DeleteShaderObject(GeometryShader);
    if (NewHandle)
        DeleteShader(NewHandle);
    return ShaderHandle::Invalid;
    }

bool DeleteShader(const ShaderHandle Handle)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];
    for (const auto &Iterator : ShaderInformation->AttachedShaderObjects)
        {
        glDetachShader(ShaderInformation->OpenGLID, ShaderObjects[Iterator.GetKey()].OpenGLID);
        DeleteShaderObject(Iterator);
        }

    glDeleteProgram(ShaderInformation->OpenGLID);
    Shaders.ReleaseIndex(Handle.GetKey());
    return true;
    }

bool LinkShader(const ShaderHandle Handle, const std::vector <ShaderObjectHandle> &ObjectHandles)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];
    std::vector <GLuint> GLIDs;
    for (const auto &Iterator : ShaderInformation->AttachedShaderObjects)
        glDetachShader(ShaderInformation->OpenGLID, ShaderObjects[Iterator.GetKey()].OpenGLID);
    for (const auto &Iterator : ObjectHandles)
        glAttachShader(ShaderInformation->OpenGLID, ShaderObjects[Iterator.GetKey()].OpenGLID);
    ShaderInformation->AttachedShaderObjects = ObjectHandles;

    // Link it, and check it
    glLinkProgram(ShaderInformation->OpenGLID);

    GLint LinkStatus;
    glGetProgramiv(ShaderInformation->OpenGLID, GL_LINK_STATUS, &LinkStatus);
    if (LinkStatus != GL_TRUE)
        {
        std::string Infolog;
        int InfoLogLength;
        glGetProgramiv(ShaderInformation->OpenGLID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength)
            {
            Infolog.resize(InfoLogLength);
            glGetProgramInfoLog(ShaderInformation->OpenGLID, InfoLogLength, NULL, (GLchar *)Infolog.c_str());
            }

        LOG_ERROR("OpenGL error during shader program linking. '%s'", Infolog.c_str());
        for (const auto &Iterator : ObjectHandles)
            {
            GLint length;
            glGetShaderiv(ShaderObjects[Iterator.GetKey()].OpenGLID, GL_SHADER_SOURCE_LENGTH, &length);
            std::string Source;
            Source.resize(length);
            glGetShaderSource(ShaderObjects[Iterator.GetKey()].OpenGLID, length, &length, (char *)Source.c_str());
            LOG_ERROR("%s", Source.c_str());
            }

        return false;
        }
    LOG_DEBUG("Linked shader program %u", ShaderInformation->OpenGLID);
    if (DetectUniformsAndAttributes(ShaderInformation->OpenGLID, ShaderInformation->Uniforms, ShaderInformation->Attributes) == false)
        {
        LOG_ERROR("Error detecting uniforms/attributes");
        return false;
        }

    return true;
    }

void GetShaderUniformList(const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderUniformType>> &UniformList)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];

    for (const auto &iterator : ShaderInformation->Uniforms)
        UniformList.push_back(std::pair <std::string, ShaderUniformType>(iterator.Name, iterator.Type));
    }

ShaderUniformHandle GetShaderUniformHandle(const ShaderHandle Handle, const std::string Name)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];

    for (unsigned cont = 0; cont < ShaderInformation->Uniforms.size(); ++cont)
        {
        if (ShaderInformation->Uniforms[cont].Name == Name)
            return ShaderUniformHandle(cont);
        }
    //LOG_ERROR ( "Invalid uniform '%s' for shader", Name.c_str() );
    return ShaderUniformHandle::Invalid;
    }

void GetShaderAttributeList(const ShaderHandle Handle, std::vector <std::pair <std::string, ShaderAttributeType>> &AttributeList)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];

    for (const auto &iterator : ShaderInformation->Attributes)
        AttributeList.push_back(std::pair <std::string, ShaderAttributeType>(iterator.Name, iterator.Type));
    }

ShaderAttributeHandle GetShaderAttributeHandle(const ShaderHandle Handle, const std::string Name)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];

    for (unsigned cont = 0; cont < ShaderInformation->Attributes.size(); ++cont)
        {
        if (ShaderInformation->Attributes[cont].Name == Name)
            return ShaderAttributeHandle(cont);
        }
    //LOG_ERROR ( "Invalid attribute '%s' for shader", Name.c_str() );
    return ShaderAttributeHandle::Invalid;
    }

void GetShaderInformation(const ShaderHandle Handle, ShaderInformation &Information)
    {
    ShaderInfo *ShaderInformation = &Shaders[Handle.GetKey()];
    for (unsigned cont = 0; cont < ShaderInformation->Uniforms.size(); ++cont)
        {
        ShaderInformation::ShaderUniformInformation Info;
        Info.Name = ShaderInformation->Uniforms[cont].Name;
        Info.Type = ShaderInformation->Uniforms[cont].Type;
        Info.Handle = ShaderUniformHandle(cont);
        Information.Uniforms.push_back(Info);
        }
    for (unsigned cont = 0; cont < ShaderInformation->Attributes.size(); ++cont)
        {
        ShaderInformation::ShaderAttributeInformation Info;
        Info.Name = ShaderInformation->Attributes[cont].Name;
        Info.Type = ShaderInformation->Attributes[cont].Type;
        Info.Handle = ShaderAttributeHandle(cont);
        Information.Attributes.push_back(Info);
        }
    Information.AttachedShaderObjects = ShaderInformation->AttachedShaderObjects;
    Information.Handle = Handle;
    }

bool DetectUniformsAndAttributes(GLuint OpenGLID, std::vector <UniformInfo> &Uniforms, std::vector <AttributeInfo> &Attributes)
    {
    GLint UniformCount, MaxUniformNameLength, UniformBlockCount;
    GLint AttributeCount, MaxAttributeNameLength;
    bool Result = false;

    glGetProgramiv(OpenGLID, GL_ACTIVE_UNIFORMS, &UniformCount);
    glGetProgramiv(OpenGLID, GL_ACTIVE_ATTRIBUTES, &AttributeCount);
    glGetProgramiv(OpenGLID, GL_ACTIVE_UNIFORM_BLOCKS, &UniformBlockCount);

    if (!CheckError())
        return false;
    if ((UniformCount == 0) && (AttributeCount == 0) && (UniformBlockCount == 0))
        return true;

    glGetProgramiv(OpenGLID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MaxUniformNameLength);
    glGetProgramiv(OpenGLID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &MaxAttributeNameLength);
    if (!CheckError())
        return false;

    Uniforms.reserve(UniformCount + UniformBlockCount);
    Attributes.reserve(AttributeCount);

    size_t NameLength = std::max(MaxUniformNameLength, MaxAttributeNameLength);
    char *Name = new char[NameLength];

    Uniforms.clear();
    for (int cont = 0; cont < UniformBlockCount; ++cont)
        {
        GLint Location;

        glGetActiveUniformBlockName(OpenGLID, cont, (GLsizei)NameLength, NULL, Name);
        if (!CheckError())
            goto cleanup;
        if (strncmp("gl_", Name, 3) == 0)
            continue;

        Location = glGetUniformBlockIndex(OpenGLID, Name);
        if (!CheckError())
            goto cleanup;
        if (Location == -1)
            {
            LOG_ERROR("Unable to get location for uniform block '%s'", Name);
            continue;
            }

        UniformInfo NewUniform;
        NewUniform.Name.assign(Name);
        NewUniform.Type = ShaderUniformType::Block;
        NewUniform.OpenGLID = Location;
        Uniforms.emplace_back(std::move(NewUniform));
        }

    for (int cont = 0; cont < UniformCount; ++cont)
        {
        GLenum GLType;
        GLint Location;
        GLint UniformSize;
        ShaderUniformType Type;

        glGetActiveUniform(OpenGLID, cont, (GLsizei)NameLength, NULL, &UniformSize, &GLType, Name);
        if (!CheckError())
            goto cleanup;
        if (strncmp("gl_", Name, 3) == 0)
            continue;

        Location = glGetUniformLocation(OpenGLID, Name);
        if (!CheckError())
            goto cleanup;
        if (Location == -1)
            {
            bool IsPartOfBlock = false;
            // TODO Check if this is part of an uniform block
            for (int BlockIterator = 0; BlockIterator < UniformBlockCount; ++BlockIterator)
                {
                if (strncmp(Name, Uniforms[BlockIterator].Name.c_str(), Uniforms[BlockIterator].Name.length()) == 0)
                    {
                    IsPartOfBlock = true;
                    break;
                    }
                }
            if (IsPartOfBlock)
                continue;// It's a uniform inside of a block. for some fucking reason, I can't get its location...
            LOG_ERROR("Unable to get location for uniform '%s'", Name);
            continue;
            }

        // If this is uniform is an array, add all other elements to the list
        if (UniformSize != 1)
            {
            char *BracketPos = strchr(Name, '[');
            *BracketPos = 0;
            for (int Index = 0; Index < UniformSize; ++Index)
                {
                Type = TranslateOpenGLUniformType(GLType);
                UniformInfo NewUniform;
                NewUniform.Name.assign(Name);
                NewUniform.Name.append("[");
                NewUniform.Name.append(std::to_string(Index));
                NewUniform.Name.append("]");
                NewUniform.Type = Type;
                NewUniform.OpenGLID = Location + Index;
                Uniforms.emplace_back(std::move(NewUniform));
                }
            }
        else
            {
            Type = TranslateOpenGLUniformType(GLType);
            UniformInfo NewUniform;
            NewUniform.Name.assign(Name);
            NewUniform.Type = Type;
            NewUniform.OpenGLID = Location;
            Uniforms.emplace_back(std::move(NewUniform));
            }
        }

    Attributes.clear();
    for (int cont = 0; cont < AttributeCount; ++cont)
        {
        GLenum GLType;
        GLint Location;
        GLint AttributeSize;
        ShaderAttributeType Type;

        glGetActiveAttrib(OpenGLID, cont, (GLsizei)NameLength, NULL, &AttributeSize, &GLType, Name);
        if (!CheckError())
            goto cleanup;
        if (strncmp("gl_", Name, 3) == 0)
            continue;

        Location = glGetAttribLocation(OpenGLID, Name);
        if (!CheckError())
            goto cleanup;
        if (Location == -1)
            {
            LOG_ERROR("Unable to get location for attribute '%s'", Name);
            goto cleanup;
            }

        Type = TranslateOpenGLUniformType(GLType);
        AttributeInfo NewAttribute;
        NewAttribute.Name.assign(Name);
        NewAttribute.Type = Type;
        NewAttribute.OpenGLID = Location;
        NewAttribute.Enabled = false;
        Attributes.emplace_back(std::move(NewAttribute));
        }
    Result = true;

cleanup:
    delete[] Name;
    return Result;
    }
    }
    }
#endif

#ifdef PLATFORM_COMPILER_MSVC
#define strdup _strdup
#endif

static bool BuildShaderObject(const char *Code, const GLenum GLShaderType, GLuint *OutputID)
    {
    GLuint ShaderObjectID = glCreateShader(GLShaderType);

    int CompileStatus = 0;
    const char *Pointers[1];
    Pointers[0] = Code;
    glShaderSource(ShaderObjectID, (GLsizei)1, Pointers, NULL);
    glCompileShader(ShaderObjectID);
    glGetShaderiv(ShaderObjectID, GL_COMPILE_STATUS, &CompileStatus);

    if ((crGL4CheckError() == false) || (CompileStatus != GL_TRUE))
        {
        GLchar *Infolog = NULL;
        int InfoLogLength;
        glGetShaderiv(ShaderObjectID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength)
            {
            Infolog = alloca(InfoLogLength);
            glGetShaderInfoLog(ShaderObjectID, InfoLogLength, NULL, Infolog);
            }
        LOG_ERROR("OpenGL error during shader object compilation. '%s'", Infolog);
        return false;
        }
    *OutputID = ShaderObjectID;
    return true;
    }

static bool DetectUniformsAndAttributes(crGL4InternalShaderInfo *ShaderInfo)
    {
    GLint UniformCount, MaxUniformNameLength, UniformBlockCount;
    GLint AttributeCount, MaxAttributeNameLength;
    bool Result = false;

    glGetProgramiv(ShaderInfo->OpenGLID, GL_ACTIVE_UNIFORMS, &UniformCount);
    glGetProgramiv(ShaderInfo->OpenGLID, GL_ACTIVE_ATTRIBUTES, &AttributeCount);
    glGetProgramiv(ShaderInfo->OpenGLID, GL_ACTIVE_UNIFORM_BLOCKS, &UniformBlockCount);

    if (!crGL4CheckError())
        return false;
    if ((UniformCount == 0) && (AttributeCount == 0) && (UniformBlockCount == 0))
        return true;

    glGetProgramiv(ShaderInfo->OpenGLID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MaxUniformNameLength);
    glGetProgramiv(ShaderInfo->OpenGLID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &MaxAttributeNameLength);
    if (!crGL4CheckError())
        return false;

    ShaderInfo->Attributes = calloc(AttributeCount, sizeof(crGL4InternalAttributeInfo));

    size_t NameLength = MAX(MaxUniformNameLength, MaxAttributeNameLength);
    char *Name = calloc(NameLength + 1, sizeof(char));

    ShaderInfo->UniformBlocks = calloc(UniformBlockCount, sizeof(crGL4InternalUniformInfo));
    assert(ShaderInfo->UniformBlocks);
    ShaderInfo->UniformBlockCount = UniformBlockCount;
    for (int cont = 0; cont < UniformBlockCount; ++cont)
        {
        GLint Location;

        glGetActiveUniformBlockName(ShaderInfo->OpenGLID, cont, (GLsizei)NameLength, NULL, Name);
        if (!crGL4CheckError())
            goto cleanup;
        if (strncmp("gl_", Name, 3) == 0)
            continue;

        Location = glGetUniformBlockIndex(ShaderInfo->OpenGLID, Name);
        if (!crGL4CheckError())
            goto cleanup;
        if (Location == -1)
            {
            LOG_ERROR("Unable to get location for uniform block '%s'", Name);
            continue;
            }

        ShaderInfo->UniformBlocks[cont].Name = strdup(Name);
        ShaderInfo->UniformBlocks[cont].OpenGLID = Location;
        ShaderInfo->UniformBlocks[cont].Type = crShaderUniformType_Block;
        }

    ShaderInfo->Uniforms = calloc(UniformCount, sizeof(crGL4InternalUniformInfo));
    assert(ShaderInfo->Uniforms);
    ShaderInfo->UniformCount = UniformCount;
    for (int cont = 0; cont < UniformCount; ++cont)
        {
        GLenum GLType;
        GLint Location;
        GLint UniformSize;

        glGetActiveUniform(ShaderInfo->OpenGLID, cont, (GLsizei)NameLength, NULL, &UniformSize, &GLType, Name);
        if (!crGL4CheckError())
            goto cleanup;
        if (strncmp("gl_", Name, 3) == 0)
            continue;

        Location = glGetUniformLocation(ShaderInfo->OpenGLID, Name);
        if (!crGL4CheckError())
            goto cleanup;
        if (Location == -1)
            {
            bool IsPartOfBlock = false;
            // TODO Check if this is part of an uniform block
            for (int BlockIterator = 0; BlockIterator < UniformBlockCount; ++BlockIterator)
                {
                if (strncmp(Name, ShaderInfo->UniformBlocks[BlockIterator].Name, strlen(ShaderInfo->UniformBlocks[BlockIterator].Name)) == 0)
                    {
                    IsPartOfBlock = true;
                    break;
                    }
                }
            if (IsPartOfBlock)
                continue;// It's a uniform inside of a block. for some fucking reason, I can't get its location...
            LOG_ERROR("Unable to get location for uniform '%s'", Name);
            continue;
            }

        // If this is uniform is an array, add all other elements to the list
        if (UniformSize != 1)
            {
            // First extend our array...
            UniformCount += UniformSize - 1;
            void *Temp = realloc(ShaderInfo->Uniforms, sizeof(crGL4InternalUniformInfo) * UniformCount);
            assert(Temp != NULL);
            ShaderInfo->Uniforms = Temp;

            char *BracketPos = strchr(Name, '[');
            *BracketPos = 0;
            for (int Index = 0; Index < UniformSize; ++Index)
                {
                int NecessarySpace = snprintf(NULL, 0, "%s[%u]", Name, Index);
                ShaderInfo->Uniforms[cont].Name = malloc(NecessarySpace + 1);
                snprintf(ShaderInfo->Uniforms[cont].Name, NecessarySpace + 1, "%s[%u]", Name, Index);
                ShaderInfo->Uniforms[cont].Type = crGL4TranslateOpenGLUniformType(GLType);
                ShaderInfo->Uniforms[cont].OpenGLID = Location + Index;
                ++cont;
                }
            }
        else
            {
            ShaderInfo->Uniforms[cont].Name = strdup(Name);
            ShaderInfo->Uniforms[cont].OpenGLID = Location;
            ShaderInfo->Uniforms[cont].Type = crGL4TranslateOpenGLUniformType(GLType);
            }
        }

    ShaderInfo->Attributes = calloc(AttributeCount, sizeof(crGL4InternalAttributeInfo));
    assert(ShaderInfo->Attributes);
    ShaderInfo->AttributeCount = AttributeCount;
    for (int cont = 0; cont < AttributeCount; ++cont)
        {
        GLenum GLType;
        GLint Location;
        GLint AttributeSize;

        glGetActiveAttrib(ShaderInfo->OpenGLID, cont, (GLsizei)NameLength, NULL, &AttributeSize, &GLType, Name);
        if (!crGL4CheckError())
            goto cleanup;
        if (strncmp("gl_", Name, 3) == 0)
            continue;

        Location = glGetAttribLocation(ShaderInfo->OpenGLID, Name);
        if (!crGL4CheckError())
            goto cleanup;
        if (Location == -1)
            {
            LOG_ERROR("Unable to get location for attribute '%s'", Name);
            goto cleanup;
            }

        ShaderInfo->Attributes[cont].Name = strdup(Name);
        ShaderInfo->Attributes[cont].OpenGLID = Location;
        ShaderInfo->Attributes[cont].Type = (crShaderAttributeType)crGL4TranslateOpenGLUniformType(GLType);
        ShaderInfo->Attributes[cont].Enabled = false;
        }
    Result = true;

cleanup:
    free(Name);
    return Result;
    }

crShaderHandle crGL4CreateShader(crShaderCode NewCode)
    {
    crGL4InternalShaderInfo *NewShader = calloc(1, sizeof(crGL4InternalShaderInfo));
    assert(NewShader);
    pointer_list_node *NewNode = NULL;

    NewShader->OpenGLID = glCreateProgram();
    if (crGL4CheckError() == false)
        return crShaderHandle_Invalid;

    GLuint VertexShaderID = (GLuint)-1, FragmentShaderID = (GLuint)-1, GeometryShaderID = (GLuint)-1;
    bool VertexShaderBuilt = false, FragmentShaderBuilt = false, GeometryShaderBuilt = false;
    if (NewCode.VertexShader)
        {
        if (BuildShaderObject(NewCode.VertexShader, GL_VERTEX_SHADER, &VertexShaderID) == false)
            goto OnError;
        VertexShaderBuilt = true;
        glAttachShader(NewShader->OpenGLID, VertexShaderID);
        }

    if (NewCode.FragmentShader)
        {
        if (BuildShaderObject(NewCode.FragmentShader, GL_FRAGMENT_SHADER, &FragmentShaderID) == false)
            goto OnError;
        FragmentShaderBuilt = true;
        glAttachShader(NewShader->OpenGLID, FragmentShaderID);
        }

    if (NewCode.GeometryShader)
        {
        if (BuildShaderObject(NewCode.GeometryShader, GL_GEOMETRY_SHADER, &GeometryShaderID) == false)
            goto OnError;
        GeometryShaderBuilt = true;
        glAttachShader(NewShader->OpenGLID, GeometryShaderID);
        }

    glLinkProgram(NewShader->OpenGLID);

    GLint LinkStatus;
    glGetProgramiv(NewShader->OpenGLID, GL_LINK_STATUS, &LinkStatus);
    if (LinkStatus != GL_TRUE)
        {
        GLchar *Infolog = NULL;
        int InfoLogLength;
        glGetProgramiv(NewShader->OpenGLID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength)
            {
            Infolog = alloca(InfoLogLength);
            glGetProgramInfoLog(NewShader->OpenGLID, InfoLogLength, NULL, Infolog);
            }

        LOG_ERROR("OpenGL error during shader program linking. '%s'", Infolog);
        goto OnError;
        }
    LOG_DEBUG("Linked shader program %u", NewShader->OpenGLID);

    if (VertexShaderBuilt)
        glDeleteShader(VertexShaderID);
    if (FragmentShaderBuilt)
        glDeleteShader(FragmentShaderID);
    if (GeometryShaderBuilt)
        glDeleteShader(GeometryShaderID);

    if (DetectUniformsAndAttributes(NewShader) == false)
        goto OnError;

    // Populate shader information structure
    NewShader->GeneralInformation.AttributeCount = NewShader->AttributeCount;
    NewShader->GeneralInformation.Attributes = calloc(NewShader->AttributeCount, sizeof(crShaderAttributeInformation));
    for (unsigned Index = 0; Index < NewShader->AttributeCount; ++Index)
        {
        NewShader->GeneralInformation.Attributes[Index].Handle = Index;
        NewShader->GeneralInformation.Attributes[Index].Name = NewShader->Attributes[Index].Name;
        NewShader->GeneralInformation.Attributes[Index].Type = NewShader->Attributes[Index].Type;
        }
    NewShader->GeneralInformation.UniformCount = NewShader->UniformCount;
    NewShader->GeneralInformation.Uniforms = calloc(NewShader->UniformCount, sizeof(crShaderUniformInformation));
    for (unsigned Index = 0; Index < NewShader->UniformCount; ++Index)
        {
        NewShader->GeneralInformation.Uniforms[Index].Handle = Index;
        NewShader->GeneralInformation.Uniforms[Index].Name = NewShader->Uniforms[Index].Name;
        NewShader->GeneralInformation.Uniforms[Index].Type = NewShader->Uniforms[Index].Type;
        }
    NewShader->GeneralInformation.UniformBlockCount = NewShader->UniformBlockCount;
    NewShader->GeneralInformation.UniformBlocks = calloc(NewShader->UniformBlockCount, sizeof(crShaderUniformInformation));
    for (unsigned Index = 0; Index < NewShader->UniformBlockCount; ++Index)
        {
        NewShader->GeneralInformation.UniformBlocks[Index].Handle = Index;
        NewShader->GeneralInformation.UniformBlocks[Index].Name = NewShader->UniformBlocks[Index].Name;
        NewShader->GeneralInformation.UniformBlocks[Index].Type = NewShader->UniformBlocks[Index].Type;
        }

    NewNode = pointer_list_add_at_end(&crGL4Information.Shaders, NewShader);
    return NewNode;

OnError:
    if (NewShader->OpenGLID)
        glDeleteProgram(NewShader->OpenGLID);
    if (VertexShaderBuilt)
        glDeleteShader(VertexShaderID);
    if (FragmentShaderBuilt)
        glDeleteShader(FragmentShaderID);
    if (GeometryShaderBuilt)
        glDeleteShader(GeometryShaderID);
    free(NewShader);
    return crShaderHandle_Invalid;
    }

bool crGL4DeleteShader(const crShaderHandle Handle)
    {
    crGL4InternalShaderInfo *ShaderInformation = (crGL4InternalShaderInfo *)pointer_list_get_node_data(Handle);

    glDeleteProgram(ShaderInformation->OpenGLID);
    for (unsigned Index = 0; Index < ShaderInformation->AttributeCount; ++Index)
        {
        SAFE_DEL_C(ShaderInformation->Attributes[Index].Name);
        }
    SAFE_DEL_C(ShaderInformation->Attributes);

    for (unsigned Index = 0; Index < ShaderInformation->UniformCount; ++Index)
        {
        SAFE_DEL_C(ShaderInformation->Uniforms[Index].Name);
        }
    SAFE_DEL_C(ShaderInformation->Uniforms);
    SAFE_DEL_C(ShaderInformation);
    pointer_list_destroy_node(&crGL4Information.Shaders, Handle);
    return true;
    }

const crShaderInformation *crGL4GetShaderInformation(const crShaderHandle Handle)
    {
    crGL4InternalShaderInfo *ShaderInformation = (crGL4InternalShaderInfo *)pointer_list_get_node_data(Handle);
    return &ShaderInformation->GeneralInformation;
    }
