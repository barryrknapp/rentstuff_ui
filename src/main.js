import Vue from 'vue'
import App from './App.vue'
import router from './router'
import component from './components/vue-dropdown-menu.vue';

import 'bootstrap/dist/css/bootstrap.min.css'
import '@/assets/css/main.css'


console.log(Vue.version);

Vue.config.productionTip = false;


var isLocal = false;

Vue.prototype.$released = true;

Vue.prototype.$nfthost = 'https://evidence.quest/api/nftstitches';
Vue.prototype.$xummhost = 'https://evidence.quest/api/xumm';
Vue.prototype.$xrplnft = 'https://evidence.quest/api/xrplnft';
Vue.prototype.$reporthost = 'https://strategyengine.one/api/reports';
Vue.prototype.$fsehost = 'https://strategyengine.one/api/distribution';

if (isLocal) {
  Vue.prototype.$xummhost = 'http://local.evidence.quest:8085';
  Vue.prototype.$nfthost = 'http://local.evidence.quest:8081';
  //  Vue.prototype.$reporthost = 'http://local.evidence.quest:8080';
  //  Vue.prototype.$fsehost = 'http://local.evidence.quest:8080';
}



const install = function installVueDropdownMenu(Vue) {
  if (install.installed) return;
  install.installed = true;
  Vue.component('VueDropdownMenu', component);
};

component.install = install;

export default component;

new Vue({
  router,
  render: h => h(App)
}).$mount('#app');