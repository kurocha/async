//
//  Protocol.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 3/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Handle.hpp"
#include <string>

namespace Async
{
	class Protocol
	{
	public:
		typedef unsigned char Byte;
		
		Protocol(Descriptor descriptor, Reactor & reactor);
		virtual ~Protocol();
		
	protected:
		Descriptor _descriptor;
		Reactor & _reactor;
	};
	
	class StreamProtocol : public Protocol
	{
	public:
		StreamProtocol(Descriptor descriptor, Reactor & reactor);
		virtual ~StreamProtocol();
		
		Byte * read(Byte * begin, const Byte * end, bool partial = true);
		std::string read(std::size_t size, bool partial = true);
		
		void write(const Byte * begin, const Byte * end);
		void write(const std::string & buffer);
		
	protected:
		enum class Status {
			OK, WAITING, INTERRUPTED
		};
		
		Status read_partial(Byte *& begin, const Byte * end);
		Status write_partial(const Byte *& begin, const Byte * end);
	};
	
	// class DatagramProtocol : public Protocol
	// {
	// public:
	// 	using Protocol::Protocol;
	// 	using Protocol::~Protocol;
	// 	
	// 	void receive_message(Byte * begin, Byte * end);
	// 	void send_message(const Byte * begin, const Byte * end);
	// };
}
