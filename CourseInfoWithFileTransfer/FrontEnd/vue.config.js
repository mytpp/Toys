module.exports = {
  "transpileDependencies": [
    "vuetify"
  ],
  devServer: {
    open: true, // 启动服务后是否打开浏览器
    host: '0.0.0.0',
    port: 8080, // 服务端口
    https: false,
    hotOnly: false,
    proxy: null, // 设置代理
    before: app => { }
  },
  configureWebpack: (config) => {
    if (process.env.NODE_ENV === 'production') {
      config.optimization.minimizer[0].options.terserOptions.compress.drop_console = true
    }
  },
}