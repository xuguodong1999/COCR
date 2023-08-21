import * as fs from 'fs';
import * as path from 'path';
import yargs from 'yargs';
import {hideBin} from 'yargs/helpers';
import {CMakeBuilder} from './cmake-builder';
import {ICMakeBuildConfig} from './cmake-config';

function main() {
    const argv = yargs(hideBin(process.argv)).options({
        config: {
            description: 'Path to a typescript file containing CMake build config.',
            type: 'string',
        }
    }).demandOption(
        ['config']
    ).usage(
        'Example: ts-node ./src/ts-cmake/cmake-build.ts --config ./src/cfg-linux/config/gcc.shared.ts'
    ).parseSync();
    const cfgFilePath = path.resolve(argv.config as string);
    if (fs.existsSync(cfgFilePath) && fs.statSync(cfgFilePath).isFile()) {
        import(cfgFilePath).then((value: { config: ICMakeBuildConfig }) => {
            const config = value.config;
            // console.log(`Use config: ${JSON.stringify(config, null, 2)}`);
            const builder = new CMakeBuilder(config);
            builder.build();
        }, (reason) => {
            throw new Error(`"${cfgFilePath}" failed to import, reason: ${reason}`);
        });
    } else {
        throw new Error(`"${cfgFilePath}" is not a valid ts file`);
    }
}

main();
