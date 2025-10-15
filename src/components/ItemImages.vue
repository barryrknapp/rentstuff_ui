<template>
  <div class="image-gallery">
    <div v-for="(imageId, index) in item.imageIds" :key="imageId.id || index">
      <img
        :src="`${apiBaseUrl}/rentstuff/rentalitems/images/${imageId}`"
        alt="Item Image"
        @error="$emit('image-error', imageId)"
      />
    </div>
  </div>
</template>

<script>
export default {
  name: "ItemImages",
  props: {
    item: {
      type: Object,
      required: true,
    },
    apiBaseUrl: {
      type: String,
      required: true,
    },
  },
};
</script>

<style scoped>
.image-gallery {
  display: flex;
  gap: 10px;
  overflow-x: auto;
  margin-bottom: 20px;
}
.image-gallery img {
  max-width: 300px;
  height: auto;
}
</style>