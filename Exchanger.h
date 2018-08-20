#include<iostream>
#include<string>
#include<memory>
#include<mutex>
#include<thread>
#include<CyclicBarrier.h>

#ifndef Exchanger_H
#define Exchanger_H

template<typename T> class Exchanger : private boost::noncopyable
{
	private:
		std::unique_ptr<CyclicBarrier> m_barrierPtr;						// We attempt to use a Cycic Barrier to implement an Exchanger item.
		std::unique_ptr<std::once_flag> m_onceFlag;							// We use once flag to execute only one time once the barrier is tripped.
		T m_item1;															// First item of Exchanger.
		T m_item2;															// Second item of Exchanger.
		std::atomic<bool> m_item1Status;									// status of set/unset for First Item.
		std::atomic<bool> m_item2Status;									// status of set/unset for the Second Item.

	public:
		Exchanger();														// Default Constructor.
		T exchange(const T&);												// Returns the exchanged item.
		void show();														// Dump the status.
		void reset();
};

#endif
