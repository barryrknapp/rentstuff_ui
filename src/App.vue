<template>
  <div id="app">
    <header>
      <div class="container">
        <nav>
          <div class="nav-left">
            <router-link to="/"
              ><img src="@/assets/images/logo.png" class="image-header"
            /></router-link>
            <!--router-link to="/new-item" class="ml-2">Add Equipment</router-link-->
            <router-link to="/my-equipment" class="ml-2"
              >My Equipment</router-link
            >
            <router-link to="/my-bookings" class="ml-2"
              >My Bookings</router-link
            >
          </div>
          <div class="nav-right">
            <span v-if="isLoggedIn"
              >Welcome {{ userFirstName }} |
              <a href="#" @click.prevent="logout">Logout</a></span
            >
            <a v-else href="#" @click.prevent="showModal = true"
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
    <LoginRegisterModal
      v-if="showModal"
      @close="showModal = false"
      @login-success="checkAuthStatus"
    />
  </div>
</template>

<script>
import axios from "axios";
import LoginRegisterModal from "./components/LoginRegisterModal.vue";

export default {
  components: { LoginRegisterModal },
  data() {
    return {
      showModal: false,
      isLoggedIn: false,
      userFirstName: "",
    };
  },
  async created() {
    await this.checkAuthStatus();
    this.$router.beforeEach((to, from, next) => {
      if (to.path === "/login") {
        this.showModal = true;
      } else {
        this.showModal = false;
      }
      next();
    });
  },
  methods: {
    async checkAuthStatus() {
      const token = localStorage.getItem("token");
      if (token && token !== "undefined") {
        try {
          const response = await axios.get("/rentstuff/auth/me", {
            headers: { Authorization: `Bearer ${token}` },
          });
          this.isLoggedIn = true;
          this.userFirstName = response.data.firstName || "User";
        } catch (error) {
          console.error(
            "Error fetching user:",
            error.response?.data?.message || error.message
          );
          this.isLoggedIn = false;
          this.userFirstName = "";
          localStorage.removeItem("token");
        }
      } else {
        this.isLoggedIn = false;
        this.userFirstName = "";
      }
    },
    async logout() {
      localStorage.removeItem("token");
      this.isLoggedIn = false;
      this.userFirstName = "";
      await this.$router.push("/");
    },
  },
};
</script>

<style scoped>
.image-header {
  max-width: 200px;
  height: auto;
}
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
.nav-left,
.nav-right {
  display: flex;
  align-items: center;
}
nav a,
nav span {
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