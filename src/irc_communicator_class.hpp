#ifndef irc_communicator_class_hpp
#define irc_communicator_class_hpp


#include "json_stuff.hpp"


//// Here is the addrinfo struct.
//struct addrinfo {
//
//	// This field specifies additional options, described below. Multiple
//	// flags are specified by bitwise OR-ing them together.
//	int              ai_flags;
//
//	// This field specifies the desired address family for the returned
//	// addresses. Valid values for this field include AF_INET and AF_INET6.
//	// The value AF_UNSPEC indicates that getaddrinfo() should return
//	// socket addresses for any address family (either IPv4 or IPv6, for
//	// example) that can be used with node and service.
//	int              ai_family;
//
//	// This field specifies the preferred socket type, for example
//	// SOCK_STREAM or SOCK_DGRAM. Specifying 0 in this field indicates that
//	// socket addresses of any type can be returned by getaddrinfo().
//	int              ai_socktype;
//
//
//	// This field specifies the protocol for the returned socket addresses.
//	// Specifying 0 in this field indicates that socket addresses with any
//	// protocol can be returned by getaddrinfo().
//	int              ai_protocol;
//
//	// All the other fields in the structure pointed to by hints must
//	// contain either 0 or a null pointer, as appropriate.
//	socklen_t        ai_addrlen;
//	struct sockaddr *ai_addr;
//	char            *ai_canonname;
//	struct addrinfo *ai_next;
//};


class IRCCommunicator
{
private:		// variables
	static constexpr auto specific_family = AF_UNSPEC;
	static constexpr auto specific_socktype = SOCK_STREAM;

	addrinfo hints, * res = nullptr;

	bool should_free_res = true;

public:		// functions
	IRCCommunicator(const std::string& some_server_name, 
		const std::string& some_port_str);
	inline ~IRCCommunicator()
	{
		clean_up();
	}


	void clean_up();

private:		// functions
	inline void free_res()
	{
		if (should_free_res)
		{
			should_free_res = false;
			freeaddrinfo(res);
		}
	}
};


#endif		// irc_communicator_class_hpp
