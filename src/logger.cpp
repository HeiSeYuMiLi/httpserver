#include "logger.h"
#include <string>

namespace httpserver {

	Logger::Logger(const std::string& file_path, unsigned long line, log_level level, const std::string& message) :_level(level) {
		int pos = file_path.find_last_of('/');
		_buff = file_path.substr(pos + 1) + ":" + std::to_string(line) + " " + message;
	}

	Logger::~Logger() {
		PrintLog(MakeLog());
	}

	std::string Logger::GetLevelString()
	{
		switch (_level)
		{
		case log_level::error: return "[error]";
		case log_level::warn: return "[warn]";
		case log_level::info: return "[info]";
		case log_level::debug: return "[debug]";
		default: return "[debug]";
		}
	}

	std::string Logger::MakeLog()
	{
		std::string log;
		log += Currtime();
		log += " ";
		log += GetLevelString();
		log += " ";
		log += _buff;

		return log;
	}

	void Logger::PrintLog(const std::string& log)
	{
		// 根据不同的等级进行不同的颜色的输出
		switch (_level)
		{
		case log_level::error:
			printf("\033[31m%s\n\033[0m", log.c_str());
			break;
		case log_level::warn:
			printf("\033[33m%s\n\033[0m", log.c_str());
			break;
		default:
			printf("%s\n", log.c_str());
			break;
		}
	}

	std::string Logger::Currtime() {

		time_t timep;

		time(&timep);

		char ch[30];
		std::string thisTime=ctime(&timep);
		return thisTime;
	}
}
