#include "configParser/ServerBlock.hpp"

/**
 * @brief Default Constructor - Builds a ServerBlock object setting default values
 * for `port`, `autoIndex` and `clientMaxBodySize`
 */
ServerBlock::ServerBlock()
	: port(0), autoIndex(false), clientMaxBodySize(512 * 1024UL) {
	defaultMethods.push_back("GET");
}

ServerBlock::~ServerBlock() {}