#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace Security {

	struct SessionToken
	{
		std::array<uint8_t, 32> randomPart;  // 32-byte random value
		uint64_t expiration;                 // 8-byte UNIX timestamp
		uint64_t userId;

		static SessionToken Generate(uint64_t userId, uint64_t validitySeconds);
		bool IsValid() const;
		bool operator==(const SessionToken& other) const;

		std::vector<uint8_t> Serialize() const;
		static SessionToken Deserialize(const std::vector<uint8_t>& data);
	};

}

namespace std {
	template<>
	struct hash<Security::SessionToken> {
		std::size_t operator()(const Security::SessionToken& token) const;
	};
}
