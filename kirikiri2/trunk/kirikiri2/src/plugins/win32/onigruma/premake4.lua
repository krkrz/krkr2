--
-- Premake 4.x build configuration script
--

build_base = "./"
project_name = "oniguruma"

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
			"onig/",
		}

		configuration "Debug"
			defines     "_DEBUG"
			flags       { "Symbols" }
			targetsuffix "-d"

		configuration "Release"
			defines     "NDEBUG"
			flags       { "Optimize" }
			targetdir   "../../../../bin/win32/plugin"

		configuration "windows"
			defines {
				"__WIN32__",
				"_CRT_SECURE_NO_WARNINGS",
				"TJS_TEXT_OUT_CRLF",
			}
		configuration {}

	project (project_name)
		targetname  (project_name)
		language    "C++"
		kind        "SharedLib"

		files
		{
			"../tp_stub.cpp",
			"../00_simplebinder/v2link.cpp",
			"Main.cpp",
		}
		links {
			"libonig",
		}

----------------------------------------------------------------
onig_base = "onig/"
	project			("libonig")
		targetname	("libonig")
		language "C++"
		kind     "StaticLib"
		includedirs {
			onig_base
		}
		files {
			onig_base .. "reg*.c",
			onig_base .. "st.c",
			onig_base .. "enc/*.c",
		}
		excludes {
			onig_base .. "enc/mktable.c",
		}
		defines {
			"HAVE_CONFIG_H",
			"NOT_RUBY",
--			"EXPORT",
			"ONIG_EXTERN=extern",
		}

		configuration "windows"
			includedirs {
				onig_base .. "win32",
			}
		configuration {}

