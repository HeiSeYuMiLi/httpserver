#pragma once

#include <string>

namespace httpserver {

	// 报文头
	struct Header {

		Header(const std::string& n, const std::string& v) :name(n), value(v) {

		}
		Header(){}

		std::string name;
		std::string value;

	};

}