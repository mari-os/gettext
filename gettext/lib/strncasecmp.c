/* Compare at most N characters of two strings without taking care for
   the case.
   Copyright (C) 1992, 1996, 1997 Free Software Foundation, Inc.

   NOTE: The canonical source of this file is maintained with the GNU C Library.
   Bugs can be reported to bug-glibc@gnu.org.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <ctype.h>

#ifndef weak_alias
# define __strncasecmp strncasecmp
# define TOLOWER(Ch) tolower (Ch)
#else
# ifdef USE_IN_EXTENDED_LOCALE_MODEL
#  define __strncasecmp __strncasecmp_l
#  define TOLOWER(Ch) __tolower_l ((Ch), loc)
# else
#  define TOLOWER(Ch) tolower (Ch)
# endif
#endif

#ifdef USE_IN_EXTENDED_LOCALE_MODEL
# define LOCALE_PARAM , loc
# define LOCALE_PARAM_DECL __locale_t loc;
#else
# define LOCALE_PARAM
# define LOCALE_PARAM_DECL
#endif

/* Compare no more than N characters of S1 and S2,
   ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less
   than, equal to or greater than S2.  */
int
__strncasecmp (s1, s2, n LOCALE_PARAM)
     const char *s1;
     const char *s2;
     size_t n;
     LOCALE_PARAM_DECL
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (p1 == p2 || n == 0)
    return 0;

  do
    {
      c1 = TOLOWER (*p1++);
      c2 = TOLOWER (*p2++);
      if (c1 == '\0' || c1 != c2)
	break;
    } while (--n > 0);

  return c1 - c2;
}
#ifndef __strncasecmp
weak_alias (__strncasecmp, strncasecmp)
#endif
