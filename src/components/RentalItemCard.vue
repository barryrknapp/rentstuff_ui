<template>
  <div class="rental-item-card">
    <img
      :src="
        item.imageUrls && item.imageUrls.length > 0
          ? item.imageUrls[0]
          : 'https://via.placeholder.com/150'
      "
      alt="Rental Item"
      class="item-image"
    />
    <h3>{{ item.name || "Unnamed Item" }}</h3>
    <p>{{ item.description || "No description available" }}</p>
    <router-link :to="`/item/${item.id}`" class="btn">View Details</router-link>
  </div>
</template>

<script>
export default {
  props: {
    item: {
      type: Object,
      required: true,
      default: () => ({
        id: null,
        name: "",
        description: "",
        imageUrls: [],
      }),
    },
  },
};
</script>

<style scoped>
.rental-item-card {
  border: 1px solid #ccc;
  padding: 10px;
  margin: 10px;
  border-radius: 5px;
  text-align: center;
}
.item-image {
  max-width: 150px;
  height: auto;
}
.btn {
  padding: 5px 10px;
  background-color: #007bff;
  color: white;
  text-decoration: none;
  border-radius: 3px;
}
</style>