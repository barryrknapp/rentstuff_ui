import { createApp } from 'vue';
import App from './App.vue';
import router from './router';
import axios from 'axios';
import './assets/main.css';

// Use environment variable for base URL
const baseURL = import.meta.env.VITE_API_BASE_URL;

// Configure axios with the base URL
axios.defaults.baseURL = baseURL;

const app = createApp(App);
app.use(router);
app.mount('#app');

// Optional: Make baseURL available globally
app.config.globalProperties.$baseURL = baseURL;