workspace "GameEngine"
    architecture "x64"
    startproject "sandBox"
    configurations {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "GameEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "GameEngine/vendor/Glad/include"
IncludeDir["imgui"] = "GameEngine/vendor/imgui"

include "GameEngine/vendor/GLFW"
include "GameEngine/vendor/Glad"
include "GameEngine/vendor/imgui"

project "GameEngine"
    location "GameEngine"
    kind "SharedLib"
    language "C++"
    staticruntime "Off"


    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hzpch.h"
    pchsource "GameEngine/src/hzpch.cpp"

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.imgui}",
    }

    links{
        "GLFW",
        "Glad",
        "imgui",
        "opengl32.lib"
    }

    postbuildcommands{
        ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
            "EG_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        symbols "On"
        runtime "Debug"
    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "On"
        runtime "Release"
    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "On"
        runtime "Release"
        


project "sandBox"
    location "sandBox"
    kind "ConsoleApp"
    language "C++"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "GameEngine/vendor/spdlog/include",
        "GameEngine/src"
    }

    links {
        "GameEngine"
    }
    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines{

            "EG_PLATFORM_WINDOWS",
        }
    filter "configurations:Debug"
        defines "EG_DEBUG"
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "On"
        runtime "Release"

    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "On"
        runtime "Release"
