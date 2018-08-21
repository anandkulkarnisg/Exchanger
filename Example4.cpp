#include<iostream>
#include<string>
#include<thread>
#include<mutex>

#include "Exchanger.h"

using namespace std;

Exchanger<std::string> exchanger;
std::mutex cout_mutex;
const long waitMilliSecs = 5000;

void printOut(const std::string& str1, const std::string& str2, const std::thread::id& id)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << str1 << id << str2 << std::endl;
	lock.unlock();
}

void testExchanger(const std::string& inputString)
{
	printOut("I am currently running from thread id = ", ".The value i am having initially is = "+inputString, std::this_thread::get_id());
	std::string returnString;
	bool isExceptionState = false;
	try
	{
		if(inputString == "Second Thread")
		{
			printOut("I am currently running from thread id = ", ".I am going to exchange with wait for =  " + std::to_string(waitMilliSecs), std::this_thread::get_id());
			returnString = exchanger.exchange(inputString, waitMilliSecs);
		}
		else
		{
			printOut("I am currently running from thread id = ", ".I am going to wait for before attempting exchange" + std::to_string(waitMilliSecs), std::this_thread::get_id());
			std::this_thread::sleep_for(std::chrono::milliseconds(waitMilliSecs));
			returnString = exchanger.exchange(inputString);
		}
	}
	catch(const std::exception& e)
	{
		printOut("Recieved the exception in thread id = ", ". Exception Message is = " + std::string(e.what()), std::this_thread::get_id());
		isExceptionState = true;
	}
	if(!isExceptionState)
		printOut("I am currently running from thread id = ", ".The value i am having now is = " + returnString, std::this_thread::get_id());
}

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

