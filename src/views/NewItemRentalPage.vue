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
        <label for="imageUrl">Image URL</label>
        <input id="imageUrl" v-model="form.imageUrl" type="url" />
      </div>
      <div class="form-group">
        <label>Unavailable Date Ranges</label>
        <DateRangePicker
          v-for="(range, index) in form.unavailableDates"
          :key="index"
          :range="range"
          @update:range="updateDateRange(index, $event)"
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
        imageUrl: "",
        unavailableDates: [],
      },
    };
  },
  async created() {
    if (this.$route.params.id) {
      this.isEdit = true;
      await this.fetchItem();
    }
  },
  methods: {
    async fetchItem() {
      try {
        const response = await axios.get(
          `http://localhost:8081/rentalitems/${this.$route.params.id}`
        );
        this.form = response.data;
      } catch (error) {
        console.error("Error fetching item:", error);
      }
    },
    addDateRange() {
      this.form.unavailableDates.push({ startDate: "", endDate: "" });
    },
    updateDateRange(index, range) {
      this.form.unavailableDates[index] = range;
    },
    async submitForm() {
      try {
        const url = this.isEdit
          ? `http://localhost:8081/rentalitems/${this.$route.params.id}`
          : "http://localhost:8081/rentalitems";
        const method = this.isEdit ? "put" : "post";
        await axios[method](url, this.form, {
          headers: { Authorization: `Bearer ${localStorage.getItem("token")}` },
        });
        alert("Item saved successfully!");
        this.$router.push("/");
      } catch (error) {
        console.error("Error saving item:", error);
        alert("Failed to save item");
      }
    },
  },
};
</script>