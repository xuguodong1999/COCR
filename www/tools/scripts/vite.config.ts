import {defineConfig} from 'vite';

// https://vitejs.dev/config/
export default defineConfig({
    server: {
        // need for android emulator 10.0.2.2, localhost not work on windows without extra settings
        // host: '127.0.0.1',
        headers: {
            'Cross-Origin-Opener-Policy': 'same-origin',
            'Cross-Origin-Embedder-Policy': 'require-corp',
            'Cross-Origin-Resource-Policy': 'cross-origin',
        }
    }
});
