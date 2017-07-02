//
//  Protocol.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 3/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Protocol.hpp"

#include <unistd.h>
#include <errno.h>

#include <system_error>

#include "Readable.hpp"
#include "Writable.hpp"

namespace Async
{
	Protocol::Protocol(Descriptor descriptor, Reactor & reactor) : _descriptor(descriptor), _reactor(reactor)
	{
	}
	
	Protocol::~Protocol()
	{
	}
	
	StreamProtocol::~StreamProtocol()
	{
	}
	
	StreamProtocol::Status StreamProtocol::read(Byte *& begin, Byte * end)
	{
		auto result = ::read(_descriptor, begin, end - begin);
		
		if (result == -1) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) return Status::WAITING;
			if (errno == EINTR) return Status::INTERRUPTED;
			
			throw std::system_error(errno, std::generic_category(), "read");
		} else {
			begin += result;
			return Status::OK;
		}
	}
	
	StreamProtocol::Status StreamProtocol::write(const Byte *& begin, const Byte * end)
	{
		auto result = ::write(_descriptor, begin, end - begin);
		
		if (result == -1) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) return Status::WAITING;
			if (errno == EINTR) return Status::INTERRUPTED;
			
			throw std::system_error(errno, std::generic_category(), "write");
		} else {
			begin += result;
			return Status::OK;
		}
	}
	
	std::string StreamProtocol::read(std::size_t size, bool partial)
	{
		Readable event(_descriptor, _reactor);
		
		Byte buffer[size];
		Byte * mark = buffer;
		
		while (mark != buffer+size) {
			auto result = read(mark, buffer+size);
			
			if (result != Status::OK) {
				event.wait();
			} else if (partial || mark == buffer+size) {
				break;
			}
		}
		
		return std::string(buffer, mark);
	}
	
	void StreamProtocol::write(const std::string & buffer)
	{
		Writable event(_descriptor, _reactor);
		
		auto mark = reinterpret_cast<const Byte *>(buffer.data());
		auto end = reinterpret_cast<const Byte *>(buffer.data() + buffer.size());
		
		while (mark != end) {
			auto result = write(mark, end);
			
			if (result != Status::OK) {
				event.wait();
			}
		}
	}
}
