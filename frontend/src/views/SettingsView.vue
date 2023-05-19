<template>
  <div>
    <PVToast />
    <ConfirmDialog>
      <template #message>
        <span style="color: red">Are you sure you want to reset everything to default?</span>
      </template>
    </ConfirmDialog>
    <form autocomplete="off" @submit.prevent="saveSettings(!v$.$invalid)">
      <Card>
        <template #title>Configure WiFi</template>
        <template #content>
          <span class="p-float-label">
            <Dropdown
              id="wifiSSID"
              v-model="v$.wifiSSID.$model" editable :options="scanResults" placeholder="Select or type an SSID"
              :class="{
                'p-invalid': v$.wifiSSID.$invalid && submitted,
                'w-full': true,
              }" />
            <label
              for="wifiSSID"
              :class="{ 'p-error': v$.wifiSSID.$invalid && submitted }"
              >SSID</label
            >
          </span>
          <span v-if="v$.wifiSSID.$error && submitted">
            <span v-for="(error, index) of v$.wifiSSID.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="wifiPassword"
              type="password"
              autocomplete="new-password"
              v-model="v$.wifiPassword.$model"
              :class="{
                'p-invalid': v$.wifiPassword.$invalid && submitted,
              }"
            />
            <label
              for="wifiPassword"
              :class="{ 'p-error': v$.wifiPassword.$invalid && submitted }"
              >WiFi Password</label
            >
          </span>
          <span v-if="v$.wifiPassword.$error && submitted">
            <span v-for="(error, index) of v$.wifiPassword.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <p>Leave the password blank to keep it unchanged</p>
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
                    <font-awesome-icon icon="fa-regular fa-floppy-disk" v-if="!!!scanTimer"/>
                    <font-awesome-icon icon="fa-solid fa-spinner" spin v-else/>
                  </span>
                </template>
              </PVButton>
            </div>
          </div>
        </template>
      </Card>
      <br />
      <Card>
        <template #title>General Settings</template>
        <template #content>
          <span class="p-float-label">
            <InputText
              id="temperatureSetpoint"
              v-model.number="v$.temperatureSetpoint.$model"
              :class="{
                'p-invalid': v$.temperatureSetpoint.$invalid && submitted,
                'w-full': true,
              }" />
            <Slider v-model="v$.temperatureSetpoint.$model" :min="70" :max="100"
              :class="{
                'p-invalid': v$.temperatureSetpoint.$invalid && submitted,
              }" />
            <label
              for="temperatureSetpoint"
              :class="{ 'p-error': v$.temperatureSetpoint.$invalid && submitted }"
              >Temperature Setpoint</label
            >
          </span>
          <span v-if="v$.temperatureSetpoint.$error && submitted">
            <span v-for="(error, index) of v$.temperatureSetpoint.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="timezoneOffset"
              type="text"
              v-model="v$.timezoneOffset.$model"
              :class="{
                'p-invalid': v$.timezoneOffset.$invalid && submitted,
              }"
            />
            <label
              for="timezoneOffset"
              :class="{ 'p-error': v$.timezoneOffset.$invalid && submitted }"
              >Timezone</label>
          </span>
          <span v-if="v$.timezoneOffset.$error && submitted">
            <span v-for="(error, index) of v$.timezoneOffset.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <!-- lightOnTime -->
          <!-- lightOffTime -->
          <span class="p-float-label">
            <InputText
              id="mdnsName"
              type="text"
              v-model="v$.mdnsName.$model"
              :class="{
                'p-invalid': v$.mdnsName.$invalid && submitted,
              }"
            />
            <label
              for="mdnsName"
              :class="{ 'p-error': v$.mdnsName.$invalid && submitted }"
              >mDNS Name</label>
          </span>
          <span v-if="v$.mdnsName.$error && submitted">
            <span v-for="(error, index) of v$.mdnsName.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <p>This is the name at which this interface can be reached on the same WiFi connection.
          This is currently <a href="http://{{ this.mdnsName }}.local">http://{{ this.mdnsName }}.local</a></p>
        </template>
        <template #footer>
          <div>
            <PVButton
              class="p-button-raised p-button-rounded"
              label="Save"
              type="submit">
              <template #icon>
                <span style="padding-right: 0.5em;">
                  <font-awesome-icon icon="fa-regular fa-floppy-disk" v-if="!!!scanTimer"/>
                  <font-awesome-icon icon="fa-solid fa-spinner" spin v-else/>
                </span>
              </template>
            </PVButton>
          </div>
        </template>
      </Card>
      <br />
      <Card>
        <template #title>Captive Portal Settings</template>
        <template #content>
          <span class="p-float-label">
            <InputText
              id="captivePortalSSID"
              type="text"
              v-model="v$.captivePortalSSID.$model"
              :class="{
                'p-invalid': v$.captivePortalSSID.$invalid && submitted,
              }"
            />
            <label
              for="captivePortalSSID"
              :class="{ 'p-error': v$.captivePortalSSID.$invalid && submitted }"
              >Captive Portal SSID</label>
          </span>
          <span v-if="v$.captivePortalSSID.$error && submitted">
            <span v-for="(error, index) of v$.captivePortalSSID.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="captivePortalPassword"
              type="password"
              autocomplete="new-password"
              v-model="v$.captivePortalPassword.$model"
              :class="{
                'p-invalid': v$.captivePortalPassword.$invalid && submitted,
              }"
            />
            <label
              for="captivePortalPassword"
              :class="{ 'p-error': v$.captivePortalPassword.$invalid && submitted }"
              >Captive Portal Password</label>
          </span>
          <span v-if="v$.captivePortalPassword.$error && submitted">
            <span v-for="(error, index) of v$.captivePortalPassword.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <p>Leave the password blank to keep it unchanged.</p>
          <br />
          <span class="p-float-label">
            <Dropdown
              id="captivePortalChannel"
              v-model="v$.captivePortalChannel.$model"
              :options="[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]"
              :class="{
                'p-invalid': v$.captivePortalChannel.$invalid && submitted,
                'w-full': true,
              }" />
            <label
              for="captivePortalChannel"
              :class="{ 'p-error': v$.captivePortalChannel.$invalid && submitted }"
              >Captive Portal Channel</label
            >
          </span>
          <span v-if="v$.captivePortalChannel.$error && submitted">
            <span v-for="(error, index) of v$.captivePortalChannel.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
        </template>
        <template #footer>
          <div>
            <PVButton
              class="p-button-raised p-button-rounded"
              label="Save"
              type="submit">
              <template #icon>
                <span style="padding-right: 0.5em;">
                  <font-awesome-icon icon="fa-regular fa-floppy-disk" v-if="!!!scanTimer"/>
                  <font-awesome-icon icon="fa-solid fa-spinner" spin v-else/>
                </span>
              </template>
            </PVButton>
          </div>
        </template>
      </Card>
      <br />
      <Card>
        <template #title>Reset to Defaults</template>
        <template #content>
          <p>Reset all settings to their default values.</p>
        </template>
        <template #footer>
          <div>
            <PVButton
              class="p-button-raised p-button-rounded p-button-danger"
              label="Reset to Defaults"
              @click="resetToDefaultConfirm()">
              <template #icon>
                <span style="padding-right: 0.5em;">
                  <font-awesome-icon icon="fa-solid fa-circle-exclamation" />
                </span>
              </template>
            </PVButton>
          </div>
        </template>
      </Card>
    </form>
  </div>
</template>

<script>
import Card from 'primevue/card';
import InputText from 'primevue/inputtext';
import Slider from 'primevue/slider';
import Button from 'primevue/button';
import Dropdown from 'primevue/dropdown';

import API from '@/services/API';

import { useVuelidate } from '@vuelidate/core';
import { maxLength, minLength, maxValue, minValue, numeric } from '@vuelidate/validators';

export default {
  components: {
    Card,
    InputText,
    PVButton: Button,
    Dropdown,
    Slider,
  },
  setup: () => ({ v$: useVuelidate() }),
  created() {},
  mounted() {
    this.getSettings();
  },
  unmounted() {},
  data: function() {
    return {
      originalSettings: {},
      submitted: false,

      captivePortalSSID: '',
      captivePortalPassword: '',
      captivePortalChannel: 6,
      temperatureSetpoint: 82,
      lightOnTime: 0,
      lightOffTime: 0,
      mdnsName: '',
      timezoneOffset: 0,

      wifiSSID: '',
      wifiPassword: '',
      scanResults: [],
      scanTimer: null,
    };
  },
  validations() {
    return {
      captivePortalSSID: {
        minLength: minLength(2),
        maxLength: maxLength(32),
      },
      captivePortalPassword: {
        minLength: minLength(8),
        maxLength: maxLength(63),
      },
      captivePortalChannel: {
        minValue: minValue(1),
        maxValue: maxValue(11),
      },
      temperatureSetpoint: {
        minValue: minValue(70),
        maxValue: maxValue(100),
      },
      // lightOnTime
      // lightOffTime
      mdnsName: {
        minLength: minLength(2),
        maxLength: maxLength(32),
      },
      timezoneOffset: {
        minValue: minValue(-720),
        maxValue: maxValue(720),
        numeric,
      },
      wifiSSID: {
        minLength: minLength(2),
        maxLength: maxLength(32),
      },
      wifiPassword: {
        minLength: minLength(8),
        maxLength: maxLength(63),
      },
    };
  },
  methods: {
    getSettings() {
      API.get('/settings')
        .then((response) => {
          this.originalSettings = response.data;
          this.captivePortalSSID = response.data.captivePortalSSID;
          this.captivePortalChannel = response.data.captivePortalChannel;
          this.temperatureSetpoint = response.data.temperatureSetpoint;
          this.lightOnTime = response.data.lightOnTime;
          this.lightOffTime = response.data.lightOffTime;
          this.wifiSSID = response.data.wifiSSID;
          this.mdnsName = response.data.mdnsName;
          this.scanResults = [response.data.wifiSSID];
        })
        .catch((error) => {
          console.log(error);
          this.$toast.add({
            summary: 'Error',
            severity: 'error',
            detail: 'An unknown error occurred',
            life: 3000,
          });
        });
    },
    saveSettings(isFormValid) {
      this.submitted = true;
      if (!isFormValid) {
        console.error('Form is invalid');
        return;
      }

      // See which items have changed from the original settings
      const changedSettings = {};
      if (this.captivePortalSSID !== this.originalSettings.captivePortalSSID) {
        changedSettings.captivePortalSSID = this.captivePortalSSID;
      }
      if (this.captivePortalPassword !== '') {
        changedSettings.captivePortalPassword = this.captivePortalPassword;
      }
      if (this.captivePortalChannel !== this.originalSettings.captivePortalChannel) {
        changedSettings.captivePortalChannel = this.captivePortalChannel;
      }
      if (this.temperatureSetpoint !== this.originalSettings.temperatureSetpoint) {
        changedSettings.temperatureSetpoint = this.temperatureSetpoint;
      }
      if (this.lightOnTime !== this.originalSettings.lightOnTime) {
        changedSettings.lightOnTime = this.lightOnTime;
      }
      if (this.lightOffTime !== this.originalSettings.lightOffTime) {
        changedSettings.lightOffTime = this.lightOffTime;
      }
      if (this.mdnsName !== this.originalSettings.mdnsName) {
        changedSettings.mdnsName = this.mdnsName;
      }

      if (this.wifiSSID !== this.originalSettings.wifiSSID) {
        changedSettings.wifiSSID = this.wifiSSID;
      }

      if (this.wifiPassword !== '') {
        changedSettings.wifiPassword = this.wifiPassword;
      }

      console.log(changedSettings);

      // If there are no changes, don't bother sending the request
      if (Object.keys(changedSettings).length === 0) {
        this.$toast.add({
          summary: 'No Changes',
          severity: 'info',
          detail: 'No changes were made',
          life: 3000,
        });
        return;
      }

      // Send the request
      API.patch('/settings', changedSettings)
        .then((response) => {
          if (response && response.data && response.data.status == 'success') {
            this.$toast.add({
              summary: 'Success',
              severity: 'success',
              detail: 'Settings saved successfully',
              life: 3000,
            });
          } else if (response && response.data && response.data.message) {
            this.$toast.add({
              summary: 'Error',
              severity: 'error',
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
          this.getSettings();
        });
    },
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
    resetToDefaultConfirm() {
      // This will open a dialog to confirm the reset
      this.$confirm.require({
        message: 'Are you sure you want to reset all settings to default?',
        header: 'Reset to Default',
        icon: 'pi pi-exclamation-triangle',
        accept: this.resetToDefault,
        reject: () => {
          this.$toast.add({
            summary: 'Reset Cancelled',
            severity: 'info',
            detail: 'Reset to default cancelled',
            life: 3000,
          });
        },
      });
    },
    resetToDefault() {
      // This will actually perform the reset
      API.post('/settings/reset')
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            this.$toast.add({
              summary: 'Success',
              severity: 'success',
              detail: response.data.message,
              life: 3000,
            });
            this.$router.push({ name: 'setup' });
          } else {
            this.$toast.add({
              summary: 'Error',
              severity: 'error',
              detail: 'An unknown error occurred',
              life: 3000,
            });
          }
          this.getSettings();
        })
        .catch((error) => {
          console.log(error);
          this.$toast.add({
            summary: 'Success',
            severity: 'error',
            detail: 'Error resetting settings: ' + error,
            life: 3000,
          });
        });
    },
  },
  computed: {},
};
</script>

<style scoped></style>
