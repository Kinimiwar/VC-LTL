
/*

用于修正异常实现

*/



#include <corecrt_internal.h>
#include <eh.h>
#include "ehdata.h"
#include <msvcrt_IAT.h>

EXTERN_C DWORD __cdecl __LTL_GetOsMinVersion();





///////////////////////////////////////////////////////////////////////////////
//
// __uncaught_exception() - Returns true after completing of a throw-expression
//                          untils completing initialization of the
//                          exception-declaration in the matching handler.
//
// __uncaught_exceptions() - Returns the number of exceptions that have been
//                           thrown but not yet caught.
//

extern "C" int __cdecl __uncaught_exceptions_downlevel()
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		//Windows XP不支持多异常处理
		return __uncaught_exception() ? 1 : 0;
	}
	else
#endif
	{
		return __acrt_getptd()->VistaOrLater_msvcrt._ProcessingThrow;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__uncaught_exceptions_downlevel);

///////////////////////////////////////////////////////////////////////////////
//
// __current_exception(), __current_exception_context(), __processing_throw()
//                        - Accessors for the per-thread exception state, used
//                          by the managed EH implementation, exception_ptr, etc.
//


extern "C" void** __cdecl __current_exception_downlevel()
{
	auto ptd = __acrt_getptd();
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._curexception;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._curexception;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__current_exception_downlevel);

extern "C" void** __cdecl __current_exception_context_downlevel()
{
	auto ptd = __acrt_getptd();
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._curcontext;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._curcontext;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__current_exception_context_downlevel);

#ifdef _ATL_XP_TARGETING
static thread_local int _ProcessingThrow;
#endif
extern "C" int* __cdecl __processing_throw_downlevel()
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		//Windows XP不支持多异常处理，只能获取有或者无

		_ProcessingThrow = __uncaught_exception() ? 1 : 0;
		return &_ProcessingThrow;
	}
	else
#endif
	{
		return &__acrt_getptd()->VistaOrLater_msvcrt._ProcessingThrow;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__processing_throw_downlevel);

////////////////////////////////////////////////////////////////////////////////
//
// __AdjustPointer - Adjust the pointer to the exception object to a pointer to a
//   base instance.
//
// Output:
//     The address point of the base.
//
// Side-effects:
//     NONE.

extern "C" void *__AdjustPointer(
	void *pThis,                        // Address point of exception object
	const PMD& pmd                      // Generalized pointer-to-member
										//   descriptor
) {
	char *pRet = (char *)pThis + pmd.mdisp;

	if (pmd.pdisp >= 0) {
		pRet += *(__int32 *)((char *)*(ptrdiff_t *)((char *)pThis + pmd.pdisp)
#if defined(_WIN64)
			+(unsigned __int64)pmd.vdisp);
#else
			+ pmd.vdisp);
#endif
		pRet += pmd.pdisp;
	}

	return pRet;
}

#ifdef _X86_
extern "C" void __stdcall _CallMemberFunction0(void *pthis, void *pmfn)
{

	__asm
	{
		mov		ecx, pthis
		call	pmfn
	}
}
#endif

#ifdef _X86_
extern "C" void __stdcall _CallMemberFunction1(void *pthis, void *pmfn, void *pthat)
{
	__asm
	{
		push    pthat
		mov     ecx, pthis
		call    pmfn
	}
}
#endif

#ifdef _X86_
extern "C" void __stdcall _CallMemberFunction2(void *pthis, void *pmfn, void *pthat, int val2)
{
	__asm
	{
		push	val2
		push	pthat
		mov		ecx, pthis
		call	pmfn
	}
}
#endif