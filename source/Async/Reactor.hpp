//
//  Reactor.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#if defined(__linux__)
	#define ASYNC_MONITOR_EPOLL
#elif defined(__MACH__)
	#define ASYNC_MONITOR_KQUEUE
#else
	#error "Unable to determine Async::Monitor implementation."
#endif

#include <vector>
#include <Time/Interval.hpp>

#if defined(ASYNC_MONITOR_EPOLL)
	#include <sys/epoll.h>
#elif defined(ASYNC_MONITOR_KQUEUE)
	#include <sys/types.h>
	#include <sys/event.h>
	#include <sys/time.h>
#else
	#error "Unable to determine Async::Monitor implementation."
#endif

namespace std {
	template<>
	struct allocator<struct kevent> {
		typedef struct kevent value_type;
		
		value_type* allocate(size_t n)
		{
			return static_cast<value_type*>(::operator new(sizeof(value_type) * n));
		}
		
		void deallocate(value_type* p, size_t n)
		{
			return ::operator delete(static_cast<void*>(p));
		}
		
		template<class U, class... Args>
		void construct(U* p, Args&&... args)
		{
			::new(static_cast<void*>(p)) U{std::forward<Args>(args)...};
		}
	};
}

namespace Async
{
	using Time::Interval;
	typedef int FileDescriptor;
	
	class Reactor
	{
	public:
		Reactor();
		virtual ~Reactor();
		
		std::size_t wait(Interval duration);
		
		FileDescriptor file_descriptor() const noexcept {return _selector;}
		std::vector<struct kevent> & changes() noexcept {return _changes;}
	private:
		FileDescriptor _selector;
		
		std::vector<struct kevent> _changes;
		std::vector<struct kevent> _events;
	};
}
