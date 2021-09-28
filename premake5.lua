
solution 'AutoPlay'
    architecture 'x86_64'
    startproject 'envi'

    configurations {
        'Debug',
        'Release'
    }

    bindir = '%{wks.location}/bin'

include 'autoplay/vendor/envi'
include 'autoplay/vendor/jsoncpp'
include 'autoplay'
