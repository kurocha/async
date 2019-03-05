//
//  Test.Job.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 10/9/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Parallel/Distributor.hpp>
#include <Concurrent/Fiber.hpp>
#include <Async/Job.hpp>

#include <iostream>

namespace Async
{
	using namespace UnitTest::Expectations;
	
	UnitTest::Suite JobTestSuite {
		"Async::Job",
		
		{"it can wait for result",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				Parallel::Distributor<Job::Reference> distributor;
				
				int result = 0;
				
				// There is still a issue that is not tested: if the fiber terminates before the job is complete. The job might have references to the fiber stack which no longer exists. When a job goes out of scope, it should always call wait in the destructor?
				Concurrent::Fiber worker([&]{
					// If this goes out of scope, the job is cancelled.
					Job job{reactor, [&](){
						result = 1337;
					}};
					
					distributor(job);
					
					job.wait();
				});
				
				worker.resume();
				
				while (worker)
					reactor.update(1.0);
				
				examiner.expect(result) == 1337;
			}
		},
		
		{"it can re-throw exceptions",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				Parallel::Distributor<Job::Reference> distributor;
				
				Concurrent::Fiber worker([&]{
					// If this goes out of scope, the job is cancelled.
					Job job{reactor, [&](){
						throw std::logic_error("I ain't doing no work");
					}};
					
					distributor(job);
					
					examiner.expect([&]{
						job.wait();
					}).to(throw_exception<std::logic_error>());
				});
				
				worker.resume();
				
				while (worker)
					reactor.update(1.0);
			}
		},
	};
}
