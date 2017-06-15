// This file is part of NeoSaveyBot.
// 
// Copyright 2017 Andrew Clark (FL4SHK).
// 
// NeoSaveyBot is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
// 
// NeoSaveyBot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with NeoSaveyBot.  If not, see <http://www.gnu.org/licenses/>.


#include "neosaveybot_class.hpp"

namespace neosaveybot
{


// Constant keys
const std::string Database::key_datetime("datetime"),
	Database::key_classic_index("classic_index"),
	Database::key_message("message"),
	Database::key_name("name"),
	Database::key_slot("slot");

// Constant values
const std::string Database::value_datetime_classic("classic");

void Database::Value::extract_from_json(const Json::Value& to_copy)
{
	datetime() = to_copy[Database::key_datetime].asString();
	classic_index_str() = to_copy[Database::key_classic_index].asString();
	message() = to_copy[Database::key_message].asString();
	name() = to_copy[Database::key_name].asString();
	slot() = to_copy[Database::key_slot].asString();
}
void Database::Value::add_to_json(Json::Value& output_root) const
{
	output_root[slot()][Database::key_datetime] = datetime();

	s64 classic_index;
	std::stringstream sstm(classic_index_str());
	sstm >> classic_index;
	output_root[slot()][Database::key_classic_index] = classic_index;

	output_root[slot()][Database::key_message] = message();
	output_root[slot()][Database::key_name] = name();
	output_root[slot()][Database::key_slot] = slot();
}

void Database::save(const std::string& message, const std::string& name, 
	const std::string& slot, bool use_lowest)
{
	if (use_lowest)
	{
		std::string&& temp_slot = convert_bignum_to_str
			(lowest_available_slot());
		
		save(message, name, temp_slot, false);
		return;
	}

	std::string datetime;
	std::stringstream dt_sstm;
	osprintout(dt_sstm, liborangepower::time::put_now_as_localtime(),
		"\n");
	getline(dt_sstm, datetime);

	savestates()[slot] = std::move(Value(datetime, "-1", message, name,
		slot));
	
	// Just do this after EVERY save()
	update_lowest_available_slot();
	
	write_file();
	printout("Your savestate was sav'd to slot number ", slot, "!\n");
}

void Database::write_file() const
{
	Json::Value output_root;

	for (const auto& iter : savestates())
	{
		iter.second.add_to_json(output_root);
	}

	//std::fstream out_file(std::string("temp_") + database_file_name(), 
	//	std::ios_base::out);
	std::fstream out_file(database_file_name(), std::ios_base::out);
	write_json(out_file, &output_root);
}

bool Database::slot_owned_by(const BigNum& slot_bignum, 
	const std::string& name)
{
	return slot_owned_by(std::move(convert_bignum_to_str(slot_bignum)),
		name);
}

void Database::load_from_file()
{
	Json::Value input_root;
	std::string errs;
	
	parse_json(database_file_name(), &input_root, &errs);

	for (auto& in_iter : input_root)
	{
		Value to_insert(in_iter);
		savestates()[to_insert.slot()] = std::move(to_insert);
	}

	set_lowest_available_slot(0);
	update_lowest_available_slot();
}

void Database::update_lowest_available_slot()
{
	printout("I started with this lowest_available_slot:  ",
		lowest_available_slot(), "\n");

	for ( ; ; ++__lowest_available_slot)
	{
		const std::string las_str 
			= std::move(convert_bignum_to_str(lowest_available_slot()));

		const auto search = savestates().find(las_str);

		if (search == savestates().end())
		{
			break;
		}
	}

	printout("I ended with this lowest available slot:  ",
		lowest_available_slot(), "\n\n");
}

const std::string NeoSaveyBot::config_file_name("config.json"),
	NeoSaveyBot::database_file_name("neosaveybot.json");


NeoSaveyBot::Configuration::Configuration()
{
	Json::Value config_root;
	std::string errs;

	parse_json(config_file_name, &config_root, &errs);

	const auto& member_names = config_root.getMemberNames();

	for (const auto& name_iter : member_names)
	{
		Server to_push;

		to_push.name = name_iter;
		const auto& iter = config_root[name_iter];
		to_push.bot_name = iter["bot_name"].asString();
		to_push.address = iter["address"].asString();
		to_push.port_str = iter["port"].asString();
		to_push.password = iter["password"].asString();

		for (const auto& join_iter : iter["joins_list"])
		{
			to_push.joins_list.push_back(join_iter.asString());
		}

		for (const auto& command_iter : iter["startup_commands"])
		{
			std::string full_command;

			for (const auto& inner_iter : command_iter)
			{
				full_command += inner_iter.asString();
			}

			to_push.startup_commands.push_back(std::move(full_command));
		}

		server_vec.push_back(to_push);
	}

	for (const auto& iter : server_vec)
	{
		printout(iter);
	}
	printout("\n");

}


NeoSaveyBot::NeoSaveyBot() : __database(database_file_name)
{
}

NeoSaveyBot::~NeoSaveyBot()
{
}


// Note:  this needs a const std::string& channel parameter and a
// Communicator& parameter.
void NeoSaveyBot::parse_command(const std::string& name,
	const std::string& whole_cmd_str)
{
	size_t i, temp_i;
	BigNum slot_bignum;
	std::string cmd, slot, message;

	next_non_blank_substr(whole_cmd_str, 0, cmd, i);
	
	auto print_found_command = [&cmd]() -> void
	{
		printout("Found a \"", cmd, "\".\n");
	};
	auto say_invalid_num_params = [&cmd]() -> void
	{
		printout("Invalid number of parameters for \"", cmd, "\".\n");
	};

	auto inner_next_non_blank_substr = [&]() -> bool
	{
		return next_non_blank_substr(whole_cmd_str, i, slot, temp_i);
	};

	auto show = [&](const Database::Value& to_show) -> void
	{
		printout(" ~ ", to_show.message(), "\n");
	};

	if (cmd == ".road")
	{
		print_found_command();

		//if (!inner_next_non_blank_substr())
		//{
		//	say_invalid_num_params();
		//	return;
		//}

		const auto offset = prng(database().size());

		auto iter = database().begin();

		for (size_t i=0; i<offset; ++i)
		{
			++iter;
		}

		show(iter->second);

	}

	else if (cmd == ".save")
	{
		print_found_command();

		if (!inner_next_non_blank_substr())
		{
			say_invalid_num_params();
			return;
		}

		// If we were given a slot
		if (str_is_integer_bignum(slot, slot_bignum))
		{
			i = temp_i;

			// We need both a slot AND a message
			if (!find_next_non_blank_index(whole_cmd_str, temp_i, i))
			{
				say_invalid_num_params();
				return;
			}
			
			if (database().slot_exists_but_not_owned_by(slot_bignum, name))
			{
				printout("That slot is owned by ", 
					database().at(slot_bignum).name(), "!\n");
			}
			else
			{
				message = std::move(whole_cmd_str.substr(i));
				__database.save(message, name, slot, false);
			}
		}
		else
		{
			slot = std::move(convert_bignum_to_str(database()
				.lowest_available_slot()));
			message = std::move(whole_cmd_str.substr(i));
			__database.save(message, name, slot, true);
		}
	}

	//else if (cmd == ".remove")
	//{
	//	print_found_command();
	//}

	else
	{
		printout("Unknown command.\n");
	}
}

bool NeoSaveyBot::find_next_non_blank_index
	(const std::string& whole_cmd_str, const size_t test_start_index, 
	size_t& i)
{
	i = test_start_index;

	if (i >= whole_cmd_str.size())
	{
		return false;
	}
	
	// Eat leading whitespace
	while (isspace(whole_cmd_str.at(i)))
	{
		++i;

		if (i >= whole_cmd_str.size())
		{
			return false;
		}
	}

	return true;
}
bool NeoSaveyBot::next_non_blank_substr(const std::string& whole_cmd_str, 
	const size_t test_start_index, std::string& ret, size_t& i)
{
	//i = test_start_index;

	//// Eat leading whitespace
	//while (isspace(whole_cmd_str.at(i)))
	//{
	//	++i;

	//	if (i >= whole_cmd_str.size())
	//	{
	//		ret = "";
	//		return false;
	//	}
	//}

	if (!find_next_non_blank_index(whole_cmd_str, test_start_index, i))
	{
		ret = "";
		return false;
	}

	const size_t start_index = i;

	while (i < whole_cmd_str.size() && !isblank(whole_cmd_str.at(i)))
	{
		++i;

		if (i >= whole_cmd_str.size())
		{
			break;
		}
	}

	const size_t end_index = i;

	ret = whole_cmd_str.substr(start_index, (end_index - start_index));
	return true;
}

std::ostream& operator << (std::ostream& os, 
	const NeoSaveyBot::Configuration::Server& to_print)
{
	osprintout(os, "name:  ", to_print.name, "\n");
	osprintout(os, "\tbot_name:  ", to_print.bot_name, "\n");
	osprintout(os, "\taddress:  ", to_print.address, "\n");
	osprintout(os, "\tport_str:  ", to_print.port_str, "\n");
	osprintout(os, "\tpassword:  ", to_print.password, "\n");

	osprintout(os, "\tjoins_list:  \n");
	for (const auto& iter : to_print.joins_list)
	{
		osprintout(os, "\t\t", iter, "\n");
	}

	osprintout(os, "\tstartup_commands:  \n");
	for (const auto& iter : to_print.startup_commands)
	{
		osprintout(os, "\t\t", iter, "\n");
	}

	return os;
}

}
