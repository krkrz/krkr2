--
-- Premake 4.x build configuration script
--

build_base = "./"
project_name = "DrawDeviceZ"

	solution (project_name)
		configurations { "Release", "Debug" }
		location   (build_base .. _ACTION)
		objdir     (build_base .. _ACTION .. "/obj")
		targetdir  ("bin")
		flags
		{
			"No64BitChecks",
--			"ExtraWarnings",
			"StaticRuntime",	-- set /MT(d) option.

			"NoManifest",		-- Prevent the generation of a manifest.
			"NoMinimalRebuild",	-- Disable minimal rebuild.
--			"NoIncrementalLink",	-- Disable incremental linking.
			"NoPCH",		-- Disable precompiled header support.
		}
		includedirs
		{
			"../",
			"../00_simplebinder/",
			"fakeFiles/",
			"drawdeviceZ/",
		}

		configuration "Debug"
			defines     "_DEBUG"
			flags       { "Symbols" }
			targetsuffix "-d"

		configuration "Release"
			defines     "NDEBUG"
			flags       { "Optimize" }
			targetdir   "../../../../bin/win32/plugin"

	project (project_name)
		targetname  (project_name)
		language    "C++"
		kind        "SharedLib"

		files
		{
			"../tp_stub.cpp",
			"../00_simplebinder/v2link.cpp",
			"fakeFiles/DrawDevice.cpp",
			"Main.cpp",
			"errmsgs.cpp",
		}
		links {
			"d3d9",
			"dxguid",
		}
