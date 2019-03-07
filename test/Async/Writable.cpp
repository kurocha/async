//
//  Writable.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Concurrent/Fiber.hpp>
#include <Async/Writable.hpp>
#include <Async/Pipe.hpp>

#include <unistd.h>

namespace Async
{
	using Concurrent::Fiber;
	
	UnitTest::Suite WritableTestSuite {
		"Async::Writable",
		
		{"it can wait for writing",
			[](UnitTest::Examiner & examiner) {
				std::string order;
				
				Reactor reactor;
				
				auto pipe = Pipe();
				
				Fiber writer([&](){
					Writable event(pipe.output, reactor);
					
					order += 'B';
					// Fill up pipe buffer:
					while (::write(pipe.output, "Hello World!", 12) != -1) {}
					order += 'C';
					
					// Wait for the buffer to be drained:
					if (errno == EWOULDBLOCK) {
						order += 'D';
						event.wait();
						order += 'F';
					}
				});
				
				order += 'A';
				writer.resume();
				
				Fiber reader([&](){
					order += 'E';
					// Be careful of blowing away stack, since the fiber stack can be small:
					unsigned char buffer[1024*4];
					
					// Drain the pipe buffer:
					while (::read(pipe.input, buffer, 1024*4) != -1) {}
				});
				
				reader.resume();
				
				while (reader || writer)
					reactor.update(1.0);
				
				order += 'G';
				
				examiner.expect(order) == "ABCDEFG";
			}
		},
	};
}
