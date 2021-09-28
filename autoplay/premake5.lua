
project 'autoplay'
    language 'C++'
    kind 'ConsoleApp'
    cppdialect 'C++17'
    staticruntime 'on'

    objdir      (bindir)
    location    (bindir)
    targetdir   (bindir .. ('/%{cfg.system}-%{cfg.buildcfg}'):lower() )

    includedirs {
        'vendor/envi/include',
        'vendor/cxxopts',
        'vendor/jsoncpp/include'
    }

    files {
        'src/autoplay.cpp'
    }

    links {
        'envi',
        'json'
    }

    filter 'system:linux'
        links {
            'pthread'
        }

    filter 'configurations:Debug'
		runtime 'Debug'
		symbols 'on'

	filter 'configurations:Release'
		runtime 'Release'
		optimize 'on'
