Name: gettext
Version: 0.14.1
Release: alt2

%define libintl libintl3

Summary: GNU libraries and utilities for producing multi-lingual messages
License: LGPL
Group: System/Base
Url: http://www.gnu.org/software/%name/
Packager: Gettext Development Team <gettext@packages.altlinux.org>

Source: ftp://ftp.gnu.org/gnu/%name/%name-%version.tar.bz2
Source1: msghack.py
Source2: %name-po-mode-start.el

Patch1: %name-0.14.1-alt-gettextize-quiet.patch
Patch2: %name-0.14.1-alt-autopoint-cvs.patch

Provides: %name-base = %version-%release
Obsoletes: %name-base
Requires: %libintl = %version-%release

%def_disable static

BuildPreReq: emacs-nox gcc-c++ gcc-g77 gcc-java jdkgcj tetex-dvips

%package -n %libintl
Summary: The dynamic %libintl library for the %name package
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
Requires(post,preun): %__install_info

%package tools-java
Summary: Tools for java developers and translators
Group: Development/Other
Requires: %name-tools = %version-%release

%package tools-python
Summary: Python tools for developers and translators
Group: Development/Other
Requires: %name-tools = %version-%release

%description
The GNU %name provides a set of tools and documentation for producing
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
The GNU %name provides a set of tools and documentation for producing
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

%prep
%setup -q
%patch1 -p1
%patch2 -p1

%build
%configure --with-included-gettext --enable-shared %{subst_enable static}
%make_build

%install
%makeinstall \
	lispdir=$RPM_BUILD_ROOT%_datadir/emacs/site-lisp \
	aclocaldir=$RPM_BUILD_ROOT%_datadir/aclocal \
	gettextsrcdir=$RPM_BUILD_ROOT%_datadir/%name/intl \
	#

%__mv $RPM_BUILD_ROOT%_datadir/%name/intl/{ABOUT-NLS,archive.tar.gz} $RPM_BUILD_ROOT%_datadir/%name/

%__mkdir_p $RPM_BUILD_ROOT%_datadir/%name/po
%__install -p -m644 %name-runtime/po/Makefile.in.in $RPM_BUILD_ROOT%_datadir/%name/po/

%__install -pD -m755 %SOURCE1 $RPM_BUILD_ROOT%_bindir/msghack
%__install -pD -m644 %SOURCE2 $RPM_BUILD_ROOT%_sysconfdir/emacs/site-start.d/%name.el

%__mkdir_p $RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d
echo libintl >$RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/%libintl
%if_enabled static
echo libintl-devel >$RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel
echo libintl-devel-static >$RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel-static
%endif
%__chmod 644 $RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/*
%__mkdir_p $RPM_BUILD_ROOT%_docdir
%__mv $RPM_BUILD_ROOT%_docdir/%name $RPM_BUILD_ROOT%_docdir/%name-%version

%find_lang %name-runtime
%find_lang %name-tools

%post -n %libintl -p %post_ldconfig
%postun -n %libintl -p %postun_ldconfig

%post tools
%install_info %name.info

%preun tools
%uninstall_info %name.info

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
%endif

%files -f %name-runtime.lang
%_bindir/%name
%_bindir/n%name
%_bindir/envsubst
%_bindir/%name.sh
%_man1dir/%name.*
%_man1dir/n%name.*
%_man1dir/envsubst.*

%files tools -f %name-tools.lang
%_libdir/%name
%exclude %_libdir/%name/gnu.gettext.*
%_libdir/lib%{name}*.so*
%_bindir/*
%exclude %_bindir/%name
%exclude %_bindir/n%name
%exclude %_bindir/envsubst
%exclude %_bindir/%name.sh
%exclude %_bindir/msghack
%_includedir/%{name}*
%_mandir/man?/*
%exclude %_man1dir/%name.*
%exclude %_man1dir/n%name.*
%exclude %_man1dir/envsubst.*
%_infodir/%name.info*
%_datadir/%name
%exclude %_datadir/%name/libintl.jar
%_datadir/aclocal/*
%_datadir/emacs/site-lisp/*.el*
%config(noreplace) %_sysconfdir/emacs/site-start.d/*.el
%_docdir/%name-%version/*

%files tools-java
%dir %_libdir/%name
%_libdir/%name/gnu.gettext.*
%dir %_datadir/%name
%_datadir/%name/libintl.jar

%files tools-python
%_bindir/msghack

%changelog
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
