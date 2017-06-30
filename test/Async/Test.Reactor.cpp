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
		
		{"it can wait for duration",
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
				reactor.update(1);
				order += 'E';
				reactor.update(1);
				order += 'G';
				
				examiner.expect(order) == "ABCDEFG";
			}
		},
		
		{"it can wait for reading",
			[](UnitTest::Examiner & examiner) {
				std::string order;
				
				Reactor reactor;
				std::vector<char> buffer(12);
				
				FileDescriptor ios[2];
				::pipe(ios);
				set_non_blocking(ios[0]);
				set_non_blocking(ios[1]);
				
				order += 'A';
				
				Fiber reader([&](){
					Readable event(ios[0], &reactor);
					
					order += 'B';
					while (::read(ios[0], buffer.data(), buffer.size()) == -1 && errno == EWOULDBLOCK) {
						order += 'C';
						event.wait();
					}
					order += 'F';
				});
				
				reader.resume();
				
				Fiber writer([&](){
					order += 'D';
					::write(ios[1], "Hello World!", 12);
					order += 'E';
				});
				
				writer.resume();
				
				reactor.wait(1);
				order += 'G';
				
				examiner.expect(std::string(buffer.data(), buffer.size())) == "Hello World!";
				examiner.expect(order) == "ABCDEFG";
			}
		},
		
		{"it can wait for writing",
			[](UnitTest::Examiner & examiner) {
				std::string order;
				
				Reactor reactor;
				
				FileDescriptor ios[2];
				::pipe(ios);
				set_non_blocking(ios[0]);
				set_non_blocking(ios[1]);
				
				Fiber writer([&](){
					Writable event(ios[1], &reactor);
					
					order += 'B';
					// Fill up pipe buffer:
					while (::write(ios[1], "Hello World!", 12) != -1) {}
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
					while (::read(ios[0], buffer, 1024*4) != -1) {}
				});
				
				reader.resume();
				reactor.wait(1);
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
						After event(0.1, &reactor);

						while (true) {
							count += 1;
							event.wait();
						}
					});

					Fiber timeout("timeout", [&](){
						After event(0.25, &reactor);

						event.wait();

						work.stop();
					});

					// Schedule the timeout:
					timeout.resume();
					work.resume();

					Fiber::current->yield();
				});

				top.resume();

				reactor.wait(2);
				
				examiner.expect(count) == 3;
			}
		}
	};
}
