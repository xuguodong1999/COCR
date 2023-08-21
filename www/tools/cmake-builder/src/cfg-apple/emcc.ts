import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvEmccKey, EnvKey} from '../ts-cmake/env';


const QT_ROOT = path.resolve(os.homedir(), 'shared/Qt/6.5.2/wasm_32');
const EMSDK = path.resolve('/opt/homebrew/Cellar/emscripten/3.1.44/libexec');
const EMSDK_NODE = path.resolve('/opt/homebrew/opt/node@18/bin/node');
const EMSDK_PYTHON = 'python3';
const LLVM_INSTALL_DIR = path.resolve(os.homedir(), 'shared/llvm');
const CLANG_BIN_DIR = path.resolve(LLVM_INSTALL_DIR, 'bin');

const PATH = [
    EMSDK,
    CLANG_BIN_DIR,
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: undefined,
        cxx: undefined,
        cmake: 'cmake',
        ninja: 'ninja',
        ctest: 'ctest',
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/wasm-simd-mt',
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
            `CMAKE_TOOLCHAIN_FILE:FILEPATH=${path.resolve(QT_ROOT, 'lib/cmake/Qt6/qt.toolchain.cmake')}`,
            '-Wno-dev',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all'],
    },
    // test: {
    //     timeout: 15,
    //     parallel: true,
    //     targets: ['all'],
    //     excludes: ['(.*)js(.*)', '(.*)qt(.*)', '(.*)COCR(.*)']
    // },
};
export {config};
