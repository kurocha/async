//
//  Reactor.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Reactor.hpp"

#include <Time/Timeout.hpp>
#include <Concurrent/Fiber.hpp>

#include <errno.h>
#include <system_error>

#include <unistd.h>

namespace Async
{
	std::size_t Reactor::wait(Interval duration)
	{
		Time::Timeout timeout(duration);
		std::size_t count = 0;
		
		timeout.start();
		
		while (true) {
			auto remaining = timeout.remaining();
			
			if (remaining < Interval(0)) break;
			
			count += update(remaining);
		}
		
		return count;
	}
	
	Reactor::~Reactor()
	{
	}
	
#if defined(ASYNC_EPOLL)
	Reactor::Reactor() : _selector(::epoll_create1(EPOLL_CLOEXEC))
	{
		_events.reserve(512);
	}
	
	std::size_t Reactor::update(Interval duration)
	{
		_events.resize(_events.capacity());
		auto result = ::epoll_wait(_selector, _events.data(), _events.size(), duration.as_milliseconds());
		
		// If we are interrupted, return gracefully.
		if (result == -1 && errno == EINTR)
			return 0;
		
		if (result == -1)
			throw std::system_error(errno, std::generic_category(), "epoll_wait");
		
		_events.resize(result);
		
		for (auto & event : _events) {
			auto fiber = reinterpret_cast<Concurrent::Fiber *>(event.data.ptr);
			
			if (fiber != nullptr)
				fiber->resume();
		}
		
		_events.resize(0);
		
		// If we received the maximum number of events, increase the size of the event buffer.
		if (std::size_t(result) == _events.capacity()) {
			_events.reserve(_events.capacity() * 2);
		}
		
		return result;
	}

	void Reactor::append(int operation, Descriptor descriptor, int events, void * data)
	{
		struct epoll_event event;
		event.events = events;
		event.data.fd = descriptor;
		event.data.ptr = data;
		
		auto result = ::epoll_ctl(_selector, operation, descriptor, &event);
		
		if (result == -1)
			throw std::system_error(errno, std::generic_category(), "epoll_ctl");
	}
	
#elif defined(ASYNC_KQUEUE)
	Reactor::Reactor() : _selector(::kqueue())
	{
		_events.reserve(512);
	}
	
	static const char * filter_name(int16_t filter) {
		switch (filter) {
			case EVFILT_READ: return "EVFILT_READ";
			case EVFILT_WRITE: return "EVFILT_WRITE";
			case EVFILT_TIMER: return "EVFILT_TIMER";
		}
		
		return "???";
	}
	
	static const char * flags_name(uint16_t flags) {
		switch (flags) {
			case EV_ADD: return "EV_ADD";
			case EV_DELETE: return "EV_DELETE";
			case EV_DISABLE: return "EV_DISABLE";
			case (EV_ADD | EV_ONESHOT): return "EV_ADD|EV_ONESHOT";
			case (EV_ADD | EV_CLEAR): return "EV_ADD|EV_CLEAR";
		}
		
		return "???";
	}
	
	std::size_t Reactor::update(Interval duration)
	{
		auto timeout = duration.as_timespec();
		
		// TODO is this slow?
		_events.resize(_events.capacity());
		auto result = kevent(_selector, _changes.data(), _changes.size(), _events.data(), _events.size(), duration < Interval(0) ? nullptr : &timeout);
		
		// std::cerr << "update kqueue = " << result << " errno = " << errno << std::endl;
		// 
		// for (auto & change : _changes) {
		// 	std::cerr << "\tchange " << change.ident << " " << filter_name(change.filter) << " " << flags_name(change.flags) << std::endl;
		// }
		
		// If we are interrupted, return gracefully.
		if (result == -1 && errno == EINTR)
			return 0;
		
		if (result == -1) 
			throw std::system_error(errno, std::generic_category(), "kqueue");
		
		_changes.clear();
		_events.resize(result);
		
		for (auto & event : _events) {
			// std::cerr << "\tfiring " << event.ident << " " << filter_name(event.filter) << " " << flags_name(event.flags) << std::endl;
			
			auto fiber = reinterpret_cast<Concurrent::Fiber *>(event.udata);
			
			if (fiber != nullptr)
				fiber->resume();
		}
		
		_events.resize(0);
		
		// If we received the maximum number of events, increase the size of the event buffer.
		if (std::size_t(result) == _events.capacity()) {
			_events.reserve(_events.capacity() * 2);
		}
		
		return result;
	}
	
	void Reactor::append(const struct kevent & event, bool flush)
	{
		_changes.push_back(event);
		
		if (flush) {
			auto result = kevent(_selector, _changes.data(), _changes.size(), nullptr, 0, nullptr);
			
			// std::cerr << "apply_changes kqueue = " << result << " errno = " << errno << std::endl;
			// 
			// for (auto & change : _changes) {
			// 	std::cerr << "\tchange " << change.ident << " " << filter_name(change.filter) << " " << flags_name(change.flags) << std::endl;
			// }
			
			_changes.clear();
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "kqueue");
		}
	}
#endif
}
