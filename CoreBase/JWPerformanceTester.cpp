#include "JWPerformanceTester.h"

using namespace JWENGINE;

// Static member variable
long long JWPerformanceTester::m_TestCount{};

void JWPerformanceTester::Start(const char* function_name)
{
	++m_TestCount;

	m_Start = m_Clock.now();

	LOG_DEBUG("[TEST #" + ConvertLongLongToSTRING(m_TestCount) + "] " + STRING(function_name) + "() Performance Test Initiated.");
}

void JWPerformanceTester::End()
{
	auto elapsed = m_Clock.now() - m_Start;

	LOG_DEBUG(" # Result: " + ConvertLongLongToSTRING(elapsed.count()) + " ns elapsed");
}

void JWPerformanceTester::EndAndStart(const char* function_name)
{
	End();
	Start(function_name);
}
