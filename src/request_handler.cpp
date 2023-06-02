#include "request_handler.h"
#include "request.h"
#include "reply.h"
#include "logger.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>

namespace httpserver {

    RequestHandler::RequestHandler() {}

    void RequestHandler::HandleRequest(Request& req, Reply& rep) {
        // 根据Request的_isCgi判断
        if (req.isCgi) {
            HandleCGI(req, rep);
            rep.headers.resize(2);
            rep.headers[0].name = "Content-Length";
            rep.headers[0].value = std::to_string(rep.body.size());
            rep.headers[1].name = "Content-Type";
            rep.headers[1].value = "text/html";
            rep.status = Reply::status_type::ok;
        }
        else
            HandleStaticFile(req, rep);
    }

    void RequestHandler::HandleStaticFile(Request& req, Reply& rep) {
        std::string full_path = GetFilePath(req.url_path);
        
        std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
        if (!is)
        {
            Logger(FILE_LOCATION, log_level::error, "File no found!");
            rep = Reply::stock_reply(Reply::status_type::not_found);
            return;
        }
        char buf[512];
        while (is.read(buf, sizeof(buf)).gcount() > 0)
            rep.body.append(buf, is.gcount());
        rep.headers.resize(2);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = std::to_string(rep.body.size());
        rep.headers[1].name = "Content-Type";
        rep.headers[1].value = "text/html";
        rep.status = Reply::status_type::ok;
    }

    void RequestHandler::HandleCGI(Request& req, Reply& rep) {
        int pipes[2];

        // 创建一个匿名管道
        int ret = pipe(pipes);

        if (ret < 0) {
            perror("create pipe error");
            return;
        }
        pid_t id = fork();
        if (id < 0) {
            perror("fork error");
            return;
        }
        else if (id > 0) {
            close(pipes[1]);

            char buff[1024];
            // 读取数据，保存在reply中的body中
            ssize_t s = read(pipes[0], buff, sizeof(buff) - 1);
            if (s > 0) {
                buff[s] = '\0';
                rep.body = std::string(buff);
            }
            else {
                perror("read error");
                exit(1);
            }
            // 等待子进程结束 避免僵尸进程
            if (waitpid(id, 0, NULL) != -1) {
                Logger(FILE_LOCATION, log_level::info, "Wait success");
            }
        }
        else {
            std::string buff;
            if (req.method == "GET")
            {
                buff = req.params["name"];
            }
            else if (req.method == "POST")
            {
                buff = req.body;
            }

            dup2(pipes[0], 0);
            dup2(pipes[1], 1);

            // 设置环境变量
            std::string env = "QUERY_STRING=" + buff;
            putenv(const_cast<char*>(env.c_str()));

            execl("./cgi_main", "cgi_main", NULL);

            // 如果程序执行到这就表明替换失败了
            //std::cerr << "替换失败" << std::endl;
            Logger(FILE_LOCATION, log_level::warn, "Replace failed!");
        }

        return;
    }

    std::string RequestHandler::GetFilePath(const std::string& url_path) {
        std::string path;
        int pos = url_path.find_last_of('/');
        if (pos < url_path.size() - 1)
            pos += 1;
        else {
            path = "index.html";
            return path;
        }
        path = url_path.substr(pos);
        return path;
    }
}
