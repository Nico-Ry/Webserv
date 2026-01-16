#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "Config.hpp"

class Config;

class ConfigValidator {
	public:
		ConfigValidator();

		void	validConfig(const Config& cfg);
};

#endif