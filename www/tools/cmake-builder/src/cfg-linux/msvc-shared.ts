import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvKey} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, GCC_HOST_ROOT} from './env';

const MSVC_ROOT = path.resolve(os.homedir(), 'shared/msvc');
const MSVC_BIN_ROOT = path.resolve(MSVC_ROOT, 'bin/x64');

const WINE64_ROOT = path.resolve(os.homedir(), 'shared/wine64');
const XGD_WINE64_RUNTIME = path.resolve(WINE64_ROOT, 'wine64');
const QT_ROOT = path.resolve(os.homedir(), 'shared/Qt/6.5.2/msvc2019_64');

const PATH = [
    WINE64_ROOT,
    MSVC_BIN_ROOT,
    path.resolve(QT_ROOT, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(GCC_HOST_ROOT, 'lib64'),
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const CXX = path.resolve(MSVC_BIN_ROOT, 'cl');
const config: ICMakeBuildConfig = {
    bin: {
        // mxe toolchain override CC and CXX
        // c: CXX,
        // cxx: CXX,
        cmake: path.resolve(CMAKE_BIN_DIR, 'wine-cmake'),
        // ninja: 'ninja',
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/win-msvc-shared',
    env: [
        [EnvKey.CC, CXX],
        [EnvKey.CXX, CXX],
        [EnvKey.PATH, PATH],
        [EnvKey.LD_LIBRARY_PATH, LD_LIBRARY_PATH],
        [EnvKey.CCACHE_DISABLE, '1'],
    ],
    config: {
        generator: 'Unix Makefiles',
        options: [
            'BUILD_SHARED_LIBS:BOOL=ON',
            `XGD_OPT_RC=OFF`,
            `CMAKE_PREFIX_PATH=${QT_ROOT}`,
            'CMAKE_SYSTEM_NAME=Windows',
            'CMAKE_BUILD_TYPE:STRING=Release',
            `XGD_WINE64_RUNTIME:FILEPATH=${XGD_WINE64_RUNTIME}`,
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['COCR'],
    }
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
