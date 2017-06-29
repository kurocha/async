//
//  Reactor.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Reactor.hpp"

#include <Concurrent/Fiber.hpp>

#include <errno.h>
#include <system_error>

#include <unistd.h>

namespace Async
{
	Reactor::Reactor()
	{
		_selector = ::kqueue();
		
		_events.reserve(512);
	}
	
	Reactor::~Reactor()
	{
		::close(_selector);
	}
	
	std::size_t Reactor::wait(Interval duration)
	{
		auto timeout = duration.as_timespec();
		
		// TODO is this slow?
		_events.resize(_events.capacity());
		auto count = kevent(_selector, _changes.data(), _changes.size(), _events.data(), _events.size(), duration < Interval(0) ? nullptr : &timeout);
		
		if (count == -1)
			throw std::system_error(errno, std::generic_category(), "kqueue");
		
		_changes.resize(0);
		_events.resize(count);
		
		for (auto & event : _events) {
			auto fiber = reinterpret_cast<Concurrent::Fiber *>(event.udata);
			
			fiber->resume();
		}
		
		_events.resize(0);
		
		// If we received the maximum number of events, increase the size of the event buffer.
		if (count == _events.capacity()) {
			_events.reserve(_events.capacity() * 2);
		}
		
		return count;
	}
}
