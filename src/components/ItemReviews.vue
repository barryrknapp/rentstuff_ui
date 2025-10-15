<template>
  <div class="reviews-section">
    <h4 @click="toggleReviews">Item Reviews ({{ reviews.length }})</h4>
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
  name: "ItemReviews",
  props: {
    itemId: {
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
  },
  methods: {
    async toggleReviews() {
      this.$emit("update:show-reviews", !this.showReviews);
      if (!this.showReviews && !this.reviews.length) {
        try {
          const response = await axios.get(
            `${this.apiBaseUrl}/rentstuff/reviews/rentalitems/${this.itemId}`
          );
          this.$emit("update:reviews", response.data || []);
        } catch (error) {
          console.error("Error fetching item reviews:", error);
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