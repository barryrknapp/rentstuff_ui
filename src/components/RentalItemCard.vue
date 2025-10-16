<template>
  <div class="card">
    <img
      v-if="item.imageIds && item.imageIds.length"
      :src="getImageUrl(item.imageIds[0])"
      alt="Item Image"
      loading="lazy"
      @error="$emit('image-error', item.imageIds[0])"
    />
    <h3>{{ item.name }}</h3>
    <p>{{ item.description }}</p>
    <p v-if="item.city && item.state">{{ item.city }}, {{ item.state }}</p>
    <p v-if="distance !== null">Distance: {{ distance.toFixed(1) }} miles</p>
    <router-link :to="`/item/${item.id}`" class="btn">View Details</router-link>
  </div>
</template>

<script>
export default {
  name: "RentalItemCard",
  props: {
    item: {
      type: Object,
      required: true,
    },
    distance: {
      type: Number,
      default: null,
    },
  },
  methods: {
    getImageUrl(imageId) {
      return `${
        import.meta.env.VITE_API_BASE_URL
      }/rentstuff/rentalitems/images/${imageId}`;
    },
  },
};
</script>

<style scoped>
.card {
  border: 1px solid #ddd;
  padding: 15px;
  border-radius: 5px;
}
.card img {
  max-width: 100%;
  height: auto;
}
.card p {
  margin: 5px 0;
  color: #ffffff;
}
.card h3 {
  color: #ffffff;
}
.btn {
  display: inline-block;
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  text-decoration: none;
  border-radius: 5px;
}
</style>