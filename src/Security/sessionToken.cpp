#include "sessionToken.h"

#include <openssl/rand.h>

#include <ctime>
#include <stdexcept>
#include <cstring>

namespace Security {

    SessionToken SessionToken::Generate(uint64_t userId, uint64_t validitySeconds) {
        SessionToken token;

        // Generate 32 bytes of cryptographically secure random data
        if (RAND_bytes(token.randomPart.data(), (int)token.randomPart.size()) != 1) {
            throw std::runtime_error("Failed to generate random session token");
        }

        // Set expiration timestamp (current time + validity duration)
        token.expiration = static_cast<uint64_t>(std::time(nullptr)) + validitySeconds;

        // Store user ID
        token.userId = userId;

        return token;
    }

    bool SessionToken::IsValid() const {
        return std::time(nullptr) <= expiration;
    }

    bool SessionToken::operator==(const SessionToken& other) const {
        return randomPart == other.randomPart && expiration == other.expiration && userId == other.userId;
    }

    std::vector<uint8_t> SessionToken::Serialize() const {
        std::vector<uint8_t> buffer(sizeof(SessionToken));
        std::memcpy(buffer.data(), this, sizeof(SessionToken));
        return buffer;
    }

    SessionToken SessionToken::Deserialize(const std::vector<uint8_t>& data) {
        if (data.size() != sizeof(SessionToken)) {
            throw std::runtime_error("Invalid session token size");
        }

        SessionToken token;
        std::memcpy(&token, data.data(), sizeof(SessionToken));
        return token;
    }

} // namespace Security

// TODO: Need to check this
namespace std {
    size_t hash<Security::SessionToken>::operator()(const Security::SessionToken& token) const {
        size_t hashValue = 0;
        for (auto byte : token.randomPart) {
            hashValue ^= static_cast<size_t>(byte) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
        }
        return hashValue ^ std::hash<uint64_t>{}(token.expiration) ^ std::hash<uint64_t>{}(token.userId);
    }
}
