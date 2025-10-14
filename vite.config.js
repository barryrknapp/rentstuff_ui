import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';

export default defineConfig({
    plugins: [vue()],
    server: {
        port: 5173,
        historyApiFallback: true,
        proxy: {
            '/rentstuff': {
                target: 'http://localhost:8081',
                changeOrigin: true,
                secure: false,
                rewrite: (path) => path,
            },
        },
    },
    define: {
        'process.env': {},
    },
});