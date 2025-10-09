import { createRouter, createWebHistory } from 'vue-router';
import LandingPage from '../views/LandingPage.vue';
import ItemDetailPage from '../views/ItemDetailPage.vue';
import CheckoutPage from '../views/CheckoutPage.vue';
import NewItemRentalPage from '../views/NewItemRentalPage.vue';

const routes = [
  { path: '/', component: LandingPage },
  { path: '/item/:id', component: ItemDetailPage },
  { path: '/checkout/:id', component: CheckoutPage },
  { path: '/new-item', component: NewItemRentalPage },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;