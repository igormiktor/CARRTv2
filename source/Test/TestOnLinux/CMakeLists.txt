
cmake_minimum_required( VERSION 2.8 )


project( TestCARRTonLinux )

set( CMAKE_BUILD_TYPE Release )

include_directories( "../.." )


set( CarrtSrcsToTestOnLinux
        ../../NavigationMap.cpp
        ../../PathSearch/ExploredList.cpp
        ../../PathSearch/FrontierList.cpp
        ../../PathSearch/Path.cpp
        ../../PathSearch/PathFinder.cpp
        ../../PathSearch/PathFinderMap.cpp
    )


add_definitions( -DCARRT_INCLUDE_NAVMAP_IN_BUILD=1 )
add_definitions( -DCARRT_ENABLE_LINUX_PATHFINDER_DEBUG=1 )

# add_definitions( -kCarrtNavigationMapGridSize=32 )


add_executable( NavMapLinuxTest LinuxNavMapTest.cpp ${CarrtSrcsToTestOnLinux} )

add_executable( VertexListsTest LinuxVertexListsTest.cpp ${CarrtSrcsToTestOnLinux} )

add_executable( PathTest LinuxPathTest.cpp ${CarrtSrcsToTestOnLinux} )

#add_executable( PathFinderTestBig LinuxPathFinderTest.cpp ${CarrtSrcsToTestOnLinux} )
#set_target_properties( PathFinderTestBig PROPERTIES COMPILE_DEFINITIONS "kCarrtNavigationMapGridSize=80;MAP=3" )

add_executable( PathFinderTestA LinuxPathFinderTest.cpp ${CarrtSrcsToTestOnLinux} )
set_target_properties( PathFinderTestA PROPERTIES COMPILE_DEFINITIONS "kCarrtNavigationMapGridSize=32" )

