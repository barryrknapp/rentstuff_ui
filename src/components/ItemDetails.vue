<template>
  <div class="item-details">
    <div class="location">
      <h3>Location</h3>
      <p v-if="item.city && item.state">
        {{ item.city }}, {{ item.state }} {{ item.zipCode ? item.zipCode : "" }}
      </p>
      <p v-else>Not specified</p>
    </div>
    <h2>{{ item.name || "Unnamed Item" }}</h2>
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
  </div>
</template>

<script>
export default {
  name: "ItemDetails",
  props: {
    item: {
      type: Object,
      required: true,
    },
  },
};
</script>

<style scoped>
.item-details {
  margin-bottom: 20px;
}
.price {
  color: #28a745;
  font-weight: bold;
}
</style>