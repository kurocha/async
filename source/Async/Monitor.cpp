//
//  Monitor.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 8/5/2018.
//  Copyright, 2018, by Samuel Williams. All rights reserved.
//

#include "Monitor.hpp"

#include <Concurrent/Fiber.hpp>

#include <iostream>

namespace Async
{
	using namespace Concurrent;
	
	Monitor::Monitor(Descriptor descriptor, Reactor & reactor) : _descriptor(descriptor), _reactor(reactor)
	{
	}
	
	Monitor::~Monitor()
	{
	}
	
	void Monitor::wait_readable()
	{
		this->wait(Event::READABLE);
	}
	
	void Monitor::wait_writable()
	{
		this->wait(Event::WRITABLE);
	}
	
#if defined(ASYNC_KQUEUE)
	void Monitor::wait(Monitor::Event events)
	{
		assert(Fiber::current);
		
		int action = EV_CLEAR | EV_ONESHOT;
		_added = true;
		
		_reactor.append({
			static_cast<uintptr_t>(_descriptor),
			events,
			EV_ADD | EV_CLEAR | EV_ONESHOT,
			0,
			0,
			(void*)Fiber::current
		}, false);
		
		Fiber::current->yield();
	}
#elif defined(ASYNC_EPOLL)
	void Monitor::wait(Monitor::Event events)
	{
		assert(Fiber::current);
		
		int action = EPOLL_CTL_ADD;
		
		if (_added) {
			action = EPOLL_CTL_MOD;
		} else {
			_added = true;
		}
		
		_reactor.append(action, _descriptor, events | EPOLLET | EPOLLONESHOT, (void*)Fiber::current);
		
		Fiber::current->yield();
	}
#endif
}
