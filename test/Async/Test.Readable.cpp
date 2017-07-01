//
//  Test.Readable.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Concurrent/Fiber.hpp>
#include <Async/Readable.hpp>
#include <Async/Pipe.hpp>

#include <unistd.h>

namespace Async
{
	using Concurrent::Fiber;
	
	UnitTest::Suite ReadableTestSuite {
		"Async::Readable",
		
		{"it can wait for reading",
			[](UnitTest::Examiner & examiner) {
				std::string order;
				
				Reactor reactor;
				std::vector<char> buffer(12);
				
				auto pipe = Pipe();
				
				order += 'A';
				
				Fiber reader([&](){
					Readable event(pipe.input, reactor);
					
					order += 'B';
					while (::read(pipe.input, buffer.data(), buffer.size()) == -1 && errno == EWOULDBLOCK) {
						order += 'C';
						event.wait();
					}
					order += 'F';
				});
				
				reader.resume();
				
				Fiber writer([&](){
					order += 'D';
					::write(pipe.output, "Hello World!", 12);
					order += 'E';
				});
				
				writer.resume();
				
				reactor.wait(1);
				order += 'G';
				
				examiner.expect(std::string(buffer.data(), buffer.size())) == "Hello World!";
				examiner.expect(order) == "ABCDEFG";
			}
		},
	};
}
