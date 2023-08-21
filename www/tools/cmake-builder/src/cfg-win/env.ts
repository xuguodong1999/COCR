import * as path from 'path';

const CUDAToolkit_ROOT = path.resolve('C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8');

const VULKAN_SDK = path.resolve('C:/Shared/VulkanSDK/1.3.231.1');
const VK_LAYER_PATH = path.resolve(VULKAN_SDK, 'Bin');

const CMAKE_BIN_DIR = path.resolve('C:/Shared/cmake/bin');
const NINJA_BIN_DIR = path.resolve('C:/Shared/cmake/bin');

const GCC_HOST_ROOT = path.resolve('C:/Shared/Qt/Tools/mingw900_64');
const QT_HOST_ROOT = path.resolve('C:/Shared/Qt/6.5.2/mingw_64_static');

export {
    CUDAToolkit_ROOT,
    VULKAN_SDK,
    VK_LAYER_PATH,
    CMAKE_BIN_DIR,
    NINJA_BIN_DIR,
    QT_HOST_ROOT,
    GCC_HOST_ROOT,
};