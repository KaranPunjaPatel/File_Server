#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include <ios>
#include <cstring>


enum struct Type : uint16_t {
	ServerIdentification,
	ConnectionRequest,   // Request to connect to the server
	ConnectionResponse,  // Server's response to a connection request
	StateUpdate,         // Game state update
	FilePacket,          // Chat between players
	KeepAlive,           // Keep-alive signal
	Disconnect,          // Notify disconnection 
	KeyExchange,		 // Exchanging Keys
	Authentication,		 // Authentication
	PublicKeyRequest,	 // Requesting Public Key 
	PublicKeyResponse,
	NewClient,
	NewSocket
};

enum struct Action : uint8_t {
	ID,
	KEY,
	SALT,
	PASSWORD,
	SESSION_TOKEN
};

typedef uint16_t Length;

struct Segment {
	Action action;
	Length length;
	std::vector<uint8_t> data;

	Segment() = default;

	template<typename DataType>
	Segment(Action action, const DataType& data) : action(action) {
		(*this) << data;  	
		length = static_cast<Length>(this->data.size());
	}

	Segment(Action action, const std::vector<uint8_t>& rawData) : action(action), data(rawData) {
    length = static_cast<Length>(rawData.size());
	}

	template<typename DataType>
	friend Segment& operator<<(Segment& msg, const DataType& data) {
		// Ensure the data is trivially copyable
		static_assert(std::is_trivially_copyable<DataType>::value, "Data must be trivially copyable");

		// Preserve existing data and append new one
		size_t start = msg.data.size();

		msg.data.resize(start + sizeof(DataType));

		// Copy data into the vector
		std::memcpy(msg.data.data() + start, &data, sizeof(DataType));

		// Update length
		msg.length = static_cast<Length>(msg.data.size());

		return msg;
	}
	
};

class SegmentBuilder {
private:
	Segment segment;

public:
	SegmentBuilder(Action action) {
		segment.action = action;
	}

	template<typename DataType>
	SegmentBuilder& withData(const DataType& data) {
		static_assert(std::is_trivially_copyable<DataType>::value, "Data must be trivially copyable");

		size_t start = segment.data.size();
		segment.data.resize(start + sizeof(DataType));
		std::memcpy(segment.data.data() + start, &data, sizeof(DataType));

		segment.length = static_cast<Length>(segment.data.size());
		return *this;
	}

	SegmentBuilder& withRawData(const std::vector<uint8_t>& rawData) {
		segment.data = rawData;
		segment.length = static_cast<Length>(rawData.size());
		return *this;
	}

	SegmentBuilder& withSalt(const std::vector<uint8_t>& rawData) {
		/*segment.data.assign(salt.begin(), salt.end());
		segment.length = static_cast<Length>(segment.data.size());
		return *this;*/
		return withRawData(rawData);
	}

	SegmentBuilder& withKey(const std::vector<uint8_t>& rawData) {
		return withRawData(rawData);
	}

	SegmentBuilder& withSession_Token(const std::vector<uint8_t>& rawData) {
		
		return withRawData(rawData);
	}

	SegmentBuilder& withId(const std::vector<uint8_t>& rawData) {
		return withRawData(rawData);
	}

	Segment build() {
		return segment;
	}
};



struct InternalPacket {
	uint64_t connectionId;		// Unique identifier for the connection
	uint64_t timestamp;			// Time received
	std::vector<uint8_t> data;	// The received packet (original format)
};

struct PacketHeader {
	uint16_t magic_bytes;
	uint16_t version;
	Type type;
	Length payload_len;
	uint8_t  reserved[8];

	PacketHeader() : magic_bytes(12345U), version(1), reserved{ 0 } {}

	friend std::ostream& operator << (std::ostream& os, const PacketHeader& msg)
	{
		os << "Magic bytes: " << msg.magic_bytes << " Version: " << msg.version << " ";
		os << "Packet Type: " << (uint16_t)msg.type << " Payload Size: " << msg.payload_len << "\n";
		return os;
	}

};

struct Packet {
	PacketHeader header;
	std::vector<uint8_t> payload;

	Packet() = default;

	friend Packet& operator << (Packet& msg, const Segment& data)
	{
		size_t i = msg.payload.size();

		// Resize the vector by the size of the data being pushed
		msg.payload.push_back(static_cast<uint8_t>(data.action));
		uint16_t length = data.length;
		uint8_t lengthBytes[2];
		std::memcpy(lengthBytes, &length, 2);

		msg.payload.push_back(lengthBytes[0]);
		msg.payload.push_back(lengthBytes[1]);

		msg.payload.insert(msg.payload.end(), data.data.begin(), data.data.end());

		msg.header.payload_len = static_cast<Length>(msg.payload.size());

		return msg;
	}

	friend std::ostream& operator << (std::ostream& os, const Packet& msg)
	{

		os << "Packet Header: \n";
		os << msg.header;
		/*for (auto el : msg.payload)
			os << el << " ";
		os << "\n";*/

		for (uint8_t byte : msg.payload)
			os << std::hex << std::setw(2) << std::setfill('0') << byte << " ";
		os << std::dec << "\n";  // Reset formatting


		return os;
	}
};

/*
	template<typename DataType>
	friend Packet& operator << (Packet& msg, const DataType& data)
	{
		// Check that the type of the data being pushed is trivially copyable
		static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

		// Cache current size of vector, as this will be the point we insert the data
		size_t i = msg.payload.size();

		// Resize the vector by the size of the data being pushed
		msg.payload.resize(msg.payload.size() + sizeof(DataType));

		// Physically copy the data into the newly allocated vector space
		std::memcpy(msg.payload.data() + i, &data, sizeof(DataType));

		// Recalculate the message size
		msg.header.payload_len = msg.payload.size();

		// Return the target message so it can be "chained"
		return msg;
	}
	*/

inline void CreateSegments(std::vector<Segment>& segments, std::vector<uint8_t> payload)
{
	int index{};
	while (index < payload.size()) {
		Segment seg;
		std::memcpy(&seg.action, &payload[index], sizeof(Action));
		index += sizeof(Action);

		std::memcpy(&seg.length, &payload[index], sizeof(Length));
		index += sizeof(Length);

		for (int i = 0; i < seg.length; i++, index++)
		{
			seg.data.push_back(payload[index]);
		}

		segments.push_back(seg);
	}
}

