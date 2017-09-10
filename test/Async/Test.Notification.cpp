//
//  Test.Notification.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 13/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Async/Notification.hpp>

#include <Concurrent/Fiber.hpp>
#include <Async/Reactor.hpp>
#include <Time/Interval.hpp>

#include <thread>

namespace Async
{
	UnitTest::Suite NotificationTestSuite {
		"Async::Notification",
		
		{"it can notify the fiber to continue",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				std::string order;
				
				Concurrent::Fiber worker([&]{
					Notification notification(reactor);
					
					order += 'A';
					
					std::thread busy([&]{
						Time::Interval interval(0.1);
						
						interval.sleep();
						order += 'C';
						
						notification.signal();
					});
					
					order += 'B';
					notification.wait();
					busy.join();
					order += 'D';
				});
				
				worker.resume();
				
				while (worker)
					reactor.update(1.0);
				
				examiner.expect(order) == "ABCD";
			}
		},
	};
}
