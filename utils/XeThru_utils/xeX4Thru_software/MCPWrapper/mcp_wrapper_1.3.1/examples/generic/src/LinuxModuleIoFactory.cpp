#include "ModuleIoFactory.hpp"
#include "LinuxModuleIo.hpp"


ModuleIo * createModuleIo()
{
    return new LinuxModuleIo();
}
