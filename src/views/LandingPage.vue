<template>
  <div>
    <h1>Rental Items</h1>
    <div class="items">
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
      const response = await axios.get("/rentstuff/rentalitems");
      this.rentalItems = response.data;
    } catch (error) {
      console.error(
        "Error fetching rental items:",
        error.response?.data || error.message
      );
    }
  },
};
</script>

<style scoped>
.items {
  display: flex;
  flex-wrap: wrap;
  gap: 20px;
  justify-content: center;
}
</style>