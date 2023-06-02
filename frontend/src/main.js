import { createApp } from 'vue';

import PrimeVue from 'primevue/config';
import ToastService from 'primevue/toastservice';
import DialogService from 'primevue/dialogservice';
import ConfirmationService from 'primevue/confirmationservice';
import Toast from 'primevue/toast';
import ConfirmDialog from 'primevue/confirmdialog';

import App from './App.vue';
import router from './router';

import 'primeflex/primeflex.scss';
import 'primevue/resources/primevue.min.css';

import './assets/main.css';
import './assets/themes/md-dark-indigo/theme.css';

import { library } from '@fortawesome/fontawesome-svg-core';
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome';
import {
  faFireFlameCurved,
  faWifi,
  faSpinner,
  faCircleExclamation,
  faUpload,
  faRotateRight,
} from '@fortawesome/free-solid-svg-icons';
import { faLightbulb, faFloppyDisk } from '@fortawesome/free-regular-svg-icons';

/* add icons to the library */
library.add(faFireFlameCurved);
library.add(faWifi);
library.add(faSpinner);
library.add(faCircleExclamation);
library.add(faUpload);
library.add(faRotateRight);

library.add(faLightbulb);
library.add(faFloppyDisk);

const app = createApp(App);

app.use(ToastService);
app.use(DialogService);
app.use(ConfirmationService);
app.use(PrimeVue);
app.use(router);

app.component('PVToast', Toast);
app.component('ConfirmDialog', ConfirmDialog);
app.component('font-awesome-icon', FontAwesomeIcon);

app.mount('#app');
