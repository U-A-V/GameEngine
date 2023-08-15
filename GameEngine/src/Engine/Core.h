#pragma once
#ifdef EG_PLATFORM_WINDOWS
#if EG_DYNAMIC_LINK
	#ifdef EG_BUILD_DLL
		#define ENGINE_API __declspec(dllexport)
	#else 
		#define ENGINE_API __declspec(dllimport)
	#endif
#else 
#define ENGINE_API
#endif
#else
	#error Engine only support Windows!!
#endif

#ifdef EG_BUILD
#define EG_ENABLE_ASSERTS
#endif

#ifdef EG_ENABLE_ASSERTS
#define EG_ASSERT(x, ...) {if(!(x)){EG_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak();}}
#define EG_CORE_ASSERT(x, ...) {if(!(x)){EG_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak();}}
#else
#define EG_ASSERT(x, ...)
#define EG_CORE_ASSERT(x, ...)
#endif // EG_ENABLE_ASSERTS


#define BIT(x) (1<<x)

#define EG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)