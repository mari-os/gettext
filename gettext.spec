Name: gettext
Version: 0.14.6
Release: alt2

%define libintl libintl3

Summary: GNU libraries and utilities for producing multi-lingual messages
License: LGPL
Group: System/Base
Url: http://www.gnu.org/software/gettext/
Packager: Dmitry V. Levin <ldv@altlinux.org>

# ftp://ftp.gnu.org/gnu/gettext/gettext-%version.tar.gz
Source: gettext-%version.tar
Source1: msghack.py
Source2: gettext-po-mode-start.el

Patch1: gettext-0.14.1-alt-gettextize-quiet.patch
Patch2: gettext-0.14.1-alt-autopoint-cvs.patch
Patch3: gettext-0.14.1-alt-m4.patch
Patch4: gettext-0.14.2-alt-tmp-autopoint.patch
Patch5: gettext-0.14.2-alt-gcc.patch
Patch6: gettext-0.14.5-alt-doc.patch

Provides: %name-base = %version-%release
Obsoletes: %name-base

%def_disable static
%def_without included_gettext

%{?_with_included_gettext:Requires: %libintl = %version-%release}
BuildPreReq: emacs-nox gcc-c++ gcc-g77 jdkgcj tetex-dvips

%package -n %libintl
Summary: The dynamic %libintl library for the gettext package
Group: System/Libraries
Provides: libintl = %version-%release
Obsoletes: libintl

%package -n %libintl-devel
Summary: Development library for %libintl
Group: Development/C
Requires: %libintl = %version-%release
Provides: libintl-devel = %version-%release
Obsoletes: libintl-devel

%package -n %libintl-devel-static
Summary: Development static library for %libintl
Group: Development/C
Requires: %libintl-devel = %version-%release
Provides: libintl-devel-static = %version-%release
Obsoletes: libintl-devel-static

%package tools
Summary: Tools and documentation for developers and translators
Group: Development/Other
Provides: %name-devel = %version-%release
Obsoletes: %name-devel
Requires: %name = %version-%release
Requires(post): %install_info
Requires(preun): %uninstall_info
Requires: mktemp >= 1:1.3.1
%define lib_suffix %nil
%{expand:%%define lib_suffix %(test %_lib != lib64 && echo %%nil || echo '()(64bit)')}
%{!?_with_included_gettext:Provides: preloadable_libintl.so%lib_suffix}

%package tools-java
Summary: Tools for java developers and translators
Group: Development/Other
Requires: %name-tools = %version-%release

%package tools-python
Summary: Python tools for developers and translators
Group: Development/Other
Requires: %name-tools = %version-%release

%package doc
Summary: The GNU gettext manual
Group: Development/Other
Requires: %name = %version-%release

%description
The GNU gettext provides a set of tools and documentation for producing
multi-lingual messages in programs.  Tools include a set of conventions about
how programs should be written to support message catalogs, a directory and
file naming organization for the message catalogs, a runtime library which
supports the retrieval of translated messages, and stand-alone programs for
handling the translatable and the already translated strings.  Gettext provides
an easy to use library and tools for creating, using, and modifying natural
language catalogs and is a powerful and simple method for internationalizing
programs.

This package contains gettext and ngettext utilities.

If you would like to internationalize or incorporate multi-lingual messages
into programs that you're developing, you should install %name-tools.

%description -n %libintl
This package contains the dynamic %libintl library.

%description -n %libintl-devel
This package contains development %libintl library.

%description -n %libintl-devel-static
This package contains static %libintl library.

%description tools
The GNU gettext provides a set of tools and documentation for producing
multi-lingual messages in programs.  Tools include a set of conventions about
how programs should be written to support message catalogs, a directory and
file naming organization for the message catalogs, a runtime library which
supports the retrieval of translated messages, and stand-alone programs for
handling the translatable and the already translated strings.  Gettext provides
an easy to use library and tools for creating, using, and modifying natural
language catalogs and is a powerful and simple method for internationalizing
programs.

If you would like to internationalize or incorporate multi-lingual messages
into programs that you're developing, you should install this package.

%description tools-java
This package adds java support to %name-tools.

%description tools-python
This package contains msghack utility.

%description doc
GNU gettext offers to programmers, translators and even users, a well
integrated set of tools and documentation that provides a framework within
which other free packages may produce multi-lingual messages.

This manual documents GNU gettext.

%prep
%setup -q
%patch1 -p1
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1

%build
%add_optflags -fno-strict-aliasing
%configure --enable-shared %{subst_enable static} \
	%{?_with_included_gettext:--with-included-gettext}
%make_build

%install
%makeinstall \
	lispdir=%buildroot%_datadir/emacs/site-lisp \
	aclocaldir=%buildroot%_datadir/aclocal \
	gettextsrcdir=%buildroot%_datadir/gettext/intl \
	#

mv %buildroot%_datadir/gettext/intl/{ABOUT-NLS,archive.tar.gz} %buildroot%_datadir/gettext/

mkdir -p %buildroot%_datadir/gettext/po
install -pm644 gettext-runtime/po/Makefile.in.in %buildroot%_datadir/gettext/po/

install -pD -m755 %SOURCE1 %buildroot%_bindir/msghack
install -pD -m644 %SOURCE2 %buildroot%_sysconfdir/emacs/site-start.d/gettext.el

%if_with included_gettext
mkdir -p %buildroot%_sysconfdir/buildreqs/packages/substitute.d
echo libintl >%buildroot%_sysconfdir/buildreqs/packages/substitute.d/%libintl
%if_enabled static
echo libintl-devel >%buildroot%_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel
echo libintl-devel-static >%buildroot%_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel-static
%endif #enabled static
chmod 644 %buildroot%_sysconfdir/buildreqs/packages/substitute.d/*
%endif #with included_gettext
mkdir -p %buildroot%_docdir
%define docdir %_docdir/%name-%version
mv %buildroot%_docdir/gettext %buildroot%docdir

%find_lang %name-runtime
%find_lang %name-tools

%post -n %libintl -p %post_ldconfig
%postun -n %libintl -p %postun_ldconfig

%post tools
%install_info gettext.info

%preun tools
%uninstall_info gettext.info

%if_with included_gettext
%files -n %libintl
%config %_sysconfdir/buildreqs/packages/substitute.d/%libintl
%_libdir/libintl*.so.*

%if_enabled static
%files -n %libintl-devel
%config %_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel
%_libdir/libintl*.so

%files -n %libintl-devel-static
%config %_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel-static
%_libdir/libintl*.a
%endif #enabled static
%endif #with included_gettext

%files -f %name-runtime.lang
%_bindir/gettext
%_bindir/ngettext
%_bindir/envsubst
%_bindir/gettext.sh
%_man1dir/gettext.*
%_man1dir/ngettext.*
%_man1dir/envsubst.*

%files tools -f %name-tools.lang
%_libdir/gettext
%exclude %_libdir/gettext/gnu.gettext.*
%_libdir/lib%{name}*.so*
%{!?_with_included_gettext:%_libdir/preloadable_libintl.so}
%_bindir/*
%exclude %_bindir/gettext
%exclude %_bindir/ngettext
%exclude %_bindir/envsubst
%exclude %_bindir/gettext.sh
%exclude %_bindir/msghack
%_includedir/%{name}*
%_mandir/man?/*
%exclude %_man1dir/gettext.*
%exclude %_man1dir/ngettext.*
%exclude %_man1dir/envsubst.*
%_infodir/gettext.info*
%_datadir/gettext
%exclude %_datadir/gettext/libintl.jar
%_datadir/aclocal/*
%_datadir/emacs/site-lisp/*.el*
%config(noreplace) %_sysconfdir/emacs/site-start.d/*.el
%dir %docdir
%docdir/FAQ.html
%docdir/tutorial.html

%files doc
%docdir
%exclude %docdir/FAQ.html
%exclude %docdir/tutorial.html

%files tools-java
%dir %_libdir/gettext
%_libdir/gettext/gnu.gettext.*
%dir %_datadir/gettext
%_datadir/gettext/libintl.jar

%files tools-python
%_bindir/msghack

%changelog
* Sat Apr 14 2007 Dmitry V. Levin <ldv@altlinux.org> 0.14.6-alt2
- Disabled optimization based on strict aliasing rules.

* Sat Sep 23 2006 Dmitry V. Levin <ldv@altlinux.org> 0.14.6-alt1
- Updated to 0.14.6.

* Thu Mar 30 2006 Dmitry V. Levin <ldv@altlinux.org> 0.14.5-alt2
- Fixed a few typos in gettext documentation (#8526).
- Corrected preloadable_libintl.so provides.

* Wed Aug 17 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.5-alt1
- Updated to 0.14.5.

* Wed Apr 13 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.4-alt1
- Updated to 0.14.4.

* Tue Mar 15 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.3-alt1
- Updated to 0.14.3.

* Wed Mar 02 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.2-alt2
- Do not build with included gettext by default.

* Tue Mar 01 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.2-alt1
- Updated to 0.14.2.
- Updated patches.

* Mon Jan 17 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.1-alt5
- Explicitly use automake_1.8 for build.

* Tue Jan 04 2005 Dmitry V. Levin <ldv@altlinux.org> 0.14.1-alt4
- Moved gettext manual to separate subpackage.
- Fixed autopoint temporary file handling.
- Applied few patches from Debian:
  + fixed xgettext misparsing of perl source;
  + improved ABOUT-NLS wording;

* Mon Sep 13 2004 Dmitry V. Levin <ldv@altlinux.org> 0.14.1-alt3
- autopoint, gettextize: Rewritten func_find_curr_installdir
  using readlink(1).
- signed.m4: Fixed "gcc -Wall -Werror" support (#5147).

* Wed Mar 10 2004 Dmitry V. Levin <ldv@altlinux.org> 0.14.1-alt2
- Updated build dependencies.

* Thu Feb 26 2004 Dmitry V. Levin <ldv@altlinux.org> 0.14.1-alt1
- Updated to 0.14.1
- Updated patches.
- Removed obsolete or merged upstream patches:
  rh-jbj
  alt-m4-compat
  alt-aclocaldir
  alt-libobjs
  alt-amproglex
  rh-gettextize
- Enabled packaging of emacs site-start.d files again.
- Do not build static libintl library by default.

* Tue Dec 09 2003 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt13
- Do not package .la files.
- Disabled packaging of emacs site-start.d files for a while.

* Fri Dec 13 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt12
- gettextize: fixed AM_GNU_GETTEXT_VERSION corruption (rh).
- msghack: moved to -tools-python subpackage.

* Sun Dec 08 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt11
- gettext.m4: relaxed error reporting (introduced in -alt10).
- Specfile cleanup (removed old unneeded hacks).

* Thu Nov 21 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt10
- gettext.m4: removed broken "compatibility" patch
  (thanks to Sergey Pinaev).
- autopoint: fixed cvs support.
- %name-po-mode-start.el: fixed autoload (#0001614).

* Tue Oct 29 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt9
- Relocated archive.tar.gz from %_datadir/%name/intl/ to
  %_datadir/%name/ in order to repair autopoint.

* Sat Oct 26 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt8
- gettext.m4: fix some compatibility issues.

* Fri Oct 25 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt7
- Fixed gettext-tools subpackage:
  + moved java support to separate subpackage;
  + removed cvs dependence.

* Thu Oct 24 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt6
- Fixed texinfo installation scripts (reported by Alexander Bokovoy).

* Wed Oct 23 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt5
- Added "quiet" option to gettextize.
- Resplit packages:
  gettext-base + gettext + gettext-devel -> gettext + gettext-tools.

* Tue Oct 15 2002 Dmitry V. Levin <ldv@altlinux.org> 0.11.5-alt4
- Updated %post/%postun scripts.
- Fixed library symlinks generation.
- Added %_libdir/gettext to gettext subpackage.
- Relocated all libraries from /lib/ back to %_libdir/.
- Relocated all binaries from /bin/ back to %_bindir/.
- Additional convention enforcement on patch file names.

* Sun Oct 13 2002 Alexey Voinov <voins@voins.program.ru> 0.11.5-alt3.1
- autopoint added to filelist of gettext-devel

* Sun Oct 13 2002 Alexey Voinov <voins@voins.program.ru> 0.11.5-alt3
- new version(0.11.5)
- INSTOBJEXT patch updated to 0.11.5
- libobjs patch added (fixes use of LIBOBJS substitution in configure.in)
- amproglex patch added (hack to please automake-1.6)
- libintl version increased
- html docs included in gettext-devel

* Tue Mar 19 2002 Dmitry V. Levin <ldv@alt-linux.org> 0.10.40-alt3
- Added buildreq substitution rules.

* Fri Mar 15 2002 Dmitry V. Levin <ldv@alt-linux.org> 0.10.40-alt2
- Updated emacs po-mode (0000554).
- Preparing to libintl2:
  + renamed libintl subpackage to libintl1;
  + moved libraries from gettext-devel to libintl-* subpackages.

* Wed Sep 19 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.40-alt1
- 0.10.40

* Mon Aug 27 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.39-alt3
- Rebuilt.

* Tue Aug 21 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.39-alt2
- Added msghack utility (rh).

* Tue Jul 31 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.39-alt1
- 0.10.39

* Fri May 25 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.38-alt1
- 0.10.38

* Sun May 06 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.37-alt2
- Resurrected INSTOBJEXT from %name-0.10.35.

* Thu Apr 26 2001 Dmitry V. Levin <ldv@altlinux.ru> 0.10.37-alt1
- 0.10.37

* Sat Nov 04 2000 Dmitry V. Levin <ldv@fandra.org> 0.10.35-ipl16mdk
- Merged RH patches.
- RE adaptions.

* Wed Aug 23 2000 Guillaume Cottenceau <gc@mandrakesoft.com> 0.10.35-16mdk
- automatically added packager tag

* Tue Aug 22 2000 Guillaume Cottenceau <gc@mandrakesoft.com> 0.10.35-15mdk
- bugfixed gettextize when headers are not there
  thanks to <rchaillat@mandrakesoft.com>

* Tue Jul 18 2000 Guillaume Cottenceau <gc@mandrakesoft.com> 0.10.35-14mdk
- macros

* Fri May  5 2000 Guillaume Cottenceau <gc@mandrakesoft.com> 0.10.35-13mdk
- quick patch to have it work!

* Sat Apr 08 2000 John Buswell <johnb@mandrakesoft.com> 0.10.35-12mdk
- added devel package

* Thu Mar 30 2000 John Buswell <johnb@mandrakesoft.com> 0.10.35-11mdk
- fixed groups
- Removed version number from spec filename
- spec-helper

* Tue Nov 02 1999 Pablo Saratxaga <pablo@mandrakesoft.com>
- rebuild for new environment

* Wed Jun 30 1999 Chmouel Boudjnah <chmouel@mandrakesoft.com>
- s/arch-RedHat/arch-Mandrake/
- msghack updates.

* Tue May 11 1999 Bernhard Rosenkraenzer <bero@mandrakesoft.com>
- Mandrake adaptions

* Sun Mar 21 1999 Cristian Gafton <gafton@redhat.com>
- auto rebuild in the new build environment (release 8)

* Mon Mar 08 1999 Cristian Gafton <gafton@redhat.com>
- added patch for misc hacks to facilitate rpm translations

* Thu Dec 03 1998 Cristian Gafton <gafton@redhat.com>
- patch to allow to build on ARM

* Wed Sep 30 1998 Jeff Johnson <jbj@redhat.com>
- add Emacs po-mode.el files.

* Sun Sep 13 1998 Cristian Gafton <gafton@redhat.com>
- include the aclocal support files

* Fri Sep  3 1998 Bill Nottingham <notting@redhat.com>
- remove devel package (functionality is in glibc)

* Tue Sep  1 1998 Jeff Johnson <jbj@redhat.com>
- update to 0.10.35.

* Mon Jun 29 1998 Jeff Johnson <jbj@redhat.com>
- add gettextize.
- create devel package for libintl.a and libgettext.h.

* Mon Apr 27 1998 Prospector System <bugs@redhat.com>
- translations modified for de, fr, tr

* Sun Nov 02 1997 Cristian Gafton <gafton@redhat.com>
- added info handling
- added misc-patch (skip emacs-lisp modofications)

* Sat Nov 01 1997 Erik Troan <ewt@redhat.com>
- removed locale.aliases as we get it from glibc now
- uses a buildroot

* Mon Jun 02 1997 Erik Troan <ewt@redhat.com>
- Built against glibc
