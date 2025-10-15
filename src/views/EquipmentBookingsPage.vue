<template>
  <div>
    <h2>Bookings for {{ itemName || "Equipment" }}</h2>
    <div v-if="loading">Loading...</div>
    <div v-else-if="error" class="error">{{ error }}</div>
    <div v-else-if="bookings.length === 0" class="no-bookings">
      <h1>No bookings for this equipment.</h1>
    </div>
    <div v-else class="booking-list">
      <div v-if="itemImage" class="item-details">
        <img :src="itemImage" alt="Item Image" class="item-image" />
      </div>
      <div v-for="booking in bookings" :key="booking.id" class="booking-item">
        <h3>Booking #{{ booking.id }}</h3>
        <p><strong>Renter ID:</strong> {{ booking.userId }}</p>
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
        <div class="actions">
          <button
            v-if="canDelete(booking.status)"
            class="btn remove"
            @click="deleteBooking(booking.id)"
          >
            Delete
          </button>
        </div>
      </div>
    </div>
    <router-link :to="'/my-equipment'" class="btn secondary"
      >Back to My Equipment</router-link
    >
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      apiBaseUrl:
        import.meta.env.VITE_API_BASE_URL || "http://localhost:8080/rentstuff",
      bookings: [],
      itemName: "",
      itemImage: "",
      loading: false,
      error: null,
    };
  },
  async created() {
    await this.fetchItemAndBookings();
  },
  methods: {
    async fetchItemAndBookings() {
      try {
        this.loading = true;
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        const itemId = this.$route.params.id;

        const itemResponse = await axios.get(
          `/rentstuff/rentalitems/${itemId}`,
          {
            headers: { Authorization: `Bearer ${token}` },
          }
        );
        this.itemName = itemResponse.data.name;
        this.itemImage = itemResponse.data.imageIds?.length
          ? `${this.apiBaseUrl}/rentstuff/rentalitems/images/${itemResponse.data.imageIds[0]}`
          : "https://via.placeholder.com/100";

        const bookingsResponse = await axios.get(
          `/rentstuff/bookings/item/${itemId}`,
          {
            headers: { Authorization: `Bearer ${token}` },
          }
        );
        this.bookings = bookingsResponse.data.filter(
          (booking) => !["CANCELLED"].includes(booking.status)
        );
      } catch (error) {
        console.error(
          "Error fetching data:",
          error.response?.data?.message || error.message
        );
        this.error =
          "Failed to load bookings or item details. Please try again.";
      } finally {
        this.loading = false;
      }
    },
    formatDate(date) {
      if (!date) return "N/A";
      return new Date(date).toLocaleString();
    },
    canDelete(status) {
      return ["PENDING"].includes(status);
    },
    async deleteBooking(bookingId) {
      if (!confirm("Are you sure you want to delete this booking?")) {
        return;
      }
      try {
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        await axios.delete(`/rentstuff/bookings/${bookingId}`, {
          headers: { Authorization: `Bearer ${token}` },
        });
        alert("Booking deleted successfully!");
        await this.fetchItemAndBookings();
      } catch (error) {
        console.error(
          "Error deleting booking:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to delete booking: ${
            error.response?.data?.message || error.message
          }`
        );
      }
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
.item-details {
  margin-bottom: 20px;
}
.item-image {
  max-width: 100px;
  height: auto;
}
.actions {
  display: flex;
  gap: 10px;
  margin-top: 10px;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
  text-decoration: none;
}
.btn.secondary {
  background-color: #6c757d;
}
.btn.remove {
  background-color: #dc3545;
}
.error {
  color: #dc3545;
}
.no-bookings {
  color: #6c757d;
  font-style: italic;
}
</style>