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
        // Log the redirect query parameter to verify
        console.log("Redirect query parameter:", this.$route.query.redirect);
        // Use the redirect query parameter or fallback to home
        const redirectPath = this.$route.query.redirect || "/";
        console.log("Redirecting to:", redirectPath);
        this.closeModal();
        this.$router.push(redirectPath);
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
        // Log the redirect query parameter to verify
        console.log("Redirect query parameter:", this.$route.query.redirect);
        // Use the redirect query parameter or fallback to home
        const redirectPath = this.$route.query.redirect || "/";
        console.log("Redirecting to:", redirectPath);
        this.closeModal();
        this.$router.push(redirectPath);
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

<style scoped>
/* Add a backdrop for the modal */
.modal-backdrop {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.5); /* Semi-transparent black backdrop */
  z-index: 999;
}

/* Modal styles */
.modal {
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  background: #ffffff; /* Solid white background */
  padding: 20px;
  border-radius: 5px;
  max-width: 400px;
  width: 100%;
  z-index: 1000; /* Ensure modal is above backdrop */
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); /* Optional: Add shadow for depth */
}

/* Tabs for login/register */
.tabs {
  display: flex;
  gap: 10px;
  margin-bottom: 20px;
}

.tabs button {
  flex: 1;
  padding: 10px;
  cursor: pointer;
  background-color: #f1f1f1;
  border: none;
  border-radius: 5px;
}

.tabs button.active {
  background-color: #007bff;
  color: white;
}

/* Form layout */
form {
  display: flex;
  flex-direction: column; /* Stack all form elements vertically */
  gap: 15px; /* Consistent spacing between elements */
}

.form-group {
  display: flex;
  flex-direction: column; /* Stack label and input vertically */
}

.form-group label {
  margin-bottom: 5px;
  font-weight: bold;
}

.form-group input {
  width: 100%;
  padding: 8px;
  border: 1px solid #ccc;
  border-radius: 5px;
  box-sizing: border-box; /* Ensure padding doesn't affect width */
}

/* Button styles */
.btn {
  padding: 10px;
  background-color: #007bff;
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  text-align: center;
}

.btn.close {
  background-color: #dc3545;
  margin-top: 10px;
}

.btn:hover {
  opacity: 0.9;
}

.btn.close:hover {
  opacity: 0.9;
}
</style>