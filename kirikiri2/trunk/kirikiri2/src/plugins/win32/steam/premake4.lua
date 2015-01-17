--
-- Premake 4.x build configuration script
--

build_base = "./"
project_name = "krkrsteam"

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
			"$(STEAMWORKS_SDK)/public/steam",
			"..",
			"../ncbind",
			"./",
		}

    libdirs { "$(STEAMWORKS_SDK)/redistributable_bin/" }

		configuration "Debug"
			defines     "_DEBUG"
			flags       { "Symbols" }
			targetsuffix "-d"

		configuration "Release"
			defines     "NDEBUG"
			flags       { "Optimize" }

	project (project_name)
		targetname  (project_name)
		language    "C++"
		kind        "SharedLib"

		files
		{
			"../tp_stub.cpp",
			"../ncbind/ncbind.cpp",
			"Main.cpp",
			"Storages.cpp",
			"SteamAchievements.cpp",
		}

