#include "utils.hpp"
#include "router/Router.hpp"

void	printParentPaths(const DescendingStrSet& paths) {

	std::cout << CYAN << "[ParentPaths]" << RES << std::endl;
	for (DescendingStrSet::const_iterator it = paths.begin(); it != paths.end(); ++it) {
		std::cout << *it << std::endl;
	}
}

void	printRouterUri(const HttpRequest& r) {
	std::cout << CYAN << "[URI]\n" << std::setw(8) << RES << r.rawTarget << std::endl;
}