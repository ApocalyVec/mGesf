#include "ModuleIoFactory.hpp"
#include "OverlappedModuleIo.hpp"

ModuleIo * createModuleIo()
{
    return new OverlappedModuleIo();
}





