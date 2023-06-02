<template>
  <div>
    <PVToast />
    <Card>
      <template #title>Updates</template>
      <template #content>
        <p><span style="font-weight: bold;">Current Version</span>: {{ version }}</p>
        <p><span style="font-weight: bold;">Update Version</span>: {{ updateVersion }}</p>
      </template>
      <template #footer>
        <div class="card-footer-wrapper">
          <div class="card-footer-left">
            <PVButton
              class="p-button-raised p-button-rounded p-button-secondary"
              label="Check for Updates"
              type="button"
              @click="checkForUpdates"
              :disabled="checkingForUpdates || !!!version">
              <template #icon>
                <span style="padding-right: 0.5em;">
                  <font-awesome-icon icon="fa-solid fa-wifi" />
                </span>
              </template>
            </PVButton>
          </div>
          <div class="card-footer-right">
            <PVButton
              class="p-button-raised p-button-rounded"
              label="Update"
              @click="update"
              :disabled="!!!firmwareUpdateURL && !!!frontendUpdateURL">
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
  </div>
</template>

<script>
import Button from 'primevue/button';
import Card from 'primevue/card';
import API from '@/services/API';

import axios from 'axios';
import semver from 'semver';

export default {
  components: {
    PVButton: Button,
    Card,
  },
  created() {
    this.getCurrentVersion().then(() => {
      this.checkForUpdates();
    }).catch((err) => {
      console.error(err);
    });
  },
  mounted() {},
  unmounted() {},
  data: function() {
    return {
      submitted: false,
      version: '',
      updateVersion: 'Querying updates...',
      firmwareUpdateURL: '',
      frontendUpdateURL: '',
    };
  },
  methods: {
    getCurrentVersion() {
      return API.get('/version')
        .then((response) => {
          this.version = response.data.split('-')[0];
        })
        .catch((error) => {
          console.log(error);
        });
    },
    update() {
      this.$toast.add({
        summary: 'Frontend Update Started',
        severity: 'info',
        detail: 'The frontend update has started. Please wait for the update to complete.',
        life: 3000,
      });
      API.post('/ota/web', {
        binary_url: this.frontendUpdateURL,
        type: 'frontend',
        restart: false,
      }).then((res) => {
        this.$toast.add({
          summary: 'Frontend Update Complete',
          severity: 'success',
          detail: 'The frontend update has completed. The firmware update will start now.',
          life: 3000,
        });
        API.post('/ota/web', {
          binary_url: this.firmwareUpdateURL,
          type: 'firmware',
        }).then((res) => {
          this.$toast.add({
            summary: 'Firmware Update Complete',
            severity: 'success',
            detail: 'The firmware update has completed. The SnekSafe will now restart.',
            life: 3000,
          });
        }).catch((err) => {
          this.$toast.add({
            summary: 'Error Updating Firmware',
            severity: 'error',
            detail: 'There was an error updating the firmware.',
            life: 3000,
          });
          console.error(err);
        });
      }).catch((err) => {
        this.$toast.add({
          summary: 'Error Updating Frontend',
          severity: 'error',
          detail: 'There was an error updating the frontend.',
          life: 3000,
        });
        console.error(err);
      });
    },
    checkForUpdates() {
      axios.get('https://api.github.com/repos/USA-RedDragon/SnekSafe/releases/latest', {
        headers: {
          'Accept': 'application/vnd.github+json',
          'X-GitHub-Api-Version': '2022-11-28',
        },
      }).then((res) => {
        if (semver.gt(res.data.tag_name, this.version)) {
          this.$toast.add({
            summary: 'Update Available',
            severity: 'info',
            detail: `A new version of SnekSafe is available: ${res.data.tag_name}`,
            life: 3000,
          });

          this.updateVersion = res.data.tag_name;
          if ('assets' in res.data) {
            for (const asset of res.data.assets) {
              if (asset.name.includes('frontend.bin')) {
                this.frontendUpdateURL = asset.browser_download_url;
              } else if (asset.name.includes('firmware.bin')) {
                this.firmwareUpdateURL = asset.browser_download_url;
              }
            }
          }
        } else {
          this.updateVersion = res.data.tag_name;
          this.$toast.add({
            summary: 'No Updates Available',
            severity: 'info',
            detail: 'You are running the latest version of SnekSafe.',
            life: 3000,
          });
        }
      }).catch((err) => {
        // Tell user that updates won't work if they're on the hotspot
        this.$toast.add({
          summary: 'Error checking for updates',
          severity: 'error',
          detail: 'You must not be connected to the SnekSafe hotspot to check for updates.',
          life: 3000,
        });
        this.updateVersion = 'Error checking for updates';
        console.error(err);
      });
    },
  },
  computed: { },
};
</script>

<style scoped></style>
