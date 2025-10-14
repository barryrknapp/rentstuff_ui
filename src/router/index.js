import { createRouter, createWebHistory } from 'vue-router';
import LandingPage from '../views/LandingPage.vue';
import ItemDetailPage from '../views/ItemDetailPage.vue';
import CheckoutPage from '../views/CheckoutPage.vue';
import RentalItemForm from "../components/RentalItemForm.vue";
import MyBookingsPage from "../views/MyBookingsPage.vue";
import ErrorPage from "../components/ErrorPage.vue"; // Optional: for error redirects
import LoginRegisterModal from '../components/LoginRegisterModal.vue';

const routes = [
  { path: '/', component: LandingPage },
  { path: '/item/:id', component: ItemDetailPage },
  { path: '/checkout/:id', component: CheckoutPage },
  { path: '/new-item', component: RentalItemForm },
  { path: '/my-equipment', component: RentalItemForm },
  { path: '/equipment/edit/:id', component: RentalItemForm },
  { path: '/login', component: LoginRegisterModal },
  { path: '/my-bookings', component: MyBookingsPage },
  { path: '/error', component: ErrorPage },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;
