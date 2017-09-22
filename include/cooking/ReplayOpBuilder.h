#ifndef _COOKING_REPLAY_OP_BUILDER_H_
#define _COOKING_REPLAY_OP_BUILDER_H_

#include "cooking/DisplayOp.h"

#include <vector>

namespace cooking
{

class ReplayOpBuilder
{
public:
	static void Replay(const std::vector<DisplayOp*>& ops, int begin, int end);

private:
#define X(Type) static void Replay##Type(const Type& op);
	MAP_OPS_BASED_ON_TYPE(X)
#undef X	

}; // ReplayOpBuilder

}

#endif // _COOKING_REPLAY_OP_BUILDER_H_