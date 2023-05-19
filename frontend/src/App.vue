<template>
  <AppHeader />
  <RouterView />
  <AppFooter />
</template>

<script>
import { RouterView } from 'vue-router';
import AppHeader from './components/AppHeader.vue';
import AppFooter from './components/AppFooter.vue';

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
  computed: {},
};
</script>

<style scoped></style>
