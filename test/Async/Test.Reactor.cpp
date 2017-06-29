//
//  Test.Reactor.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 30/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Concurrent/Fiber.hpp>
#include <Async/Reactor.hpp>

#include <Async/After.hpp>
#include <Async/Readable.hpp>
#include <Async/Writable.hpp>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

namespace Async
{
	using Concurrent::Fiber;
	
	static void set_non_blocking(FileDescriptor file_descriptor, bool value = true)
	{
		int flags = fcntl(file_descriptor, F_GETFL, 0);

		if (flags == -1)
			throw std::system_error(errno, std::generic_category(), "fcntl(..., F_GETFL, ...)");

		if (value)
			flags |= O_NONBLOCK;
		else
			flags &= ~O_NONBLOCK;

		if (fcntl(file_descriptor, F_SETFL, flags) == -1)
			throw std::system_error(errno, std::generic_category(), "fcntl(..., F_SETFL, ...)");
	}
	
	UnitTest::Suite ReactorTestSuite {
		"Async::Reactor",
		
		{"it can wait for a moment",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				std::string order;
				
				Fiber fiber([&](){
					order += 'B';
					After event(0.1, &reactor);
					event.wait();
					order += 'D';
					event.wait();
					order += 'F';
				});
				
				order += 'A';
				fiber.resume();
				order += 'C';
				reactor.wait(1);
				order += 'E';
				reactor.wait(1);
				order += 'G';
				
				examiner.expect(order) == "ABCDEFG";
			}
		},
		
		{"it can wait for input/output",
			[](UnitTest::Examiner & examiner) {
				Reactor reactor;
				std::vector<char> buffer(12);
				
				FileDescriptor ios[2];
				::pipe(ios);
				set_non_blocking(ios[0]);
				set_non_blocking(ios[1]);
				
				Fiber reader([&](){
					Readable event(ios[0], &reactor);
					
					while (::read(ios[0], buffer.data(), buffer.size()) == -1 && errno == EWOULDBLOCK) {
						event.wait();
					}
				});
				
				reader.resume();
				
				Fiber writer([&](){
					::write(ios[1], "Hello World!", 12);
				});
				
				writer.resume();
				
				reactor.wait(1);
				
				examiner.expect(std::string(buffer.data(), buffer.size())) == "Hello World!";
			}
		},

	};
}
