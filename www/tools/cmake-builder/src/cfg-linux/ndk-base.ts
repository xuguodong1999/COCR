import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {AndroidEnvKey, EnvKey} from '../ts-cmake/env';
import {GCC_HOST_ROOT, QT_HOST_ROOT} from './env';

const ANDROID_PLATFORM = '24';
const ANDROID_ABI = 'arm64-v8a';
const NDK_VERSION = '25.1.8937393';
const ANDROID_HOME = path.resolve(os.homedir(), 'Android/Sdk');
const ANDROID_NDK = path.resolve(ANDROID_HOME, 'ndk', NDK_VERSION);

const QT_ROOT = path.resolve(
    os.homedir(),
    `shared/Qt/6.5.2/android_${ANDROID_ABI.replace('-', '_')}`);

const CMAKE_BIN_DIR = path.resolve(os.homedir(), 'shared/cmake/bin');
const JAVA_HOME = path.resolve(os.homedir(), 'shared/jdk11');

const PATH = [
    path.resolve(QT_HOST_ROOT, 'bin'),
    path.resolve(JAVA_HOME, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(GCC_HOST_ROOT, 'lib64'),
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: undefined,
        cxx: undefined,
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake'),
        ninja: 'ninja',
        ctest: undefined,
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/ndk-base',
    env: [
        [EnvKey.PATH, PATH],
        [EnvKey.LD_LIBRARY_PATH, LD_LIBRARY_PATH],
        [AndroidEnvKey.JAVA_HOME, JAVA_HOME],
        [AndroidEnvKey.ANDROID_HOME, ANDROID_HOME],
    ],
    config: {
        generator: 'Ninja',
        options: [
            `CMAKE_TOOLCHAIN_FILE:FILEPATH=${path.resolve(ANDROID_NDK, 'build/cmake/android.toolchain.cmake')}`,
            `QT_HOST_PATH:PATH=${QT_HOST_ROOT}`,
            `CMAKE_PREFIX_PATH:PATH=${QT_ROOT}`,
            `CMAKE_FIND_ROOT_PATH:PATH=${QT_ROOT}`,
            `ANDROID_SDK_ROOT:PATH=${ANDROID_HOME}`,
            `ANDROID_ABI:STRING=${ANDROID_ABI}`,
            `ANDROID_NDK:PATH=${ANDROID_NDK}`,
            `ANDROID_PLATFORM:STRING=${ANDROID_PLATFORM}`,
            'ANDROID_STL=c++_shared',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['COCR'],
    },
};
export {config};
