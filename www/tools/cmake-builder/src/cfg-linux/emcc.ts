import * as os from 'os';
import * as path from 'path';
import {mergeBIntoAByKey} from '../ts-cmake/utils';
import {config as base} from './emcc-base';

const QT_ROOT = path.resolve(os.homedir(), 'shared/Qt/6.5.2/wasm_32');
const config = mergeBIntoAByKey(base, {
    buildDir: 'cmake-build-all/wasm-simd-mt',
    config: {
        options: [
            `CMAKE_TOOLCHAIN_FILE:FILEPATH=${path.resolve(QT_ROOT, 'lib/cmake/Qt6/qt.toolchain.cmake')}`,
        ]
    },
    // test: {
    //     timeout: 15,
    //     parallel: true,
    //     targets: ['all'],
    //     excludes: ['(.*)js(.*)', '(.*)qt(.*)', '(.*)COCR(.*)']
    // },
});
export {config};