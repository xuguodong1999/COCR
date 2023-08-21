import * as fs from 'fs';
import * as path from 'path';
import yargs from 'yargs';
import {hideBin} from 'yargs/helpers';

function convertDumpbinLogToCMake(buffer: string) {
    const lines = buffer.split(/[(\r\n)\r\n]+/);
    const section = 'Dump of file RDKi';
    let lastModule = '';
    let lastModuleDeps = [];
    for (let line of lines) {
        line = line.trim();
        if (line.startsWith(section)) {
            const tokens = line.split(/[\s|-]+/);
            if (lastModule.length > 0 && lastModuleDeps.length > 0) {
                console.log(`xgd_link_rdkit(${lastModule.replace('RDKit', 'rdkit_')} COMPONENTS ${lastModuleDeps.join(' ')})`);
            }
            lastModuleDeps = [];
            lastModule = tokens[3];
        }
        if (line.endsWith('.dll') && line.startsWith('RDKit')) {
            const tokens = line.split('-');
            lastModuleDeps.push(tokens[0].replace('RDKit', ''));
        }
    }
}

function main() {
    const argv = yargs(hideBin(process.argv)).options({
        logfile: {
            description: 'need an input file from dumpbin of all rdkit libraries. Hint: conda install rdkit -c rdkit, run "dumpbin /dependents *.dll > D:/log.txt" under Lib/site-packages/rdkit.libs, use "D:/log.txt" as input file',
            type: 'string',
        }
    }).demandOption(
        ['logfile']
    ).usage(
        'Example: ts-node ./scripts/extract-rdkit-module-graph.ts --logfile D:/log.txt'
    ).parseSync();
    const logfile = path.resolve(argv.logfile);
    if (fs.existsSync(logfile) && fs.statSync(logfile).isFile()) {
        const buffer = fs.readFileSync(logfile);
        convertDumpbinLogToCMake(buffer.toString());
    } else {
        throw new Error(`"${logfile}" is not a valid log file`);
    }
}

main();