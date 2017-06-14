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


#include "gmp_stuff.hpp"

namespace neosaveybot
{

bool str_is_integer_bignum(const std::string& str, mpz_class& ret)
{
	size_t i = 0;

	bool negative = false;

	if (str.front() == '-')
	{
		negative = true;
		++i;
	}

	if (isdigit(str.at(i)))
	{
		ret = str.at(i) - '0';
		++i;

		for (; i<str.size(); ++i)
		{
			if (isdigit(str.at(i)))
			{
				//ret = (ret * 10) + (str.at(i) - '0');
				ret = (ret * 10) + convert_single_digit(str, i);
			}
			else // if (!isdigit(str.at(i)))
			{
				return false;
			}
			
		}

		if (negative)
		{
			ret *= -1;
		}

		return true;
	}

	return false;
}

}
