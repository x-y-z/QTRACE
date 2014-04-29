#include "qtrace.h"

void InstructionCB(unsigned type)
{
  QTRACE_TEST_FETCH(type);
}
