import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';

export default defineConfig({
    plugins: [vue()],
    server: {
        port: 5173,
        historyApiFallback: true,
    },
    define: {
        // Ensure environment variables are available
        'process.env': {},
    },
});