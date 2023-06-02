#pragma once

#include <vector>
#include <map>
#include "header.h"

namespace httpserver {

	// 请求报文
	struct Request {

		std::string method;
		std::string url;
		std::string url_path;
		std::map<std::string, std::string> params;
		std::vector<Header> headers;
		std::string body;
		bool isCgi = false;
	};

}