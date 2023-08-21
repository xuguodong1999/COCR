const fs = require('fs');
const path = require('path');

const getDirectories = source => fs.readdirSync(source, {withFileTypes: true})
    .filter(dirent => dirent.isDirectory())
    .map(dirent => dirent.name);

const toDeleteSubDirs = ['node_modules', 'dist', 'dist-qt'];
const frontendRoot = path.resolve(__dirname, '..', 'frontend');
const frontendDirs = getDirectories(frontendRoot);
const toDeleteDir = [path.resolve(__dirname, 'node_modules')];

frontendDirs.forEach((value) => {
    if ('string' !== typeof value) {
        throw new Error(`${value} is not a valid workspace`);
    }
    toDeleteSubDirs.forEach((dir) => {
        toDeleteDir.push(path.resolve(frontendRoot, value, dir));
    })
});

toDeleteDir.forEach((value) => {
    if (fs.existsSync(value) && fs.statSync(value).isDirectory()) {
        console.log(`remove ${value}`);
        fs.rmSync(value, {recursive: true});
    }
});

