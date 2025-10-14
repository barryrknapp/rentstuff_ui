<template>
  <div>
    <h2>{{ item.name || "Unnamed Item" }}</h2>
    <div class="image-gallery">
      <img
        v-for="imageId in item.imageIds"
        :key="imageId"
        :src="`/rentstuff/rentalitems/images/${imageId}`"
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
        :min-date="currentDateString"
        :start-date="currentDateString"
        :month="currentMonth"
        :year="currentYear"
        :model-config="modelConfig"
        @update:model-value="handleDateUpdate"
      />
      <p v-if="dateValidationError" class="error">{{ dateValidationError }}</p>
      <div v-if="isValidRentalDates" class="time-pickers">
        <div class="time-picker">
          <label>Pickup Time</label>
          <VueDatePicker
            v-model="pickupTime"
            time-picker
            :enable-seconds="false"
            format="hh:mm a"
            :model-config="{ type: 'string', mask: 'HH:mm' }"
            @update:model-value="(time) => handleTimeUpdate(time, true)"
          />
        </div>
        <div class="time-picker">
          <label>Drop-off Time</label>
          <VueDatePicker
            v-model="dropoffTime"
            time-picker
            :enable-seconds="false"
            format="hh:mm a"
            :model-config="{ type: 'string', mask: 'HH:mm' }"
            @update:model-value="(time) => handleTimeUpdate(time, false)"
          />
        </div>
      </div>
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
      :class="{ disabled: !isValidRentalDates || !isValidTimes || item.paused }"
      @click="logRedirect"
    >
      Rent Item
    </router-link>
    <p v-if="item.paused" class="error">
      This item is currently paused and cannot be rented.
    </p>
  </div>
</template>

<script>
import axios from "axios";
import VueDatePicker from "@vuepic/vue-datepicker";
import "@vuepic/vue-datepicker/dist/main.css";

export default {
  components: { VueDatePicker },
  data() {
    const currentDate = new Date();
    return {
      item: {
        imageIds: [],
        prices: [],
        unavailableDates: [],
        minDays: 1,
        maxDays: 30,
      },
      bookings: [],
      selectedDates: [],
      pickupTime: null,
      dropoffTime: null,
      itemReviews: [],
      userReviews: [],
      showItemReviews: false,
      showUserReviews: false,
      dateValidationError: "",
      currentDateString: `${currentDate.getFullYear()}-${String(
        currentDate.getMonth() + 1
      ).padStart(2, "0")}-${String(currentDate.getDate()).padStart(2, "0")}`,
      currentMonth: currentDate.getMonth(),
      currentYear: currentDate.getFullYear(),
      modelConfig: {
        type: "string",
        mask: "YYYY-MM-DD",
      },
    };
  },
  computed: {
    startDateTime() {
      if (
        !this.selectedDates[0] ||
        !this.pickupTime ||
        !this.isValidRentalDates ||
        !this.isValidTimes
      ) {
        return "";
      }
      return `${this.selectedDates[0]}T${this.pickupTime}:00`;
    },
    endDateTime() {
      if (
        !this.selectedDates[1] ||
        !this.dropoffTime ||
        !this.isValidRentalDates ||
        !this.isValidTimes
      ) {
        return "";
      }
      return `${this.selectedDates[1]}T${this.dropoffTime}:00`;
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
      const durationDays = Math.ceil((end - start) / (1000 * 60 * 60 * 24)) + 1;
      if (durationDays < this.item.minDays) {
        this.dateValidationError = `Rental must be at least ${this.item.minDays} days`;
        return false;
      }
      if (durationDays > this.item.maxDays) {
        this.dateValidationError = `Rental cannot exceed ${this.item.maxDays} days`;
        return false;
      }
      const allDisabledDates = [
        ...this.item.unavailableDates,
        ...this.bookings.map((booking) => ({
          startDate: booking.startDate,
          endDate: booking.endDate,
        })),
      ];
      for (const unavailable of allDisabledDates) {
        const unavailableStart = new Date(unavailable.startDate);
        const unavailableEnd = new Date(unavailable.endDate);
        if (start <= unavailableEnd && end >= unavailableStart) {
          this.dateValidationError = `Selected dates overlap with unavailable or booked dates (${this.formatDate(
            unavailable.startDate
          )} to ${this.formatDate(unavailable.endDate)})`;
          return false;
        }
      }
      this.dateValidationError = "";
      return true;
    },
    isValidTimes() {
      if (!this.pickupTime || !this.dropoffTime) {
        this.dateValidationError = "Please select pickup and drop-off times";
        return false;
      }
      if (this.selectedDates[0] === this.selectedDates[1]) {
        const [startHour, startMinute] = this.pickupTime.split(":").map(Number);
        const [endHour, endMinute] = this.dropoffTime.split(":").map(Number);
        if (
          startHour > endHour ||
          (startHour === endHour && startMinute >= endMinute)
        ) {
          this.dateValidationError =
            "Drop-off time must be after pickup time on the same day";
          return false;
        }
      }
      this.dateValidationError = "";
      return true;
    },
    disabledDates() {
      return (date) => {
        const allDisabledDates = [
          ...this.item.unavailableDates,
          ...this.bookings.map((booking) => ({
            startDate: booking.startDate,
            endDate: booking.endDate,
          })),
        ];
        return allDisabledDates.some((unavailable) => {
          const start = new Date(unavailable.startDate);
          const end = new Date(unavailable.endDate);
          return date >= start && date <= end;
        });
      };
    },
  },
  async created() {
    await Promise.all([this.fetchItem(), this.fetchBookings()]);
  },
  mounted() {
    console.log("Current date string:", this.currentDateString);
    console.log("Current month:", this.currentMonth);
    console.log("Current year:", this.currentYear);
    console.log("Selected dates:", this.selectedDates);
    console.log("Pickup time:", this.pickupTime);
    console.log("Dropoff time:", this.dropoffTime);
    console.log("Start DateTime:", this.startDateTime);
    console.log("End DateTime:", this.endDateTime);
  },
  methods: {
    async fetchItem() {
      try {
        const response = await axios.get(
          `/rentstuff/rentalitems/${this.$route.params.id}`
        );
        this.item = {
          ...response.data,
          imageIds: response.data.imageIds || [],
          prices: response.data.prices || [],
          minDays: response.data.minDays || 1,
          maxDays: response.data.maxDays || 30,
          unavailableDates: response.data.unavailableDates || [],
        };
      } catch (error) {
        console.error(
          "Error fetching item:",
          error.response?.data?.message || error.message
        );
      }
    },
    async fetchBookings() {
      try {
        const response = await axios.get(
          `/rentstuff/bookings/item/${this.$route.params.id}`,
          {
            headers: {
              Authorization: `Bearer ${localStorage.getItem("token")}`,
            },
          }
        );
        this.bookings = response.data.filter(
          (booking) => !["CANCELLED"].includes(booking.status)
        );
      } catch (error) {
        console.error(
          "Error fetching bookings:",
          error.response?.data?.message || error.message
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
    handleDateUpdate(dates) {
      this.selectedDates = dates.map((date) => {
        if (date instanceof Date) {
          const year = date.getFullYear();
          const month = String(date.getMonth() + 1).padStart(2, "0");
          const day = String(date.getDate()).padStart(2, "0");
          return `${year}-${month}-${day}`;
        }
        return date;
      });
      console.log("Date picker updated:", this.selectedDates);
      this.validateDates();
    },
    handleTimeUpdate(time, isPickup) {
      let timeStr;
      if (
        time &&
        typeof time === "object" &&
        time.hours != null &&
        time.minutes != null
      ) {
        timeStr = `${String(time.hours).padStart(2, "0")}:${String(
          time.minutes
        ).padStart(2, "0")}`;
      } else {
        timeStr = time || null;
      }
      if (isPickup) {
        this.pickupTime = timeStr;
      } else {
        this.dropoffTime = timeStr;
      }
      console.log("Time picker updated:", {
        time,
        pickup: this.pickupTime,
        dropoff: this.dropoffTime,
      });
      this.validateTimes();
    },
    validateDates() {
      this.isValidRentalDates;
    },
    validateTimes() {
      this.isValidTimes;
    },
    formatDate(date) {
      if (!date) return "N/A";
      return new Date(date).toLocaleString();
    },
    logRedirect() {
      console.log("Redirecting with:", {
        selectedDates: this.selectedDates,
        pickupTime: this.pickupTime,
        dropoffTime: this.dropoffTime,
        startDateTime: this.startDateTime,
        endDateTime: this.endDateTime,
      });
    },
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
.time-pickers {
  display: flex;
  gap: 20px;
  margin-top: 10px;
}
.time-picker {
  display: flex;
  flex-direction: column;
  gap: 5px;
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
:deep(.dp__time_input) {
  padding: 5px;
  border: 1px solid #ccc;
  border-radius: 4px;
}
</style>