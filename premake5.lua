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
IncludeDir["glm"]="GameEngine/vendor/glm"
IncludeDir["stb"]="GameEngine/vendor/stb_image"

include "GameEngine/vendor/GLFW"
include "GameEngine/vendor/Glad"
include "GameEngine/vendor/imgui"

project "GameEngine"
    location "GameEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"


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
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
    }

    links{
        "GLFW",
        "Glad",
        "imgui",
        "opengl32.lib"
    }

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
    filter "configurations:Release"
        defines "EG_RELEASE"
        optimize "on"
        runtime "Release"
    filter "configurations:Dist"
        defines "EG_DIST"
        optimize "on"
        runtime "Release"
        


project "Engine-Editor"
    location "Engine-Editor"
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
        "%{IncludeDir.imgui}"

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
        "%{IncludeDir.imgui}"

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

