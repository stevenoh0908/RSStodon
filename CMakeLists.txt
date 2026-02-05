cmake_minimum_required(VERSION 3.20)
project(RSStodon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(RSStodon src/main.cpp src/rss.cpp src/feed.cpp)

find_package(CURL REQUIRED)
target_link_libraries(RSStodon PRIVATE CURL::libcurl)
find_package(LibXml2 REQUIRED)
target_link_libraries(RSStodon PRIVATE LibXml2::LibXml2)