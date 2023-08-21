import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvKey, EnvMsvcKey, generateMsvcEnv} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, CUDAToolkit_ROOT, NINJA_BIN_DIR, VK_LAYER_PATH, VULKAN_SDK} from './env';

const env = generateMsvcEnv({
    winKitVersion: '10.0.22000.0',
    winKitDir: path.resolve('C:/Program Files (x86)/Windows Kits/10'),
    msvcDir: path.resolve('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.35.32215'),
});

const QT_ROOT = path.resolve('C:/Shared/Qt/6.5.2/msvc2022_64_static_mt');

const PATH = env.PATH.concat([
    path.resolve(QT_ROOT, 'bin'),
    path.resolve(VULKAN_SDK, 'bin'),
    path.resolve(CUDAToolkit_ROOT, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter));

const config: ICMakeBuildConfig = {
    bin: {
        c: 'cl',
        cxx: 'cl',
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake.exe'),
        ninja: path.resolve(NINJA_BIN_DIR, 'ninja.exe'),
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest.exe'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/win-msvc-static',
    env: [
        [EnvKey.VULKAN_SDK, VULKAN_SDK],
        [EnvKey.VK_LAYER_PATH, VK_LAYER_PATH],
        [EnvKey.CUDAToolkit_ROOT, CUDAToolkit_ROOT],
        [EnvKey.PATH, PATH],
        [EnvMsvcKey.LIB, env.LIB],
        [EnvMsvcKey.LIB_PATH, env.LIB],
        [EnvMsvcKey.INCLUDE, env.INCLUDE],
    ],
    config: {
        generator: 'Ninja',
        options: [
            'XGD_NO_DEBUG_CONSOLE=ON',
            `CMAKE_PREFIX_PATH:PATH=${QT_ROOT}`,
            'CMAKE_CUDA_ARCHITECTURES:STRING="86;89"',
            'CMAKE_MSVC_RUNTIME_LIBRARY:STRING=MultiThreaded',
            'BUILD_SHARED_LIBS:BOOL=OFF',
            'CMAKE_BUILD_TYPE:STRING=Release',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all']
    },
    test: {
        parallel: true,
    },
};
export {config};
