#include<iostream>
#include<string>

#include "Exchanger.h"

using namespace std;

// First implement the Constructor for the Exchanger.
template<typename T> Exchanger<T>::Exchanger()
{
	m_barrierPtr.reset(new CyclicBarrier(2));		// We need a barrier of 2 for exchanger.
	m_onceFlag.reset(new std::once_flag);			// Get a new std::once flag.		
	m_item1Status.store(false, std::memory_order_seq_cst);
	m_item2Status.store(false, std::memory_order_seq_cst);
}

// Implement a exchange function without wait feature.
template<typename T> T Exchanger<T>::exchange(const T& inputItem)
{
	// First we wait for the barrier and wont break out till the barrier breaks.
	int barrierCount;
	try
	{
		barrierCount = m_barrierPtr->await();
	}
	catch(const std::exception& e)
	{
		//std::cout << "recieved the exception = " << e.what() << std::endl;
		throw;	// Pass on the exceptions as is for further analysis or debugging.
	}

	// We have come here means barrier is broken and two exchanger threads have successfully entered.
	if(barrierCount == 1)
	{
		m_item1 = inputItem;
		m_item1Status.store(true, std::memory_order_seq_cst);
	}
	else
	{
		m_item2 = inputItem;
		m_item2Status.store(true, std::memory_order_seq_cst);
	}

	// This is no cost while spin as it exists for extremely short period of time and should not take very long time to break out of this.
	while(m_item1Status != true || m_item2Status != true)
	{
		// Spin and nothing.
	}

	// Return the items to respective other threads.
	if(barrierCount == 1)
	{
		return(m_item2);
	}
	else
	{
		return(m_item1);
	}
}

// Implement exchange mechanism through a timeout feature.
template<typename T> T Exchanger<T>::exchange(const T& inputItem, const long& waitTime)
{
	// First we wait for the barrier and wont break out till the barrier breaks.
	int barrierCount;
	try
	{   
		barrierCount = m_barrierPtr->await(waitTime);
	}
	catch(const std::exception& e)
	{
		//std::cout << "recieved the exception = " << e.what() << std::endl;
		throw; // Pass on the exceptions as is for further analysis or debugging.
	}

	// We have come here means barrier is broken and two exchanger threads have successfully entered.
	if(barrierCount == 1)
	{
		m_item1 = inputItem;
		m_item1Status.store(true, std::memory_order_seq_cst);
	}
	else
	{
		m_item2 = inputItem;
		m_item2Status.store(true, std::memory_order_seq_cst);
	}

	// This is no cost while spin as it exists for extremely short period of time and should not take very long time to break out of this.
	// Reason is this : both participating threads will immediately move further on from await as soon as both hit it. Then we want to make sure that both item1 and item2 are set properly before
	// We want to return the values from the function.

	while(m_item1Status != true || m_item2Status != true)
	{
		// Spin and nothing.
	}

	// Return the items to respective other threads.
	if(barrierCount == 1)
	{
		return(m_item2);
	}
	else
	{
		return(m_item1);
	}
}

template<typename T> void Exchanger<T>::reset()
{
	try
	{
		m_barrierPtr->reset();
	}
	catch(const std::exception& e)
	{
		// This can happen if the barrier is already active and somebody tries to reset it. Currently for Exchanger we dont need to do anything for this.	
	}
	m_item1Status.store(false, std::memory_order_seq_cst);
	m_item2Status.store(false, std::memory_order_seq_cst);
}

template<typename T> void Exchanger<T>::show()
{
	std::cout << "m_item1 = " << m_item1 << std::endl;
	std::cout << "m_item2 = " << m_item2 << std::endl;
}

template class Exchanger<std::string>;
