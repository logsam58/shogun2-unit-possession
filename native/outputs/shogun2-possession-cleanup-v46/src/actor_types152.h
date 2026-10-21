#pragma once
#include <cstdint>
namespace actor152 {
// Final soldier classes from native14da10 and constructors rooted at3596e0.
// These share the human actor layout but carry their OWN config/actions.
// Intermediate base classes, fauna and vehicle bodies are not soldiers.
inline bool leader(uintptr_t engine,uintptr_t table){return table==engine+0x15dbd68||table==engine+0x15dbfb4;}
inline bool individual(uintptr_t engine,uintptr_t table){
    if(leader(engine,table))return true;
    if(table<engine)return false;
    switch(table-engine){
    case 0x15dc590: // bugler34e830
    case 0x15dc1a8: // drummer34eff0
    case 0x15dc39c: // flautist3513c0
    case 0x15dcf54: // standard bearer35f560
    case 0x15dc978: // admiral34dcd0
    case 0x15dcb6c: // commodore34ecc0
    case 0x15dcd60: // naval officer353c50
        return true;
    default:return false;
    }
}
inline bool human(uintptr_t engine,uintptr_t table){return table==engine+0x15dfec8||individual(engine,table);}
inline bool kind(uintptr_t engine,uintptr_t table,unsigned k){return k==0?human(engine,table):k==1&&table==engine+0x15e8524;}
}
