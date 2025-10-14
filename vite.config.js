import { defineConfig, loadEnv } from 'vite';
import vue from '@vitejs/plugin-vue';
import path from 'path';

export default defineConfig(({ mode }) => {
    const env = loadEnv(mode, process.cwd(), '');

    return {
        plugins: [vue()],
        resolve: {
            alias: {
                '@': path.resolve(__dirname, './src'),
            },
        },
        server: {
            port: 5173,
            historyApiFallback: true,
            proxy: {
                '/rentstuff': {
                    target: env.VITE_API_BASE_URL || 'http://localhost:8080/rentstuff',
                    changeOrigin: true,
                    secure: mode === 'development' ? false : true,
                    rewrite: (path) => path,
                },
            },
        },
        define: {
            'import.meta.env.VITE_API_BASE_URL': JSON.stringify(env.VITE_API_BASE_URL || 'http://localhost:8080/rentstuff'),
        },
    };
});