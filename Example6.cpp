#include<iostream>
#include<string>
#include<thread>
#include<vector>

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

void producerThread(const std::vector<std::string>& dataSource)
{
	printOut("I am producer thread currently running from thread id =", "",  std::this_thread::get_id());
	std::string returnString;
	int i=0;

	while(i<runDemoLoop)
	{
		try
		{
			returnString = exchanger.exchange(dataSource[i]);			
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << std::endl;
			exchanger.reset();
		}
		++i;
	}
}

void consumerThread()
{

	printOut("I am consumer thread currently running from thread id =", "",  std::this_thread::get_id());
	std::string returnString;
	int i=0;
	int badMessageCount=0;

	while(i<runDemoLoop)
	{
		try
		{
			returnString = exchanger.exchange("", 50);           
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << std::endl;
			exchanger.reset();
			++badMessageCount;
		}
		printOut("I am consumer thread currently running from thread id =", ".The value i am having now is = " + returnString, std::this_thread::get_id());
		++i;
	}
	printOut("The consumer thread running from thread id = ", ".Recieved/missed in total " + std::to_string(badMessageCount) +" number of bad messages",std::this_thread::get_id());
}


// This is a demo of the producer and consumer thread written using Exchanger. The producer thread produces messages and wait for consumer thread on Exchanger synchronization.
// When they meet the Producer sends the message and recives nothing which is fine. The consumer gets the message and sends a blank string which producer can ignore.

int main(int argc, char* argv[])
{
	std::vector<std::string> dataSource;
	dataSource.reserve(runDemoLoop);

	for(unsigned int i=0; i<runDemoLoop; ++i)
		dataSource.emplace_back("Sample Message : " + std::to_string(i));

	std::thread t1(&producerThread, std::cref(dataSource));
	std::thread t2(&consumerThread);

	t1.join();
	t2.join();		

	return(0);
}

