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
            <a href="#" @click.prevent="showLoginModal = true"
              >Login / Register</a
            >
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
    <LoginRegisterModal v-if="showLoginModal" @close="showLoginModal = false" />
  </div>
</template>

<script>
import LoginRegisterModal from "./components/LoginRegisterModal.vue";

export default {
  components: { LoginRegisterModal },
  data() {
    return {
      showLoginModal: false,
    };
  },
};
</script>

<style>
.ml-2 {
  margin-left: 1rem;
}
</style>