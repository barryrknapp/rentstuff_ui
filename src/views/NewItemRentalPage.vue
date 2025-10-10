<template>
  <div>
    <h2>{{ isEdit ? "Edit Rental Item" : "Add New Rental Item" }}</h2>
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
      <div class="form-group">
        <label>Prices</label>
        <div
          v-for="(price, index) in form.prices"
          :key="index"
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
          <button type="button" class="btn remove" @click="removePrice(index)">
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
        <label for="imageUrls">Image URLs</label>
        <input
          v-for="(url, index) in form.imageUrls"
          :key="index"
          v-model="form.imageUrls[index]"
          type="url"
          placeholder="Enter image URL"
        />
        <button type="button" class="btn secondary" @click="addImageUrl">
          Add Image URL
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
      <div class="form-group">
        <label>Unavailable Date Ranges</label>
        <DateRangePicker
          v-for="(range, index) in form.unavailableDates"
          :key="index"
          v-model="form.unavailableDates[index]"
        />
        <button type="button" class="btn secondary" @click="addDateRange">
          Add Date Range
        </button>
      </div>
      <button type="submit" class="btn">Save Item</button>
    </form>
  </div>
</template>

<script>
import axios from "axios";
import DateRangePicker from "../components/DateRangePicker.vue";

export default {
  components: { DateRangePicker },
  data() {
    return {
      isEdit: false,
      form: {
        name: "",
        description: "",
        minDays: 1,
        maxDays: 30,
        imageUrls: [],
        taxonomyId: null,
        taxonomyIds: [],
        unavailableDates: [],
        ownerId: null,
        prices: [], // Updated to array of price objects
      },
      taxonomies: [],
    };
  },
  async created() {
    await this.fetchTaxonomies();
    if (this.$route.params.id) {
      this.isEdit = true;
      await this.fetchItem();
    } else {
      const token = localStorage.getItem("token");
      console.log("Retrieved Token:", token);
      if (!token || token === "undefined") {
        console.error("No valid token found, redirecting to login");
        this.$router.push("/login");
        return;
      }
      try {
        const response = await axios.get("/rentstuff/auth/me", {
          headers: { Authorization: `Bearer ${token}` },
        });
        console.log("User Response:", response.data);
        this.form.ownerId = response.data.id;
      } catch (error) {
        console.error(
          "Error fetching user:",
          error.response?.data || error.message
        );
        this.$router.push("/login");
      }
    }
  },
  methods: {
    async fetchTaxonomies() {
      try {
        const response = await axios.get("/rentstuff/taxonomies");
        this.taxonomies = response.data;
      } catch (error) {
        console.error("Error fetching taxonomies:", error);
      }
    },
    async fetchItem() {
      try {
        const response = await axios.get(
          `/rentstuff/rentalitems/${this.$route.params.id}`
        );
        this.form = {
          ...response.data,
          imageUrls: response.data.imageUrls || [],
          taxonomyIds: response.data.taxonomyIds || [],
          unavailableDates: response.data.unavailableDates || [],
          ownerId: response.data.ownerId,
          prices: response.data.prices || [], // Updated to handle prices
        };
      } catch (error) {
        console.error("Error fetching item:", error);
      }
    },
    addImageUrl() {
      this.form.imageUrls.push("");
    },
    addDateRange() {
      this.form.unavailableDates.push({ startDate: "", endDate: "" });
    },
    addPrice() {
      this.form.prices.push({ price: 0, minDaysRented: 1 });
    },
    removePrice(index) {
      this.form.prices.splice(index, 1);
    },
    async submitForm() {
      try {
        const token = localStorage.getItem("token");
        console.log("Submitting with Token:", token);
        console.log("Form Data:", JSON.stringify(this.form));
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
          console.error("At least one price is required");
          alert("Please add at least one price");
          return;
        }
        const url = this.isEdit
          ? `/rentstuff/rentalitems/${this.$route.params.id}`
          : "/rentstuff/rentalitems";
        const method = this.isEdit ? "put" : "post";
        await axios[method](url, this.form, {
          headers: { Authorization: `Bearer ${token}` },
        });
        alert("Item saved successfully!");
        this.$router.push("/");
      } catch (error) {
        console.error(
          "Error saving item:",
          error.response?.data || error.message
        );
        alert(
          "Failed to save item: " + (error.response?.data || error.message)
        );
      }
    },
  },
};
</script>

<style>
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
.price-entry {
  display: flex;
  gap: 10px;
  align-items: center;
  margin-bottom: 10px;
}
.price-entry input {
  flex: 1;
}
.btn {
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
}
.btn.secondary {
  background-color: #6c757d;
}
.btn.remove {
  background-color: #dc3545;
}
</style>