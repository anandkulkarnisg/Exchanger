#include<iostream>
#include<string>
#include<memory>
#include<mutex>
#include<thread>
#include<CyclicBarrier.h>
#include<boost/core/noncopyable.hpp>

#include "TimeUtils.h"

#ifndef Exchanger_H
#define Exchanger_H

template<typename T> class Exchanger : private boost::noncopyable
{
	private:
		std::unique_ptr<CyclicBarrier> m_preBarrierPtr;						// We attempt to use a Cyclic Barrier to implement an Exchanger item.
		std::unique_ptr<CyclicBarrier> m_postBarrierPtr;					// We attempt to use a pair of pre and post barriers to sync up exchanging threads [ Always 2 of them ].
		T m_item1;															// First item of Exchanger.
		T m_item2;															// Second item of Exchanger.

	public:
		Exchanger();														// Default Constructor.
		T exchange(const T&);												// Returns the exchanged item.
		T exchange(const T&, const long&, const TimeUnit& = TimeUnit::MilliSeconds); // Implement a exchange with timeout and exception mechanism.
		void reset();														// The barriers can go broken due to timeouts. Hence need a reset interface.
};

#endif
