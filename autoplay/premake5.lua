
project 'autoplay'
    language 'C++'
    kind 'ConsoleApp'
    cppdialect 'C++17'
    staticruntime 'on'

    objdir      (bindir)
    location    (bindir)
    targetdir   (bindir .. ('/%{cfg.system}-%{cfg.buildcfg}'):lower() )

    includedirs {
        'include',
        'vendor/envi/include',
        'vendor/cxxopts',
        'vendor/jsoncpp/include',
        'vendor/valijson',
        'vendor/indicators/include'
    }

    files {
        'src/Autoplay.cpp',
        'src/AutoplayCmdConfig.cpp',
        'src/AutoplayJsonConfig.cpp',
        'src/AutoplayClient.cpp',
        'src/AutoplayActionTree.cpp',
        'src/AutoplayCmdVisializer.cpp'
    }

    links {
        'envi',
        'json'
    }

    filter 'system:linux'
        linkoptions {
            '-fpermissive'
        }

        links {
            'pthread'
        }

    filter 'configurations:Debug'
        defines {
            '_DEBUG'
        }
		runtime 'Debug'
		symbols 'on'

	filter 'configurations:Release'
		runtime 'Release'
		optimize 'on'
