cmake_minimum_required ( VERSION 3.10 )

project ( C_CrossRenderer )

list ( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/ExternalModules/CMakeUtils )
include ( target_warning_options )

include_directories(ExternalModules)

set ( DEPENDENCY_PROJECTS Platform C_Utils Math )

foreach ( DEPENDENCY IN LISTS DEPENDENCY_PROJECTS )
    if ( NOT TARGET ${DEPENDENCY} )
        add_subdirectory ( ExternalModules/${DEPENDENCY} )
    endif()
endforeach()

# OpenGL backend
set ( OpenGL_GL_PREFERENCE GLVND )
find_package ( OpenGL QUIET )
if ( OPENGL_FOUND )
    option ( CROSS_RENDERER_OPENGL_CORE_SUPPORT "Enable support for OpenGL Core" TRUE )
endif()

if ( CROSS_RENDERER_OPENGL_CORE_SUPPORT )
    message ( STATUS "${PROJECT_NAME} - Building OpenGL4 core backend" )
    file ( GLOB CROSS_RENDERER_OPENGL_SOURCES "OpenGL4/*.c" "OpenGL4/*.h" )
    list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_OPENGL_SOURCES} )

    # GLAD core
    if (NOT TARGET GLAD)
        file ( GLOB_RECURSE GLAD_SOURCES "ExternalModules/glad_core/*")
        add_library ( GLAD ${GLAD_SOURCES} )
        target_warning_options ( GLAD DISABLED )
    endif()
    target_include_directories ( GLAD PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ExternalModules/glad_core/include )
    list ( APPEND CROSS_RENDERER_LIBRARIES GLAD )
    if ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
        foreach ( LINUX_LIBRARY dl )
            find_library ( LIBRARY_LOCATION NAMES ${LINUX_LIBRARY} )
            list ( APPEND CROSS_RENDERER_LIBRARIES ${LIBRARY_LOCATION} )
        endforeach()
    endif()
    if(PLATFORM_WINDOWS)
        # GLAD WGL 
        file ( GLOB_RECURSE GLAD_WGL_SOURCES "ExternalModules/glad_wgl/*")
        add_library ( GLAD_WGL ${GLAD_WGL_SOURCES} )
        target_warning_options ( GLAD_WGL DISABLED )
        target_include_directories ( GLAD_WGL PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ExternalModules/glad_wgl/include ${CMAKE_CURRENT_LIST_DIR}/ExternalModules/glad_core/include )
        list ( APPEND CROSS_RENDERER_LIBRARIES GLAD_WGL )
        list ( APPEND CROSS_RENDERER_LIBRARIES "OpenGL32" )

        # WGL backend
        file ( GLOB CROSS_RENDERER_WGL_SOURCES "OpenGL4/WGL/*.c" "OpenGL4/WGL/*.h" )
        list ( APPEND CROSS_RENDERER_SOURCE_FILES ${CROSS_RENDERER_WGL_SOURCES} )
    endif()
endif()

if ( PLATFORM_WINDOWS)
    message ( STATUS "${PROJECT_NAME} - Building Windows backend" )
    file(GLOB WINDOW_MANAGER_SOURCES "Windows/*" )
endif()

# Main library
configure_file ( CrossRendererConfig.h.in ${CMAKE_CURRENT_LIST_DIR}/CrossRendererConfig.h )
file ( GLOB SOURCES "*.c" "*.h" "Renderer/*.c" "Renderer/*.h" ${WINDOW_MANAGER_SOURCES} "Internal/*" CrossRendererConfig.h.in AstyleConfig.astylerc )
list ( APPEND CROSS_RENDERER_SOURCE_FILES ${SOURCES} )
add_library ( C_CrossRenderer ${CROSS_RENDERER_SOURCE_FILES} )
target_include_directories ( C_CrossRenderer PUBLIC ${CROSS_RENDERER_INCLUDE_DIRS} ExternalModules )
target_link_libraries ( C_CrossRenderer PUBLIC ${CROSS_RENDERER_LIBRARIES} )
target_link_libraries ( C_CrossRenderer PUBLIC Platform )
target_link_libraries ( C_CrossRenderer PUBLIC C_Utils )
target_link_libraries ( C_CrossRenderer PUBLIC Math )
target_warning_options ( C_CrossRenderer HIGH AS_ERRORS )

# Tests
file ( GLOB TESTS "Tests/*" )
file ( GLOB COMMON_TEST_SOURCES "Tests/*.c" "Tests/*.h")
foreach ( TEST_PATH ${TESTS} )
    if (NOT IS_DIRECTORY ${TEST_PATH} )
        continue()
    endif()
	get_filename_component ( TEST_NAME ${TEST_PATH} NAME_WLE )
	file ( GLOB TEST_SOURCES "${TEST_PATH}/*.c" "${TEST_PATH}/*.h" )
	add_executable ( ${TEST_NAME} ${TEST_SOURCES} ${COMMON_TEST_SOURCES} )
	target_link_libraries ( ${TEST_NAME} C_CrossRenderer )
    target_include_directories ( ${TEST_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
	target_compile_definitions ( ${TEST_NAME} PRIVATE DATA_PATH="${TEST_PATH}/")
    target_warning_options ( ${TEST_NAME} HIGH AS_ERRORS )
endforeach()
