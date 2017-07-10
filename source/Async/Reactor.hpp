//
//  Reactor.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#if defined(__linux__)
	#define ASYNC_EPOLL
#elif defined(__MACH__)
	#define ASYNC_KQUEUE
#endif

#include <vector>
#include <Time/Interval.hpp>

#if defined(ASYNC_EPOLL)
	#include <sys/epoll.h>
#elif defined(ASYNC_KQUEUE)
	#include <sys/types.h>
	#include <sys/event.h>
	#include <sys/time.h>
#else
	#error "Unable to determine Async implementation."
#endif

#include "Handle.hpp"

namespace Async
{
	using Time::Interval;
	
	class Reactor
	{
	public:
		Reactor();
		virtual ~Reactor();
		
		// Run the reactor once, waiting for at most the given duration for events to occur.
		std::size_t update(Interval duration);
		
		/// Invoke update multiple times for the given duration.
		std::size_t wait(Interval duration);
		
		const Handle & handle() const noexcept {return _selector;}
		Handle & handle() noexcept {return _selector;}
		
	private:
		Handle _selector;

#if defined(ASYNC_EPOLL)
	public:
		void append(int operation, Descriptor descriptor, int events, void * data);
		
	private:
		std::vector<struct epoll_event> _events;
#elif defined(ASYNC_KQUEUE)
	public:
		void append(const struct kevent & event, bool flush = true);
		
	private:
		std::vector<struct kevent> _changes;
		std::vector<struct kevent> _events;
#endif
;
	};
}
