<template>
  <div>
    <div class="flexRow">
      <Card class="halfWidth">
        <template #title>WiFi Status</template>
        <template #content>
          <p><span style="font-weight: bold;">Mode</span>: {{ mode }}</p>
          <p><span style="font-weight: bold;">Status</span>: {{ status }}</p>
          <p><span style="font-weight: bold;">SSID</span>: {{ ssid }}</p>
          <p><span style="font-weight: bold;">IP Address</span>: {{ ip }}</p>
        </template>
      </Card>
      <Card class="halfWidth">
        <template #title>Controls</template>
        <template #content>
          <p><span style="font-weight: bold;">Last Update</span>: {{ lastUpdate }}</p>
          <p><span style="font-weight: bold;">Temperature</span>: {{ temperature }} &deg;F</p>
          <p><span style="font-weight: bold;">Humidity</span>: {{ humidity }}%</p>
          <br />
          <font-awesome-icon icon="fa-regular fa-lightbulb" style="padding-right: 0.5em;"/>
          <label for="light">Light</label>
          <InputSwitch v-model="light" inputId="light" style="margin-left: 1em; margin-top: -1em;" />
          <br />
          <font-awesome-icon icon="fa-solid fa-fire-flame-curved" style="padding-right: 0.5em;"/>
          <label for="heat">Heat</label>
          <InputSwitch v-model="heat" inputId="heat" style="margin-left: 1em; margin-top: -1em;" />
        </template>
      </Card>
    </div>
    <br />
    <Card>
      <template #title>Temperature History</template>
      <template #content>
        <Chart type="line" :data="temperatureChartData" :options="temperatureChartOptions" />
      </template>
    </Card>
    <br />
    <Card>
      <template #title>Humidity History</template>
      <template #content>
        <Chart type="line" :data="humidityChartData" :options="humidityChartOptions" />
      </template>
    </Card>
  </div>
</template>

<script>
import Card from 'primevue/card';
import Chart from 'primevue/chart';
import InputSwitch from 'primevue/inputswitch';

import API from '@/services/API';

export default {
  components: {
    Card,
    Chart,
    InputSwitch,
  },
  created() {
    this.getConnectionStatus();
    this.getTemperatureHistory();
    this.getHumidityHistory();
    this.getState();
  },
  mounted() {},
  unmounted() {},
  data: function() {
    const documentStyle = getComputedStyle(document.documentElement);
    const textColor = documentStyle.getPropertyValue('--text-color');
    const textColorSecondary = documentStyle.getPropertyValue('--text-color-secondary');
    const surfaceBorder = documentStyle.getPropertyValue('--surface-border');

    return {
      mode: '',
      status: '',
      ssid: '',
      ip: '',
      temperature: 0,
      humidity: 0,
      lastUpdate: '',
      heat: false,
      light: false,
      historyTemperature: [],
      historyHumidity: [],
      chartOptions: {
        stacked: false,
        maintainAspectRatio: false,
        aspectRatio: 0.6,
        plugins: {
          legend: {
            labels: {
              color: textColor,
            },
          },
        },
        scales: {
          x: {
            ticks: {
              color: textColorSecondary,
            },
            grid: {
              color: surfaceBorder,
            },
          },
          humidity: {
            type: 'linear',
            display: true,
            position: 'left',
            min: 0,
            max: 100,
            ticks: {
              color: textColorSecondary,
            },
            grid: {
              color: surfaceBorder,
            },
          },
          temperature: {
            type: 'linear',
            display: true,
            position: 'right',
            ticks: {
              color: textColorSecondary,
            },
            grid: {
              drawOnChartArea: false,
              color: surfaceBorder,
            },
          },
        },
      },
    };
  },
  methods: {
    getConnectionStatus() {
      API.get('/wifi/status')
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            this.mode = response.data.mode;
            this.status = response.data.status;
            this.ssid = response.data.ssid;
            this.ip = response.data.ip;
          }
        })
        .catch((error) => {
          console.log(error);
        });
    },
    getTemperatureHistory() {
      this.historyTemperature = [80, 81, 80, 83, 82, 81];
    },
    getHumidityHistory() {
      this.historyHumidity = [51, 50, 49, 48, 47, 46];
    },
    getState() {
    },
  },
  computed: {
    temperatureChartData() {
      const documentStyle = getComputedStyle(document.documentElement);
      return {
        labels: ['January', 'February', 'March', 'April', 'May', 'June'],
        datasets: [
          {
            label: 'Temperature',
            fill: false,
            borderColor: documentStyle.getPropertyValue('--green-500'),
            yAxisID: 'temperature',
            tension: 0.4,
            data: this.historyTemperature,
          },
        ],
      };
    },
    humidityChartData() {
      const documentStyle = getComputedStyle(document.documentElement);
      return {
        labels: ['January', 'February', 'March', 'April', 'May', 'June'],
        datasets: [
          {
            label: 'Humidity',
            fill: false,
            borderColor: documentStyle.getPropertyValue('--blue-500'),
            yAxisID: 'humidity',
            tension: 0.4,
            data: this.historyHumidity,
          },
        ],
      };
    },
  },
};
</script>

<style scoped>
.flexRow {
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  widows: 100%;
}

.halfWidth {
  width: 49%;
}
</style>
