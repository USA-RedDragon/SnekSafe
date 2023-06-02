<template>
  <header>
    <h1>
      <RouterLink to="/">SnekSafe</RouterLink>
    </h1>
    <div class="wrapper">
      <nav>
        <RouterLink to="/">Home</RouterLink>
        <RouterLink to="/settings">Settings</RouterLink>
        <RouterLink to="/updates">Updates</RouterLink>
      </nav>
    </div>
  </header>
</template>

<script>
export default {
  components: {},
  data: function() {
    return {};
  },
  mounted() {},
  methods: {},
  computed: {},
};
</script>

<style scoped>
header {
  text-align: center;
  max-height: 100vh;
}

header a {
  text-decoration: none;
}

header a,
header a:visited,
header a:link {
  color: var(--primary-text-color);
}

header nav .router-link-active {
  color: var(--cyan-300) !important;
}

header nav a:active {
  color: var(--cyan-500) !important;
}

header h1 a,
header h1 a:hover,
header h1 a:visited {
  color: var(--primary-text-color);
  background-color: inherit;
}

nav {
  width: 100%;
  font-size: 1rem;
  text-align: center;
  margin-top: 0.5rem;
  margin-bottom: 1rem;
}

nav a {
  display: inline-block;
  padding: 0 1rem;
  border-left: 2px solid #444;
}

nav a:first-of-type {
  border: 0;
}
</style>
