import * as path from 'path';

export enum EnvKey {
    VULKAN_SDK = 'VULKAN_SDK',
    VK_LAYER_PATH = 'VK_LAYER_PATH',
    CC = 'CC',
    CXX = 'CXX',
    PATH = 'PATH',
    LD_LIBRARY_PATH = 'LD_LIBRARY_PATH',
    CUDAToolkit_ROOT = 'CUDAToolkit_ROOT',
    CMAKE = 'CMAKE',
    CCACHE_DISABLE = 'CCACHE_DISABLE',
    QML_DISABLE_DISK_CACHE = 'QML_DISABLE_DISK_CACHE',
}

export enum AndroidEnvKey {
    JAVA_HOME = 'JAVA_HOME',
    ANDROID_HOME = 'ANDROID_HOME',
}

export enum EnvEmccKey {
    EMSDK = 'EMSDK',
    EMSDK_NODE = 'EMSDK_NODE',
    EMSDK_PYTHON = 'EMSDK_PYTHON',
}

export enum EnvMsvcKey {
    LIB = 'LIB',
    LIB_PATH = 'LIB_PATH',
    INCLUDE = 'INCLUDE',
}

export enum EnvGccKey {
    CXX = 'CXX',
    CC = 'CC',
}

export function generateMsvcEnv(cfg: {
    winKitVersion: string,
    winKitDir: string,
    msvcDir: string,
}) {
    const winKitInclude = path.resolve(cfg.winKitDir, 'include', cfg.winKitVersion);
    const winKitLib = path.resolve(cfg.winKitDir, 'lib', cfg.winKitVersion);
    return {
        LIB: [
            path.resolve(winKitLib, 'um', 'x64'),
            path.resolve(winKitLib, 'ucrt', 'x64'),
            path.resolve(cfg.msvcDir, 'lib', 'x64'),
            path.resolve(cfg.msvcDir, 'lib', 'onecore', 'x64'),
        ].join(path.delimiter),
        INCLUDE: [
            path.resolve(cfg.msvcDir, 'include'),
            path.resolve(winKitInclude, 'ucrt'),
            path.resolve(winKitInclude, 'shared'),
            path.resolve(winKitInclude, 'um'),
            path.resolve(winKitInclude, 'winrt'),
            path.resolve(winKitInclude, 'cppwinrt'),
        ].join(path.delimiter),
        PATH: [
            path.resolve(cfg.msvcDir, 'bin', 'Hostx64', 'x64'),
            path.resolve(cfg.winKitDir, 'bin', cfg.winKitVersion, 'x64'),
            ''
        ].join(path.delimiter),
    };
}
