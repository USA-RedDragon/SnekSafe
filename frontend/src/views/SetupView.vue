<template>
  <div>
    <PVToast />
    <form autocomplete="off" @submit.prevent="sendWifiCredentials(!v$.$invalid)">
      <Card>
        <template #title>Configure WiFi</template>
        <template #content>
          <span class="p-float-label">
            <Dropdown
              id="ssid"
              v-model="v$.ssid.$model" editable :options="scanResults" placeholder="Select or type an SSID"
              :class="{
                'p-invalid': v$.ssid.$invalid && submitted,
                'w-full': true,
              }" />
            <label
              for="ssid"
              :class="{ 'p-error': v$.ssid.$invalid && submitted }"
              >SSID</label
            >
          </span>
          <span v-if="v$.ssid.$error && submitted">
            <span v-for="(error, index) of v$.ssid.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="wifi_password"
              type="password"
              autocomplete="new-password"
              v-model="v$.wifi_password.$model"
              :class="{
                'p-invalid': v$.wifi_password.$invalid && submitted,
              }"
            />
            <label
              for="wifi_password"
              :class="{ 'p-error': v$.wifi_password.$invalid && submitted }"
              >WiFi Password</label
            >
          </span>
          <span v-if="v$.wifi_password.$error && submitted">
            <span v-for="(error, index) of v$.wifi_password.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
        </template>
        <template #footer>
          <div class="card-footer-wrapper">
            <div class="card-footer-left">
              <PVButton
                class="p-button-raised p-button-rounded p-button-secondary"
                :loading="!!scanTimer"
                label="Scan"
                type="button"
                @click="scan"
                :disabled="!!scanTimer"
              >
                <template #icon>
                  <span style="padding-right: 0.5em;">
                    <font-awesome-icon icon="fa-solid fa-wifi" v-if="!!!scanTimer"/>
                    <font-awesome-icon icon="fa-solid fa-spinner" spin v-else/>
                  </span>
                </template>
              </PVButton>
            </div>
            <div class="card-footer-right">
              <PVButton
                class="p-button-raised p-button-rounded"
                label="Save"
                type="submit"
                :loading="!!connectTimer"
                :disabled="!!connectTimer">
                <template #icon>
                  <span style="padding-right: 0.5em;">
                    <font-awesome-icon icon="fa-regular fa-floppy-disk" v-if="!!!connectTimer"/>
                    <font-awesome-icon icon="fa-solid fa-spinner" spin v-else/>
                  </span>
                </template>
              </PVButton>
            </div>
          </div>
        </template>
      </Card>
    </form>
  </div>
</template>

<script>
import InputText from 'primevue/inputtext';
import Button from 'primevue/button';
import Card from 'primevue/card';
import Dropdown from 'primevue/dropdown';
import API from '@/services/API';

import { useVuelidate } from '@vuelidate/core';
import { required, maxLength, minLength } from '@vuelidate/validators';

export default {
  components: {
    InputText,
    PVButton: Button,
    Card,
    Dropdown,
  },
  setup: () => ({ v$: useVuelidate() }),
  created() {
    this.checkConnectStatus();
    this.scan();
  },
  mounted() {},
  unmounted() {},
  data: function() {
    return {
      scanResults: [],
      ssid: '',
      wifi_password: '',
      submitted: false,
      connectTimer: null,
      scanTimer: null,
    };
  },
  validations() {
    return {
      ssid: {
        required,
        minLength: minLength(2),
        maxLength: maxLength(32),
      },
      wifi_password: {
        required,
        minLength: minLength(8),
        maxLength: maxLength(63),
      },
    };
  },
  methods: {
    scan() {
      API.post('/wifi/scan', {})
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            this.$toast.add({
              summary: 'Success',
              severity: 'success',
              detail: response.data.message,
              life: 3000,
            });
            this.scanTimer = setInterval(this.checkScanStatus, 500);
          } else {
            this.$toast.add({
              summary: 'Error',
              severity: 'error',
              detail: 'An unknown error occurred',
              life: 3000,
            });
          }
        })
        .catch((error) => {
          console.log(error);
          this.$toast.add({
            summary: 'Scan Failed',
            severity: 'error',
            detail: error,
            life: 3000,
          });
        });
    },
    checkScanStatus() {
      API.get('/wifi/scan/status').then((response) => {
        if (response.data && response.data.status != 'error') {
          if (response.data.status == 'success' && response.data.message == 'Scan complete') {
            this.getWifiScanResults();
            clearInterval(this.scanTimer);
            this.scanTimer = null;
          }
        }
      });
    },
    getWifiScanResults() {
      API.get('/wifi/scan/results')
        .then((response) => {
          this.scanResults = response.data.data;
        })
        .catch((error) => {
          console.log(error);
          this.$toast.add({
            summary: 'Success',
            severity: 'error',
            detail: 'Error getting scan results: ' + error,
            life: 3000,
          });
        });
    },
    checkConnectStatus() {
      API.get('/wifi/status').then((response) => {
        if (response.data && response.data.status != 'error') {
          if (response.data.ssid) {
            clearInterval(this.connectTimer);
            this.connectTimer = null;
            this.$router.push({ name: 'home' });
          } else if (response.data.status == 'no ssid available') {
            clearInterval(this.connectTimer);
            this.connectTimer = null;
            if (this.submitted) {
              this.$toast.add({
                summary: 'Error',
                severity: 'error',
                detail: 'SSID not found',
                life: 3000,
              });
            }
          } else if (
            response.data.status == 'connect failed' ||
              response.data.status == 'disconnected') {
            clearInterval(this.connectTimer);
            this.connectTimer = null;
            if (this.submitted) {
              this.$toast.add({
                summary: 'Error',
                severity: 'error',
                detail: 'Connection failed',
                life: 3000,
              });
            }
          } else if (response.data.status == 'connection lost') {
            clearInterval(this.connectTimer);
            this.connectTimer = null;
            if (this.submitted) {
              this.$toast.add({
                summary: 'Error',
                severity: 'error',
                detail: 'Connection lost',
                life: 3000,
              });
            }
          }
        }
      });
    },
    sendWifiCredentials(isFormValid) {
      this.submitted = true;
      if (!isFormValid) {
        return;
      }

      API.post('/wifi/connect', {
        ssid: this.ssid,
        password: this.wifi_password,
      })
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            setTimeout(() => setInterval(this.checkConnectStatus, 500), 500);
            this.$toast.add({
              summary: 'Success',
              severity: 'success',
              detail: response.data.message,
              life: 3000,
            });
          } else {
            this.$toast.add({
              summary: 'Error',
              severity: 'error',
              detail: 'An unknown error occurred',
              life: 3000,
            });
          }
        })
        .catch((err) => {
          console.log(err);
          if (err.response && err.response.status && err.response.status == 'error') {
            this.$toast.add({
              summary: 'Error',
              severity: 'error',
              detail: err.response.message,
              life: 3000,
            });
          } else {
            this.$toast.add({
              summary: 'Error',
              severity: 'error',
              detail: 'An unknown error occurred',
              life: 3000,
            });
          }
        });
    },
  },
  computed: { },
};
</script>

<style scoped></style>
