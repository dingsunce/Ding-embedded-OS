#include "CppUTest/CommandLineTestRunner.h"
#include "TestReset.h"

int main(int argc, char** argv)
{
  TestReset();
  // MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(argc, argv);
}
