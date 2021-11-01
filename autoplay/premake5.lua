
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
        'vendor/indicators/include',
        'vendor/spdlog/include',

        '%{opencv.include.cv4}',
        '%{opencv.include.cv2}'
    }

    files {
        'src/Autoplay.cpp',
        'src/AutoplayCmdConfig.cpp',
        'src/AutoplayJsonConfig.cpp',
        'src/AutoplayClient.cpp',
        'src/AutoplayActionTree.cpp',
        'src/AutoplayCmdVisializer.cpp',
        'src/AutoplayVisualProcessor.cpp',
        'src/AutoplayLog.cpp'
    }

    libdirs {
        '%{opencv.lib}'
    }

    links {
        'envi',
        'json',

        '%{opencv.libs.core}',
        '%{opencv.libs.flann}',
        '%{opencv.libs.highgui}',
        '%{opencv.libs.imgproc}',
        '%{opencv.libs.dnn}'
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
            '_DEBUG',
            'APLAY_PROFILING'
        }
		runtime 'Debug'
		symbols 'on'

	filter 'configurations:Release'
		runtime 'Release'
		optimize 'on'
