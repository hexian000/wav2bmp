cmake_minimum_required(VERSION 3.10)
project(wav2bmp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra")

add_executable(wav2bmp
        main.cpp options.hpp wav2bmp.cpp wav2bmp.hpp wav2bmp_error.hpp
        sndfile.cpp sndfile.hpp fft.cpp fft.hpp spectrum.cpp spectrum.hpp thread_pool.cpp thread_pool.hpp options.cpp)
target_link_libraries(wav2bmp
        libboost_program_options.a
        sndfile fftw3 png
        pthread)
