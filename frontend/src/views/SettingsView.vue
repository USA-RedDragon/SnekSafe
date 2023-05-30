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
                type="submit">
                <template #icon>
                  <span style="padding-right: 0.5em;">
                    <font-awesome-icon icon="fa-regular fa-floppy-disk" />
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
            <Calendar id="turnOnTime" v-model="turnOnTime" timeOnly hourFormat="12" />
            <label
              for="turnOnTime"
              >Light Turn On Time</label
            >
          </span>
          <br />
          <span class="p-float-label">
            <Calendar id="turnOffTime" v-model="turnOffTime" timeOnly hourFormat="12" />
            <label
              for="turnOffTime"
              >Light Turn Off Time</label
            >
          </span>
          <br />
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
        <template #title>PID Tuning</template>
        <template #content>
          <span class="p-float-label">
            <InputText
              id="pGain"
              v-model.number="v$.pGain.$model"
              :class="{
                'p-invalid': v$.pGain.$invalid && submitted,
                'w-full': true,
              }" />
            <Slider v-model="v$.pGain.$model" :min="0" :max="100" :step="0.25"
              :class="{
                'p-invalid': v$.pGain.$invalid && submitted,
              }" />
            <label
              for="pGain"
              :class="{ 'p-error': v$.pGain.$invalid && submitted }"
              >Proportional Gain</label
            >
          </span>
          <span v-if="v$.pGain.$error && submitted">
            <span v-for="(error, index) of v$.pGain.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="iGain"
              v-model.number="v$.iGain.$model"
              :class="{
                'p-invalid': v$.iGain.$invalid && submitted,
                'w-full': true,
              }" />
            <Slider v-model="v$.iGain.$model" :min="0" :max="100" :step="0.25"
              :class="{
                'p-invalid': v$.iGain.$invalid && submitted,
              }" />
            <label
              for="iGain"
              :class="{ 'p-error': v$.iGain.$invalid && submitted }"
              >Integral Gain</label
            >
          </span>
          <span v-if="v$.iGain.$error && submitted">
            <span v-for="(error, index) of v$.iGain.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="dGain"
              v-model.number="v$.dGain.$model"
              :class="{
                'p-invalid': v$.dGain.$invalid && submitted,
                'w-full': true,
              }" />
            <Slider v-model="v$.dGain.$model" :min="0" :max="100" :step="0.25"
              :class="{
                'p-invalid': v$.dGain.$invalid && submitted,
              }" />
            <label
              for="dGain"
              :class="{ 'p-error': v$.dGain.$invalid && submitted }"
              >Derivative Gain</label
            >
          </span>
          <span v-if="v$.dGain.$error && submitted">
            <span v-for="(error, index) of v$.dGain.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="iMin"
              v-model.number="v$.iMin.$model"
              :class="{
                'p-invalid': v$.iMin.$invalid && submitted,
                'w-full': true,
              }" />
            <Slider v-model="v$.iMin.$model" :min="0" :max="100" :step="0.25"
              :class="{
                'p-invalid': v$.iMin.$invalid && submitted,
              }" />
            <label
              for="iMin"
              :class="{ 'p-error': v$.iMin.$invalid && submitted }"
              >Minimum Integral Term</label
            >
          </span>
          <span v-if="v$.iMin.$error && submitted">
            <span v-for="(error, index) of v$.iMin.$errors" :key="index">
              <small class="p-error">{{ error.$message }}</small>
            </span>
            <br />
          </span>
          <br />
          <span class="p-float-label">
            <InputText
              id="iMax"
              v-model.number="v$.iMax.$model"
              :class="{
                'p-invalid': v$.iMax.$invalid && submitted,
                'w-full': true,
              }" />
            <Slider v-model="v$.iMax.$model" :min="0" :max="100" :step="0.25"
              :class="{
                'p-invalid': v$.iMax.$invalid && submitted,
              }" />
            <label
              for="iMax"
              :class="{ 'p-error': v$.iMax.$invalid && submitted }"
              >Maximum Integral Term</label
            >
          </span>
          <span v-if="v$.iMax.$error && submitted">
            <span v-for="(error, index) of v$.iMax.$errors" :key="index">
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
import Calendar from 'primevue/calendar';
import InputText from 'primevue/inputtext';
import Slider from 'primevue/slider';
import Button from 'primevue/button';
import Dropdown from 'primevue/dropdown';

import moment from 'moment';

import API from '@/services/API';

import { useVuelidate } from '@vuelidate/core';
import { maxLength, minLength, maxValue, minValue, numeric } from '@vuelidate/validators';

export default {
  components: {
    Card,
    Calendar,
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
      lightOnHour: 0,
      lightOnMinute: 0,
      lightOffHour: 0,
      lightOffMinute: 0,
      mdnsName: '',
      pGain: 0,
      iGain: 0,
      dGain: 0,
      iMin: 0,
      iMax: 0,

      wifiSSID: '',
      wifiPassword: '',
      scanResults: [],
      scanTimer: null,
      turnOffTime: null,
      turnOnTime: null,
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
      mdnsName: {
        minLength: minLength(2),
        maxLength: maxLength(32),
      },
      wifiSSID: {
        minLength: minLength(2),
        maxLength: maxLength(32),
      },
      wifiPassword: {
        minLength: minLength(8),
        maxLength: maxLength(63),
      },
      pGain: {
        numeric,
        minValue: minValue(0),
      },
      iGain: {
        numeric,
        minValue: minValue(0),
      },
      dGain: {
        numeric,
        minValue: minValue(0),
      },
      iMin: {
        numeric,
        minValue: minValue(0),
      },
      iMax: {
        numeric,
        minValue: minValue(0),
      },
    };
  },
  watch: {
    turnOnTime: function(val) {
      // Take the local time and convert it to UTC
      const turnOnTimeUTC = moment(val).utc();
      this.lightOnHour = turnOnTimeUTC.hour();
      this.lightOnMinute = turnOnTimeUTC.minute();
    },
    turnOffTime: function(val) {
      const turnOffTimeUTC = moment(val).utc();
      this.lightOffHour = turnOffTimeUTC.hour();
      this.lightOffMinute = turnOffTimeUTC.minute();
    },
  },
  methods: {
    getSettings() {
      API.get('/settings')
        .then((response) => {
          this.originalSettings = response.data;
          this.captivePortalSSID = response.data.captivePortalSSID;
          this.captivePortalChannel = response.data.captivePortalChannel;
          this.temperatureSetpoint = response.data.temperatureSetpoint;
          this.lightOnHour = response.data.lightOnHour;
          this.lightOnMinute = response.data.lightOnMinute;
          // turnOnTime is in UTC, so we need to convert it to local time
          this.turnOnTime = moment().utc().hour(response.data.lightOnHour).minute(response.data.lightOnMinute).toDate();
          this.lightOffHour = response.data.lightOffHour;
          this.lightOffMinute = response.data.lightOffMinute;
          this.turnOffTime = moment().utc().hour(response.data.lightOffHour).minute(response.data.lightOffMinute).toDate();
          this.wifiSSID = response.data.wifiSSID;
          this.mdnsName = response.data.mdnsName;
          this.pGain = response.data.pGain;
          this.iGain = response.data.iGain;
          this.dGain = response.data.dGain;
          this.iMin = response.data.iMin;
          this.iMax = response.data.iMax;
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
      if (this.lightOnHour !== this.originalSettings.lightOnHour) {
        changedSettings.lightOnHour = this.lightOnHour;
      }
      if (this.lightOnMinute !== this.originalSettings.lightOnMinute) {
        changedSettings.lightOnMinute = this.lightOnMinute;
      }
      if (this.lightOffMinute !== this.originalSettings.lightOffMinute) {
        changedSettings.lightOffMinute = this.lightOffMinute;
      }
      if (this.lightOffHour !== this.originalSettings.lightOffHour) {
        changedSettings.lightOffHour = this.lightOffHour;
      }
      if (this.mdnsName !== this.originalSettings.mdnsName) {
        changedSettings.mdnsName = this.mdnsName;
      }
      if (this.pGain !== this.originalSettings.pGain) {
        changedSettings.pGain = this.pGain;
      }
      if (this.iGain !== this.originalSettings.iGain) {
        changedSettings.iGain = this.iGain;
      }
      if (this.dGain !== this.originalSettings.dGain) {
        changedSettings.dGain = this.dGain;
      }
      if (this.iMin !== this.originalSettings.iMin) {
        changedSettings.iMin = this.iMin;
      }
      if (this.iMax !== this.originalSettings.iMax) {
        changedSettings.iMax = this.iMax;
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
