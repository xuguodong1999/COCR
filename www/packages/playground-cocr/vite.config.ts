import * as path from 'path';
import {defineConfig} from 'vite';
import {viteStaticCopy} from 'vite-plugin-static-copy';

const COCR_BUILD_DIR = '../../../cmake-build-all/wasm-simd-mt/bin/COCR';
const qtCOIHandler = (content: Buffer, filename: string) => {
    if (-1 === filename.indexOf('.html')) {
        return content;
    }
    const bodyBeg = `<body onload="init()">`;
    const workerInitScript = `<script src="/coi-serviceworker.min.js"></script>`;
    return Buffer.from(content.toString().replace(bodyBeg, `${bodyBeg}\n    ${workerInitScript}`));
};
// https://vitejs.dev/config/
export default defineConfig({
    build: {
        target: 'es6',
        lib: {
            entry: path.resolve(__dirname, 'index.ts'),
            formats: ['es', 'cjs'],
            fileName: 'index',
        },
    },
    plugins: [
        viteStaticCopy({
            targets: [{
                src: './node_modules/coi-serviceworker/coi-serviceworker.min.js',
                dest: './',
            }, {
                src: `${path.resolve(__dirname, COCR_BUILD_DIR)}/*`,
                dest: './',
                transform: {
                    encoding: 'buffer',
                    handler: qtCOIHandler,
                }
            },]
        }),
    ],
});
