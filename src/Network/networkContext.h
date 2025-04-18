
#pragma once
#include "./../Common/util.h"

#include <memory>

namespace Network {

	class NetworkContextImpl;

	class NetworkContext {
	public:
		explicit NetworkContext(ConnectionSystem system);
		~NetworkContext();

		NetworkContext(const NetworkContext&) = delete;
		NetworkContext& operator=(const NetworkContext&) = delete;

		std::unique_ptr<NetworkContextImpl> impl; 
	};

	

}