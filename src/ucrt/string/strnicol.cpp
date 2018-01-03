/***
*strnicoll.c - Collate locale strings without regard to case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information.
*       Compares at most n characters of two strings.
*
*******************************************************************************/
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include "..\..\winapi_thunks.h"
#include <msvcrt_IAT.h>

/***
*int _strnicoll() - Collate locale strings without regard to case
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information
*       without regard to case.
*       Compares at most n characters of two strings.
*
*Entry:
*       const char *s1 = pointer to the first string
*       const char *s2 = pointer to the second string
*       size_t count - maximum number of characters to compare
*
*Exit:
*       Less than 0    = first string less than second string
*       0              = strings are equal
*       Greater than 0 = first string greater than second string
*       Returns _NLSCMPERROR is something went wrong
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

#ifdef _ATL_XP_TARGETING
extern "C" int __cdecl _strnicoll_l_downlevel (
        const char *_string1,
        const char *_string2,
        size_t count,
        _locale_t plocinfo
        )
{
	if (!plocinfo)
		return _strnicoll(_string1, _string2, count);

    int ret;
    //_LocaleUpdate _loc_update(plocinfo);

    if (!count)
        return 0;

    /* validation section */
    _VALIDATE_RETURN(_string1 != nullptr, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(_string2 != nullptr, EINVAL, _NLSCMPERROR);
    _VALIDATE_RETURN(count <= INT_MAX, EINVAL, _NLSCMPERROR);

    if ( plocinfo->locinfo->lc_handle[LC_COLLATE] == 0 )
        return _strnicmp_l(_string1, _string2, count, plocinfo);

    if ( 0 == (ret = __crtCompareStringA(
                    plocinfo,
                    plocinfo->locinfo->lc_handle[LC_COLLATE],
                    SORT_STRINGSORT | NORM_IGNORECASE,
                    _string1,
                    (int)count,
                    _string2,
                    (int)count,
                    plocinfo->locinfo->lc_collate_cp)) )
    {
        errno = EINVAL;
        return _NLSCMPERROR;
    }

    return (ret - 2);
}

_LCRT_DEFINE_IAT_SYMBOL(_strnicoll_l_downlevel);

#endif

//extern "C" int __cdecl _strnicoll (
//        const char *_string1,
//        const char *_string2,
//        size_t count
//        )
//{
//    if (!__acrt_locale_changed())
//    {
//        return _strnicmp(_string1, _string2, count);
//    }
//    else
//    {
//        return _strnicoll_l(_string1, _string2, count, nullptr);
//    }
//}
