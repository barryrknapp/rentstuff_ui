<template>
  <div>
    <h2>{{ item.name }}</h2>
    <img
      :src="item.imageUrl || 'https://via.placeholder.com/300'"
      alt="Rental Item"
    />
    <p>{{ item.description }}</p>
    <p class="price">${{ item.pricePerDay }} / day</p>
    <p>Min Days: {{ item.minDays }} | Max Days: {{ item.maxDays }}</p>

    <div class="calendar-widget">
      <h3>Select Rental Dates</h3>
      <DateRangePicker v-model:range="rentalDates" />
    </div>

    <div class="reviews-section">
      <h4 @click="toggleItemReviews">
        Item Reviews ({{ itemReviews.length }})
      </h4>
      <div class="reviews-content" :class="{ active: showItemReviews }">
        <div v-for="review in itemReviews" :key="review.id" class="review">
          <p>{{ review.comment }}</p>
          <p>Rating: {{ review.rating }} / 5</p>
        </div>
      </div>
    </div>

    <div class="reviews-section">
      <h4 @click="toggleUserReviews">
        User Reviews ({{ userReviews.length }})
      </h4>
      <div class="reviews-content" :class="{ active: showUserReviews }">
        <div v-for="review in userReviews" :key="review.id" class="review">
          <p>{{ review.comment }}</p>
          <p>Rating: {{ review.rating }} / 5</p>
        </div>
      </div>
    </div>

    <router-link
      :to="`/checkout/${item.id}?startDate=${rentalDates.startDate}&endDate=${rentalDates.endDate}`"
      class="btn"
      :class="{ disabled: !rentalDates.startDate || !rentalDates.endDate }"
    >
      Rent Item
    </router-link>
  </div>
</template>

<script>
import axios from "axios";
import DateRangePicker from "../components/DateRangePicker.vue";

export default {
  components: { DateRangePicker },
  data() {
    return {
      item: {},
      rentalDates: { startDate: "", endDate: "" },
      itemReviews: [],
      userReviews: [],
      showItemReviews: false,
      showUserReviews: false,
    };
  },
  async created() {
    await this.fetchItem();
  },
  methods: {
    async fetchItem() {
      try {
        const response = await axios.get(
          `http://localhost:8081/rentalitems/${this.$route.params.id}`
        );
        this.item = response.data;
      } catch (error) {
        console.error("Error fetching item:", error);
      }
    },
    async toggleItemReviews() {
      this.showItemReviews = !this.showItemReviews;
      if (this.showItemReviews && !this.itemReviews.length) {
        try {
          const response = await axios.get(
            `http://localhost:8081/rentalitems/${this.$route.params.id}/reviews`
          );
          this.itemReviews = response.data;
        } catch (error) {
          console.error("Error fetching item reviews:", error);
        }
      }
    },
    async toggleUserReviews() {
      this.showUserReviews = !this.showUserReviews;
      if (this.showUserReviews && !this.userReviews.length) {
        try {
          const response = await axios.get(
            `http://localhost:8081/users/${this.item.ownerId}/reviews`
          );
          this.userReviews = response.data;
        } catch (error) {
          console.error("Error fetching user reviews:", error);
        }
      }
    },
  },
};
</script>