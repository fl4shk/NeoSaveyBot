// This file is part of SaveyBot.
// 
// Copyright 2017-2018 Andrew Clark (FL4SHK).
// 
// SaveyBot is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
// 
// SaveyBot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with SaveyBot.  If not, see <http://www.gnu.org/licenses/>.


#ifndef irc_communicator_class_hpp
#define irc_communicator_class_hpp


//#include "json_stuff.hpp"
#include "misc_includes.hpp"
#include "communicator_class.hpp"
#include "string_stuff.hpp"

//#include "select_stuff.hpp"

namespace saveybot
{

class RealMain;
class SaveyBot;

// This should be moved to some IRC-related thing
class IrcConfiguration
{
public:		// classes
	class Server
	{
	private:		// variables
		std::string __name,
			__bot_name,
			__address,
			__port_str,
			__password;

		std::vector<std::string> __joins_list,
			__startup_commands;

	public:		// functions
		inline Server()
		{
		}

		Server(const Server& to_copy) = default;
		Server(Server&& to_move) = default;

		inline ~Server()
		{
		}

		Server& operator = (const Server& to_copy) = default;
		Server& operator = (Server&& to_move) = default;


		gen_getter_and_setter_by_con_ref(name);
		gen_getter_and_setter_by_con_ref(bot_name);
		gen_getter_and_setter_by_con_ref(address);
		gen_getter_and_setter_by_con_ref(port_str);
		gen_getter_and_setter_by_con_ref(password);
		gen_getter_and_setter_by_con_ref(joins_list);
		gen_getter_and_setter_by_con_ref(startup_commands);

		gen_getter_by_ref(joins_list);
		gen_getter_by_ref(startup_commands);

		gen_setter_by_rval_ref(name);
		gen_setter_by_rval_ref(bot_name);
		gen_setter_by_rval_ref(address);
		gen_setter_by_rval_ref(port_str);
		gen_setter_by_rval_ref(password);
		gen_setter_by_rval_ref(joins_list);
		gen_setter_by_rval_ref(startup_commands);


	};


private:		// variables
	std::vector<Server> __server_vec;

public:		// functions
	IrcConfiguration();

	IrcConfiguration(const IrcConfiguration& to_copy) = default;
	IrcConfiguration(IrcConfiguration&& to_move) = default;


	IrcConfiguration& operator = (const IrcConfiguration& to_copy) 
		= default;
	IrcConfiguration& operator = (IrcConfiguration&& to_move) = default;

	gen_getter_by_con_ref(server_vec);
	
};


std::ostream& operator << (std::ostream& os, 
	const IrcConfiguration::Server& to_print);


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


class IrcCommunicator : public Communicator
{
friend class RealMain;
friend void do_select_for_read
	(const std::vector<IrcCommunicator*>& comm_vec, fd_set* readfds);

public:		// static variables
	static const std::string config_file_name,
		msg_suffix, ping_suffix;

private:		// variables
	// Allow IPv4 or IPv6
	static constexpr auto __specific_family = AF_UNSPEC;

	// SOCK_STREAM - Provides sequenced, reliable, bidirectional,
	// connection- mode byte streams, and may provide a transmission
	static constexpr auto __specific_socktype = SOCK_STREAM;

	static constexpr size_t raw_buf_size = 9001;

	// "Whois" stuff (getaddrinfo())
	addrinfo __hints, * __res = nullptr;

	// Socket file descriptor
	int __sock_fd;

	// Stuff for clean_up()
	bool __did_alloc_res = false, __did_open_sock_fd = false;


	
	SaveyBot* __bot_ptr;
	
	// This is a pointer to a constant IrcConfiguration::Server.
	const IrcConfiguration::Server* __config_server_ptr;

	// The array of characters read() sends its data to
	std::array<char, raw_buf_size> raw_buf;

	struct
	{
		bool did_joins = false;
		bool did_ping = false;
		bool wants_select = true;
	} __state;

	std::string __line = "", buf_str = "";

	//size_t last_index = 0;


public:		// functions
	//IrcCommunicator(const std::string& some_server_name, 
	//	const std::string& some_port_str, const std::string& nick_command,
	//	const std::string& user_command, 
	//	const std::vector<std::string>& joins_list);
	IrcCommunicator(SaveyBot* s_bot_ptr, 
		const IrcConfiguration::Server* s_config_server_ptr);

	IrcCommunicator(const IrcCommunicator& to_copy) = default;
	IrcCommunicator(IrcCommunicator&& to_move) = default;

	virtual inline ~IrcCommunicator()
	{
		clean_up();
	}


	IrcCommunicator& operator = (const IrcCommunicator& to_copy) = default;
	IrcCommunicator& operator = (IrcCommunicator&& to_move) = default;


	gen_getter_by_val(specific_family);
	gen_getter_by_val(specific_socktype);

	gen_getter_by_con_ref(hints);
	gen_getter_by_con_ref(res);
	gen_getter_by_val(sock_fd);

	gen_getter_by_val(did_alloc_res);
	gen_getter_by_val(did_open_sock_fd);

	inline const IrcConfiguration::Server& config_server() const
	{
		return *__config_server_ptr;
	}

	gen_getter_by_con_ref(line);


	void iterate(fd_set* readfds);



protected:		// functions
	virtual void inner_send_regular_msg(std::string&& full_msg);

	void __reinit();
	inline void clean_up_then_reinit()
	{
		clean_up();
		__reinit();
	}
	


private:		// functions
	gen_getter_by_ref(hints);
	gen_getter_by_ref(res);
	gen_setter_by_val(sock_fd);
	
	gen_setter_by_val(did_alloc_res);
	gen_setter_by_val(did_open_sock_fd);

	inline SaveyBot& bot() const
	{
		return *__bot_ptr;
	}


	void do_full_read();
	bool do_select_and_also_full_read();
	bool do_full_read_if_fd_isset(fd_set* readfds);
	void update_line();

	void inner_send_raw_msg(std::string&& full_msg) const;

	template<typename FirstType, typename... RemArgTypes>
	inline void send_raw_msg(const FirstType& first_val, 
		RemArgTypes&&...  rem_args)
	{
		std::string temp = sconcat(first_val, rem_args...);
		printerr("Sending to server \"", config_server().name(), "\":  ",
			temp, "\n");

		inner_send_raw_msg(std::move(temp));
	}


	void do_getaddrinfo(const std::string& some_server_name, 
		const std::string& some_port_str);
	void do_socket_and_connect();


	void clean_up();

	inline void free_res()
	{
		if (did_alloc_res())
		{
			set_did_alloc_res(false);
			freeaddrinfo(res());
		}
	}
	inline void close_sock_fd()
	{
		if (did_open_sock_fd())
		{
			set_did_open_sock_fd(false);
			close(sock_fd());
		}
	}

	inline bool __attempt_do_joins()
	{
		if (!__state.did_joins)
		{
			sleep(1);

			__state.did_joins = true;

			for (auto iter : config_server().joins_list())
			{
				send_raw_msg("JOIN ", iter);
			}

			return true;
		}
		return false;
	}

	//inline void __handle_ping(size_t& i, bool do_attempt_joins=true)
	inline void __handle_ping(size_t& i)
	{
		std::string second_substr;
		next_non_blank_substr(line(), i, second_substr, i);
		//printout("PING Debug:  ", second_substr, "\n");
		send_raw_msg("PONG ", second_substr);

		//if (do_attempt_joins)
		//{
		//	__attempt_do_joins();
		//}
	}

	inline bool __handle_pong(size_t i)
	{
		std::string second_substr, third_substr, fourth_substr;
		next_non_blank_substr(line(), i, second_substr, i);
		//next_non_blank_substr(line(), i, third_substr, i);
		//next_non_blank_substr(line(), i, fourth_substr, i);

		if (second_substr == "PONG")
		{
			if (__state.did_ping)
			{
				next_non_blank_substr(line(), i, third_substr, i);
				next_non_blank_substr(line(), i, fourth_substr, i);
				if (fourth_substr.find(ping_suffix) == 0)
				{
					__state.did_ping = false;
					return true;
				}
				else
				{
					err("PONG Eek 0!");
				}
			}
			else
			{
				err("PONG Eek 1!");
			}
		}
		return false;
	}

	inline bool __substr_is_config_server_address
		(const std::string& substr) const
	{
		return (substr == sconcat(":", config_server().address()));
	}

	inline bool __can_ignore(const std::string& first_substr, 
		size_t& i)
	{
		std::string second_substr;

		if (first_substr == "NOTICE")
		{
			next_non_blank_substr(line(), i, second_substr, i);

			if (second_substr == "AUTH")
			{
				return true;
			}
		}


		if (__substr_is_config_server_address(first_substr))
		{
			//printout("__can_ignore():  ", line(), "\n");
			next_non_blank_substr(line(), i, second_substr, i);
			bool only_found_digits = true;
			for (auto c : second_substr)
			{
				if (!isdigit(c))
				{
					only_found_digits = false;
					break;
				}
			}

			if (only_found_digits)
			{
				//std::string substr_a, substr_b, substr_c, substr_d;
				return true;
			}

			//printout("second_substr:  ", second_substr, "\n");

			if (second_substr == "NOTICE")
			{
				std::string third_substr;
				next_non_blank_substr(line(), i, third_substr, i);

				//printout("third_substr:  ", third_substr, "\n");

				if (third_substr == "AUTH")
				{
					//printout("returning true.\n");
					return true;
				}
			}
		}

		//{
		//next_non_blank_substr(line(), i, second_substr, i);

		//if (second_substr == "JOIN")
		//{
		//	return true;
		//}
		//}

		return false;
	}

	int __handle_ctcp_version(const std::string& first_substr, 
		std::string& second_substr, std::string& third_substr, 
		std::string& other_substr, size_t* i, size_t& exclam_index, 
		size_t& space_index, std::string& user_nick);

	void __initial_ignoring();

	void check_timeout_with_ping();

};



}


#endif		// irc_communicator_class_hpp
