<template>
  <div>
    <ItemDetails :item="item" />
    <ItemImages
      :item="item"
      :api-base-url="apiBaseUrl"
      @image-error="handleImageError"
    />
    <RentalDateTimePicker
      :item="item"
      :bookings="bookings"
      :api-base-url="apiBaseUrl"
      :selected-dates="selectedDates"
      :pickup-time="pickupTime"
      :dropoff-time="dropoffTime"
      @update:selected-dates="selectedDates = $event"
      @update:pickup-time="pickupTime = $event"
      @update:dropoff-time="dropoffTime = $event"
      @update:date-validation-error="dateValidationError = $event"
    />
    <ItemReviews
      :item-id="item.id"
      :api-base-url="apiBaseUrl"
      v-model:reviews="itemReviews"
      v-model:show-reviews="showItemReviews"
    />
    <UserReviews
      :owner-id="item.ownerId"
      :api-base-url="apiBaseUrl"
      v-model:reviews="userReviews"
      v-model:show-reviews="showUserReviews"
      v-model:average-rating="averageUserRating"
    />
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
import ItemDetails from "@/components/ItemDetails.vue";
import ItemImages from "@/components/ItemImages.vue";
import RentalDateTimePicker from "@/components/RentalDateTimePicker.vue";
import ItemReviews from "@/components/ItemReviews.vue";
import UserReviews from "@/components/UserReviews.vue";

export default {
  components: {
    ItemDetails,
    ItemImages,
    RentalDateTimePicker,
    ItemReviews,
    UserReviews,
  },
  data() {
    const currentDate = new Date();
    return {
      apiBaseUrl:
        import.meta.env.VITE_API_BASE_URL || "http://localhost:8080/rentstuff",
      item: {
        imageIds: [],
        prices: [],
        unavailableDates: [],
        minDays: 1,
        maxDays: 30,
      },
      bookings: [],
      selectedDates: [],
      pickupTime: "09:00 AM",
      dropoffTime: "09:00 AM",
      itemReviews: [],
      userReviews: [],
      showItemReviews: false,
      showUserReviews: false,
      averageUserRating: "N/A",
      dateValidationError: "",
      imageErrors: new Set(),
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
      const time = this.convertTo24Hour(this.pickupTime);
      return `${this.selectedDates[0]}T${time}:00`;
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
      const time = this.convertTo24Hour(this.dropoffTime);
      return `${this.selectedDates[1]}T${time}:00`;
    },
    isValidRentalDates() {
      return !this.dateValidationError;
    },
    isValidTimes() {
      return !this.dateValidationError;
    },
  },
  async created() {
    await Promise.all([this.fetchItem(), this.fetchBookings()]);
  },
  methods: {
    async fetchItem() {
      try {
        const response = await axios.get(
          `${this.apiBaseUrl}/rentstuff/rentalitems/${this.$route.params.id}`
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
          `${this.apiBaseUrl}/rentstuff/bookings/item/${this.$route.params.id}`,
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
    handleImageError(imageId) {
      this.imageErrors.add(imageId);
      console.warn(`Failed to load image with ID ${imageId}`);
    },
    convertTo24Hour(timeStr) {
      if (!timeStr) return null;
      const [time, ampm] = timeStr.split(" ");
      let [hours, minutes] = time.split(":").map(Number);
      if (ampm === "PM" && hours !== 12) hours += 12;
      if (ampm === "AM" && hours === 12) hours = 0;
      return `${String(hours).padStart(2, "0")}:${String(minutes).padStart(
        2,
        "0"
      )}`;
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
</style>