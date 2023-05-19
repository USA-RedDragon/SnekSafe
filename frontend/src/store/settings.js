import { defineStore } from 'pinia';

export const useSettingsStore = defineStore('settings', {
  state: () => ({
    captivePortalSSID: '',
    captivePortalPassword: '',
    captivePortalChannel: 6,
    temperatureSetpoint: 82,
    lightOnTime: 0,
    lightOffTime: 0,
    wifiSSID: '',
    wifiPassword: '',
    mdnsName: 'snek',
    otaPassword: 'snekysnek',
  }),
  getters: {},
  actions: {},
});
