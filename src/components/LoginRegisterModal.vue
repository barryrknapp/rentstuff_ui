<template>
  <div class="modal">
    <div class="modal-content">
      <button class="close-btn" @click="$emit('close')">×</button>
      <h2>{{ isLogin ? "Login" : "Register" }}</h2>
      <form @submit.prevent="submitForm">
        <div class="form-group">
          <label for="email">Email</label>
          <input id="email" v-model="form.email" type="email" required />
        </div>
        <div class="form-group">
          <label for="password">Password</label>
          <input
            id="password"
            v-model="form.password"
            type="password"
            required
          />
        </div>
        <div v-if="!isLogin" class="form-group">
          <label for="name">Name</label>
          <input id="name" v-model="form.name" type="text" required />
        </div>
        <button type="submit" class="btn">
          {{ isLogin ? "Login" : "Register" }}
        </button>
        <p class="mt-1">
          {{ isLogin ? "Need an account?" : "Already have an account?" }}
          <a href="#" @click.prevent="isLogin = !isLogin">{{
            isLogin ? "Register" : "Login"
          }}</a>
        </p>
      </form>
    </div>
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      isLogin: true,
      form: {
        email: "",
        password: "",
        name: "",
      },
    };
  },
  methods: {
    async submitForm() {
      try {
        const url = this.isLogin ? "/api/auth/login" : "/api/auth/register";
        const response = await axios.post(
          `http://localhost:8081${url}`,
          this.form
        );
        // Assuming the API returns a token
        localStorage.setItem("token", response.data.token);
        this.$emit("close");
        // Emit an event to notify parent of successful auth
        this.$emit("auth-success");
      } catch (error) {
        console.error("Auth error:", error);
        alert("Authentication failed");
      }
    },
  },
};
</script>