/* Message list charset and locale charset handling.
   Copyright (C) 2001-2003, 2005-2006 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <alloca.h>

/* Specification.  */
#include "msgl-iconv.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_ICONV
# include <iconv.h>
#endif

#include "progname.h"
#include "basename.h"
#include "message.h"
#include "po-charset.h"
#include "iconvstring.h"
#include "msgl-ascii.h"
#include "xalloc.h"
#include "xallocsa.h"
#include "strstr.h"
#include "xvasprintf.h"
#include "po-xerror.h"
#include "gettext.h"

#define _(str) gettext (str)


#if HAVE_ICONV

static void conversion_error (const struct conversion_context* context)
#if defined __GNUC__ && ((__GNUC__ == 2 && __GNUC_MINOR__ >= 5) || __GNUC__ > 2)
     __attribute__ ((noreturn))
#endif
;
static void
conversion_error (const struct conversion_context* context)
{
  if (context->to_code == po_charset_utf8)
    /* If a conversion to UTF-8 fails, the problem lies in the input.  */
    po_xerror (PO_SEVERITY_FATAL_ERROR, context->message, NULL, 0, 0, false,
	       xasprintf (_("%s: input is not valid in \"%s\" encoding"),
			  context->from_filename, context->from_code));
  else
    po_xerror (PO_SEVERITY_FATAL_ERROR, context->message, NULL, 0, 0, false,
	       xasprintf (_("\
%s: error while converting from \"%s\" encoding to \"%s\" encoding"),
			  context->from_filename, context->from_code,
			  context->to_code));
  /* NOTREACHED */
  abort ();
}

char *
convert_string (iconv_t cd, const char *string,
		const struct conversion_context* context)
{
  size_t len = strlen (string) + 1;
  char *result = NULL;
  size_t resultlen;

  if (iconv_string (cd, string, string + len, &result, &resultlen) == 0)
    /* Verify the result has exactly one NUL byte, at the end.  */
    if (resultlen > 0 && result[resultlen - 1] == '\0'
	&& strlen (result) == resultlen - 1)
      return result;

  conversion_error (context);
  /* NOTREACHED */
  return NULL;
}

static void
convert_string_list (iconv_t cd, string_list_ty *slp,
		     const struct conversion_context* context)
{
  size_t i;

  if (slp != NULL)
    for (i = 0; i < slp->nitems; i++)
      slp->item[i] = convert_string (cd, slp->item[i], context);
}

static void
convert_msgid (iconv_t cd, message_ty *mp,
	       const struct conversion_context* context)
{
  if (mp->msgctxt != NULL)
    mp->msgctxt = convert_string (cd, mp->msgctxt, context);
  mp->msgid = convert_string (cd, mp->msgid, context);
  if (mp->msgid_plural != NULL)
    mp->msgid_plural = convert_string (cd, mp->msgid_plural, context);
}

static void
convert_msgstr (iconv_t cd, message_ty *mp,
		const struct conversion_context* context)
{
  char *result = NULL;
  size_t resultlen;

  if (!(mp->msgstr_len > 0 && mp->msgstr[mp->msgstr_len - 1] == '\0'))
    abort ();

  if (iconv_string (cd, mp->msgstr, mp->msgstr + mp->msgstr_len,
		    &result, &resultlen) == 0)
    /* Verify the result has a NUL byte at the end.  */
    if (resultlen > 0 && result[resultlen - 1] == '\0')
      /* Verify the result has the same number of NUL bytes.  */
      {
	const char *p;
	const char *pend;
	int nulcount1;
	int nulcount2;

	for (p = mp->msgstr, pend = p + mp->msgstr_len, nulcount1 = 0;
	     p < pend;
	     p += strlen (p) + 1, nulcount1++);
	for (p = result, pend = p + resultlen, nulcount2 = 0;
	     p < pend;
	     p += strlen (p) + 1, nulcount2++);

	if (nulcount1 == nulcount2)
	  {
	    mp->msgstr = result;
	    mp->msgstr_len = resultlen;
	    return;
	  }
      }

  conversion_error (context);
}

#endif


bool
iconv_message_list (message_list_ty *mlp,
		    const char *canon_from_code, const char *canon_to_code,
		    const char *from_filename)
{
  bool canon_from_code_overridden = (canon_from_code != NULL);
  bool msgids_changed;
  size_t j;

  /* If the list is empty, nothing to do.  */
  if (mlp->nitems == 0)
    return false;

  /* Search the header entry, and extract and replace the charset name.  */
  for (j = 0; j < mlp->nitems; j++)
    if (is_header (mlp->item[j]) && !mlp->item[j]->obsolete)
      {
	const char *header = mlp->item[j]->msgstr;

	if (header != NULL)
	  {
	    const char *charsetstr = strstr (header, "charset=");

	    if (charsetstr != NULL)
	      {
		size_t len;
		char *charset;
		const char *canon_charset;
		size_t len1, len2, len3;
		char *new_header;

		charsetstr += strlen ("charset=");
		len = strcspn (charsetstr, " \t\n");
		charset = (char *) xallocsa (len + 1);
		memcpy (charset, charsetstr, len);
		charset[len] = '\0';

		canon_charset = po_charset_canonicalize (charset);
		if (canon_charset == NULL)
		  {
		    if (!canon_from_code_overridden)
		      {
			/* Don't give an error for POT files, because POT
			   files usually contain only ASCII msgids.  */
			const char *filename = from_filename;
			size_t filenamelen;

			if (filename != NULL
			    && (filenamelen = strlen (filename)) >= 4
			    && memcmp (filename + filenamelen - 4, ".pot", 4)
			       == 0
			    && strcmp (charset, "CHARSET") == 0)
			  canon_charset = po_charset_ascii;
			else
			  po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0, 0,
				     false, xasprintf (_("\
present charset \"%s\" is not a portable encoding name"),
						charset));
		      }
		  }
		else
		  {
		    if (canon_from_code == NULL)
		      canon_from_code = canon_charset;
		    else if (canon_from_code != canon_charset)
		      po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0,  0,
				 false,
				 xasprintf (_("\
two different charsets \"%s\" and \"%s\" in input file"),
					    canon_from_code, canon_charset));
		  }
		freesa (charset);

		len1 = charsetstr - header;
		len2 = strlen (canon_to_code);
		len3 = (header + strlen (header)) - (charsetstr + len);
		new_header = (char *) xmalloc (len1 + len2 + len3 + 1);
		memcpy (new_header, header, len1);
		memcpy (new_header + len1, canon_to_code, len2);
		memcpy (new_header + len1 + len2, charsetstr + len, len3 + 1);
		mlp->item[j]->msgstr = new_header;
		mlp->item[j]->msgstr_len = len1 + len2 + len3 + 1;
	      }
	  }
      }
  if (canon_from_code == NULL)
    {
      if (is_ascii_message_list (mlp))
	canon_from_code = po_charset_ascii;
      else
	po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0, 0, false,
		   _("\
input file doesn't contain a header entry with a charset specification"));
    }

  msgids_changed = false;

  /* If the two encodings are the same, nothing to do.  */
  if (canon_from_code != canon_to_code)
    {
#if HAVE_ICONV
      iconv_t cd;
      struct conversion_context context;

      /* Avoid glibc-2.1 bug with EUC-KR.  */
# if (__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) && !defined _LIBICONV_VERSION
      if (strcmp (canon_from_code, "EUC-KR") == 0)
	cd = (iconv_t)(-1);
      else
# endif
      cd = iconv_open (canon_to_code, canon_from_code);
      if (cd == (iconv_t)(-1))
	po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0, 0, false,
		   xasprintf (_("\
Cannot convert from \"%s\" to \"%s\". %s relies on iconv(), \
and iconv() does not support this conversion."),
			      canon_from_code, canon_to_code,
			      basename (program_name)));

      context.from_code = canon_from_code;
      context.to_code = canon_to_code;
      context.from_filename = from_filename;

      for (j = 0; j < mlp->nitems; j++)
	{
	  message_ty *mp = mlp->item[j];

	  if ((mp->msgctxt != NULL && !is_ascii_string (mp->msgctxt))
	      || !is_ascii_string (mp->msgid))
	    msgids_changed = true;
	  context.message = mp;
	  convert_string_list (cd, mp->comment, &context);
	  convert_string_list (cd, mp->comment_dot, &context);
	  convert_msgid (cd, mp, &context);
	  convert_msgstr (cd, mp, &context);
	}

      iconv_close (cd);

      if (msgids_changed)
	if (message_list_msgids_changed (mlp))
	  po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0, 0, false,
		     xasprintf (_("\
Conversion from \"%s\" to \"%s\" introduces duplicates: \
some different msgids become equal."),
				canon_from_code, canon_to_code));
#else
	  po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0, 0, false,
		     xasprintf (_("\
Cannot convert from \"%s\" to \"%s\". %s relies on iconv(). \
This version was built without iconv()."),
				canon_from_code, canon_to_code,
				basename (program_name)));
#endif
    }

  return msgids_changed;
}

msgdomain_list_ty *
iconv_msgdomain_list (msgdomain_list_ty *mdlp,
		      const char *to_code,
		      const char *from_filename)
{
  const char *canon_to_code;
  size_t k;

  /* Canonicalize target encoding.  */
  canon_to_code = po_charset_canonicalize (to_code);
  if (canon_to_code == NULL)
    po_xerror (PO_SEVERITY_FATAL_ERROR, NULL, NULL, 0, 0, false,
	       xasprintf (_("\
target charset \"%s\" is not a portable encoding name."),
			  to_code));

  for (k = 0; k < mdlp->nitems; k++)
    iconv_message_list (mdlp->item[k]->messages, mdlp->encoding, canon_to_code,
			from_filename);

  mdlp->encoding = canon_to_code;
  return mdlp;
}
