
project 'spdlog'
    language 'C++'
    kind 'SharedLib'
    cppdialect 'C++17'
    staticruntime 'on'

    objdir      (bindir)
    location    (bindir)
    targetdir   (bindir .. ('/%{cfg.system}-%{cfg.buildcfg}'):lower() )

    includedirs {
        'include/'
    }

    files {
        'src/*.cpp'
    }

    defines {
        'SPDLOG_COMPILED_LIB'
    }

    filter 'system:linux'
        linkoptions {
            '-fpermissive'
        }

        links {
            'pthread'
        }

    filter 'configurations:Debug'
		runtime 'Debug'
		symbols 'on'

	filter 'configurations:Release'
		runtime 'Release'
		optimize 'on'
 
