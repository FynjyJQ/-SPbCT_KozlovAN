#include "pch.h"
#include <string>
#include <random>
#include <vector>
#include "StringsDll.h"

namespace StringsFunc
{
	std::vector<std::string> MyStringsFunc::getStrs(std::string source)
	{
		std::vector<std::string> strs;

		int length = source.length();

		std::vector<int> ixs;

		for (int i = 0; i < 10; i += 1) {
			ixs.push_back(i * 10);
		}
		ixs.push_back(length);

		int prevIx = 0;
		auto it = ixs.begin();
		while (it != ixs.end()) {
			strs.push_back(
				source.substr(prevIx, *it - prevIx - 1)
			);

			prevIx = *it;
			it++;
		}

		return strs;
	}
}
