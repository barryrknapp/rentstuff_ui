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
        <label for="pricePerDay">Price per Day</label>
        <input
          id="pricePerDay"
          v-model.number="form.pricePerDay"
          type="number"
          required
        />
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
        pricePerDay: 0,
        minDays: 1,
        maxDays: 30,
        imageUrls: [],
        taxonomyId: null,
        taxonomyIds: [],
        unavailableDates: [],
        ownerId: null,
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
          ownerId: response.data.ownerId, // Ensure ownerId is set
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
    async submitForm() {
      try {
        const token = localStorage.getItem("token");
        console.log("Submitting with Token:", token);
        console.log("Form Data:", this.form); // Debug form data
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