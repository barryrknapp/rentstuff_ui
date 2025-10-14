<template>
  <div>
    <h2>My Equipment</h2>
    <div v-if="loading">Loading...</div>
    <div v-else-if="error" class="error">{{ error }}</div>
    <div v-else-if="items.length === 0" class="no-items">
      <h2>You have no equipment listed for rental.</h2>
    </div>
    <div v-else class="equipment-list">
      <div v-for="item in items" :key="item.id" class="equipment-item">
        <h3>{{ item.name }}</h3>
        <div v-if="item.imageIds?.length" class="item-image">
          <img :src="getImageUrl(item.imageIds[0])" alt="Item Image" />
        </div>
        <p>{{ item.description }}</p>
        <p><strong>Status:</strong> {{ item.paused ? "Paused" : "Active" }}</p>
        <div class="prices">
          <p v-for="price in item.prices" :key="price.id">
            ${{ price.price.toFixed(2) }} per day for min
            {{ price.minDaysRented }} days
          </p>
        </div>
        <div class="actions">
          <router-link :to="`/equipment/edit/${item.id}`" class="btn"
            >Edit</router-link
          >
          <router-link :to="`/equipment/${item.id}/bookings`" class="btn"
            >View Bookings</router-link
          >
          <button
            class="btn"
            :class="{ 'btn-pause': !item.paused, 'btn-resume': item.paused }"
            @click="togglePause(item.id, item.paused)"
          >
            {{ item.paused ? "Resume Rentals" : "Pause Rentals" }}
          </button>
        </div>
      </div>
    </div>
    <router-link to="/new-item" class="btn secondary"
      >Add New Equipment</router-link
    >
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      items: [],
      loading: false,
      error: null,
    };
  },
  async created() {
    await this.fetchMyEquipment();
  },
  methods: {
    async fetchMyEquipment() {
      try {
        this.loading = true;
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        const response = await axios.get("/rentstuff/rentalitems/owned", {
          headers: { Authorization: `Bearer ${token}` },
        });
        this.items = response.data;
      } catch (error) {
        console.error(
          "Error fetching equipment:",
          error.response?.data?.message || error.message
        );
        this.error = "Failed to load equipment. Please try again.";
      } finally {
        this.loading = false;
      }
    },
    async togglePause(itemId, currentPaused) {
      try {
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        await axios.patch(
          `/rentstuff/rentalitems/${itemId}/pause`,
          { paused: !currentPaused },
          {
            headers: { Authorization: `Bearer ${token}` },
          }
        );
        alert(`Item ${currentPaused ? "resumed" : "paused"} successfully!`);
        await this.fetchMyEquipment();
      } catch (error) {
        console.error(
          "Error toggling pause status:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to ${currentPaused ? "resume" : "pause"} item: ${
            error.response?.data?.message || error.message
          }`
        );
      }
    },
    getImageUrl(imageId) {
      return `${
        import.meta.env.VITE_API_BASE_URL
      }/rentstuff/rentalitems/images/${imageId}`;
    },
  },
};
</script>

<style scoped>
.equipment-list {
  display: grid;
  gap: 20px;
}
.equipment-item {
  border: 1px solid #ccc;
  padding: 15px;
  border-radius: 5px;
}
.item-image img {
  max-width: 100px;
  height: auto;
  margin-bottom: 10px;
}
.prices {
  margin: 10px 0;
}
.actions {
  display: flex;
  gap: 10px;
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
.btn-pause {
  background-color: #dc3545;
}
.btn-resume {
  background-color: #28a745;
}
.error {
  color: #dc3545;
}
.no-items {
  color: #6c757d;
  font-style: italic;
}
</style>