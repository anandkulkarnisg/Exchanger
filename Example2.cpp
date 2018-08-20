#include<iostream>
#include<string>
#include<thread>
#include<mutex>
#include<shared_mutex>

#include "Exchanger.h"

using namespace std;

Exchanger<std::string> exchanger;
std::mutex cout_mutex;
std::shared_mutex sync_mutex;

void printOut(const std::string& str1, const std::string& str2, const std::thread::id& id)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << str1 << id << str2 << std::endl;
	lock.unlock();
}

void testExchanger(const std::string& inputString,const bool& resetStatus)
{
	printOut("I am currently running from thread id =", ".The value i am having initially is = "+inputString, std::this_thread::get_id());
	std::string returnString;
	int i=0;
	
	// This has a dead lock situation! between lines of shared_lock and exclusive lock.
	
	while(i<1000)
	{
		try
		{
			std::shared_lock<std::shared_mutex> lock(sync_mutex);
			returnString = exchanger.exchange(inputString);
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		printOut("I am currently running from thread id =", ".The value i am having now is = " + returnString, std::this_thread::get_id());
		if(resetStatus)
		{
			std::unique_lock<std::shared_mutex> lock(sync_mutex);
			exchanger.reset();
			lock.unlock();
		}
		++i;
	}
}

int main(int argc, char* argv[])
{
	std::string firstString = "First Thread";
	std::string secondString = "Second Thread";

	std::thread t1(&testExchanger, firstString, true);
	std::thread t2(&testExchanger, secondString, false);

	t1.join();
	t2.join();		

	return(0);
}

