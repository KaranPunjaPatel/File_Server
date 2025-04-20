#pragma once

#include <array>
#include <cstdint>
#include <memory>

namespace Security {

	class SecurityRSAKey;

	void InitializeSecurity();

	void CleanupSecurity();

	void GenerateRSAKeyPair(std::shared_ptr<SecurityRSAKey>& publicKey, std::shared_ptr<SecurityRSAKey>& privateKey);

	void GenerateSalt(std::array<uint8_t, 32>& salt);

}