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
  background: #ffffff; /* White background for high contrast */
  border: 2px solid #17ae85; /* Teal border to match logo */
  padding: 15px;
  margin: 10px;
  border-radius: 10px; /* Slightly rounder corners for modern look */
  text-align: center;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); /* Subtle shadow for depth */
  transition: transform 0.3s ease, box-shadow 0.3s ease; /* Smooth hover effect */
}

.rental-item-card:hover {
  transform: translateY(-5px); /* Lift effect on hover */
  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3); /* Stronger shadow on hover */
}

.rental-item-card h3 {
  color: #29383b; /* Dark logo color for heading */
  font-family: "Poppins", sans-serif;
  font-size: 1.8rem;
  margin: 10px 0;
}

.rental-item-card p {
  color: #29383b; /* Dark logo color for description */
  font-family: "Poppins", sans-serif;
  font-size: 1rem;
  margin: 10px 0;
}

.item-image {
  max-width: 100%; /* Allow image to scale properly */
  height: auto;
}

.item-image img {
  max-width: 150px; /* Increased size for better visibility */
  height: auto;
  margin-bottom: 15px;
  border-radius: 8px; /* Rounded image corners */
  border: 1px solid #17ae85; /* Teal border for consistency */
}

.btn {
  display: inline-block;
  padding: 8px 15px;
  background-color: #17ae85; /* Teal button to match logo */
  color: #ffffff; /* White text for contrast */
  text-decoration: none;
  border-radius: 5px;
  font-family: "Poppins", sans-serif;
  font-weight: 600;
  transition: background-color 0.3s ease;
}

.btn:hover {
  background-color: #29383b; /* Dark logo color on hover */
}
</style>