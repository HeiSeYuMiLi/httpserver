#include "request_parser.h"
#include "request.h"
#include "logger.h"
#include <string>

namespace httpserver {

	RequestParser::RequestParser() {}

	/*
	* ���ĸ�ʽ
	* ��һ�У����󷽷� url �汾�� \r������ \n���з�
	* �ڶ��У������� \r������ \n���з�
	* �����У����ӷ�ʽ�������ӡ������ӣ� \r������ \n���з�
	* ��n�У������ֶ� \r������ \n���з�
	* ��m�У�����
	*/

	bool RequestParser::Parse(Request& req, std::string buffer) {
		// ��������а��зֽ�
		std::vector<std::string> data = StringSplit(buffer, '\n');

		// �����һ�� method��url��version
		std::vector<std::string> first_line = StringSplit(data[0], ' ');
		req.method = first_line[0];
		req.url = first_line[1];
		UrlParse(req.url, req);

		// ������ͷ
		// ��⵽���м���header����
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

		// �����get���󣬲���Ҫ��ȡbody
		if (req.method == "GET") {
			Logger(FILE_LOCATION, log_level::info, "Received a GET request");
			return true;
		}

		Logger(FILE_LOCATION, log_level::info, "Received a POST request");

		// ���ҳ����ֶ�
		std::string len = "";
		for (auto it : req.headers) {
			if (it.name == "Content-Length") {
				len = it.value;
				break;
			}
		}
		// �����post���󣬵�û��Content��length�ֶΣ�����Ϊ�˴�����ʧ��
		if (req.method == "POST" && len == "") {
			Logger(FILE_LOCATION, log_level::error, "POST Request has no Content-Length!");
			return false;
		}

		// �����POST�����г����ֶ�
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
		// ��?Ϊ�ָ����ǰ����path�������ǲ���������֮��ʹ��&����������

		// �в���
		if (url.find('?') != std::string::npos) {
			std::vector<std::string> vec = StringSplit(url, '?');
			req.url_path = vec[0];

			// �������
			// �ж������
			if (vec[1].find('&') != std::string::npos) {
				std::vector<std::string> param_vec = StringSplit(vec[1], '&');
				for (auto i : param_vec) {
					std::vector<std::string> param = StringSplit(i, '=');
					req.params.insert(make_pair(param[0], param[1]));
				}
			}
			else {// ����һ������
				std::vector<std::string> param = StringSplit(vec[1], '=');
				req.params.insert(make_pair(param[0], param[1]));
			}
			req.isCgi = true;
		}
		else {// û�в���
			req.url_path = url;
		}
	}
}