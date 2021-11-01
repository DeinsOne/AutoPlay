
solution 'AutoPlay'
    architecture 'x86_64'
    startproject 'envi'

    configurations {
        'Debug',
        'Release'
    }

    bindir = '%{wks.location}/bin'

include 'opencv.lua'
include 'autoplay/vendor/envi'
include 'autoplay/vendor/jsoncpp'
include 'autoplay/vendor/spdlog'
include 'autoplay'
