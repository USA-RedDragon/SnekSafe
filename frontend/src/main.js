import './assets/main.css';
import './assets/themes/md-dark-indigo/theme.css';

import { createApp } from 'vue';
import { createPinia } from 'pinia';

import PrimeVue from 'primevue/config';

import ToastService from 'primevue/toastservice';
import Toast from 'primevue/toast';

import 'primeflex/primeflex.scss';
import 'primeicons/primeicons.css';
import 'primevue/resources/primevue.min.css';

import App from './App.vue';
import router from './router';

const app = createApp(App);

app.use(createPinia());
app.use(router);
app.use(PrimeVue);
app.use(ToastService);

app.component('PVToast', Toast);

app.mount('#app');
