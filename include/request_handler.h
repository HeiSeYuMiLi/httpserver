#pragma once

#include <string>

namespace httpserver {

	class Request;
	class Reply;

	// 处理读到的请求
	class RequestHandler {
	public:
		RequestHandler();

		// 判断处理方式：按静态文件处理或动态生成
		void HandleRequest(Request& req, Reply& rep);
	private:
		// 静态文件处理
		void HandleStaticFile(Request& req, Reply& rep);

		// 动态文件处理 CGI程序
		void HandleCGI(Request& req, Reply& rep);

		// 获取文件的路径
		std::string GetFilePath(const std::string& url_path);
	};

}