import * as os from 'os';
import * as path from 'path';

const GCC_HOST_ROOT = path.resolve(os.homedir(), 'shared/gcc-11.3.0');

const LLVM_INSTALL_DIR = path.resolve(os.homedir(), 'shared/llvm');

const CMAKE_BIN_DIR = path.resolve(os.homedir(), 'shared/cmake/bin');

const CUDAToolkit_ROOT = path.resolve('/usr/local/cuda');

const QT_HOST_ROOT = path.resolve(os.homedir(), 'shared/Qt/6.5.2/gcc_64');

const VULKAN_SDK = path.resolve(os.homedir(), 'shared/vulkan-sdk/x86_64');
const VK_LAYER_PATH = path.resolve(VULKAN_SDK, 'etc/vulkan/explicit_layer.d');

export {
    GCC_HOST_ROOT,
    LLVM_INSTALL_DIR,
    CMAKE_BIN_DIR,
    CUDAToolkit_ROOT,
    QT_HOST_ROOT,
    VULKAN_SDK,
    VK_LAYER_PATH,
};