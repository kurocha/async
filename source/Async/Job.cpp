//
//  Job.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 10/9/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Job.hpp"

namespace Async
{
	Job::Job(Reactor & reactor, WorkT work) : _notification(reactor), _work(work)
	{
	}
	
	Job::~Job()
	{
	}
	
	void Job::operator()() {
		try {
			_work();
		} catch (...) {
				_exception = std::current_exception();
		}
		
		_notification.signal();
	}
	
	void Job::wait()
	{
		_notification.wait();
		
		// Once we yield back to the caller, if there was an exception, we rethrow it.
		if (_exception) {
			std::rethrow_exception(_exception);
		}
	}
}
