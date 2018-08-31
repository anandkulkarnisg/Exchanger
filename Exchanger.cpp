#include<iostream>
#include<string>

#include "Exchanger.h"

using namespace std;

// First implement the Constructor for the Exchanger.
template<typename T> Exchanger<T>::Exchanger()
{
	m_preBarrierPtr.reset(new CyclicBarrier(2));			// We need a barrier of 2 for exchanger.
	m_postBarrierPtr.reset(new CyclicBarrier(2));			// We need two barriers to sync up the threads.
}

// Implement a exchange function without wait feature.
template<typename T> T Exchanger<T>::exchange(const T& inputItem)
{
	int barrierCount;
	try
	{
		barrierCount = m_preBarrierPtr->await();
	}
	catch(const std::exception& e)
	{
		//std::cout << "recieved the exception = " << e.what() << std::endl;
		throw;	// Pass on the exceptions as is for further analysis or debugging.
	}

	// We have come here means barrier is broken and two exchanger threads have successfully entered.
	if(barrierCount == 1)
		m_item1 = inputItem;
	else
		m_item2 = inputItem;

	// We sync up again using post exchange barrier.
	try
	{
		m_postBarrierPtr->await();
	}
	catch(const std::exception& e)
	{
		//std::cout << "recieved the exception = " << e.what() << std::endl;
		throw;  // Pass on the exceptions as is for further analysis or debugging.
	}

	// Return the items to respective other threads.
	if(barrierCount == 1)
		return(m_item2);
	else
		return(m_item1);
}

// Implement exchange mechanism through a timeout feature.
template<typename T> T Exchanger<T>::exchange(const T& inputItem, const long& waitTime)
{
	// First we wait for the barrier and wont break out till the barrier breaks.
	int barrierCount;
	try
	{   
		barrierCount = m_preBarrierPtr->await(waitTime);
	}
	catch(const std::exception& e)
	{
		//std::cout << "recieved the exception = " << e.what() << std::endl;
		throw; // Pass on the exceptions as is for further analysis or debugging.
	}

	// We have come here means barrier is broken and two exchanger threads have successfully entered.
	if(barrierCount == 1)
		m_item1 = inputItem;
	else
		m_item2 = inputItem;

	// We sync up again using post exchange barrier. Note this one does not need to wait. This needs to sync immediately.
	try
	{   
		m_postBarrierPtr->await();
	}
	catch(const std::exception& e)
	{
		//std::cout << "recieved the exception = " << e.what() << std::endl;
		throw;  // Pass on the exceptions as is for further analysis or debugging.
	}

	// Return the items to respective other threads.
	if(barrierCount == 1)
		return(m_item2);
	else
		return(m_item1);
}

template<typename T> void Exchanger<T>::reset()
{
	if(m_preBarrierPtr->isBroken())
		m_preBarrierPtr->reset();
	if(m_postBarrierPtr->isBroken())
		m_postBarrierPtr->reset();	
}

template class Exchanger<std::string>;
