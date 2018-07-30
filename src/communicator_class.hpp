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


#ifndef src__slash__communicator_class_hpp
#define src__slash__communicator_class_hpp

// src/communicator_class.hpp


//#include "json_stuff.hpp"
#include "misc_includes.hpp"

namespace saveybot
{

class SaveyBot;

// Here is the base class for the various protocols, including outputting
// to the command line.
class Communicator
{

public:		// static variables
	static const std::string config_file_name;

private:		// variables
	// This is controlled by a SaveyBot instance, but it isn't relevant for
	// DiscordCommunicator.
	std::string __channel;

protected:		// variables
	SaveyBot* __bot_ptr;

protected:		// functions
	// In IRC's case, send a PRIVMSG 
	virtual void inner_send_regular_msg(std::string&& full_msg) = 0;

	SaveyBot& bot() const;

public:		// functions
	Communicator(SaveyBot* s_bot_ptr);
	virtual inline ~Communicator()
	{
	}

	template<typename FirstType, typename... RemArgTypes>
	void send_regular_msg(const FirstType& first_val, 
		const RemArgTypes&... rem_args)
	{
		inner_send_regular_msg(std::move(sconcat(first_val, rem_args...)));
	}

	template<typename FirstType, typename... RemArgTypes>
	inline void send_saveybot_msg(const FirstType& first_val,
		const RemArgTypes&... rem_args)
	{
		send_regular_msg("~ ", first_val, rem_args...);
	}

	gen_setter_by_rval_ref(channel);
	gen_getter_and_setter_by_con_ref(channel);
	
};

} // namespace saveybot




#endif		// src__slash__communicator_class_hpp
