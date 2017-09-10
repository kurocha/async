//
//  Test.After.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Concurrent/Fiber.hpp>
#include <Async/After.hpp>

#include <unistd.h>

namespace Async
{
	using Concurrent::Fiber;
	
	UnitTest::Suite AfterTestSuite {
		"Async::After",
		
		{"it can wait for duration",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				std::string order;
				
				Fiber fiber([&](){
					order += 'B';
					After event(0.1, reactor);
					event.wait();
					order += 'D';
					event.wait();
					order += 'F';
				});
				
				order += 'A';
				fiber.resume();
				order += 'C';
				reactor.update(1);
				order += 'E';
				reactor.update(1);
				order += 'G';
				
				examiner.expect(order) == "ABCDEFG";
			}
		},
		
		{"it can stop after a timeout",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				unsigned count = 0;

				Fiber top("top", [&](){
					Fiber work("work", [&](){
						After event(0.1, reactor);

						while (true) {
							count += 1;
							event.wait();
						}
					});

					Fiber timeout("timeout", [&](){
						After event(0.25, reactor);

						event.wait();

						work.stop();
					});

					// Schedule the timeout:
					timeout.resume();
					work.resume();

					Fiber::current->yield();
				});

				top.resume();

				reactor.wait(1.0);
				
				examiner.expect(count) == 3;
			}
		}
	};
}
