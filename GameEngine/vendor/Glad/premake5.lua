project "Glad"
	kind "StaticLib"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c",
	}


	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
	
	includedirs{
		"include"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"
		buildoptions "/MT"

	filter "configurations:Dist"
		runtime "Release"
		optimize "speed"
		symbols "off"