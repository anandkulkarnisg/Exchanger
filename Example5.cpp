#include<iostream>
#include<string>
#include<thread>

#include "Exchanger.h"

using namespace std;

Exchanger<std::string> exchanger;
std::mutex cout_mutex;
const long runDemoLoop = 10000;

void printOut(const std::string& str1, const std::string& str2, const std::thread::id& id)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << str1 << id << str2 << std::endl;
	lock.unlock();
}

void testExchanger(std::string inputString)
{
	printOut("I am currently running from thread id =", ".The value i am having initially is = "+inputString, std::this_thread::get_id());
	std::string returnString;
	int i=0;

	while(i<runDemoLoop)
	{
		try
		{
			returnString = exchanger.exchange(inputString);			
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		printOut("I am currently running from thread id =", ".The value i am having now is = " + returnString, std::this_thread::get_id());
		inputString = returnString;
		++i;
	}
}

// CRITICAL NOTE : This is a still in work desired example. It does not work but i am trying to make it work. The aim is to somehow achieve the reset happen on exchanger while the other thread gets blocked
// at exchange method. I am not able to do it at the moment. Example2.cpp is a clumsy but working implementation which uses a bit of spinning to avoid the issue of synchronization.

int main(int argc, char* argv[])
{
	std::string firstString = "First Thread";
	std::string secondString = "Second Thread";

	std::thread t1(&testExchanger, firstString);
	std::thread t2(&testExchanger, secondString);

	t1.join();
	t2.join();		

	return(0);
}

