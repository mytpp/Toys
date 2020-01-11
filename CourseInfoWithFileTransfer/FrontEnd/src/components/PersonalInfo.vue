<template>
  <v-container>
    <v-row class="flex-column" justify="space-around">
      <!-- 个人信息 -->
      <v-card class="ma-3" raised>
        <v-card-title>个人信息</v-card-title>
        <v-row justify="space-around">
          <v-col cols="auto">
            <v-img class="ma-3 rounded-image" src="../assets/avatar.jpg"
                   max-width="200" max-height="204"/>
          </v-col>
          <v-col>
            <v-row>
              <v-col v-for="(item, i) in personalInfo" :key="i">
                <v-list-item two-line>
                  <v-list-item-content>
                    <v-list-item-title v-text="item.title"/>
                    <v-list-item-subtitle v-text="item.value"/>
                  </v-list-item-content>
                </v-list-item>
              </v-col>
            </v-row>
          </v-col>
        </v-row>
      </v-card>
      <!-- 课程的各次实验信息 -->
      <v-card class="ma-3" raised>
        <v-card-title>各次实验信息</v-card-title>
        <v-row class="flex-column" justify="space-around">
          <v-col v-for="(item, i) in experimentsInfo" :key="i">
            <v-card class="ma-auto">
              <v-card-title v-text="item.ordinal + item.title"/>
              <v-card-subtitle v-text="'Score: ' + item.score"/>
              <v-card-text> 
                要求：{{item.content}} <br/> 提交内容：{{item.submit}}
              </v-card-text>
            </v-card>
          </v-col>
        </v-row>
      </v-card>
      <!-- 媒体文件 -->
      <v-card class="ma-3" raised>
        <v-card-title>媒体文件（单击下载）</v-card-title>
        <v-list class="mx-3" shaped>
          <v-list-item-group v-model="fileToDownload" primary>
            <v-list-item v-for="(item, i) in multiMediaItems" :key="i">
              <v-list-item-content v-text="item.name" @click="download(item.name)"/>
              <v-list-item-action>
                <v-list-item-action-text v-text="beautifySize(item.size)"/>
              </v-list-item-action>
            </v-list-item>
          </v-list-item-group>
          <v-dialog v-model="failDialog" max-width="300">
            <v-card>
              <v-card-title class="headline">哎呀，出错了</v-card-title>
              <v-card-actions>
                <v-btn color="blue" text @click="failDialog = false"> 好吧 </v-btn>
              </v-card-actions>
            </v-card>
          </v-dialog>
        </v-list>

        <v-row class="mx-3">
          <v-col>
            <v-file-input show-size outlined label="选择文件" ref="fileInput"
                          @change="fileSelected"/>
          </v-col>
          <v-col cols="auto">
            <v-btn outlined primary x-large @click="upload">上传</v-btn>
          </v-col>
          <v-dialog v-model="succDialog" max-width="300">
            <v-card>
              <v-card-title class="headline">上传成功</v-card-title>
              <v-card-actions>
                <v-btn color="green" text @click="succDialog = false"> OK </v-btn>
              </v-card-actions>
            </v-card>
          </v-dialog>
        </v-row>
      </v-card>
    </v-row>
  </v-container>
</template>

<script>
export default {
  name: 'PersonInfo',
  data: () => ({
    personalInfo: [
      { title: '姓名', value: '檀XX' },
      { title: '学校', value: '北京邮电大学' },
      { title: '学历', value: '本科' },
      { title: '学院', value: '计算机学院' },
      { title: '专业', value: '计算机科学与技术' },
      { title: '邮箱', value: 'yourtpp@outlook.com' },
      // { title: '', value: '' },
    ],
    experimentsInfo: [
      { ordinal: '1', title: '音频采集与处理', score: 20, content: '用Windows录音机录制一首自己唱的歌或朗诵，准备一段背景音乐，使用Cool Edit导入录制的声音文件和背景音乐，并进行合成。', submit: '背景音乐文件、声音文件、合成作品、实验报告（电子版+打印版）。'},
      { ordinal: '2', title: '图像处理', score: 20, content: '1）选取适当的图片素材和世界地图，运用各种选取方法制作一幅由世界名胜照片揉和在一起的背景。利用图层效果制作一幅有地形质感的世界地图。调整并合并所有层存储为各种图像文件格式并压缩。（2）显示一个bmp文件的C程序，并实现图像亮度、对比度调整、图像平移、放大、旋转和镜像。', submit: '图片素材、合成图片、显示bmp的程序代码、显示bmp的可执行文件、实验报告（电子版+打印版）。'},
      { ordinal: '3', title: '动画制作', score: 30, content: '根据实验1中得到的歌曲或配乐朗诵，做一段Flash不少于1分半钟，并合成为一段动画MV。', submit: '动画素材、Flash源文件、Flash导出影片、实验报告（电子版+打印版）。'},
      { ordinal: '4', title: '网站制作', score: 30, content: '个人页面开发，包含个人基本信息，整个课程的各次实验信息，并实现媒体文件上传、下载功能实现。', submit: '网页制作素材、网页源代码、实验报告（电子版+打印版）。'},
    ],
    multiMediaItems: [
      //{name: 'abc.mp3', type: 'mp3', size: 12345},
    ],
    fileToDownload: null,
    fileToUpload: null,
    succDialog: false,
    failDialog: false,
  }),

  methods: {
    getFiles() {
      this.axios.get('http://localhost:8081/files')
      .then(resp => {
        //console.log(resp.data);
        this.multiMediaItems = resp.data
              .map(val => { return {
                name: val.name,
                type: val.name.substr(val.name.lastIndexOf('.')),
                size: val.size,
              } });
      })
      .catch(err => {
        console.log(err);
      })
    },

    fileSelected(file) {
      console.log(file);
      this.fileToUpload = file;
    },

    upload() {
      let formData = new FormData();
      if (this.fileToUpload == null) {
        console.log('No file selected!');
        return;
      }
      formData.append('filename', this.fileToUpload.name);
      formData.append('file', this.fileToUpload);

      this.axios.post('http://localhost:8081/files', formData,
          { header: {'Content-Type': 'multipart/form-data'} })
      .then(resp => {
        console.log(resp);
        this.getFiles();
        this.succDialog = true;
        this.$refs.fileInput.value = null;
      })
      .catch(err => {
        console.log(err);
      })
    },

    download(filename) {
      console.log(filename);
      this.axios.get('http://localhost:8081/files/' + filename,
                { responseType: 'blob' })
      .then(resp => {
        const url = window.URL.createObjectURL(new Blob([resp.data]));
        const link = document.createElement('a');
        link.href = url;
        link.download = filename;
        link.click();
        this.fileToDownload = null;
      })
      .catch(err => {
        console.log(err);
        this.failDialog = true;
        this.getFiles();
      });
    },

    beautifySize(size) {
      if (size < 1024)
        return size.toFixed(2) + 'B';
      size /= 1024;
      if (size < 1024)
        return size.toFixed(2) + 'KB';
      size /= 1024;
      if (size < 1024)
        return size.toFixed(2) + 'MB';
    }
  },

  beforeMount () {
    this.getFiles();
  }
}
</script>

<style>
.rounded-image {
  border-radius: 20px;
}
</style>