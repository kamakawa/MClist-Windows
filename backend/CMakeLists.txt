cmake_minimum_required(VERSION 3.24)
project(MClist LANGUAGES CXX)

option(MCLIST_BUILD_GPU "Build CUDA backend" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/EHsc /O2)
else()
  add_compile_options(-O3)
endif()

find_package(GSL REQUIRED)
find_package(OpenMP REQUIRED)

set(MCLIST_COMMON_CPP
  src/anchoring.cpp
  src/anchoring_fg.cpp
  src/anchoring_homeotropic.cpp
  src/anchoring_rp.cpp
  src/anchoring_strong.cpp
  src/evolve.cpp
  src/evolve_electric.cpp
  src/evolve_quench.cpp
  src/evolve_step.cpp
  src/evolve_thermal.cpp
  src/geometry.cpp
  src/geometry_bulk.cpp
  src/geometry_custom.cpp
  src/geometry_slab.cpp
  src/geometry_sphere.cpp
  src/ic.cpp
  src/io.cpp
  src/main.cpp
  src/parameter_order.cpp
  src/potential.cpp
)

add_executable(mc_sim_cpu
  ${MCLIST_COMMON_CPP}
  src/simulator.cpp
)

target_include_directories(mc_sim_cpu PRIVATE include)
target_link_libraries(mc_sim_cpu PRIVATE GSL::gsl GSL::gslcblas OpenMP::OpenMP_CXX)

if(MCLIST_BUILD_GPU)
  set(CMAKE_CUDA_COMPILER "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.2/bin/nvcc.exe")
  enable_language(CUDA)
  set(CMAKE_CUDA_STANDARD 17)
  set(CMAKE_CUDA_STANDARD_REQUIRED ON)

  add_executable(mc_sim
    ${MCLIST_COMMON_CPP}
    src/simulatorGPU.cpp
    src/evolve.cu
    src/evolve_electric.cu
    src/evolve_quench.cu
    src/evolve_step.cu
    src/evolve_thermal.cu
    src/geometry.cu
    src/potential.cu
    src/reduce.cu
    src/stringGPU.cu
  )

  target_include_directories(mc_sim PRIVATE include)
  target_compile_definitions(mc_sim PRIVATE CUDA__="CUDA")
  set_target_properties(mc_sim PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES 86
  )
  target_link_libraries(mc_sim PRIVATE GSL::gsl GSL::gslcblas OpenMP::OpenMP_CXX)
endif()
