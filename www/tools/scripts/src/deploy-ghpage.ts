import * as fs from 'fs';
import * as path from 'path';

function copy(from: string, to: string) {
    if (!fs.existsSync(from)) {
        console.error(`from: ${from} not exist`);
        return;
    }
    if (!fs.existsSync(to)) {
        console.error(`to: ${to} not exist`);
        return;
    }
    try {
        fs.cpSync(from, to, {recursive: true});
    } catch (e) {
        console.error(`copy from ${from} to ${to} failed, msg:\n${e}`);
    }
}

function main() {
    const homepageDist = path.resolve(__dirname, '../../../packages/playground-cocr/dist');
    const githubPageRoot = path.resolve(__dirname, '../../../../../gh-page');
    copy(homepageDist, githubPageRoot);
}

main();