cmake_minimum_required(VERSION 2.8.0)
project(smoke_trt)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CUDA_COMPILER "/usr/local/cuda-11.6/bin/nvcc")
find_package(CUDA REQUIRED)
find_package(OpenCV REQUIRED)
# find_library(NVINFER NAMES nvinfer)
# find_library(NVINFER_PLUGIN NAMES nvinfer_plugin)
# find_library(NVONNXPARSER nvonnxparser)


include_directories(
        ${CUDA_INCLUDE_DIRS}
        # ~/TensorRT-8.0.3.4/include
        /media/ros/A666B94D66B91F4D/ros/new_deploy/TensorRT-8.4.3.1.Linux.x86_64-gnu.cuda-11.6.cudnn8.4/TensorRT-8.4.3.1/include
        plugins/modulated_deform_conv2
)

link_directories("/usr/local/cuda-11.6/targets/x86_64-linux/lib")
link_directories(
        #${TENSORRT_LIBRARY_DIRS}
        # /usr/local/cuda-11.6/lib64
        /media/ros/A666B94D66B91F4D/ros/new_deploy/TensorRT-8.4.3.1.Linux.x86_64-gnu.cuda-11.6.cudnn8.4/TensorRT-8.4.3.1/lib
        /media/ros/A666B94D66B91F4D/ros/new_deploy/cudnn-linux-x86_64-8.4.1.50_cuda11.6-archive/lib
        # /usr/lib/aarch64-linux-gnu
        # /usr/aarch64-linux-gnu/lib/
)

cuda_add_library(trt_modulated_deform_conv_plugin SHARED
        plugins/modulated_deform_conv2/trt_modulated_deform_conv_kernel.cu
        plugins/modulated_deform_conv2/trt_modulated_deform_conv.cpp
        plugins/modulated_deform_conv2/trt_cuda_helper.cu
        )

target_link_libraries(trt_modulated_deform_conv_plugin libnvinfer.so cublas ${CUDA_LIBRARIES})

add_executable(${PROJECT_NAME}
        smoke.cpp
        main.cpp
        )

target_link_libraries(${PROJECT_NAME}
        libnvinfer.so
        libnvonnxparser.so
        libnvinfer_plugin.so
        ${CUDA_LIBRARIES}
        ${OpenCV_LIBRARIES}
        trt_modulated_deform_conv_plugin
        )
