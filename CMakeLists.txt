cmake_minimum_required(VERSION 3.20)

# set project name
project(kazen)

# find packages under rez-env
# find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(TBB REQUIRED)
find_package(OpenImageIO REQUIRED)
# find_package(embree 3.13.0 REQUIRED) # Here should point a spesific version
find_package(fmt 7.1.3 REQUIRED)
find_package(pugixml 1.11 REQUIRED)


# Set platform-specific flags
if (WIN32)
  add_definitions(-D__WINDOWS__)
elseif(UNIX)
  if(APPLE)
    add_definitions(-D__OSX__)
  else()
    add_definitions(-D__LINUX__)
  endif()
endif()


# C++17
include(CheckCXXCompilerFlag)
if (CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|Emscripten|Intel)$")
  CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP17_FLAG)

  if (HAS_CPP17_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  else()
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" HAS_CPP1Z_FLAG)
    if (HAS_CPP1Z_FLAG)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    else()
      message(FATAL_ERROR "Unsupported compiler -- nanogui requires C++17 support!")
    endif()
  endif()
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()



# enoki
add_subdirectory($ENV{REZ_ENOKI_ROOT} ext_build)
enoki_set_compile_flags()
enoki_set_native_flags()
# include_directories($ENV{REZ_ENOKI_ROOT}/include)

# Add executable
add_executable(kazen 
    # headers
    include/kazen/bbox.h
    include/kazen/bitmap.h
    include/kazen/block.h
    include/kazen/color.h
    include/kazen/common.h
    include/kazen/define.h
    include/kazen/dpdf.h
    include/kazen/frame.h
    include/kazen/object.h
    include/kazen/parser.h
    include/kazen/proplist.h
    include/kazen/progress.h
    include/kazen/ray.h
    include/kazen/renderer.h
    include/kazen/rfilter.h  
    include/kazen/timer.h
    include/kazen/vector.h
    include/kazen/transform.h
    include/kazen/warp.h

    # source code
    src/kazen/bitmap.cpp
    src/kazen/block.cpp
    src/kazen/common.cpp
    src/kazen/object.cpp
    src/kazen/parser.cpp
    src/kazen/progress.cpp
    src/kazen/proplist.cpp
    src/kazen/renderer.cpp
    src/kazen/rfilter.cpp
    src/kazen/sampler.cpp

    # main.cpp
    src/kazen/main.cpp
)

# link against target
target_link_libraries(kazen PUBLIC
    # Boost::filesystem
    TBB::tbb
    OpenImageIO::OpenImageIO
    # embree
    fmt::fmt
    pugixml::pugixml
)

# header only ext
target_include_directories(kazen PUBLIC
    ## kazen include files ##
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    
    # ## enoki ##
    $ENV{REZ_ENOKI_ROOT}/include

    # ## tbb ##
    $ENV{REZ_TBB_ROOT}/include
)


target_compile_features(kazen PUBLIC cxx_std_17)