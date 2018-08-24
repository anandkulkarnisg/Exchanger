#include<iostream>
#include<string>
#include<thread>
#include<mutex>
#include<shared_mutex>

#include "Exchanger.h"

using namespace std;

Exchanger<std::string> exchanger;
std::mutex cout_mutex;
std::mutex sync_mutex;
std::atomic<int> syncStatus;
std::condition_variable m_cond;
std::atomic<bool> processingStatus;
const long runDemoLoop = 10000;

void printOut(const std::string& str1, const std::string& str2, const std::thread::id& id)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << str1 << id << str2 << std::endl;
	lock.unlock();
}

void resetExchanger()
{
	while(processingStatus != false)
	{
		std::unique_lock<std::mutex> lock(sync_mutex);
		m_cond.wait(lock, []() { return(syncStatus == 0); });
		exchanger.reset();
		syncStatus.store(2, std::memory_order_seq_cst);
		lock.unlock();
	}
}

void testExchanger(std::string inputString)
{
	printOut("I am currently running from thread id =", ".The value i am having initially is = "+inputString, std::this_thread::get_id());
	std::string returnString;
	int i=0;

	while(i<runDemoLoop)
	{
		while(syncStatus != 2) { m_cond.notify_all();  }
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
		syncStatus.fetch_sub(1, std::memory_order_seq_cst);
		m_cond.notify_all();
		++i;
	}
}

int main(int argc, char* argv[])
{
	std::string firstString = "First Thread";
	std::string secondString = "Second Thread";
	syncStatus.store(2, std::memory_order_seq_cst);
	processingStatus.store(true, std::memory_order_seq_cst);

	std::thread t1(&testExchanger, firstString);
	std::thread t2(&testExchanger, secondString);
	std::thread t3(&resetExchanger);

	t1.join();
	t2.join();		
	processingStatus.store(false, std::memory_order_seq_cst);
	if(syncStatus != 0)
	{
		syncStatus.store(0, std::memory_order_seq_cst); 
	}
	m_cond.notify_all(); 
	t3.join();

	return(0);
}

