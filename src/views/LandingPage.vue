<template>
  <div>
    <div class="summary">
      List gear free, earn cash. Rent with no platform fees. Your stuff, your
      rules—join RentStuff CLUB today!
    </div>
    <div class="filter">
      <div class="form-group">
        <label for="zipCode">Your ZIP Code</label>
        <input
          id="zipCode"
          v-model="userZipCode"
          type="text"
          pattern="\d{5}"
          placeholder="Enter 5-digit ZIP code"
          @input="fetchUserLatLong"
        />
      </div>
    </div>
    <div class="items">
      <RentalItemCard
        v-for="item in filteredItems"
        :key="item.id"
        :item="item"
        :distance="item.distance"
        :api-base-url="apiBaseUrl"
      />
    </div>
  </div>
</template>

<script>
import axios from "axios";
import RentalItemCard from "@/components/RentalItemCard.vue";

export default {
  components: { RentalItemCard },
  data() {
    return {
      apiBaseUrl:
        import.meta.env.VITE_API_BASE_URL || "http://localhost:8080/rentstuff",
      rentalItems: [],
      userZipCode: localStorage.getItem("userZipCode") || "",
      userLatLong: null,
    };
  },
  computed: {
    filteredItems() {
      if (!this.userLatLong || !this.userZipCode) {
        return this.rentalItems.map((item) => ({ ...item, distance: null }));
      }
      return this.rentalItems
        .map((item) => {
          const distance =
            item.latitude && item.longitude
              ? this.haversineDistance(
                  this.userLatLong.latitude,
                  this.userLatLong.longitude,
                  item.latitude,
                  item.longitude
                )
              : null;
          return { ...item, distance };
        })
        .sort((a, b) => (a.distance || 0) - (b.distance || 0));
    },
  },
  watch: {
    userZipCode(newZip) {
      localStorage.setItem("userZipCode", newZip);
      this.fetchUserLatLong();
    },
  },
  async created() {
    await this.fetchItems();
    if (this.userZipCode) {
      await this.fetchUserLatLong();
    }
  },
  methods: {
    async fetchItems() {
      try {
        const response = await axios.get(
          `${this.apiBaseUrl}/rentstuff/rentalitems`
        );
        this.rentalItems = response.data;
        console.log("API base url " + this.apiBaseUrl);
      } catch (error) {
        console.error(
          "Error fetching rental items:",
          error.response?.data?.message || error.message
        );
      }
    },
    async fetchUserLatLong() {
      if (!this.userZipCode.match(/^\d{5}$/)) {
        this.userLatLong = null;
        return;
      }
      try {
        const response = await axios.get(
          `https://api.zippopotam.us/us/${this.userZipCode}`
        );
        if (response.data.places && response.data.places.length > 0) {
          this.userLatLong = {
            latitude: parseFloat(response.data.places[0].latitude),
            longitude: parseFloat(response.data.places[0].longitude),
          };
        } else {
          this.userLatLong = null;
          alert("Invalid ZIP code");
        }
      } catch (error) {
        console.error("Error fetching user lat/long:", error.message);
        this.userLatLong = null;
        alert("Failed to fetch location for ZIP code");
      }
    },
    haversineDistance(lat1, lon1, lat2, lon2) {
      const R = 3958.8; // Earth radius in miles
      const dLat = ((lat2 - lat1) * Math.PI) / 180;
      const dLon = ((lon2 - lon1) * Math.PI) / 180;
      const a =
        Math.sin(dLat / 2) * Math.sin(dLat / 2) +
        Math.cos((lat1 * Math.PI) / 180) *
          Math.cos((lat2 * Math.PI) / 180) *
          Math.sin(dLon / 2) *
          Math.sin(dLon / 2);
      const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
      return R * c;
    },
  },
};
</script>

<style scoped>
.items {
  display: flex;
  flex-wrap: wrap;
  gap: 20px;
  justify-content: center;
  padding: 20px;
  background: #29383b; /* Dark logo color as background */
}

.items > * {
  flex: 1 1 200px; /* Base width for items, allows 1-2 on small screens */
  max-width: 300px; /* Caps item width to prevent over-stretching */
}

/* Styling for the summary text */
.summary {
  text-align: center;
  font-family: "Poppins", sans-serif; /* Modern, clean font */
  font-size: 2.5rem;
  font-weight: 700;
  color: #ffffff; /* White text for contrast */
  text-shadow: 3px 3px 6px rgba(0, 0, 0, 0.5); /* Strong shadow for depth */
  padding: 30px 20px;
  margin: 0 auto;
  max-width: 800px;
  background: rgba(
    23,
    174,
    133,
    0.8
  ); /* Semi-transparent logo teal for overlay */
  border: 2px solid #17ae85; /* Solid teal border to reinforce brand */
  border-radius: 15px; /* Rounded corners */
  animation: fadeIn 1.5s ease-in-out; /* Smooth fade-in animation */
}

/* Animation for pop effect */
@keyframes fadeIn {
  0% {
    opacity: 0;
    transform: scale(0.9);
  }
  100% {
    opacity: 1;
    transform: scale(1);
  }
}

/* Styling for the filter section */
.filter {
  display: flex;
  gap: 20px;
  margin: 20px auto;
  justify-content: center;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-group label {
  margin-bottom: 5px;
  color: #ffffff;
  font-family: "Poppins", sans-serif;
  font-weight: 500;
}

.form-group input {
  padding: 8px;
  width: 200px;
  border-radius: 5px;
  border: 1px solid #17ae85;
  background: #ffffff;
  color: #29383b;
}

/* Responsive adjustments */
@media (max-width: 768px) {
  .summary {
    font-size: 1.8rem;
    padding: 20px;
  }

  .form-group input {
    width: 100%;
    max-width: 200px;
  }

  .items > * {
    flex: 1 1 45%; /* Allows 1-2 items per row on small screens */
    max-width: 45%;
  }
}

@media (min-width: 1200px) {
  .items > * {
    flex: 1 1 16%; /* Allows up to 5-6 items per row on wide screens */
    max-width: 18%;
  }
}
</style>