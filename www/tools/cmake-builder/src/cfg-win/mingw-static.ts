import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvGccKey, EnvKey} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, GCC_HOST_ROOT, NINJA_BIN_DIR, QT_HOST_ROOT as QT_ROOT, VK_LAYER_PATH, VULKAN_SDK} from './env';

const CC = path.resolve(GCC_HOST_ROOT, 'bin/gcc.exe');
const CXX = path.resolve(GCC_HOST_ROOT, 'bin/g++.exe');

const PATH = [
    path.resolve(QT_ROOT, 'bin'),
    path.resolve(VULKAN_SDK, 'bin'),
    path.resolve(GCC_HOST_ROOT, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: CC,
        cxx: CXX,
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake.exe'),
        ninja: path.resolve(NINJA_BIN_DIR, 'ninja.exe'),
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest.exe'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/win-mingw-static',
    env: [
        [EnvGccKey.CC, CC],
        [EnvGccKey.CXX, CXX],
        [EnvKey.VULKAN_SDK, VULKAN_SDK],
        [EnvKey.VK_LAYER_PATH, VK_LAYER_PATH],
        [EnvKey.PATH, PATH],
    ],
    config: {
        generator: 'Ninja',
        options: [
            'XGD_NO_DEBUG_CONSOLE=ON',
            'XGD_USE_VK=OFF',
            'XGD_OPT_RC=OFF', // windres error: The command line is too long
            'CMAKE_BUILD_TYPE:STRING=Release',
            `CMAKE_PREFIX_PATH:PATH=${QT_ROOT}`,
            'BUILD_SHARED_LIBS:BOOL=OFF',
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
