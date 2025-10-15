<template>
  <div>
    <div class="summary">
      Rent out your gear, rake in cash. Need equipment? Grab what's listed. Your
      stuff, your rules—start renting or listing today and join the RentStuff
      CLUB!
    </div>
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
      console.log("API base url " + this.apiBaseUrl);
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
  padding: 20px;
  background: #29383b; /* Dark logo color as background */
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

/* Responsive adjustments */
@media (max-width: 768px) {
  .summary {
    font-size: 1.8rem;
    padding: 20px;
  }
}
</style>