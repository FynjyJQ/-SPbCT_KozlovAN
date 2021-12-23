#pragma once

#ifdef STRINGSDLL_EXPORTS
#define STRINGDLL_API __declspec(dllexport)
#else
#define STRINGDLL_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

namespace StringsFunc
{

	class MyStringsFunc
	{
	public:

		static STRINGDLL_API std::vector<std::string> getStrs(std::string source);
	};
}
