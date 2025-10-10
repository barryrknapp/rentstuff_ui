<template>
  <div id="app">
    <header>
      <div class="container">
        <nav>
          <div>
            <router-link to="/">RentStuff</router-link>
            <router-link to="/new-item" class="ml-2">Add Equipment</router-link>
          </div>
          <div>
            <a href="#" @click.prevent="showModal = true">Login / Register</a>
          </div>
        </nav>
      </div>
    </header>
    <main class="container">
      <router-view />
    </main>
    <footer>
      <p>&copy; 2025 RentStuff</p>
    </footer>
    <LoginRegisterModal v-if="showModal" @close="showModal = false" />
  </div>
</template>

<script>
import LoginRegisterModal from "./components/LoginRegisterModal.vue";

export default {
  components: { LoginRegisterModal },
  data() {
    return {
      showModal: false, // Controls LoginRegisterModal visibility
    };
  },
  created() {
    // Show modal on /login route
    this.$router.beforeEach((to, from, next) => {
      if (to.path === "/login") {
        this.showModal = true;
      } else {
        this.showModal = false; // Hide modal on other routes
      }
      next();
    });
  },
};
</script>

<style>
.ml-2 {
  margin-left: 1rem;
}
.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 0 15px;
}
nav {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1rem 0;
}
nav a {
  text-decoration: none;
  color: #007bff;
}
nav a:hover {
  text-decoration: underline;
}
footer {
  text-align: center;
  padding: 1rem 0;
  margin-top: 2rem;
  border-top: 1px solid #ccc;
}
</style>