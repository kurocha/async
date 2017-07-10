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
	
	StreamProtocol::StreamProtocol(Descriptor descriptor, Reactor & reactor) : Protocol(descriptor, reactor)
	{
	}
	
	StreamProtocol::~StreamProtocol()
	{
	}
	
	StreamProtocol::Status StreamProtocol::read_partial(Byte *& begin, const Byte * end)
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
	
	StreamProtocol::Status StreamProtocol::write_partial(const Byte *& begin, const Byte * end)
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
	
	Protocol::Byte * StreamProtocol::read(Byte * begin, const Byte * end, bool partial)
	{
		Readable event(_descriptor, _reactor);
		
		auto mark = begin;
		
		while (mark != end) {
			auto result = read_partial(mark, end);
			
			if (result != Status::OK) {
				event.wait();
			} else if (partial || mark == end) {
				break;
			}
		}
		
		return mark;
	}
	
	std::string StreamProtocol::read(std::size_t size, bool partial)
	{
		Byte buffer[size];
		
		auto mark = read(buffer, buffer+size, partial);
		
		return std::string(buffer, mark);
	}
	
	void StreamProtocol::write(const Byte * begin, const Byte * end)
	{
		Writable event(_descriptor, _reactor);
		
		auto mark = begin;
		
		while (mark != end) {
			auto result = write_partial(mark, end);
			
			if (result != Status::OK) {
				event.wait();
			}
		}
	}
	
	void StreamProtocol::write(const std::string & buffer)
	{
		auto begin = reinterpret_cast<const Byte *>(buffer.data());
		auto end = reinterpret_cast<const Byte *>(buffer.data() + buffer.size());
		
		write(begin, end);
	}
}
