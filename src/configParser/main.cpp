/*
High-level configuration parsing flow:

1) A Config object is created. (Config.cpp/hpp)
   - Config is the top-level container and stores all ServerBlock objects.

2) The configuration file is tokenised. (Tokeniser.cpp/hpp)
   - Tokeniser reads the raw config file.
   - It produces a vector of Token objects (keywords, values, symbols).
   - No semantic meaning is applied at this stage.

3) The token stream is parsed. (ConfigParser.cpp/hpp)
   - ConfigParser consumes tokens sequentially.
   - Server blocks are parsed first. (parseServerBlock.cpp)
   - Each ServerBlock parses its own directives and nested LocationBlocks. (parseLocationBlock.cpp)
   - Each directive is handled by a dedicated parsing function.
   - Syntax and semantic errors are detected here.

Data structure layout:

Config															 (Config.hpp)
└── vector<ServerBlock>												  |
    └── ServerBlock											(ServerBlock.hpp)
        ├── server-level configuration/info							  |
        └── vector<LocationBlock>									  |
            └── LocationBlock							  (LocationBlock.hpp)
                └── location-level configuration/info

Notes:
- LocationBlocks represent more specific configuration than ServerBlocks.
- Inheritance and fallback (server -> location) are resolved at runtime,
  not during parsing.
- Parsing only builds and validates the configuration structure.
*/


#include "Config.hpp"

int	main(int ac, char** av) {
	if (ac != 2)
		return -1;
	try {
		Config	cfg(av[1]);
		std::cout << "[DEBUG] PARSING SUCCESS" << std::endl;

	}
	catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
	return 0;
}