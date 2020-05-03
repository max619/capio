#pragma once
#include <crtdbg.h>
#include "CppUnitTest.h"

//source https://stackoverflow.com/questions/2980917/c-is-it-possible-to-implement-memory-leak-testing-in-a-unit-test
struct CrtCheckMemory
{
    _CrtMemState state1;
    _CrtMemState state2;
    _CrtMemState state3;
    CrtCheckMemory()
    {
        _CrtMemCheckpoint(&state1);
    }
    ~CrtCheckMemory()
    {
        _CrtMemCheckpoint(&state2);
        // else just do this to dump the leaked blocks to stdout.
        if (_CrtMemDifference(&state3, &state1, &state2))
        {
            _CrtMemDumpStatistics(&state3);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(L"Memory leak detected");
        }
    }
};

#if _DEBUG
#define CHECK_MEM_LEAK() CrtCheckMemory memLeakTestState
#else
#define CHECK_MEM_LEAK()
#endif