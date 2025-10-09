<template>
  <div class="rental-item-card">
    <img
      :src="item.imageUrl || 'https://via.placeholder.com/150'"
      alt="Rental Item"
    />
    <h3>{{ item.name }}</h3>
    <p>{{ item.description }}</p>
    <p class="price">${{ item.pricePerDay }} / day</p>
    <router-link :to="`/item/${item.id}`" class="btn">View Details</router-link>
  </div>
</template>

<script>
export default {
  props: {
    item: {
      type: Object,
      required: true,
    },
  },
};
</script>