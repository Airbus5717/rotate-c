#pragma once

#include "token.h"

// NOTE(5717): not to be confused with TknIdx
// Type Index will pointer multiple symbol tables and will differ by the type
typedef uint TypeIndex;  // Type index
typedef uint ExprIdx;    // Expression index
typedef uint ExprLocIdx; // Expression location index

typedef enum
{
    BT_Invalid,
    BT_Void, // NOTE(5717): ONLY ALLOWED FOR FUNCTIONS
    BT_Int,
    BT_UInt,
    BT_Float,
    BT_Char, // u8
    BT_Bool,
    BT_Array, // strings are char arrays
    BT_Struct,
    BT_Enum, // a member
    BT_Id,   // Identifier, TO BE VALIDATED
    BT_TBD,  // TO BE DETERMINED
} BaseType;
