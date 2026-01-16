#include "configParser/ConfigValidator.hpp"

ConfigValidator::ConfigValidator() {}

void	ConfigValidator::validConfig(const Config& cfg) {
	if (cfg.servers.empty())
		throw std::runtime_error("No servers defined in Config");
}