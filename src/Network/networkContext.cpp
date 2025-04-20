
#include "networkContext.h"
#include "networkClass.h"

namespace Network {


	NetworkContext::NetworkContext(ConnectionSystem system)
		: impl(std::make_unique<NetworkContextImpl>(system)) {
	}

	NetworkContext::~NetworkContext() {}  // Destructor needed for unique_ptr

}
