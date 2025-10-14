<template>
  <div>
    <h2>My Bookings</h2>
    <div v-if="loading">Loading...</div>
    <div v-else-if="error" class="error">{{ error }}</div>
    <div v-else-if="bookings.length === 0" class="no-bookings">
      You have no bookings.
    </div>
    <div v-else class="booking-list">
      <div v-for="booking in bookings" :key="booking.id" class="booking-item">
        <h3>Booking #{{ booking.id }}</h3>
        <p><strong>Item ID:</strong> {{ booking.itemId }}</p>
        <p><strong>Start Date:</strong> {{ formatDate(booking.startDate) }}</p>
        <p><strong>End Date:</strong> {{ formatDate(booking.endDate) }}</p>
        <p>
          <strong>Total Price:</strong> ${{ booking.totalPrice.toFixed(2) }}
        </p>
        <p><strong>Status:</strong> {{ booking.status }}</p>
        <p><strong>Created:</strong> {{ formatDate(booking.createDate) }}</p>
        <p v-if="booking.modifyDate">
          <strong>Modified:</strong> {{ formatDate(booking.modifyDate) }}
        </p>
        <p>
          <strong>Payments:</strong>
          {{
            booking.paymentIds.length ? booking.paymentIds.join(", ") : "None"
          }}
        </p>
      </div>
    </div>
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      bookings: [],
      loading: false,
      error: null,
    };
  },
  async created() {
    await this.fetchMyBookings();
  },
  methods: {
    async fetchMyBookings() {
      try {
        this.loading = true;
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        const response = await axios.get("/rentstuff/bookings/my-bookings", {
          headers: { Authorization: `Bearer ${token}` },
        });
        this.bookings = response.data;
      } catch (error) {
        console.error(
          "Error fetching bookings:",
          error.response?.data?.message || error.message
        );
        this.error = "Failed to load bookings. Please try again.";
      } finally {
        this.loading = false;
      }
    },
    formatDate(date) {
      if (!date) return "N/A";
      return new Date(date).toLocaleString();
    },
  },
};
</script>

<style scoped>
.booking-list {
  display: grid;
  gap: 20px;
}
.booking-item {
  border: 1px solid #ccc;
  padding: 15px;
  border-radius: 5px;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
  text-decoration: none;
}
.error {
  color: #dc3545;
}
.no-bookings {
  color: #6c757d;
  font-style: italic;
}
</style>