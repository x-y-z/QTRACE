#include "qtrace.h"

void InstructionCallBack(unsigned type)
{
  QTRACE_TEST_FETCH(type);
}
