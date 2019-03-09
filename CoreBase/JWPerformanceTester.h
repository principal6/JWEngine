#pragma once

#include <chrono>
#include "JWCommon.h"

namespace JWENGINE
{
	class JWPerformanceTester final
	{
	public:
		JWPerformanceTester() {};
		JWPerformanceTester(const char* function_name)
		{
			Start(function_name);
		};
		~JWPerformanceTester() {};

		void Start(const char* function_name);
		void End();
		void EndAndStart(const char* function_name);

	private:
		static long long m_TestCount;

		std::chrono::high_resolution_clock m_Clock{};
		std::chrono::high_resolution_clock::time_point m_Start{};
	};
}