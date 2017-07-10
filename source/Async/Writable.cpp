//
//  Writable.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 29/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Writable.hpp"

#include <Concurrent/Fiber.hpp>

#include <cassert>

namespace Async
{
	using namespace Concurrent;
	
	Writable::Writable(Descriptor descriptor, Reactor & reactor) : _descriptor(descriptor), _reactor(reactor)
	{
	}
	
	Writable::~Writable()
	{
		if (_invoked) {
#if defined(ASYNC_KQUEUE)
			_reactor.append({
				static_cast<uintptr_t>(_descriptor),
				EVFILT_WRITE,
				EV_DELETE,
				0,
				0,
				nullptr
			});
#elif defined(ASYNC_EPOLL)
			_reactor.append(EPOLL_CTL_DEL, {
				.events = EPOLLIN|EPOLLOUT,
				{
					.fd = _descriptor,
				}
			});
#endif
		}
	}
	
	void Writable::wait()
	{
		assert(Fiber::current);
		
		if (!_invoked) {
#if defined(ASYNC_KQUEUE)
			_reactor.append({
				static_cast<uintptr_t>(_descriptor),
				EVFILT_WRITE,
				EV_ADD | EV_CLEAR,
				0,
				0,
				(void*)Fiber::current
			}, false);
#elif defined(ASYNC_EPOLL)
			_reactor.append(EPOLL_CTL_ADD, {
				.events = EPOLLOUT|EPOLLET,
				.data = {
					.fd = _descriptor,
					.data = (void*)Fiber::current
				}
			});
#endif
			
			_invoked = true;
		}
	
		Fiber::current->yield();
	}
}
