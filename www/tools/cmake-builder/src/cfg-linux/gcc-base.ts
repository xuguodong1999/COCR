import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvGccKey, EnvKey} from '../ts-cmake/env';
import {
    CMAKE_BIN_DIR,
    CUDAToolkit_ROOT,
    GCC_HOST_ROOT,
    QT_HOST_ROOT as QT_ROOT,
    VK_LAYER_PATH,
    VULKAN_SDK
} from './env';

const CC = path.resolve(GCC_HOST_ROOT, 'bin/gcc');
const CXX = path.resolve(GCC_HOST_ROOT, 'bin/g++');


const PATH = [
    path.resolve(QT_ROOT, 'bin'),
    path.resolve(VULKAN_SDK, 'bin'),
    path.resolve(CUDAToolkit_ROOT, 'bin'),
    path.resolve(GCC_HOST_ROOT, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(QT_ROOT, 'lib'),
    path.resolve(VULKAN_SDK, 'lib'),
    path.resolve(CUDAToolkit_ROOT, 'lib'),
    path.resolve(GCC_HOST_ROOT, 'lib64'),
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: CC,
        cxx: CXX,
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake'),
        ninja: 'ninja',
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/linux-gcc-base',
    env: [
        [EnvGccKey.CC, CC],
        [EnvGccKey.CXX, CXX],
        [EnvKey.VULKAN_SDK, VULKAN_SDK],
        [EnvKey.VK_LAYER_PATH, VK_LAYER_PATH],
        [EnvKey.CUDAToolkit_ROOT, CUDAToolkit_ROOT],
        [EnvKey.PATH, PATH],
        [EnvKey.LD_LIBRARY_PATH, LD_LIBRARY_PATH],
    ],
    config: {
        generator: 'Ninja',
        options: [
            'CMAKE_CUDA_ARCHITECTURES:STRING="86;89"',
            'CMAKE_BUILD_TYPE:STRING=Release',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all']
    },
    // test: {
    //     parallel: true,
    // },
};
export {config};
