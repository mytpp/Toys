<template>
  <v-app>
    <v-app-bar app color="primary" dark>
      <v-spacer></v-spacer>
      <v-toolbar-title>数字媒体内容综合设计与实验 - 网站制作</v-toolbar-title>
      <v-spacer></v-spacer>
    </v-app-bar>

    <v-content>
      <PersonalInfo/>
    </v-content>
  </v-app>
</template>

<script>
import PersonalInfo from './components/PersonalInfo';

export default {
  name: 'App',

  components: {
    PersonalInfo,
  },

  data: () => ({
      
  }),
};
</script>
