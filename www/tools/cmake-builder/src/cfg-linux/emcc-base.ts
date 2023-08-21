import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvEmccKey, EnvKey} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, GCC_HOST_ROOT} from './env';

const EMSDK = path.resolve(os.homedir(), 'git/emsdk');
// const EMSDK_NODE_BIN_DIR = path.resolve(EMSDK, 'node/16.20.0_64bit/bin');
const EMSDK_NODE_BIN_DIR = path.resolve(os.homedir(), 'shared/node');
const EMSDK_NODE = path.resolve(EMSDK_NODE_BIN_DIR, 'node');
const EMSDK_PYTHON = 'python3';
const CLANG_BIN_DIR = path.resolve(EMSDK, 'upstream/bin');
const EMCC_BIN_DIR = path.resolve(EMSDK, 'upstream/emscripten');

const PATH = [
    EMSDK,
    EMSDK_NODE_BIN_DIR,
    CLANG_BIN_DIR,
    CMAKE_BIN_DIR,
    EMCC_BIN_DIR,
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(GCC_HOST_ROOT, 'lib64'),
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: path.resolve(EMCC_BIN_DIR, 'emcc'),
        cxx: path.resolve(EMCC_BIN_DIR, 'em++'),
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake'),
        ninja: 'ninja',
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/wasm-base',
    env: [
        [EnvKey.PATH, PATH],
        [EnvEmccKey.EMSDK, EMSDK],
        [EnvEmccKey.EMSDK_NODE, EMSDK_NODE],
        [EnvEmccKey.EMSDK_PYTHON, EMSDK_PYTHON],
        [EnvKey.LD_LIBRARY_PATH, LD_LIBRARY_PATH],
    ],
    config: {
        generator: 'Ninja',
        options: [
            'BUILD_SHARED_LIBS:BOOL=OFF',
            'CMAKE_BUILD_TYPE:STRING=Release',
            '-Wno-dev',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all'],
    },
};

export {config};
