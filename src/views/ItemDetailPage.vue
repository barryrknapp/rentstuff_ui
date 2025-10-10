<template>
  <div>
    <h2>{{ item.name || "Unnamed Item" }}</h2>
    <div class="image-gallery">
      <img
        v-for="(url, index) in item.imageUrls"
        :key="index"
        :src="url || 'https://via.placeholder.com/300'"
        alt="Rental Item"
      />
    </div>
    <p>{{ item.description || "No description available" }}</p>
    <div v-for="price in item.prices" :key="price.id">
      <p class="price">
        ${{ price.price }} per day if rented at least
        {{ price.minDaysRented }} days.
      </p>
    </div>
    <p>
      You can rent this item for a minimum of {{ item.minDays || "N/A" }} days
      and a maximum of {{ item.maxDays || "N/A" }} days.
    </p>
    <div class="calendar-widget">
      <h3>Select Rental Dates</h3>

      <VueDatePicker
        v-model="selectedDates"
        range
        inline
        :enable-time-picker="false"
        :disabled-dates="disabledDates"
        :min-date="currentDate"
        :start-date="currentDate"
        @update:model-value="validateDates"
      />
      <p v-if="dateValidationError" class="error">{{ dateValidationError }}</p>
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
      :to="`/checkout/${item.id}?startDateTime=${startDateTime}&endDateTime=${endDateTime}`"
      class="btn"
      :class="{ disabled: !isValidRentalDates }"
    >
      Rent Item
    </router-link>
  </div>
</template>

<script>
import axios from "axios";
import VueDatePicker from "@vuepic/vue-datepicker";
import "@vuepic/vue-datepicker/dist/main.css";

export default {
  components: { VueDatePicker },
  data() {
    const currentDate = new Date(); // October 10, 2025
    return {
      item: {
        imageUrls: [],
        prices: [],
        unavailableDates: [],
        minDays: 1,
        maxDays: 30,
      },
      selectedDates: [], // Array for range selection
      itemReviews: [],
      userReviews: [],
      showItemReviews: false,
      showUserReviews: false,
      dateValidationError: "",
      currentDate: new Date(),
    };
  },
  computed: {
    startDateTime() {
      return this.selectedDates[0] ? `${this.selectedDates[0]}T00:00:00` : "";
    },
    endDateTime() {
      return this.selectedDates[1] ? `${this.selectedDates[1]}T23:59:59` : "";
    },
    isValidRentalDates() {
      if (!this.selectedDates[0] || !this.selectedDates[1]) {
        this.dateValidationError = "Please select a start and end date";
        return false;
      }

      const start = new Date(this.selectedDates[0]);
      const end = new Date(this.selectedDates[1]);
      const today = new Date();
      today.setHours(0, 0, 0, 0);

      if (start < today) {
        this.dateValidationError = "Start date must be today or later";
        return false;
      }
      if (end < start) {
        this.dateValidationError = "End date must be after start date";
        return false;
      }

      const durationDays = Math.ceil((end - start) / (1000 * 60 * 60 * 24)) + 1; // Inclusive
      if (durationDays < this.item.minDays) {
        this.dateValidationError = `Rental must be at least ${this.item.minDays} days`;
        return false;
      }
      if (durationDays > this.item.maxDays) {
        this.dateValidationError = `Rental cannot exceed ${this.item.maxDays} days`;
        return false;
      }

      for (const unavailable of this.item.unavailableDates) {
        const unavailableStart = new Date(unavailable.startDate);
        const unavailableEnd = new Date(unavailable.endDate);
        if (start <= unavailableEnd && end >= unavailableStart) {
          this.dateValidationError = `Selected dates overlap with unavailable dates (${unavailable.startDate} to ${unavailable.endDate})`;
          return false;
        }
      }

      this.dateValidationError = "";
      return true;
    },
    disabledDates() {
      return (date) => {
        return this.item.unavailableDates.some((unavailable) => {
          const start = new Date(unavailable.startDate);
          const end = new Date(unavailable.endDate);
          return date >= start && date <= end;
        });
      };
    },
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
        this.item = {
          ...response.data,
          imageUrls: response.data.imageUrls || [],
          prices: response.data.prices || [],
          minDays: response.data.minDays || 1,
          maxDays: response.data.maxDays || 30,
          unavailableDates: response.data.unavailableDates || [],
        };
      } catch (error) {
        console.error(
          "Error fetching item:",
          error.response?.data || error.message
        );
      }
    },
    async toggleItemReviews() {
      this.showItemReviews = !this.showItemReviews;
      if (this.showItemReviews && !this.itemReviews.length) {
        try {
          const response = await axios.get(
            `/rentstuff/reviews/rentalitems/${this.$route.params.id}`
          );
          this.itemReviews = response.data || [];
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
          this.userReviews = reviewsResponse.data || [];
          this.averageUserRating = ratingResponse.data
            ? ratingResponse.data.toFixed(1)
            : "N/A";
        } catch (error) {
          console.error("Error fetching user reviews:", error);
        }
      }
    },
    validateDates() {
      this.isValidRentalDates;
    },
  },
  mounted() {
    console.log("Current date in Vue instance:", this.currentDate);
  },
};
</script>

<style scoped>
.image-gallery {
  display: flex;
  gap: 10px;
  overflow-x: auto;
}
.image-gallery img {
  max-width: 300px;
  height: auto;
}
.price {
  color: #28a745;
  font-weight: bold;
}
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
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  text-decoration: none;
  border-radius: 5px;
}
.btn.disabled {
  background-color: #6c757d;
  pointer-events: none;
}
.error {
  color: #dc3545;
  font-size: 0.9em;
}
.calendar-widget {
  margin: 20px 0;
}
:deep(.dp__cell_disabled) {
  background-color: #dc3545 !important;
  color: white !important;
  position: relative;
}
:deep(.dp__cell_disabled::after) {
  content: "X";
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-weight: bold;
}
:deep(.dp__range_start, .dp__range_end, .dp__range_between) {
  background-color: #007bff !important;
  color: white !important;
}
</style>