workspace "CG-Sandbox"
	architecture "x86_64"
	configurations{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "CGSandbox"
	flags
	{
		"MultiProcessorCompile"
	}


outputdir="%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"
--include directories relative to root folder 


IncludeDir={}
IncludeDir["extern"]="CGCore/extern/"

group "Depencencies"
	include "CGCore/extern/"
group ""

project "CGCore"
	location "CGCore"
	kind "StaticLib"
	language "C++"
	cppdialect "c++17"
	systemversion "latest"
	staticruntime "on"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{IncludeDir.extern}/stb_img/**.h",
		"%{IncludeDir.extern}/stb_img/**.cpp"
	}

	includedirs{
		"%{prj.name}/extern/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.extern}/stb_img/",
		"%{IncludeDir.extern}/imgui/",
		"%{IncludeDir.extern}/glfw/include",
		"%{IncludeDir.extern}/Glad/include",
		"%{IncludeDir.extern}/glm",
		"%{IncludeDir.extern}/tinyobjloader",
		"%{IncludeDir.extern}/entt/single_include"
	}
	links{
		"glfw",
		"Glad",
		"imgui",
		"opengl32.lib"
	}
	pchheader "pch.h"
	pchsource "CGCore/src/pch.cpp"

	filter "system:windows"
		defines{
		"_CRT_SECURE_NO_WARNINGS"
		}
	filter "configurations:Debug"
		defines {"CG_DEBUG"}
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines "CG_RELEASE"
		runtime "Release"
		optimize "On"
	filter "configurations:Dist"
		defines "CG_DIST"
		runtime "Release"
		optimize "On"

project "CGSandbox"
	location "CGSandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++17"
	systemversion "latest"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"CGCore/src",
		"CGCore/extern/spdlog/include",
		"%{IncludeDir.extern}/stb_img/",
		"%{IncludeDir.extern}/imgui/",
		"%{IncludeDir.extern}/glfw/include",
		"%{IncludeDir.extern}/Glad/include",
		"%{IncludeDir.extern}/glm",
		"%{IncludeDir.extern}/tinyobjloader",
		"%{IncludeDir.extern}/entt/single_include"

	}
	links{
		"CGCore"
	}
		filter "system:windows"
		defines{
		"_CRT_SECURE_NO_WARNINGS"
		}
	filter "configurations:Debug"
		defines {"CG_DEBUG"}
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines "CG_RELEASE"
		runtime "Release"
		optimize "On"
	filter "configurations:Dist"
		defines "CG_DIST"
		runtime "Release"
		optimize "On"
project "CGEditor"
	location "CGEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++17"
	systemversion "latest"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"CGCore/src",
		"CGCore/extern/spdlog/include",
		"%{IncludeDir.extern}/stb_img/",
		"%{IncludeDir.extern}/imgui/",
		"%{IncludeDir.extern}/glfw/include",
		"%{IncludeDir.extern}/Glad/include",
		"%{IncludeDir.extern}/glm",
		"%{IncludeDir.extern}/tinyobjloader",
		"%{IncludeDir.extern}/entt/single_include"

	}
	links{
		"CGCore"
	}
		filter "system:windows"
		defines{
		"_CRT_SECURE_NO_WARNINGS"
		}
	filter "configurations:Debug"
		defines {"CG_DEBUG"}
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines "CG_RELEASE"
		runtime "Release"
		optimize "On"
	filter "configurations:Dist"
		defines "CG_DIST"
		runtime "Release"
		optimize "On"
