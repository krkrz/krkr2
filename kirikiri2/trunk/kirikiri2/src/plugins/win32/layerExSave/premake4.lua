--
-- Premake 4.x build configuration script
--

build_base = "./"
project_name = "layerExSave"

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
			"../../../tools/win32/krdevui/tpc",
			"../ncbind",
			"../zlib",
			"../",
			"./LodePNG",
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
			"../tp_stub.*",
			"../ncbind/ncbind.cpp",
			"../../../tools/win32/krdevui/tpc/tlg5/slide.cpp",
			"../../../tools/win32/krdevui/tpc/tlg5/slide.h",

			"Main.cpp",
			"savepng.cpp",
			"savetlg5.cpp",
			"utils.cpp",

			"LodePNG/lodepng.cpp",
		}

		defines
		{
--			"LAYEREXSAVE_DISABLE_LODEPNG=1",	-- purge LodePNG setting
		}

		links {
			"deflate",
		}

----------------------------------------------------------------
zlib_base = "../zlib/"
	project			("deflate")
		targetname	("deflate")
		language "C++"
		kind     "StaticLib"
		includedirs {
			zlib_base
		}
		files {
			zlib_base .. "adler32.c",
			zlib_base .. "compress.c",
			zlib_base .. "crc32.c",
			zlib_base .. "deflate.c",
			zlib_base .. "trees.c",
			zlib_base .. "zutil.c",
		}
