import {mergeBIntoAByKey} from '../ts-cmake/utils';
import {config as base} from './clang-static-base';

const config = mergeBIntoAByKey(base, {
    buildDir: 'cmake-build-all/apple-clang-static-x86',
    config: {
        options: [
            'CMAKE_OSX_ARCHITECTURES="x86_64"',
        ]
    },
});
export {config};