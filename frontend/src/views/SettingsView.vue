<template>
  <div>
    <PVToast />
    <form @submit.prevent="saveSettings(!v$.$invalid)">
      <Card>
        <template #title>Settings</template>
        <template #content>
          <p>Settings</p>
          <p>{{ settings }}</p>
        </template>
      </Card>
    </form>
  </div>
</template>

<script>
import Card from 'primevue/card';
import InputText from 'primevue/inputtext';
import Button from 'primevue/button';

import API from '@/services/API';

import { useVuelidate } from '@vuelidate/core';
import { maxLength, minLength, maxValue, minValue } from '@vuelidate/validators';

export default {
  components: {
    Card,
    InputText,
    PVButton: Button,
  },
  setup: () => ({ v$: useVuelidate() }),
  created() {
    this.getSettings();
  },
  mounted() {},
  unmounted() {},
  data: function() {
    return {
      originalSettings: {},
      captivePortalSSID: '',
      captivePortalPassword: '',
      captivePortalChannel: 6,
      temperatureSetpoint: 0,
      lightOnTime: 0,
      lightOffTime: 0,
      mdnsName: '',

      wifiSSID: '',
      wifiPassword: '',
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
    saveSettings() {

    },
  },
  computed: {},
};
</script>

<style scoped></style>
