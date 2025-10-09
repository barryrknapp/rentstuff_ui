<template>
  <div>
    <h2>Available Rentals</h2>
    <div class="rental-items-grid">
      <RentalItemCard v-for="item in rentalItems" :key="item.id" :item="item" />
    </div>
  </div>
</template>

<script>
import axios from "axios";
import RentalItemCard from "../components/RentalItemCard.vue";

export default {
  components: { RentalItemCard },
  data() {
    return {
      rentalItems: [],
    };
  },
  async created() {
    try {
      const response = await axios.get(
        "http://localhost:8081/rentalitems/by-taxonomy/1"
      ); // Adjust taxonomyId
      this.rentalItems = response.data;
    } catch (error) {
      console.error("Error fetching rental items:", error);
    }
  },
};
</script>