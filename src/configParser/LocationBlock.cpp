#include "configParser/LocationBlock.hpp"

/**
 * @brief Default constructor - Builds a LocationBlock object copying relevant
 * values from the ServerBlock passed as argument. Also sets other defaults unique
 * to LocationBlocks
 * @note copies: root, index, autoIndex, clientMaxBodySize, hasRedirect and
 * redirectCode.
 * @param root where to look in our filesystem for the given URI
 * @param index default file to feed for GET requests
 * @param autoIndex Enables/disables directory listing if GET requested a directory
 * @param clientMaxBodySize The max size allowed for any client HTTP request
 * @param hasRedirect Indicates whether a redirecction was defined
 * in the config file or not
 * @param redirectCode The HTTP status Code linked to the redirect as
 * defined in the config file. Set to 0 by default
 * @attention Note that some element such as `errorPages` are purposely
 * omitted from being copied over to the LocationBlock
 */
LocationBlock::LocationBlock()
	: autoIndex(false),
	  clientMaxBodySize(0),
	  hasRedirect(false),
	  redirectCode(0),
	  hasCgiExtension(false),
	  hasCgiBin(false)
{}



/**
 * @brief Builds a LocationBlock, copying over root, index, autoIndex and clientMaxBodySize
 * from the ServerBlock provided
 * @note Sets `hasRediect` to false and `redirectCode` to 0
 */
LocationBlock::LocationBlock(const ServerBlock& s) :
	methods(s.defaultMethods),
	root(s.root),
	index(s.index),
	autoIndex(s.autoIndex),
	errorPages(s.errorPages),
	clientMaxBodySize(s.clientMaxBodySize),
	hasRedirect(false),
	redirectCode(0),//	Set to Zero by default
	hasCgiExtension(false),
	hasCgiBin(false),
	uploadDir(s.uploadDir)
	{}

LocationBlock::~LocationBlock() {}
