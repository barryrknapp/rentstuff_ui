<template>
  <div>
    <h2>Checkout: {{ item.name }}</h2>
    <p>{{ item.description }}</p>
    <div v-for="price in item.prices" :key="price.id">
      <p class="price">
        ${{ price.price }} per day if rented at least
        {{ price.minDaysRented }} days.
      </p>
    </div>
    <p>
      Rental Period: {{ rentalDates.startDateTime }} to
      {{ rentalDates.endDateTime }}
    </p>
    <p v-if="totalPrice !== null" class="total-price">
      Total Price: ${{ totalPrice.toFixed(2) }} for {{ rentalDays }} days
    </p>
    <p v-else class="error">Unable to calculate price</p>
    <button class="btn" :disabled="totalPrice === null" @click="confirmRental">
      Confirm Rental
    </button>
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      item: { name: "", description: "", prices: [] },
      rentalDates: { startDateTime: "", endDateTime: "" },
      totalPrice: null,
      rentalDays: 0,
    };
  },
  async created() {
    const { startDateTime, endDateTime } = this.$route.query;
    this.rentalDates.startDateTime = startDateTime;
    this.rentalDates.endDateTime = endDateTime;

    try {
      const itemResponse = await axios.get(
        `/rentstuff/rentalitems/${this.$route.params.id}`
      );
      this.item = itemResponse.data;

      if (startDateTime && endDateTime) {
        const priceResponse = await axios.get(
          `/rentstuff/rentalitems/${this.$route.params.id}/calculate-price`,
          {
            params: { startDateTime, endDateTime },
          }
        );
        this.totalPrice = priceResponse.data.totalPrice;
        this.rentalDays = priceResponse.data.days;
      }
    } catch (error) {
      console.error(
        "Error fetching item or price:",
        error.response?.data || error.message
      );
      this.totalPrice = null;
    }
  },
  methods: {
    async confirmRental() {
      try {
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          // Log the fullPath to verify
          console.log("Current route fullPath:", this.$route.fullPath);
          this.$router.push({
            path: "/login",
            query: { redirect: this.$route.fullPath },
          });
          return;
        }
        await axios.post(
          `/rentstuff/bookings`,
          {
            itemId: this.item.id,
            startDate: this.rentalDates.startDateTime,
            endDate: this.rentalDates.endDateTime,
            totalPrice: this.totalPrice.toFixed(2),
          },
          {
            headers: { Authorization: `Bearer ${token}` },
          }
        );
        alert("Rental confirmed!");
        this.$router.push("/");
      } catch (error) {
        console.error(
          "Error confirming rental:",
          error.response?.data || error.message
        );
        alert(
          "Failed to confirm rental: " + (error.response?.data || error.message)
        );
      }
    },
  },
};
</script>

<style scoped>
.price {
  color: #28a745;
  font-weight: bold;
}
.total-price {
  font-size: 1.2em;
  font-weight: bold;
  color: #007bff;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
}
.btn:disabled {
  background-color: #6c757d;
  cursor: not-allowed;
}
.error {
  color: #dc3545;
  font-size: 0.9em;
}
</style>