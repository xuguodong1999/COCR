import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvKey} from '../ts-cmake/env';
import {GCC_HOST_ROOT} from './env';

const MXE_ROOT = path.resolve(os.homedir(), 'git/mxe');
const MXE_BIN_ROOT = path.resolve(MXE_ROOT, 'usr/x86_64-pc-linux-gnu/bin');

const NINJA = path.resolve(MXE_BIN_ROOT, 'ninja');
const CTEST = path.resolve(MXE_BIN_ROOT, 'ctest');
const CMAKE = path.resolve(MXE_ROOT, 'usr/bin/x86_64-w64-mingw32.static-cmake');
const WINE64_ROOT = path.resolve(os.homedir(), 'shared/wine64');
const XGD_WINE64_RUNTIME = path.resolve(WINE64_ROOT, 'wine64');
const PATH = [
    path.resolve(MXE_ROOT, 'usr/bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(GCC_HOST_ROOT, 'lib64'),
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        // mxe toolchain override CC and CXX
        c: undefined,
        cxx: undefined,
        cmake: CMAKE,
        ninja: NINJA,
        ctest: CTEST,
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/win-mxe-w64',
    env: [
        [EnvKey.PATH, PATH],
        [EnvKey.LD_LIBRARY_PATH, LD_LIBRARY_PATH],
        // [EnvKey.CCACHE_DISABLE, '1'],
    ],
    config: {
        generator: 'Ninja',
        options: [
            'BUILD_SHARED_LIBS:BOOL=OFF',
            'CMAKE_BUILD_TYPE:STRING=Release',
            `XGD_WINE64_RUNTIME:FILEPATH=${XGD_WINE64_RUNTIME}`,
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all'],
    },
    // test: {
    //     parallel: true,
    // },
};

if (!fs.existsSync(XGD_WINE64_RUNTIME)) {
    // echo "\$1" > ~/build/wine64/wine64
    throw new Error(`
    "${XGD_WINE64_RUNTIME}" not found.
    If you are using linux, build wine64 from source.
    If you are using wsl, mock wine64 with "echo "\\$1" > ${XGD_WINE64_RUNTIME}", translating "wine64 a.exe" into "a.exe"`);
}

export {config};
