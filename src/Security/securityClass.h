#pragma once

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

#include <memory>
#include <vector>
#include <iostream>
#include <cstring>

#include "./../Common/util.h"

namespace Security {


	class SecurityRSAKey {
	private:

		std::shared_ptr<EVP_PKEY> key;
		KeyType type;

	public:
		SecurityRSAKey() : type(KeyType::PUBLIC) {}

		SecurityRSAKey(std::shared_ptr<EVP_PKEY> key, KeyType type) : key(key), type(type) {}

		~SecurityRSAKey(){}
		
		bool MatchParameters(EVP_PKEY* pair)
		{
			if (!key || !pair) {
				std::cerr << "Error: Null key in MatchParameters\n";
				return false;
			}
			return EVP_PKEY_copy_parameters(key.get(), pair) == 1;
		}

		int GetKeySize() 
		{
			if (!key) {
				std::cerr << "Error: Key is null in GetKeySize\n";
				return -1;
			}

			int size = 0;
			if (type == KeyType::PUBLIC) {
				size = i2d_PUBKEY(key.get(), nullptr);
			}
			else if (type == KeyType::PRIVATE) {
				size = i2d_PrivateKey(key.get(), nullptr);
			}

			if (size <= 0) {
				std::cerr << "Error: Failed to get key size\n";
				return -1;
			}
			return size;
		}

		std::vector<uint8_t> SerializeKey() 
		{

			if (!key || type != KeyType::PUBLIC) {
				std::cerr << "Error: Key is null or not public in SerializeKey\n";
				return {};
			}

			int keySize = GetKeySize();
			if (keySize <= 0) {
				std::cerr << "Failed to get key size\n";
				return {};
			}

			std::vector<uint8_t> buffer(keySize);
			uint8_t* p = buffer.data();

			if (i2d_PUBKEY(key.get(), &p) <= 0) {
				std::cerr << "Failed to serialize public key\n";
				return {};
			}

			return buffer;
		}

		void DeserializeKey(const std::vector<uint8_t>& buffer) 
		{

			if (buffer.empty()) {
				std::cerr << "Error: Buffer is empty in DeserializeKey\n";
				return;
			}

			const uint8_t* p = buffer.data();
			EVP_PKEY* rawKey = d2i_PUBKEY(nullptr, &p, (long)buffer.size());

			if (!rawKey) {
				std::cerr << "Error: Failed to deserialize public key\n";
				ERR_print_errors_fp(stderr); // Print OpenSSL errors
				return;
			}

			// Store it safely in a shared_ptr with a proper deleter
			key = std::shared_ptr<EVP_PKEY>(rawKey, EVP_PKEY_free);
			type = KeyType::PUBLIC;
		}

		void Print()
		{

			if (!key) {
				std::cerr << "Error: EVP_PKEY is null\n";
				return;
			}

			BIO* bio = BIO_new(BIO_s_mem());
			if (!bio) {
				std::cerr << "Error: Failed to allocate BIO\n";
				return;
			}

			if (type == KeyType::PRIVATE)
			{
				if (!PEM_write_bio_PrivateKey(bio, key.get(), nullptr, nullptr, 0, nullptr, nullptr)) {
					std::cerr << "Error: Failed to write private key\n";
					BIO_free(bio);
					return;
				}
				std::cout << "Private Key:\n";
			}
			else if (type == KeyType::PUBLIC)
			{
				if (!PEM_write_bio_PUBKEY(bio, key.get())) {
					std::cerr << "Error: Failed to write public key\n";
					BIO_free(bio);
					return;
				}
				std::cout << "Public Key:\n";
			}

			char* buf = nullptr;
			size_t len = BIO_get_mem_data(bio, &buf);
			std::cout << std::string(buf, len) << std::endl;

			BIO_free(bio);
		}

		std::vector<uint8_t> EncryptBuffer(const std::vector<uint8_t>& plaintext)
		{
			if (!key || type == KeyType::PRIVATE) {
				std::cerr << "Error: Invalid key type for encryption (must be public)\n";
				return {};
			}

			EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key.get(), nullptr);
			if (!ctx) {
				std::cerr << "Error: Failed to create encryption context\n";
				return {};
			}

			if (EVP_PKEY_encrypt_init(ctx) <= 0) {
				std::cerr << "Error: Encryption initialization failed\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			// Use OAEP padding (recommended)
			if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
				std::cerr << "Error: Failed to set RSA OAEP padding\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			// Determine the output size
			size_t outLen = 0;
			if (EVP_PKEY_encrypt(ctx, nullptr, &outLen, plaintext.data(), plaintext.size()) <= 0) {
				std::cerr << "Error: Failed to determine encrypted length\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			std::vector<uint8_t> ciphertext(outLen);
			if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &outLen, plaintext.data(), plaintext.size()) <= 0) {
				std::cerr << "Error: Encryption failed\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			EVP_PKEY_CTX_free(ctx);
			return ciphertext;
		}

		std::vector<uint8_t> DecryptBuffer(const std::vector<uint8_t>& ciphertext)
		{
			if (!key || type != KeyType::PRIVATE) {
				std::cerr << "Error: Invalid key type for decryption (must be private)\n";
				return {};
			}

			EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(key.get(), nullptr);
			if (!ctx) {
				std::cerr << "Error: Failed to create decryption context\n";
				return {};
			}

			if (EVP_PKEY_decrypt_init(ctx) <= 0) {
				std::cerr << "Error: Decryption initialization failed\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			// Use OAEP padding
			if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
				std::cerr << "Error: Failed to set RSA OAEP padding\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			// Determine output size
			size_t outLen = 0;
			if (EVP_PKEY_decrypt(ctx, nullptr, &outLen, ciphertext.data(), ciphertext.size()) <= 0) {
				std::cerr << "Error: Failed to determine decrypted length\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			std::vector<uint8_t> plaintext(outLen);
			if (EVP_PKEY_decrypt(ctx, plaintext.data(), &outLen, ciphertext.data(), ciphertext.size()) <= 0) {
				std::cerr << "Error: Decryption failed\n";
				EVP_PKEY_CTX_free(ctx);
				return {};
			}

			EVP_PKEY_CTX_free(ctx);
			return plaintext;
		}

		template <typename T>
		std::vector<uint8_t> Encrypt(const T& data)
		{
			std::vector<uint8_t> buffer = Serialize(data);
			return EncryptBuffer(buffer);
		}

		// Overload for std::string
		std::vector<uint8_t> Serialize(const std::string& str)
		{
			return std::vector<uint8_t>(str.begin(), str.end());
		}

		// Overload for std::array<uint8_t, N>
		std::vector<uint8_t> Serialize(const std::vector<uint8_t>& vec)
		{
			return vec;
		}

		template <typename T>
		std::vector<uint8_t> Serialize(const T& value)
		{
			static_assert(std::is_trivially_copyable_v<T>, "Serialize only supports trivially copyable types.");

			std::vector<uint8_t> buffer(sizeof(T));
			std::memcpy(buffer.data(), &value, sizeof(T));
			return buffer;
		}

		// Overload for std::array<uint8_t, N>
		template <size_t N>
		std::vector<uint8_t> Serialize(const std::array<uint8_t, N>& arr)
		{
			return std::vector<uint8_t>(arr.begin(), arr.end());
		}

	};

}