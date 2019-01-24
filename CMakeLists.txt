# Arbitrarily chosen minimum.  Latest (as of Jan 20, 2019) is 3.13
cmake_minimum_required(VERSION 3.5)
project(sdltestgame CXX)

find_package(SDL2 REQUIRED)

add_executable(game main.cpp)
target_link_libraries(game
  PRIVATE
  SDL2::SDL2

  # FIXME: This only works on GNU/Linux machines where all the
  # libraries get installed to the same place
  SDL2_image
  )
