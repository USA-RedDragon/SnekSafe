import './assets/main.css';
import './assets/themes/md-dark-indigo/theme.css';

import { createApp } from 'vue';

import PrimeVue from 'primevue/config';

import ToastService from 'primevue/toastservice';
import Toast from 'primevue/toast';

import 'primeflex/primeflex.scss';
import 'primevue/resources/primevue.min.css';

import { library } from '@fortawesome/fontawesome-svg-core';
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome';
import { faFireFlameCurved, faWifi, faSpinner } from '@fortawesome/free-solid-svg-icons';
import { faLightbulb, faFloppyDisk } from '@fortawesome/free-regular-svg-icons';

/* add icons to the library */
library.add(faFireFlameCurved);
library.add(faWifi);
library.add(faSpinner);
library.add(faLightbulb);
library.add(faFloppyDisk);

import App from './App.vue';
import router from './router';

const app = createApp(App);

app.use(router);
app.use(PrimeVue);
app.use(ToastService);

app.component('PVToast', Toast);
app.component('font-awesome-icon', FontAwesomeIcon);

app.mount('#app');
