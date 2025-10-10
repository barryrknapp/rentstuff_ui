import { createRouter, createWebHistory } from 'vue-router';
import LandingPage from '../views/LandingPage.vue';
import ItemDetailPage from '../views/ItemDetailPage.vue';
import CheckoutPage from '../views/CheckoutPage.vue';
import NewItemRentalPage from '../views/NewItemRentalPage.vue';
import LoginRegisterModal from '../components/LoginRegisterModal.vue';

const routes = [
  { path: '/', component: LandingPage },
  { path: '/item/:id', component: ItemDetailPage },
  { path: '/checkout/:id', component: CheckoutPage },
  { path: '/new-item', component: NewItemRentalPage },
  { path: '/login', component: LoginRegisterModal },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;