<template>
  <div class="reviews-section">
    <h4 @click="toggleReviews">
      User Reviews ({{ reviews.length }})
      <span v-if="averageRating !== 'N/A'"> (Avg: {{ averageRating }})</span>
    </h4>
    <div class="reviews-content" :class="{ active: showReviews }">
      <div v-for="review in reviews" :key="review.id" class="review">
        <p>{{ review.comment }}</p>
        <p>Rating: {{ review.rating }} / 5</p>
      </div>
    </div>
  </div>
</template>

<script>
import axios from "axios";

export default {
  name: "UserReviews",
  props: {
    ownerId: {
      type: [Number, String],
      required: true,
    },
    apiBaseUrl: {
      type: String,
      required: true,
    },
    reviews: {
      type: Array,
      default: () => [],
    },
    showReviews: {
      type: Boolean,
      default: false,
    },
    averageRating: {
      type: String,
      default: "N/A",
    },
  },
  methods: {
    async toggleReviews() {
      this.$emit("update:show-reviews", !this.showReviews);
      if (!this.showReviews && !this.reviews.length) {
        try {
          const [reviewsResponse, ratingResponse] = await Promise.all([
            axios.get(
              `${this.apiBaseUrl}/rentstuff/reviews/users/${this.ownerId}`
            ),
            axios.get(
              `${this.apiBaseUrl}/rentstuff/reviews/users/${this.ownerId}/average-rating`
            ),
          ]);
          this.$emit("update:reviews", reviewsResponse.data || []);
          this.$emit(
            "update:average-rating",
            ratingResponse.data ? ratingResponse.data.toFixed(1) : "N/A"
          );
        } catch (error) {
          console.error("Error fetching user reviews:", error);
        }
      }
    },
  },
};
</script>

<style scoped>
.reviews-section {
  margin: 20px 0;
}
.reviews-content {
  display: none;
}
.reviews-content.active {
  display: block;
}
.review {
  border-bottom: 1px solid #ccc;
  padding: 10px 0;
}
</style>