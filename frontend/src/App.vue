<template>
  <AppHeader />
  <RouterView />
  <AppFooter />
</template>

<script>
import { RouterView } from 'vue-router';
import AppFooter from './components/AppFooter.vue';
import AppHeader from './components/AppHeader.vue';

import { mapStores } from 'pinia';
import { useSettingsStore } from '@/store/settings';

export default {
  name: 'App',
  components: {
    RouterView,
    AppHeader,
    AppFooter,
  },
  data() {
    return {};
  },
  created() {},
  mounted() {},
  unmounted() {},
  methods: {},
  computed: {
    ...mapStores(useSettingsStore),
  },
};
</script>

<style scoped></style>
