//
//  Job.hpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 10/9/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Notification.hpp"

#include <functional>

namespace Async
{
	// A job which is scheduled on a work queue (e.g. Concurrent::Distributor).
	class Job
	{
	public:
		using WorkT = std::function<void()>;
		
		Job(Reactor & reactor, WorkT work);
		~Job();
		
		void operator()();
		void wait();
		
		using Reference = std::reference_wrapper<Job>;
		
	private:
		Notification _notification;
		std::function<void()> _work;
		
		std::exception_ptr _exception;
	};
}
