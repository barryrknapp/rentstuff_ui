<template>
  <div>
    <h1>Rental Items</h1>
    <div class="items">
      <RentalItemCard
        v-for="item in rentalItems"
        :key="item.id"
        :item="item"
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
    };
  },
  async created() {
    try {
      const response = await axios.get("/rentstuff/rentalitems");
      this.rentalItems = response.data;
    } catch (error) {
      console.error(
        "Error fetching rental items:",
        error.response?.data?.message || error.message
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