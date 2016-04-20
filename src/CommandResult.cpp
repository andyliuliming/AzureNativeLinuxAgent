#include "CommandResult.h"


CommandResult::CommandResult()
{
}

CommandResult::~CommandResult()
{
    if (this->output)
    {
        delete this->output;
        this->output = NULL;
    }
}
