
#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <functional>

namespace Utils {

	static std::vector<std::string> splitByRegex(const std::string& string, char regex) {
		std::stringstream ss;
		ss.str(string);
		std::string item;

		std::vector<std::string> result;
		while (std::getline(ss, item, regex)) {
			result.push_back(item);
		}
		return result;
	}

	template<typename T>
	std::vector<T> splitByRegexT(const std::string& string, char regex, std::function<T(std::string)> f) {
		std::stringstream ss;
		ss.str(string);
		std::string item;

		std::vector<T> result;
		while (std::getline(ss, item, regex))
			result.push_back(f(item));
		return result;
	}

	static std::vector<std::string> split(const std::string& string) {
		return splitByRegex(string, ' ');
	}

}

#endif