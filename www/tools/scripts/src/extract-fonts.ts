import * as fs from 'fs';
import * as path from 'path';

const readFile = (filePath: string) => {
    return fs.readFileSync(path.resolve(__dirname, filePath), {encoding: 'utf8'});
};
const chars1 = readFile('../assets/COCR_zh_CN.ts').split('');
const chars2 = readFile('../assets/datasets/couch-subset/utf8.txt').split('');
const chars3 = Array.from(Array(126 + 32).keys()).slice(32).map((item) => {
    return String.fromCharCode(item);
});

const Fontmin = require('fontmin');
const fontmin = new Fontmin()
    .src(path.resolve('/usr/share/fonts/win10/simfang.ttf'))
    .dest(path.resolve(__dirname, '../assets/fonts/cocr.simfang.ttf'))
    .use(Fontmin.glyph({
        text: [...new Set([...chars1, ...chars2, ...chars3])].join(''),
        hinting: false,
    }));

fontmin.run(function (err: any, files: any) {
    if (err) {
        console.log(err);
    }
});