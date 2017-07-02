//
//  After.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 29/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "After.hpp"

#include <Concurrent/Fiber.hpp>

#include <cassert>

namespace Async
{
	using namespace Concurrent;
	
	After::After(Interval duration, Reactor & reactor) : _duration(duration), _reactor(reactor)
	{
	}
	
	After::~After()
	{
	}
	
	void After::wait()
	{
		assert(Fiber::current);
		
		_reactor.append({
			reinterpret_cast<uintptr_t>(this),
			EVFILT_TIMER,
			EV_ADD | EV_ONESHOT,
			// TODO this may overflow
			NOTE_USECONDS,
			static_cast<intptr_t>(static_cast<double>(_duration) * 1000 * 1000),
			Fiber::current
		});
		
		try {
			Fiber::current->yield();
		} catch (...) {
			_reactor.append({
				reinterpret_cast<uintptr_t>(this),
				EVFILT_TIMER,
				EV_ADD | EV_ONESHOT,
				0,
				0,
				nullptr
			}, false);
			
			throw;
		}
	}
}
