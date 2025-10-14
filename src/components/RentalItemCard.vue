<template>
  <div class="rental-item-card">
    <h3>{{ item.name || "Unnamed Item" }}</h3>
    <p>{{ item.description || "No description available" }}</p>
    <div v-if="item.imageIds?.length" class="item-image">
      <img :src="getImageUrl(item.imageIds[0])" alt="Item Image" />
    </div>
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
  methods: {
    getImageUrl(imageId) {
      console.log("Vite URL " + import.meta.env.VITE_API_BASE_URL);
      return `${
        import.meta.env.VITE_API_BASE_URL
      }/rentstuff/rentalitems/images/${imageId}`;
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
  max-width: 20px;
  height: auto;
}
.item-image img {
  max-width: 100px;
  height: auto;
  margin-bottom: 10px;
}
.btn {
  padding: 5px 10px;
  background-color: #007bff;
  color: white;
  text-decoration: none;
  border-radius: 3px;
}
</style>