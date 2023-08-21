import {execSync} from 'child_process';
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';
import {exit} from 'yargs';
import {ICMakeBuildConfig} from './cmake-config';
import {appendCmd} from './utils';

class CMakeBuilder {
    readonly buildDir: string;
    readonly sourceDir: string;
    readonly cfg: ICMakeBuildConfig;

    constructor(cfg: ICMakeBuildConfig) {
        this.buildDir = path.join(__dirname, '../../../../../', cfg.buildDir);
        this.sourceDir = path.resolve(__dirname, '../../../../../', cfg.sourceDir);
        this.cfg = cfg;
    }

    build() {
        this.setupEnvironment({clearCache: true});
        // this.setupEnvironment();
        this.configCMakeProject();
        this.buildCMakeProject();
        // try {
        //     this.testCMakeProject('tests');
        //     this.testCMakeProject('hello_world');
        // } catch {
        // }
    }

    private setupEnvironment(cfg?: {
        clearCache: boolean,
    }) {
        if (this.cfg.env) {
            for (const [key, value] of this.cfg.env) {
                process.env[key] = value;
            }
        }
        if (cfg?.clearCache) {
            const cmakeCacheFile = path.join(this.buildDir, 'CMakeCache.txt');
            if (fs.existsSync(cmakeCacheFile)) {
                fs.rmSync(cmakeCacheFile);
            }
        }
    }

    private testCMakeProject(subDir: string) {
        if (this.cfg.bin.ctest && this.cfg.test) {
            let cmd = this.cfg.bin.ctest;
            cmd = appendCmd(cmd, `--test-dir ${path.join(this.buildDir, subDir)}`);
            cmd = appendCmd(cmd, `--output-on-failure`);
            // cmd = appendCmd(cmd, '--extra-verbose');
            if (this.cfg.test?.parallel) {
                cmd = appendCmd(cmd, `-j ${os.cpus().length}`);
            }
            const timeout = this.cfg.test?.timeout;
            if (timeout) {
                cmd = appendCmd(cmd, `--timeout ${timeout}`);
            }
            const targets = this.cfg.test?.targets;
            if (targets && targets.length > 0 && -1 === targets.indexOf('all')) {
                let buf = `(${targets[0]})`;
                for (let i = 1; i < targets.length; i++) {
                    const target = targets[i];
                    buf += `|(${target})`;
                }
                cmd = appendCmd(cmd, `--tests-regex "${buf}"`);
            }
            const excludes = this.cfg.test?.excludes;
            if (excludes && excludes.length > 0) {
                let buf = `(${excludes[0]})`;
                for (let i = 1; i < excludes.length; i++) {
                    const target = excludes[i];
                    buf += `|(${target})`;
                }
                cmd = appendCmd(cmd, `--exclude-regex "${buf}"`);
            }
            this.exec(cmd);
        }
    }

    private buildCMakeProject() {
        let cmd = this.cfg.bin.cmake;
        cmd = appendCmd(cmd, `--build ${this.buildDir}`);
        const build = this.cfg.build;
        if (build) {
            if (build.parallel) {
                cmd = appendCmd(cmd, '--parallel');
            }
            const options = build.options;
            if (options) {
                for (const entry of options) {
                    cmd = appendCmd(cmd, entry);
                }
            }
            const targets = build.targets;
            if (targets) {
                cmd = appendCmd(cmd, '--target');
                for (const entry of targets) {
                    cmd = appendCmd(cmd, entry);
                }
            }
        }
        this.exec(cmd);
    }

    private configCMakeProject() {
        let cmd = this.cfg.bin.cmake;
        cmd = appendCmd(cmd, `-S ${this.sourceDir}`);
        cmd = appendCmd(cmd, `-B ${this.buildDir}`);
        if (this.cfg.bin.c) {
            cmd = appendCmd(cmd, `-DCMAKE_C_COMPILER=${this.cfg.bin.c}`);
        }
        if (this.cfg.bin.cxx) {
            cmd = appendCmd(cmd, `-DCMAKE_CXX_COMPILER=${this.cfg.bin.cxx}`);
        }
        const generator = this.cfg.config?.generator ?? 'Ninja';
        cmd = appendCmd(cmd, `-G "${generator}"`);
        if (this.cfg.bin.ninja) {
            cmd = appendCmd(cmd, `-DCMAKE_MAKE_PROGRAM=${this.cfg.bin.ninja}`);
        }
        let options = this.cfg.config?.options;
        if (options) {
            for (const entry of options) {
                if (entry.startsWith('-')) {
                    cmd = appendCmd(cmd, entry);
                } else {
                    cmd = appendCmd(cmd, `-D${entry}`);
                }
            }
        }
        this.exec(cmd);
    }

    private exec(cmd: string) {
        console.log(`Run: ${cmd}\n${'-'.repeat(50)}`);
        try {
            execSync(cmd, {stdio: 'inherit', encoding: 'utf-8'});
        } catch (err) {
            exit(-1, new Error(`execSync Failed...${err}`));
        }
    }
}

export {CMakeBuilder};