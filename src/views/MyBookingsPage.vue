<template>
  <div>
    <h2>My Bookings</h2>
    <div v-if="loading">Loading...</div>
    <div v-else-if="error" class="error">{{ error }}</div>
    <div v-else-if="bookings.length === 0" class="no-bookings">
      You have no bookings.
    </div>
    <div v-else class="booking-list">
      <div v-for="booking in bookings" :key="booking.id" class="booking-item">
        <h3>Booking #{{ booking.id }}</h3>
        <div v-if="items[booking.itemId]" class="item-details">
          <img
            :src="
              items[booking.itemId].imageIds?.[0]
                ? `getImageUrl(${
                    items[booking.itemId].imageIds[0]
                  }`)
                : 'https://via.placeholder.com/100'
            "
            alt="Item Image"
            class="item-image"
          />
          <p><strong>Item:</strong> {{ items[booking.itemId].name }}</p>
        </div>
        <p v-else><strong>Item:</strong> Loading...</p>
        <p><strong>Start Date:</strong> {{ formatDate(booking.startDate) }}</p>
        <p><strong>End Date:</strong> {{ formatDate(booking.endDate) }}</p>
        <p>
          <strong>Total Price:</strong> ${{ booking.totalPrice.toFixed(2) }}
        </p>
        <p><strong>Status:</strong> {{ booking.status }}</p>
        <p><strong>Created:</strong> {{ formatDate(booking.createDate) }}</p>
        <p v-if="booking.modifyDate">
          <strong>Modified:</strong> {{ formatDate(booking.modifyDate) }}
        </p>
        <p>
          <strong>Payments:</strong>
          {{
            booking.paymentIds.length ? booking.paymentIds.join(", ") : "None"
          }}
        </p>
        <div class="actions">
          <button
            v-if="canEdit(booking.status)"
            class="btn"
            @click="openEditModal(booking)"
          >
            Edit
          </button>
          <button
            v-if="canDelete(booking.status)"
            class="btn remove"
            @click="deleteBooking(booking.id)"
          >
            Delete
          </button>
        </div>
      </div>
    </div>

    <div v-if="showEditModal" class="modal-backdrop">
      <div class="modal">
        <h3>Edit Booking #{{ editForm.id }}</h3>
        <form @submit.prevent="submitEditForm">
          <div class="form-group">
            <label for="startDate">Start Date</label>
            <DateRangePicker
              v-model="editForm.dates"
              :single-date="true"
              date-type="start"
            />
          </div>
          <div class="form-group">
            <label for="endDate">End Date</label>
            <DateRangePicker
              v-model="editForm.dates"
              :single-date="true"
              date-type="end"
            />
          </div>
          <button type="submit" class="btn">Save Changes</button>
          <button
            type="button"
            class="btn close"
            @click="showEditModal = false"
          >
            Cancel
          </button>
        </form>
      </div>
    </div>
  </div>
</template>

<script>
import axios from "axios";
import DateRangePicker from "../components/DateRangePicker.vue";

export default {
  components: { DateRangePicker },
  data() {
    return {
      bookings: [],
      items: {},
      loading: false,
      error: null,
      showEditModal: false,
      editForm: {
        id: null,
        itemId: null,
        dates: { startDate: "", endDate: "" },
      },
    };
  },
  async created() {
    await this.fetchMyBookings();
  },
  methods: {
    getImageUrl(imageId) {
      return `${
        import.meta.env.VITE_API_BASE_URL
      }/rentstuff/rentalitems/images/${imageId}`;
    },
    async fetchMyBookings() {
      try {
        this.loading = true;
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        const response = await axios.get("/rentstuff/bookings/my-bookings", {
          headers: { Authorization: `Bearer ${token}` },
        });
        this.bookings = response.data;
        await this.fetchItemDetails();
      } catch (error) {
        console.error(
          "Error fetching bookings:",
          error.response?.data?.message || error.message
        );
        this.error = "Failed to load bookings. Please try again.";
      } finally {
        this.loading = false;
      }
    },
    async fetchItemDetails() {
      try {
        const itemIds = [
          ...new Set(this.bookings.map((booking) => booking.itemId)),
        ];
        const requests = itemIds.map((id) =>
          axios.get(`/rentstuff/rentalitems/${id}`, {
            headers: {
              Authorization: `Bearer ${localStorage.getItem("token")}`,
            },
          })
        );
        const responses = await Promise.all(requests);
        responses.forEach((response) => {
          const item = response.data;
          this.items[item.id] = {
            name: item.name,
            imageIds: item.imageIds || [],
          };
        });
      } catch (error) {
        console.error(
          "Error fetching item details:",
          error.response?.data?.message || error.message
        );
      }
    },
    formatDate(date) {
      if (!date) return "N/A";
      return new Date(date).toLocaleString();
    },
    canEdit(status) {
      return ["PENDING", "CONFIRMED"].includes(status);
    },
    canDelete(status) {
      return ["PENDING"].includes(status);
    },
    openEditModal(booking) {
      this.editForm = {
        id: booking.id,
        itemId: booking.itemId,
        dates: {
          startDate: booking.startDate,
          endDate: booking.endDate,
        },
      };
      this.showEditModal = true;
    },
    async submitEditForm() {
      try {
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        if (
          new Date(this.editForm.dates.startDate) >=
          new Date(this.editForm.dates.endDate)
        ) {
          alert("Start date must be before end date");
          return;
        }
        const payload = {
          id: this.editForm.id,
          itemId: this.editForm.itemId,
          startDate: this.editForm.dates.startDate,
          endDate: this.editForm.dates.endDate,
          status: "PENDING",
        };
        await axios.put(`/rentstuff/bookings/${this.editForm.id}`, payload, {
          headers: { Authorization: `Bearer ${token}` },
        });
        alert("Booking updated successfully!");
        this.showEditModal = false;
        await this.fetchMyBookings();
      } catch (error) {
        console.error(
          "Error updating booking:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to update booking: ${
            error.response?.data?.message || error.message
          }`
        );
      }
    },
    async deleteBooking(bookingId) {
      if (!confirm("Are you sure you want to delete this booking?")) {
        return;
      }
      try {
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        await axios.delete(`/rentstuff/bookings/${bookingId}`, {
          headers: { Authorization: `Bearer ${token}` },
        });
        alert("Booking deleted successfully!");
        await this.fetchMyBookings();
      } catch (error) {
        console.error(
          "Error deleting booking:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to delete booking: ${
            error.response?.data?.message || error.message
          }`
        );
      }
    },
  },
};
</script>

<style scoped>
.booking-list {
  display: grid;
  gap: 20px;
}
.booking-item {
  border: 1px solid #ccc;
  padding: 15px;
  border-radius: 5px;
}
.item-details {
  display: flex;
  align-items: center;
  gap: 10px;
  margin-bottom: 10px;
}
.item-image {
  max-width: 100px;
  height: auto;
}
.actions {
  display: flex;
  gap: 10px;
  margin-top: 10px;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
  text-decoration: none;
}
.btn.remove {
  background-color: #dc3545;
}
.btn.close {
  background-color: #dc3545;
  margin-left: 10px;
}
.error {
  color: #dc3545;
}
.no-bookings {
  color: #6c757d;
  font-style: italic;
}
.modal-backdrop {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.5);
  z-index: 999;
}
.modal {
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  background: #ffffff;
  padding: 20px;
  border-radius: 5px;
  max-width: 400px;
  width: 100%;
  z-index: 1000;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
}
.form-group {
  margin-bottom: 15px;
}
.form-group label {
  display: block;
  font-weight: bold;
}
</style>