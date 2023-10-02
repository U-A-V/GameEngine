include "Dependencies.lua"

workspace "GameEngine"
    architecture "x64"
    startproject "Engine-Editor"
    configurations {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "GameEngine/vendor/GLFW"
include "GameEngine/vendor/Glad"
include "GameEngine/vendor/imgui"
include "GameEngine/vendor/yaml-cpp"

project "GameEngine"
    location "GameEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"


    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hzpch.h"
    pchsource "GameEngine/src/hzpch.cpp"

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
        "%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
    }

    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.shaderc}",

    }

    links{
        "GLFW",
        "Glad",
        "imgui",
        "opengl32.lib",
        "yaml-cpp"
    }
    filter "files:%{prj.name}/vendor/ImGuizmo/**.cpp"
    flags{"NoPCH"}

    filter "system:windows"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
            "EG_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        symbols "on"
        runtime "Debug"
        
		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}
    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "on"
        runtime "Release"
        
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "on"
        runtime "Release"
        
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
        


project "Engine-Editor"
    location "Engine-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",

    }

    includedirs {
        "GameEngine/vendor/spdlog/include",
        "GameEngine/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.shaderc}",
    }

    links {
        "GameEngine"
    }
    filter "system:windows"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "on"
        runtime "Release"

    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "on"
        runtime "Release"

project "sandBox"
    location "sandBox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",

    }

    includedirs {
        "GameEngine/vendor/spdlog/include",
        "GameEngine/src",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.entt}",

    }

    links {
        "GameEngine"
    }
    filter "system:windows"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "on"
        runtime "Release"

    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "on"
        runtime "Release"

