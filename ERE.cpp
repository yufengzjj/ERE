#include "Set.h"
#include "Automata.h"
#include "BREParser.h"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"
#include <iostream>
#include <forward_list>
#include <cstring>
namespace po = boost::program_options;
void event_handler(BREParser::event e_, ATMPL::any msg_)
{
	switch (e_)
	{
	case BREParser::event::NFA_complete:
		ATMPL::any::cast<FA>(msg_).print("NFA.dot");
		break;
	case BREParser::event::NFA_determinated:
		ATMPL::any::cast<FA>(msg_).print("DFA.dot");
		break;
	case BREParser::event::DFA_minimized:
		ATMPL::any::cast<FA>(msg_).print("min_DFA.dot");
		break;
	}
}

int main(int argc, char* argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("expr,e", po::value<std::string>(), "set regular expression")
		("string,s", po::value<std::string>(), "string to check by regular expression")
		("alphabet,a", po::value<std::string>()->default_value(FA::alphabet), "set regular expression alphabet");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("expr") && vm.count("string"))
	{
		try
		{
			FA::set_alphabet(vm["alphabet"].as<std::string>());
			BREParser bre_parser;
			bre_parser.register_event(BREParser::event::NFA_complete, event_handler);
			bre_parser.register_event(BREParser::event::NFA_determinated, event_handler);
			bre_parser.register_event(BREParser::event::DFA_minimized, event_handler);
			auto fa = bre_parser.parse(vm["expr"].as<std::string>());
			if (fa.check(vm["string"].as<std::string>()))
			{
				std::cout << "matching!" << std::endl;
			}
			else
			{
				std::cout << "not matching!" << std::endl;
			}
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else
	{
		std::cout << desc;
	}

}

