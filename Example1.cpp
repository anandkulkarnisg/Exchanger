#include<iostream>
#include<string>
#include<thread>
#include<mutex>

#include "Exchanger.h"

using namespace std;

Exchanger<std::string> exchanger;
std::mutex cout_mutex;

void printOut(const std::string& str1, const std::string& str2, const std::thread::id& id)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << str1 << id << str2 << std::endl;
	lock.unlock();
}

void testExchanger(const std::string& inputString)
{
	printOut("I am currently running from thread id =", ".The value i am having initially is = "+inputString, std::this_thread::get_id());
	std::string returnString;
	try
	{
		returnString = exchanger.exchange(inputString);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	printOut("I am currently running from thread id =", ".The value i am having now is = " + returnString, std::this_thread::get_id());
}

int main(int argc, char* argv[])
{
	std::string firstString = "First Thread";
	std::string secondString = "Second Thread";

	int i=0;
	while(i<1000)
	{
		std::thread t1(&testExchanger, firstString);
		std::thread t2(&testExchanger, secondString);

		t1.join();
		t2.join();		

		++i;
	}

	return(0);
}

