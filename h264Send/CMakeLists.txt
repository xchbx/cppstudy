cmake_minimum_required(VERSION 3.5.0)

project(h264Send)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11")
SET(CMAKE_SOURCE_DIR .)
SET(CMAKE_INSTALL_PREFIX ".")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


set(HEADERS
    .
)

set(SOURCES
    h264bytestreamparser.cpp
    h264nalpacket.cpp
    h264rtppacketizer.cpp
    h264stream.cpp
    h264types.cpp
    InputFile.cpp
    main.cpp
    rtprandom.cpp
    rtprandomrands.cpp
    tcpacceptor.cpp
    tcpconnector.cpp
    tcpstream.cpp
    udpdatagram.cpp
)


INCLUDE_DIRECTORIES(${HEADERS})

ADD_EXECUTABLE(h264Send ${SOURCES})
TARGET_LINK_LIBRARIES(h264Send
                        pthread)
