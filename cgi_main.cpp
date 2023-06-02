#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>

int main(int argc, char* argv[])
{
	const char* query_string = getenv("QUERY_STRING");

	std::cout << "<!DOCTYPE HTML>\n";
	std::cout << "<html>\n";
	std::cout << "<head>\n";
	std::cout << "<link rel=\"icon\" href=\"data:; base64, =\">\n";
	std::cout << "<title>动态生成的页面</title>\n";
	std::cout << "<meta charset=\"utf-8\" />\n";
	std::cout << "</head>\n";

	std::cout << "<body>\n";
	std::cout << "<h1>";
	std::cout << "动态生成的页面\n";
	std::cout << "</h1>\n";
	std::cout << "你刚才输入的内容是：\n";
	std::cout << std::string(query_string) << "\n";
	std::cout << "</body>\n";

	std::cout << "</html>\n";

	return 0;

}
