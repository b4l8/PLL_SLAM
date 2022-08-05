cmake_minimum_required(VERSION 3.16)
project(PLL_SLAM CUDA)

set(CMAKE_CUDA_STANDARD 14)

add_executable(PLL_SLAM main.cu)

set_target_properties(
		PLL_SLAM
		PROPERTIES
		CUDA_SEPARABLE_COMPILATION ON)