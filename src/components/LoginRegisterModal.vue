<template>
  <div v-if="isVisible" class="modal">
    <div class="tabs">
      <button :class="{ active: isLogin }" @click="isLogin = true">
        Login
      </button>
      <button :class="{ active: !isLogin }" @click="isLogin = false">
        Register
      </button>
    </div>

    <!-- Login Form -->
    <form v-if="isLogin" @submit.prevent="login">
      <h2>Login</h2>
      <div class="form-group">
        <label for="login-email">Email</label>
        <input
          id="login-email"
          v-model="loginForm.email"
          type="email"
          required
        />
      </div>
      <div class="form-group">
        <label for="login-password">Password</label>
        <input
          id="login-password"
          v-model="loginForm.password"
          type="password"
          required
        />
      </div>
      <button type="submit" class="btn">Login</button>
    </form>

    <!-- Register Form -->
    <form v-else @submit.prevent="register">
      <h2>Register</h2>
      <div class="form-group">
        <label for="register-email">Email</label>
        <input
          id="register-email"
          v-model="registerForm.email"
          type="email"
          required
        />
      </div>
      <div class="form-group">
        <label for="register-password">Password</label>
        <input
          id="register-password"
          v-model="registerForm.password"
          type="password"
          required
        />
      </div>
      <div class="form-group">
        <label for="register-firstName">First Name</label>
        <input
          id="register-firstName"
          v-model="registerForm.firstName"
          type="text"
          required
        />
      </div>
      <div class="form-group">
        <label for="register-lastName">Last Name</label>
        <input
          id="register-lastName"
          v-model="registerForm.lastName"
          type="text"
          required
        />
      </div>
      <button type="submit" class="btn">Register</button>
    </form>
    <button class="btn close" @click="closeModal">Close</button>
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      isVisible: true,
      isLogin: true,
      loginForm: {
        email: "",
        password: "",
      },
      registerForm: {
        email: "",
        password: "",
        firstName: "",
        lastName: "",
      },
    };
  },
  methods: {
    async login() {
      try {
        const response = await axios.post("/rentstuff/auth/login", {
          email: this.loginForm.email,
          password: this.loginForm.password,
        });
        const token = response.data;
        console.log("Login Response Token:", token);
        if (!token || typeof token !== "string" || !token.startsWith("eyJ")) {
          throw new Error("Invalid token received");
        }
        localStorage.setItem("token", token);
        console.log(
          "Stored Token in localStorage:",
          localStorage.getItem("token")
        );
        this.closeModal();
        this.$router.push("/new-item");
      } catch (error) {
        console.error("Login failed:", error.response?.data || error.message);
        alert("Login failed: " + (error.response?.data || error.message));
      }
    },
    async register() {
      try {
        const response = await axios.post("/rentstuff/auth/register", {
          email: this.registerForm.email,
          password: this.registerForm.password,
          firstName: this.registerForm.firstName,
          lastName: this.registerForm.lastName,
          role: "USER",
        });
        const token = response.data;
        console.log("Register Response Token:", token);
        if (!token || typeof token !== "string" || !token.startsWith("eyJ")) {
          throw new Error("Invalid token received");
        }
        localStorage.setItem("token", token);
        console.log(
          "Stored Token in localStorage:",
          localStorage.getItem("token")
        );
        this.closeModal();
        this.$router.push("/new-item");
      } catch (error) {
        console.error(
          "Registration failed:",
          error.response?.data || error.message
        );
        alert(
          "Registration failed: " + (error.response?.data || error.message)
        );
      }
    },
    closeModal() {
      this.isVisible = false;
      this.$emit("close");
    },
  },
};
</script>

<style>
.modal {
  background: white;
  padding: 20px;
  border-radius: 5px;
  max-width: 400px;
  margin: auto;
}
.tabs {
  display: flex;
  gap: 10px;
  margin-bottom: 20px;
}
.tabs button {
  padding: 10px;
  cursor: pointer;
}
.tabs button.active {
  background-color: #007bff;
  color: white;
}
.form-group {
  margin-bottom: 15px;
}
.form-group label {
  display: block;
}
.form-group input {
  width: 100%;
  padding: 8px;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
}
.btn.close {
  background-color: #dc3545;
  margin-top: 10px;
}
</style>