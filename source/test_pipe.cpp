#include<unistd.h>   
#include<iostream> 
#include<string>  
#include"../sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
const int N = 100; 

void Pipe()
{
	SYLAR_LOG_INFO(g_logger) << "Proc_pipe start..." << std::endl;

    int fds1[2], fds2[2];
	std::string str1 = "Message from parent!";
	std::string str2 = "Message from child!";
    char buf[N];
    pid_t pid;

    pipe(fds1);
    pipe(fds2);
    pid = fork();
    if (!pid) 
    {
        write(fds1[1], str2.c_str(), sizeof(str1));
        read(fds2[0], buf, N);

		SYLAR_LOG_INFO(g_logger) << "I'm the child proc: ";
		SYLAR_LOG_INFO(g_logger) << "Child proc output: " << buf << std::endl;
    }
    else
    {
        read(fds1[0], buf, N);

		SYLAR_LOG_INFO(g_logger) << "I'm the parent proc:  ";
		SYLAR_LOG_INFO(g_logger) << "Parent proc output: " << buf;

        write(fds2[1], str1.c_str(), sizeof(str2));
		sleep(1);
		SYLAR_LOG_INFO(g_logger) << "Proc_pipe end...";
    }

	close(fds1[0]); 
	close(fds1[1]);
	close(fds2[0]);
	close(fds2[1]);
}

int main(int argc, char** argv) {

	SYLAR_LOG_INFO(g_logger) << "Run...";
	sylar::Thread::ptr thr(new sylar::Thread(&Pipe, "proc_pipe"));
	thr->join();

	return 0;
}
