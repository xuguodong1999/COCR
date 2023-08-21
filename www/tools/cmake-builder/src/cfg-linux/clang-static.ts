import {mergeBIntoAByKey} from '../ts-cmake/utils';
import {config as base} from './clang-base';

const config = mergeBIntoAByKey(base, {
    buildDir: 'cmake-build-all/linux-clang-static',
    config: {
        options: [
            'BUILD_SHARED_LIBS:BOOL=OFF',
        ]
    },
});
export {config};