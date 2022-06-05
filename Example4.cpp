#include<iostream>
#include<string>
#include<thread>
#include<mutex>

#include "Exchanger.h"

using namespace std;

Exchanger<string> exchanger;
mutex cout_mutex;
const long waitMilliSecs = 5000;

void printOut(const string& str1, const string& str2, const thread::id& id)
{
	unique_lock<mutex> lock(cout_mutex);
	cout << str1 << id << str2 << endl;
	lock.unlock();
}

void testExchanger(const string& inputString)
{
	printOut("I am currently running from thread id = ", ".The value i am having initially is = "+inputString, this_thread::get_id());
	string returnString;
	bool isExceptionState = false;
	try
	{
		if(inputString == "Second Thread")
		{
			printOut("I am currently running from thread id = ", ".I am going to exchange with wait for =  " + to_string(waitMilliSecs), this_thread::get_id());
			returnString = exchanger.exchange(inputString, waitMilliSecs, TimeUnit::MilliSeconds);
		}
		else
		{
			printOut("I am currently running from thread id = ", ".I am going to wait for before attempting exchange" + to_string(waitMilliSecs), this_thread::get_id());
			// This hit about 50 micro seconds earlier than the other Second Thread is about to time out waiting. It makes it sometimes and doesnt sometimes!
			this_thread::sleep_for(chrono::microseconds((waitMilliSecs*1000)-50));
			returnString = exchanger.exchange(inputString);
		}
	}
	catch(const exception& e)
	{
		printOut("Recieved the exception in thread id = ", ". Exception Message is = " + string(e.what()), this_thread::get_id());
		isExceptionState = true;
	}
	if(!isExceptionState)
		printOut("I am currently running from thread id = ", ".The value i am having now is = " + returnString, this_thread::get_id());
}

int main(int argc, char* argv[])
{
	const string firstString = "First Thread";
	const string secondString = "Second Thread";

	thread t1(&testExchanger, cref(firstString));
	thread t2(&testExchanger, cref(secondString));

	t1.join();
	t2.join();		

	return(0);
}

