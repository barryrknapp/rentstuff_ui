<template>
  <div>
    <h2>{{ isEdit ? "Edit Rental Item" : "Add New Rental Item" }}</h2>
    <div v-if="uploading" class="uploading">Uploading...</div>
    <form @submit.prevent="submitForm">
      <div class="form-group">
        <label for="name">Item Name</label>
        <input id="name" v-model="form.name" type="text" required />
      </div>
      <div class="form-group">
        <label for="description">Description</label>
        <textarea
          id="description"
          v-model="form.description"
          required
        ></textarea>
      </div>
      <!-- Location Fields -->
      <div class="form-group">
        <label for="city">City</label>
        <input id="city" v-model="form.city" type="text" required />
      </div>
      <div class="form-group">
        <label for="state">State</label>
        <select id="state" v-model="form.state" required>
          <option value="" disabled>Select a state</option>
          <option v-for="state in states" :key="state" :value="state">
            {{ state }}
          </option>
        </select>
      </div>
      <div class="form-group">
        <label for="zipCode">ZIP Code (Optional)</label>
        <input
          id="zipCode"
          v-model="form.zipCode"
          type="text"
          pattern="\d{5}"
          title="Enter a 5-digit ZIP code"
        />
      </div>
      <div class="form-group">
        <label>Prices</label>
        <div
          v-for="(price, index) in form.prices"
          :key="price.id || index"
          class="price-entry"
        >
          <label :for="'price-' + index">Price per Day</label>
          <input
            :id="'price-' + index"
            v-model.number="form.prices[index].price"
            type="number"
            step="0.01"
            required
          />
          <label :for="'minDaysRented-' + index">Min Days Rented</label>
          <input
            :id="'minDaysRented-' + index"
            v-model.number="form.prices[index].minDaysRented"
            type="number"
            required
          />
          <button
            type="button"
            class="btn remove"
            @click="removePrice(price, index)"
            v-if="form.prices.length > 1 || price.id"
          >
            Remove
          </button>
        </div>
        <button type="button" class="btn secondary" @click="addPrice">
          Add Price
        </button>
      </div>
      <div class="form-group">
        <label for="minDays">Minimum Days</label>
        <input
          id="minDays"
          v-model.number="form.minDays"
          type="number"
          required
        />
      </div>
      <div class="form-group">
        <label for="maxDays">Maximum Days</label>
        <input
          id="maxDays"
          v-model.number="form.maxDays"
          type="number"
          required
        />
      </div>
      <div class="form-group">
        <label for="paused">Paused (Prevent New Rentals)</label>
        <input id="paused" v-model="form.paused" type="checkbox" />
      </div>
      <div class="form-group">
        <label>New Images</label>
        <div
          v-for="(image, index) in form.newImages"
          :key="'new-' + index"
          class="image-entry"
        >
          <input
            type="file"
            :id="'image-' + index"
            accept="image/*"
            @change="handleImageChange($event, index)"
          />
          <button
            type="button"
            class="btn remove"
            @click="removeNewImage(index)"
          >
            Remove
          </button>
        </div>
        <button type="button" class="btn secondary" @click="addNewImage">
          Add Image
        </button>
      </div>
      <div class="form-group">
        <label>Existing Images</label>
        <div
          v-for="imageId in form.imageIds"
          :key="imageId"
          class="image-preview"
        >
          <img
            :src="getImageUrl(imageId)"
            alt="Existing Image"
            style="max-width: 100px"
            @error="handleImageError(imageId)"
          />
          <button
            type="button"
            class="btn remove"
            @click="removeImageId(imageId)"
          >
            Remove
          </button>
        </div>
      </div>
      <div class="form-group">
        <label>Unavailable Date Ranges</label>
        <div
          v-for="(range, index) in form.unavailableDates"
          :key="range.id || index"
          class="date-entry"
        >
          <DateRangePicker v-model="form.unavailableDates[index]" />
          <button
            type="button"
            class="btn remove"
            @click="removeDateRange(range, index)"
          >
            Remove
          </button>
        </div>
        <button type="button" class="btn secondary" @click="addDateRange">
          Add Date Range
        </button>
      </div>
      <div class="form-group">
        <label for="taxonomyIds">Taxonomies</label>
        <select id="taxonomyIds" v-model="form.taxonomyIds" multiple required>
          <option
            v-for="taxonomy in taxonomies"
            :key="taxonomy.id"
            :value="taxonomy.id"
          >
            {{ taxonomy.name }}
          </option>
        </select>
      </div>
      <button type="submit" class="btn">Save Item</button>
    </form>
  </div>
</template>

<script>
import axios from "axios";
import DateRangePicker from "../components/DateRangePicker.vue";

export default {
  name: "RentalItemForm",
  components: { DateRangePicker },
  data() {
    return {
      isEdit: false,
      form: {
        name: "",
        description: "",
        city: "", // New field
        state: "", // New field
        zipCode: "", // New field
        minDays: 1,
        maxDays: 30,
        paused: false,
        newImages: [],
        imageIds: [],
        taxonomyIds: [],
        unavailableDates: [],
        ownerId: null,
        prices: [],
      },
      taxonomies: [],
      uploading: false,
      imageErrors: new Set(),
      // US states for dropdown
      states: [
        "AL",
        "AK",
        "AZ",
        "AR",
        "CA",
        "CO",
        "CT",
        "DE",
        "FL",
        "GA",
        "HI",
        "ID",
        "IL",
        "IN",
        "IA",
        "KS",
        "KY",
        "LA",
        "ME",
        "MD",
        "MA",
        "MI",
        "MN",
        "MS",
        "MO",
        "MT",
        "NE",
        "NV",
        "NH",
        "NJ",
        "NM",
        "NY",
        "NC",
        "ND",
        "OH",
        "OK",
        "OR",
        "PA",
        "RI",
        "SC",
        "SD",
        "TN",
        "TX",
        "UT",
        "VT",
        "VA",
        "WA",
        "WV",
        "WI",
        "WY",
      ],
    };
  },
  async created() {
    await this.fetchTaxonomies();
    if (this.$route.params.id) {
      this.isEdit = true;
      await this.fetchItem();
    } else {
      this.form.prices = [{ price: 0, minDaysRented: 1 }];
      await this.fetchOwnerId();
    }
  },
  methods: {
    getImageUrl(imageId) {
      return `${
        import.meta.env.VITE_API_BASE_URL
      }/rentstuff/rentalitems/images/${imageId}`;
    },
    async fetchTaxonomies() {
      try {
        const response = await axios.get("/rentstuff/taxonomies");
        this.taxonomies = response.data;
      } catch (error) {
        console.error(
          "Error fetching taxonomies:",
          error.response?.data?.message || error.message
        );
        this.$router.push("/error");
      }
    },
    async fetchItem() {
      try {
        const response = await axios.get(
          `/rentstuff/rentalitems/${this.$route.params.id}`,
          {
            headers: {
              Authorization: `Bearer ${localStorage.getItem("token")}`,
            },
          }
        );
        this.form = {
          ...response.data,
          newImages: [],
          imageIds: response.data.imageIds || [],
          taxonomyIds: response.data.taxonomyIds || [],
          unavailableDates: response.data.unavailableDates || [],
          ownerId: response.data.ownerId,
          prices:
            response.data.prices && response.data.prices.length
              ? response.data.prices
              : [{ price: 0, minDaysRented: 1 }],
          paused: response.data.paused || false,
          city: response.data.city || "", // New field
          state: response.data.state || "", // New field
          zipCode: response.data.zipCode || "", // New field
        };
      } catch (error) {
        console.error(
          "Error fetching item:",
          error.response?.data?.message || error.message
        );
        this.$router.push("/error");
      }
    },
    async fetchOwnerId() {
      try {
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        const response = await axios.get("/rentstuff/auth/me", {
          headers: { Authorization: `Bearer ${token}` },
        });
        this.form.ownerId = response.data.id;
      } catch (error) {
        console.error(
          "Error fetching user:",
          error.response?.data?.message || error.message
        );
        this.$router.push("/login");
      }
    },
    addNewImage() {
      this.form.newImages.push(null);
    },
    removeNewImage(index) {
      this.form.newImages.splice(index, 1);
    },
    async removeImageId(imageId) {
      try {
        const token = localStorage.getItem("token");
        await axios.delete(`/rentstuff/rentalitems/images/${imageId}`, {
          headers: { Authorization: `Bearer ${token}` },
        });
        this.form.imageIds = this.form.imageIds.filter((id) => id !== imageId);
        this.imageErrors.delete(imageId);
        alert("Image removed successfully");
      } catch (error) {
        console.error(
          "Error removing image:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to remove image: ${
            error.response?.data?.message || error.message
          }`
        );
      }
    },
    addDateRange() {
      this.form.unavailableDates.push({ startDate: "", endDate: "" });
    },
    async removeDateRange(range, index) {
      try {
        if (range.id) {
          const token = localStorage.getItem("token");
          await axios.delete(
            `/rentstuff/rentalitems/unavailable-dates/${range.id}`,
            {
              headers: { Authorization: `Bearer ${token}` },
            }
          );
          alert("Date range removed successfully");
        }
        this.form.unavailableDates.splice(index, 1);
      } catch (error) {
        console.error(
          "Error removing date range:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to remove date range: ${
            error.response?.data?.message || error.message
          }`
        );
      }
    },
    addPrice() {
      this.form.prices.push({ price: 0, minDaysRented: 1 });
    },
    async removePrice(price, index) {
      try {
        if (price.id) {
          const token = localStorage.getItem("token");
          await axios.delete(`/rentstuff/rentalitems/prices/${price.id}`, {
            headers: { Authorization: `Bearer ${token}` },
          });
          alert("Price removed successfully");
        }
        this.form.prices.splice(index, 1);
        if (this.form.prices.length === 0) {
          this.form.prices.push({ price: 0, minDaysRented: 1 });
        }
      } catch (error) {
        console.error(
          "Error removing price:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to remove price: ${
            error.response?.data?.message || error.message
          }`
        );
      }
    },
    handleImageChange(event, index) {
      const file = event.target.files[0];
      const maxSizeMB = 200;
      const maxSizeBytes = maxSizeMB * 1024 * 1024;
      if (file && file.type.startsWith("image/") && file.size <= maxSizeBytes) {
        this.form.newImages[index] = file;
      } else {
        alert(`Please select a valid image file (max ${maxSizeMB}MB)`);
        this.form.newImages[index] = null;
        event.target.value = "";
      }
    },
    handleImageError(imageId) {
      this.imageErrors.add(imageId);
      console.warn(`Failed to load image with ID ${imageId}`);
      this.form.imageIds = this.form.imageIds.filter((id) => id !== imageId);
    },
    async submitForm() {
      try {
        this.uploading = true;
        const token = localStorage.getItem("token");
        if (!token || token === "undefined") {
          console.error("No valid token found, redirecting to login");
          this.$router.push("/login");
          return;
        }
        if (!this.form.ownerId) {
          console.error("Owner ID is missing, redirecting to login");
          this.$router.push("/login");
          return;
        }
        if (!this.form.prices.length) {
          alert("Please add at least one price");
          return;
        }
        if (!this.form.city || !this.form.state) {
          alert("Please provide both city and state");
          return;
        }

        // Update item
        const formData = new FormData();
        formData.append(
          "item",
          new Blob(
            [
              JSON.stringify({
                id: this.form.id,
                name: this.form.name,
                description: this.form.description,
                city: this.form.city, // New field
                state: this.form.state, // New field
                zipCode: this.form.zipCode, // New field
                minDays: this.form.minDays,
                maxDays: this.form.maxDays,
                paused: this.form.paused,
                taxonomyIds: this.form.taxonomyIds,
                ownerId: this.form.ownerId,
              }),
            ],
            { type: "application/json" }
          )
        );

        this.form.newImages.forEach((image, index) => {
          if (image) {
            formData.append("images", image);
          }
        });

        const url = this.isEdit
          ? `/rentstuff/rentalitems/${this.$route.params.id}`
          : "/rentstuff/rentalitems";
        const config = {
          headers: {
            Authorization: `Bearer ${token}`,
            "Content-Type": "multipart/form-data",
          },
        };
        const response = await axios({
          method: this.isEdit ? "put" : "post",
          url,
          data: formData,
          ...config,
        });

        const itemId = this.isEdit ? this.$route.params.id : response.data.id;

        // Add new prices
        const newPrices = this.form.prices.filter((price) => !price.id);
        for (const price of newPrices) {
          await axios.post(
            "/rentstuff/rentalitems/prices",
            {
              price: price.price,
              minDaysRented: price.minDaysRented,
              itemId,
            },
            { headers: { Authorization: `Bearer ${token}` } }
          );
        }

        // Add new unavailable dates
        const newDates = this.form.unavailableDates.filter(
          (date) => !date.id && date.startDate && date.endDate
        );
        for (const date of newDates) {
          await axios.post(
            "/rentstuff/rentalitems/unavailable-dates",
            {
              startDate: date.startDate,
              endDate: date.endDate,
              itemId,
            },
            { headers: { Authorization: `Bearer ${token}` } }
          );
        }

        alert("Item saved successfully!");
        this.$router.push("/my-equipment");
      } catch (error) {
        console.error(
          "Error saving item:",
          error.response?.data?.message || error.message
        );
        alert(
          `Failed to save item: ${
            error.response?.data?.message || error.message
          }`
        );
      } finally {
        this.uploading = false;
      }
    },
  },
};
</script>

<style scoped>
.form-group {
  margin-bottom: 15px;
}
.form-group label {
  display: block;
}
.form-group input,
.form-group textarea,
.form-group select {
  width: 100%;
  padding: 8px;
}
.image-entry,
.image-preview,
.price-entry,
.date-entry {
  display: flex;
  gap: 10px;
  align-items: center;
  margin-bottom: 10px;
}
.image-entry input,
.price-entry input {
  flex: 1;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
  text-decoration: none;
}
.btn.secondary {
  background-color: #6c757d;
}
.btn.remove {
  background-color: #dc3545;
}
.uploading {
  color: #007bff;
  font-style: italic;
  margin-bottom: 10px;
}
</style>