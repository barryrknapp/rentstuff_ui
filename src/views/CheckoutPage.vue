<template>
  <div class="checkout-details">
    <h2>Checkout</h2>
    <p>Item: {{ item.name }}</p>
    <p>
      Rental Dates: {{ rentalDates.startDate }} to {{ rentalDates.endDate }}
    </p>
    <p>Total Price: ${{ totalPrice }}</p>

    <div v-if="!isAuthenticated">
      <h3>Please Log In or Register</h3>
      <LoginRegisterModal @close="checkAuth" @auth-success="checkAuth" />
    </div>

    <button class="btn" :disabled="!isAuthenticated" @click="completeCheckout">
      Complete Checkout
    </button>
  </div>
</template>

<script>
import axios from "axios";

export default {
  data() {
    return {
      item: {},
      rentalDates: {
        startDate: this.$route.query.startDate,
        endDate: this.$route.query.endDate,
      },
      isAuthenticated: !!localStorage.getItem("token"),
    };
  },
  computed: {
    totalPrice() {
      if (
        !this.item.pricePerDay ||
        !this.rentalDates.startDate ||
        !this.rentalDates.endDate
      )
        return 0;
      const start = new Date(this.rentalDates.startDate);
      const end = new Date(this.rentalDates.endDate);
      const days = Math.ceil((end - start) / (1000 * 60 * 60 * 24)) + 1;
      return days * this.item.pricePerDay;
    },
  },
  async created() {
    await this.fetchItem();
    this.checkAuth();
  },
  methods: {
    async fetchItem() {
      try {
        const response = await axios.get(
          `/rentstuff/rentalitems/${this.$route.params.id}`
        );
        this.item = response.data;
      } catch (error) {
        console.error("Error fetching item:", error);
      }
    },
    checkAuth() {
      this.isAuthenticated = !!localStorage.getItem("token");
    },
    async completeCheckout() {
      try {
        await axios.post(
          "/rentstuff/rentals",
          {
            itemId: this.$route.params.id,
            startDate: this.rentalDates.startDate,
            endDate: this.rentalDates.endDate,
          },
          {
            headers: {
              Authorization: `Bearer ${localStorage.getItem("token")}`,
            },
          }
        );
        alert("Rental successful!");
        this.$router.push("/");
      } catch (error) {
        console.error("Checkout error:", error);
        alert("Checkout failed");
      }
    },
  },
};
</script>