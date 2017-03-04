
cmake_minimum_required( VERSION 2.8 )


project( TestCARRTonLinux )

set( CMAKE_BUILD_TYPE Release )

include_directories( "../.." )


set( CarrtSrcsToTestOnLinux
        ../../NavigationMap.cpp
    )


add_definitions( -DCARRT_INCLUDE_NAVMAP_IN_BUILD=1 )
add_definitions( -DkNavigationMapSizeRealWorldUnits=80 )


add_executable( NavMapLinuxTest LinuxNavMapTest.cpp ${CarrtSrcsToTestOnLinux} )


