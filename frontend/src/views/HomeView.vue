<template>
  <div>
    <Card>
      <template #title>Status</template>
      <template #content>
        <p><span style="font-weight: bold;">WiFi Status</span>: {{ status }}</p>
        <p><span style="font-weight: bold;">WiFi SSID</span>: {{ ssid }}</p>
        <p><span style="font-weight: bold;">WiFi IP Address</span>: {{ ip }}</p>
        <p><span style="font-weight: bold;">Temperature</span>: {{ temperature }} &deg;F</p>
        <p><span style="font-weight: bold;">Humidity</span>: {{ humidity }}%</p>
        <p><span style="font-weight: bold;">Heater Pulse Width</span>: {{ heaterPulseWidth }}%</p>
        <br />
        <font-awesome-icon icon="fa-regular fa-lightbulb" style="padding-right: 0.5em;"/>
        <label for="light">Light</label>
        <InputSwitch v-model="light" inputId="light" style="margin-left: 1em; margin-top: -1em;"
          @change="toggleLight" />
        <br />
      </template>
    </Card>
    <br />
    <Card>
      <template #title>Temperature History</template>
      <template #content>
        <Chart ref="tempChart" type="line" :data="temperatureChartData" :options="temperatureChartOptions" />
      </template>
    </Card>
    <br />
    <Card>
      <template #title>Humidity History</template>
      <template #content>
        <Chart ref="humidityChart" type="line" :data="humidityChartData" :options="humidityChartOptions" />
      </template>
    </Card>
  </div>
</template>

<script>
import Card from 'primevue/card';
import Chart from 'primevue/chart';
import InputSwitch from 'primevue/inputswitch';
import { Chart as ChartJS } from 'chart.js';
import annotationPlugin from 'chartjs-plugin-annotation';
ChartJS.register(annotationPlugin);
import moment from 'moment';

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

    if (window.innerWidth < 600) {
      this.temperatureChartOptions.scales.x.display = false;
      this.humidityChartOptions.scales.x.display = false;
    }

    if (window.EventSource) {
      this.setupEvents();
    } else {
      console.log('Events Not Supported');
    }
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
      heaterPulseWidth: 0,
      light: false,
      historyTemperature: [],
      historyTemperatureTimes: [],
      temperatureSetpoint: 0,
      historyHumidity: [],
      historyHumidityTimes: [],
      expoBackoff: 0,
      humidityChartOptions: {
        animation: {
          duration: 0,
        },
        plugins: {
          zoom: false,
          legend: {
            labels: {
              color: textColor,
            },
          },
        },
        scales: {
          x: {
            display: true,
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
        },
      },
      temperatureChartOptions: {
        animation: {
          duration: 0,
        },
        plugins: {
          zoom: false,
          legend: {
            labels: {
              color: textColor,
            },
          },
          annotation: {
            annotations: {
              line1: {
                type: 'line',
                yMin: 82,
                yMax: 82,
                borderColor: 'rgb(255, 99, 132)',
                borderWidth: 2,
              },
            },
          },
        },
        scales: {
          x: {
            display: true,
            ticks: {
              color: textColorSecondary,
            },
            grid: {
              color: surfaceBorder,
            },
          },
          temperature: {
            display: true,
            position: 'left',
            ticks: {
              color: textColorSecondary,
            },
            grid: {
              color: surfaceBorder,
            },
          },
        },
      },
    };
  },
  methods: {
    setupEvents() {
      let url = '/events';
      if (window.location.port == 5173) {
        url = 'http://snek.local/events';
      }

      const source = new EventSource(url);

      source.addEventListener('open', (_e) => {
        console.log('Events Connected');
      }, false);

      source.addEventListener('error', (e) => {
        if (e.target.readyState != EventSource.OPEN) {
          // Reconnect with exponential backoff
          setTimeout(() => {
            this.expoBackoff = this.expoBackoff + 1;
            this.setupEvents();
          }, Math.pow(2, this.expoBackoff) * 1000);
        }
      }, false);

      source.addEventListener('state', (e) => {
        const state = JSON.parse(e.data);
        this.temperatureChartOptions.plugins.annotation.annotations.line1.yMin = this.temperatureSetpoint;
        this.temperatureChartOptions.plugins.annotation.annotations.line1.yMax = this.temperatureSetpoint;

        if ('temperature' in state) {
          this.temperature = state.temperature.toFixed(2);
          if (this.historyTemperature.length > 1000) {
            this.$refs.tempChart.chart.data.labels = this.$refs.tempChart.chart.data.labels.slice(1);
            this.$refs.tempChart.chart.data.datasets[0].data =
              this.$refs.tempChart.chart.data.datasets[0].data.slice(1);
          }
          this.$refs.tempChart.chart.data.datasets[0].data =
            [...this.$refs.tempChart.chart.data.datasets[0].data, state.temperature];
          this.$refs.tempChart.chart.data.labels =
            [...this.$refs.tempChart.chart.data.labels, moment.unix(state.lastUpdate).format('hh:mm:ss')];
          this.$refs.tempChart.chart.update();
        }
        if ('humidity' in state) {
          this.humidity = state.humidity.toFixed(2);
          if (this.historyHumidity.length > 1000) {
            this.$refs.humidityChart.chart.data.labels = this.$refs.humidityChart.chart.data.labels.slice(1);
            this.$refs.humidityChart.chart.data.datasets[0].data =
              this.$refs.humidityChart.chart.data.datasets[0].data.slice(1);
          }
          this.$refs.humidityChart.chart.data.datasets[0].data =
            [...this.$refs.humidityChart.chart.data.datasets[0].data, state.humidity];
          this.$refs.humidityChart.chart.data.labels =
            [...this.$refs.humidityChart.chart.data.labels, moment.unix(state.lastUpdate).format('hh:mm:ss')];
          this.$refs.humidityChart.chart.update();
        }

        this.lastUpdate = moment.unix(state.lastUpdate).fromNow();
        this.heaterPulseWidth = ((state.heaterPulseWidth / 255) * 100).toFixed(2);
        this.temperatureSetpoint = state.temperatureSetpoint;
        this.light = state.light;
      }, false);
    },
    toggleLight() {
      API.post('/toggle/light', { light: this.light })
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            this.light = response.data.status == 'on';
          }
        })
        .catch((error) => {
          console.log(error);
        });
    },
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
      API.get('/history/temperature')
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            this.historyTemperature = response.data.data;
            const times = [];
            for (let i = 0; i < response.data.timestamps.length; i++) {
              times.push(moment.unix(response.data.timestamps[i]).format('hh:mm:ss'));
            }
            this.historyTemperatureTimes = times;
          }
        })
        .catch((error) => {
          console.log(error);
        });
    },
    getHumidityHistory() {
      API.get('/history/humidity')
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            this.historyHumidity = response.data.data;
            const times = [];
            for (let i = 0; i < response.data.timestamps.length; i++) {
              times.push(moment.unix(response.data.timestamps[i]).format('hh:mm:ss'));
            }
            this.historyHumidityTimes = times;
          }
        })
        .catch((error) => {
          console.log(error);
        });
    },
    getState() {
      API.get('/state')
        .then((response) => {
          if (response.data && response.data.status != 'error') {
            // Trim all but two decimal places from the temperature and humidity
            this.temperature = response.data.temperature.toFixed(2);
            this.humidity = response.data.humidity.toFixed(2);
            // lastUpdate is the number of seconds since the epoch
            this.lastUpdate = moment.unix(response.data.lastUpdate).fromNow();
            this.temperatureSetpoint = response.data.temperatureSetpoint;
            this.temperatureChartOptions.plugins.annotation.annotations.line1.yMin = this.temperatureSetpoint;
            this.temperatureChartOptions.plugins.annotation.annotations.line1.yMax = this.temperatureSetpoint;
            // heaterPulseWidth is between 0 and 255, but we want to display it as a percentage
            this.heaterPulseWidth = ((response.data.heaterPulseWidth / 255) * 100).toFixed(2);
            this.light = response.data.light;
          }
        })
        .catch((error) => {
          console.log(error);
        });
    },
  },
  computed: {
    temperatureChartData() {
      const documentStyle = getComputedStyle(document.documentElement);
      return {
        labels: this.historyTemperatureTimes,
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
        labels: this.historyHumidityTimes,
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
