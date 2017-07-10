//
//  Readable.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 29/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Readable.hpp"

#include <Concurrent/Fiber.hpp>

#include <cassert>

namespace Async
{
	using namespace Concurrent;
	
	Readable::Readable(Descriptor descriptor, Reactor & reactor) : _descriptor(descriptor), _reactor(reactor)
	{
	}
	
	Readable::~Readable()
	{
		if (_invoked) {
#if defined(ASYNC_KQUEUE)
			_reactor.append({
				static_cast<uintptr_t>(_descriptor),
				EVFILT_READ,
				EV_DELETE,
				0,
				0,
				nullptr
			});
#elif defined(ASYNC_EPOLL)
			_reactor.append(EPOLL_CTL_DEL, _descriptor, 0, nullptr);
#endif
		}
	}
	
	void Readable::wait()
	{
		assert(Fiber::current);
		
		if (!_invoked) {
#if defined(ASYNC_KQUEUE)
			_reactor.append({
				static_cast<uintptr_t>(_descriptor),
				EVFILT_READ,
				EV_ADD | EV_CLEAR,
				0,
				0,
				Fiber::current
			}, false);
#elif defined(ASYNC_EPOLL)
			_reactor.append(EPOLL_CTL_ADD, _descriptor, EPOLLIN|EPOLLET, (void*)Fiber::current);
#endif
			
			_invoked = true;
		}
		
		Fiber::current->yield();
	}
}
