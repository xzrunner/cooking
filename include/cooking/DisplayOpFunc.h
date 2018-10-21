#ifndef _COOKING_DISPLAY_OP_FUNC_H_
#define _COOKING_DISPLAY_OP_FUNC_H_

#include "cooking/DisplayOp.h"

namespace cooking
{

class DisplayOpFunc
{
public:
	static void Replay(const DisplayOp* op);

	static size_t Size(const DisplayOp& op);

private:
#define X(Type) static void Replay##Type(const Type& op);
	MAP_OPS_BASED_ON_TYPE(X)
#undef X

}; // DisplayOpFunc

}

#endif // _COOKING_DISPLAY_OP_FUNC_H_