#pragma once
#include <string>
#include <vector>

namespace httpserver {

	class Request;

	// 对读取到的请求进行解析 构造成Request对象
	class RequestParser {
	public:
		RequestParser();

		// 解析请求
		bool Parse(Request& req, std::string buffer);
	private:
		// 分割字符串
		std::vector<std::string> StringSplit(const std::string& str, char delim);

		// 对url进行解析
		void UrlParse(const std::string& url, Request& req);
	};

}