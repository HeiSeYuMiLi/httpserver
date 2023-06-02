#include "request_parser.h"
#include "request.h"
#include "logger.h"
#include <string>

namespace httpserver {

	RequestParser::RequestParser() {}

	/*
	* 报文格式
	* 第一行：请求方法 url 版本号 \r结束符 \n换行符
	* 第二行：主机号 \r结束符 \n换行符
	* 第三行：连接方式（长连接、短连接） \r结束符 \n换行符
	* 第n行：长度字段 \r结束符 \n换行符
	* 第m行：正文
	*/

	bool RequestParser::Parse(Request& req, std::string buffer) {
		// 对请求进行按行分解
		std::vector<std::string> data = StringSplit(buffer, '\n');

		// 处理第一行 method、url、version
		std::vector<std::string> first_line = StringSplit(data[0], ' ');
		req.method = first_line[0];
		req.url = first_line[1];
		UrlParse(req.url, req);

		// 处理报文头
		// 检测到空行即是header结束
		int i = 1;
		while (1) {
			if (data[i] == "\r")
				break;
			std::vector<std::string> str_vec = StringSplit(data[i], ':');
			req.headers.push_back(Header(str_vec[0], str_vec[1]));
			i++;
			if (i > data.size() - 1)
				break;
		}

		// 如果是get请求，不需要读取body
		if (req.method == "GET") {
			Logger(FILE_LOCATION, log_level::info, "Received a GET request");
			return true;
		}

		Logger(FILE_LOCATION, log_level::info, "Received a POST request");

		// 查找长度字段
		std::string len = "";
		for (auto it : req.headers) {
			if (it.name == "Content-Length") {
				len = it.value;
				break;
			}
		}
		// 如果是post请求，但没有Content—length字段，则认为此次请求失败
		if (req.method == "POST" && len == "") {
			Logger(FILE_LOCATION, log_level::error, "POST Request has no Content-Length!");
			return false;
		}

		// 如果是POST并且有长度字段
		int content_length = atoi(len.c_str());
		req.body = data[data.size() - 1].substr(0, content_length);

		return true;
	}

	std::vector<std::string> RequestParser::StringSplit(const std::string& str, char delim) {
		std::size_t previous = 0;
		std::size_t current = str.find(delim);
		std::vector<std::string> elems;
		while (current != std::string::npos) {
			if (current > previous) {
				elems.push_back(str.substr(previous, current - previous));
			}
			previous = current + 1;
			current = str.find(delim, previous);
		}
		if (previous != str.size()) {
			elems.push_back(str.substr(previous));
		}
		return elems;
	}

	void RequestParser::UrlParse(const std::string& url, Request& req) {
		// 以?为分割符，前面是path，后面是参数，参数之间使用&进行了连接

		// 有参数
		if (url.find('?') != std::string::npos) {
			std::vector<std::string> vec = StringSplit(url, '?');
			req.url_path = vec[0];

			// 处理参数
			// 有多个参数
			if (vec[1].find('&') != std::string::npos) {
				std::vector<std::string> param_vec = StringSplit(vec[1], '&');
				for (auto i : param_vec) {
					std::vector<std::string> param = StringSplit(i, '=');
					req.params.insert(make_pair(param[0], param[1]));
				}
			}
			else {// 仅有一个参数
				std::vector<std::string> param = StringSplit(vec[1], '=');
				req.params.insert(make_pair(param[0], param[1]));
			}
			req.isCgi = true;
		}
		else {// 没有参数
			req.url_path = url;
		}
	}
}