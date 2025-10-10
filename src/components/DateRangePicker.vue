<template>
  <div class="date-range-group">
    <div class="form-group">
      <label>Start Date</label>
      <input
        type="date"
        :value="modelValue.startDate"
        @input="updateStartDate($event)"
      />
    </div>
    <div class="form-group">
      <label>End Date</label>
      <input
        type="date"
        :value="modelValue.endDate"
        @input="updateEndDate($event)"
      />
    </div>
  </div>
</template>

<script>
export default {
  props: {
    modelValue: {
      type: Object,
      default: () => ({ startDate: "", endDate: "" }),
    },
  },
  methods: {
    updateStartDate(event) {
      this.$emit("update:modelValue", {
        ...this.modelValue,
        startDate: event.target.value,
      });
    },
    updateEndDate(event) {
      this.$emit("update:modelValue", {
        ...this.modelValue,
        endDate: event.target.value,
      });
    },
  },
};
</script>