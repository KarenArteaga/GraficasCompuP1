workspace "AdvancedProjectComputerGraphics"
architecture "x64"

configurations {
	"Debug",
	"Release"
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include Directories relative to root folder
includeDir = {}
includeDir["GLFW"] = "AdvancedProjectComputerGraphics/vendor/GLFW/include"
includeDir["Glad"] = "AdvancedProjectComputerGraphics/vendor/Glad/include"
includeDir["glm"] = "AdvancedProjectComputerGraphics/vendor/glm/"

-- ? this will search for lua files
include "AdvancedProjectComputerGraphics/vendor/GLFW"
include "AdvancedProjectComputerGraphics/vendor/Glad" -- Search for lua files

project "AdvancedProjectComputerGraphics"
location "AdvancedProjectComputerGraphics"
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
staticruntime "on"

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

files {
	"%{prj.name}/src/**.h",
	"%{prj.name}/src/**.cpp",
	"%{prj.name}/res/**/**.png",
	"%{prj.name}/res/**/**.glsl",
	"%{prj.name}/res/**/**.vs",
	"%{prj.name}/res/**/**.fs",
	"%{prj.name}/res/**/**.gs",
}

defines {
	"_CRT_SECURE_NO_WARNINGS"
}

includedirs {
	"%{prj.name}/src",
	"%{prj.name}/src/vendor",
	"%{includeDir.GLFW}",
	"%{includeDir.Glad}",
	"%{includeDir.glm}",
}

links {
	"GLFW",
	"Glad",
	"opengl32.lib"
}

filter "system:windows"
staticruntime "On"
systemversion "latest"

defines {
	"GLFW_INCLUDE_NONE"
}

filter "configurations:Debug"
runtime "Debug"
symbols "On"

filter "configurations:Release"
runtime "Release"
symbols "On"
