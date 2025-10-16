<template>
  <div v-if="isVisible" class="modal-backdrop">
    <div class="modal">
      <!-- Login Form -->
      <form v-if="isLogin" @submit.prevent="login">
        <div class="tabs">
          <button :class="{ active: isLogin }" @click="isLogin = true">
            Login
          </button>
          <button :class="{ active: !isLogin }" @click="isLogin = false">
            Register
          </button>
        </div>
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
        <button class="btn close" @click="closeModal">Close</button>
      </form>

      <!-- Register Form -->
      <form v-else @submit.prevent="register">
        <div class="tabs">
          <button :class="{ active: isLogin }" @click="isLogin = true">
            Login
          </button>
          <button :class="{ active: !isLogin }" @click="isLogin = false">
            Register
          </button>
        </div>
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
        <div class="form-group terms-checkbox">
          <label>
            <input
              type="checkbox"
              v-model="registerForm.termsAgreed"
              required
            />
            I agree to the
            <router-link to="/terms" target="_blank"
              >Terms and Conditions</router-link
            >
          </label>
        </div>
        <button type="submit" class="btn" :disabled="!registerForm.termsAgreed">
          Register
        </button>
        <button class="btn close" @click="closeModal">Close</button>
      </form>
    </div>
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
        termsAgreed: false, // Added to track checkbox state
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
          throw new Error("Invalid credentials");
        }
        localStorage.setItem("token", token);
        console.log(
          "Stored Token in localStorage:",
          localStorage.getItem("token")
        );
        console.log("Redirect query parameter:", this.$route.query.redirect);
        const redirectPath = this.$route.query.redirect || "/";
        console.log("Redirecting to:", redirectPath);
        this.closeModal();
        this.$emit("login-success");
        this.$router.push(redirectPath);
      } catch (error) {
        console.error(
          "Login failed:",
          error.response?.data?.message || error.message
        );
        alert(
          "Login failed: " + (error.response?.data?.message || error.message)
        );
      }
    },

    async register() {
      if (!this.registerForm.termsAgreed) {
        alert("You must agree to the Terms and Conditions to register.");
        return;
      }
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
        console.log("Redirect query parameter:", this.$route.query.redirect);
        const redirectPath = this.$route.query.redirect || "/";
        console.log("Redirecting to:", redirectPath);
        this.closeModal();
        this.$emit("login-success");
        this.$router.push(redirectPath);
      } catch (error) {
        console.error(
          "Registration failed:",
          error.response?.data?.message || error.message
        );
        alert(
          "Registration failed: " +
            (error.response?.data?.message || error.message)
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
/* Backdrop for the modal */
.modal-backdrop {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.5);
  z-index: 999;
  display: flex;
  justify-content: center;
  align-items: center;
}

/* Modal styles */
.modal {
  background: #ffffff;
  padding: 20px;
  border-radius: 10px;
  max-width: 400px;
  width: 100%;
  z-index: 1000;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
  font-family: "Poppins", sans-serif;
  color: #29383b; /* Dark logo color for text */
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
  font-family: "Poppins", sans-serif;
  font-weight: 600;
  color: #29383b;
  transition: background-color 0.3s ease;
}

.tabs button.active {
  background-color: #17ae85; /* Teal logo color */
  color: #ffffff;
}

.tabs button:hover {
  background-color: #17ae85;
  color: #ffffff;
}

/* Form layout */
form {
  display: flex;
  flex-direction: column;
  gap: 15px;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-group label {
  margin-bottom: 5px;
  font-weight: 600;
  color: #29383b;
}

.form-group input {
  width: 100%;
  padding: 8px;
  border: 1px solid #17ae85; /* Teal border */
  border-radius: 5px;
  box-sizing: border-box;
  font-family: "Poppins", sans-serif;
}

.terms-checkbox {
  display: flex;
  align-items: center;
  gap: 10px;
}

.terms-checkbox label {
  display: flex;
  align-items: center;
  gap: 5px;
  font-size: 0.9rem;
  color: #29383b;
}

.terms-checkbox input[type="checkbox"] {
  width: 20px;
  height: 20px;
  accent-color: #17ae85; /* Teal checkbox */
}

.terms-checkbox a {
  color: #17ae85;
  text-decoration: none;
  font-weight: 600;
}

.terms-checkbox a:hover {
  color: #29383b;
  text-decoration: underline;
}

/* Button styles */
.btn {
  padding: 10px;
  background-color: #17ae85; /* Teal logo color */
  color: #ffffff;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  text-align: center;
  font-family: "Poppins", sans-serif;
  font-weight: 600;
  transition: background-color 0.3s ease;
}

.btn:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}

.btn.close {
  background-color: #29383b; /* Dark logo color */
}

.btn:hover:not(:disabled) {
  background-color: #29383b; /* Dark logo color on hover */
}

.btn.close:hover {
  background-color: #17ae85; /* Teal on hover */
}

/* Responsive adjustments */
@media (max-width: 768px) {
  .modal {
    max-width: 90%;
    padding: 15px;
  }

  h2 {
    font-size: 1.5rem;
  }
}
</style>