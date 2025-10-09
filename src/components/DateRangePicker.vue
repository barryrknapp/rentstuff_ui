<template>
  <div class="date-range-group">
    <div class="form-group">
      <label>Start Date</label>
      <input type="date" v-model="localRange.startDate" @change="updateRange" />
    </div>
    <div class="form-group">
      <label>End Date</label>
      <input type="date" v-model="localRange.endDate" @change="updateRange" />
    </div>
  </div>
</template>

<script>
export default {
  props: {
    range: {
      type: Object,
      default: () => ({ startDate: "", endDate: "" }),
    },
  },
  data() {
    return {
      localRange: { ...this.range },
    };
  },
  methods: {
    updateRange() {
      this.$emit("update:range", this.localRange);
    },
  },
};
</script>