cmake_minimum_required(VERSION 3.20)
project(RSStodon)

set(CMAKE_CXX_STANDARD 17)

add_executable(RSStodon src/main.c src/rss.c)

find_package(CURL REQUIRED)
target_link_libraries(RSStodon PRIVATE CURL::libcurl)
find_package(LibXml2 REQUIRED)
target_link_libraries(RSStodon PRIVATE LibXml2::LibXml2)