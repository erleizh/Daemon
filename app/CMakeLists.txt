cmake_minimum_required(VERSION 3.4.1)

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets/daemon/${ANDROID_ABI})

add_library( # Sets the name of the library.
             native_daemon

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/jni/native_daemon.c
             src/main/jni/daemon.c)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


add_executable( daemon

                src/main/jni/daemon.c)

target_link_libraries( # Specifies the target library.
                       daemon
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_link_libraries( # Specifies the target library.
                       native_daemon
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )