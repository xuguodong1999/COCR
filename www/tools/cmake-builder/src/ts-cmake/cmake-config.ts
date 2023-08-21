export interface ICMakeBinaryConfig {
    c?: string | 'gcc' | 'clang' | 'cl',
    cxx?: string | 'g++' | 'clang++' | 'cl',
    cmake: string | 'cmake',
    ctest?: string | 'ctest',
    ninja?: string | 'ninja',
}

export interface ICMakeBuildConfig {
    bin: ICMakeBinaryConfig,
    sourceDir: string,
    buildDir: string,

    preEnvCommands?: Array<string>,
    env?: Array<[string, string]>,
    config?: {
        generator?: string,
        options?: Array<string>,
    },
    build?: {
        options?: Array<string>,
        parallel?: boolean,
        targets?: Array<string>,
    },
    test?: {
        commands?: Array<string>,
        targets?: Array<string>,
        excludes?: Array<string>,
        parallel?: boolean,
        // seconds
        timeout?: number,
    }
}

