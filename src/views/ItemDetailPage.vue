<template>
  <div>
    <h2>{{ item.name }}</h2>
    <div class="image-gallery">
      <img
        v-for="(url, index) in item.imageUrls"
        :key="index"
        :src="url || 'https://via.placeholder.com/300'"
        alt="Rental Item"
      />
    </div>
    <p>{{ item.description }}</p>
    <p class="price">${{ item.pricePerDay }} / day</p>
    <p>Min Days: {{ item.minDays }} | Max Days: {{ item.maxDays }}</p>
    <div class="calendar-widget">
      <h3>Select Rental Dates</h3>
      <DateRangePicker v-model="rentalDates" />
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
      item: { imageUrls: [] },
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
          `/rentstuff/rentalitems/${this.$route.params.id}`
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
            `/rentstuff/reviews/rentalitems/${this.$route.params.id}`
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
          const [reviewsResponse, ratingResponse] = await Promise.all([
            axios.get(`/rentstuff/reviews/users/${this.item.ownerId}`),
            axios.get(
              `/rentstuff/reviews/users/${this.item.ownerId}/average-rating`
            ),
          ]);
          this.userReviews = reviewsResponse.data;
          this.averageUserRating = ratingResponse.data.toFixed(1);
        } catch (error) {
          console.error("Error fetching user reviews:", error);
        }
      }
    },
  },
};
</script>

<style>
.image-gallery {
  display: flex;
  gap: 10px;
  overflow-x: auto;
}
.image-gallery img {
  max-width: 300px;
  height: auto;
}
</style>