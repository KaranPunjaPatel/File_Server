

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

#include <iostream>
#include <vector>

#include "asymmetricKey.h"
#include "securityClass.h"

namespace Security {

	// Global OpenSSL library context (for explicit fetching)
	static OSSL_LIB_CTX* libctx = nullptr;

	// Global RSA key generation context
	static EVP_PKEY_CTX* rsaCtx = nullptr;

	void InitializeSecurity() 
	{
		if (!libctx) {
			libctx = OSSL_LIB_CTX_new();  // Create OpenSSL library context
			if (!libctx) {
				std::cerr << "Error: Failed to create OpenSSL library context\n";
				return;
			}
		}

		if (!rsaCtx) {
			rsaCtx = EVP_PKEY_CTX_new_from_name(libctx, "RSA", NULL);
			if (!rsaCtx) {
				std::cerr << "Error: Failed to create RSA key context\n";
				return;
			}

			if (EVP_PKEY_keygen_init(rsaCtx) <= 0) {
				std::cerr << "Error initializing key generation: "
					<< ERR_error_string(ERR_get_error(), nullptr) << std::endl;
				return;
			}

			// Set default RSA key size to 1024 bits
			if (EVP_PKEY_CTX_set_rsa_keygen_bits(rsaCtx, 1024) <= 0) {
				std::cerr << "Error setting RSA key size\n";
				return;
			}
		}
	}

	// Function to clean up OpenSSL context once at the end of the program
	void CleanupSecurity() 
	{
		if (rsaCtx) {
			EVP_PKEY_CTX_free(rsaCtx);
			rsaCtx = nullptr;
		}
		if (libctx) {
			OSSL_LIB_CTX_free(libctx);
			libctx = nullptr;
		}
	}

	void GenerateRSAKeyPair(std::shared_ptr<SecurityRSAKey>& publicKey, std::shared_ptr<SecurityRSAKey>& privateKey) 
	{

		InitializeSecurity();

		if (!libctx || !rsaCtx) {
			std::cerr << "Error: RSA key generation context is not initialized\n";
			return;
		}

		EVP_PKEY* key = nullptr;
		if (EVP_PKEY_generate(rsaCtx, &key) <= 0) {
			std::cerr << "Error generating RSA key\n";
			return;
		}

		auto privateKeyPtr = std::shared_ptr<EVP_PKEY>(key, EVP_PKEY_free);
		privateKey = std::make_shared<SecurityRSAKey>(privateKeyPtr, KeyType::PRIVATE);

		auto publicKeyPtr = std::shared_ptr<EVP_PKEY>(EVP_PKEY_dup(key), EVP_PKEY_free);
		publicKey = std::make_shared<SecurityRSAKey>(publicKeyPtr, KeyType::PUBLIC);

		if (!publicKey->MatchParameters(key)) {
			std::cerr << "Error extracting public key\n";
		}

		//publicKey->Print();
		//privateKey->Print();


	}


	void GenerateSalt(std::array<uint8_t, 32>& salt) {
		if (RAND_bytes(salt.data(), (int)salt.size()) != 1) {
			std::cerr << "Error generating random salt\n";
			throw std::runtime_error("Failed to generate salt");
		}
	}

}