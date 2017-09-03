#ifndef _COOKING_CALLBACK_H_
#define _COOKING_CALLBACK_H_

namespace mt { class Task; }

namespace cooking
{

class Callback
{
public:
	struct Funs
	{
		void (*submit_task)(mt::Task*);
	};

	static void RegisterCallback(const Funs& funs);

	//////////////////////////////////////////////////////////////////////////

	static void SubmitTask(mt::Task*);

}; // Callback

}

#endif // _COOKING_CALLBACK_H_