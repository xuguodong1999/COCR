import {mergeBIntoAByKey} from '../ts-cmake/utils';
import {config as base} from './gcc-base';

const config = mergeBIntoAByKey(base, {
    buildDir: 'cmake-build-all/linux-gcc-shared',
    config: {
        options: [
            'BUILD_SHARED_LIBS:BOOL=ON',
        ]
    },
});
export {config};