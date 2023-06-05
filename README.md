## HttpServer

基于 Boost.Asio 实现一个简易的 http 服务器，可以通过 CGI 程序实现简单的动态页面生成

****

### 实现的功能

1. 通过浏览器，发送一个标准的HTTP请求，服务器可以返回一个标准的HTTP响应。
2. 如果请求的是一个 html 网页，那么就能在浏览器中看到对应的页面。
3. 服务器能够接收标准的 HTTP 请求:
    - GET 方法
    - POST 方法
2. 服务器能够返回的响应类型:
    - 200 OK 
    - 400 Bad Request
    - 404 Not Found

### 关于 CGI

本项目对 CGI 的处理偏向简单，通过收到的请求是一个GET并带有QUERY_STRING或者是POST方法为依据进行 CGI 逻辑。

CGI 程序仅仅返回客户端输入的内容。