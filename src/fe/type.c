#include "type.h"

cstr
get_base_type_string(BaseType t)
{
    switch (t)
    {
        case BT_Void: return "VOID";
        case BT_Int: return "INT";
        case BT_Bool: return "BOOL";
        case BT_Char: return "CHAR";
        case BT_UInt: return "uint";
        case BT_Float: return "FLOAT";
        case BT_Array: return "ARRAY";
        case BT_Enum: return "ENUM";
        case BT_Struct: return "STRUCT";
        case BT_Invalid: return "INVALID";
        case BT_Id: return "ID";
        case BT_TBD: return "TBD";
    }
    UNREACHABLE();
    return nullptr;
}
