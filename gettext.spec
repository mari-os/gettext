Name: gettext
Version: 0.10.40
Release: alt3

%define libintl libintl1

Summary: GNU libraries and utilities for producing multi-lingual messages
License: LGPL
Group: System/Base
Url: http://www.gnu.org/software/%name/

Source: ftp://ftp.gnu.org/gnu/%name/%name-%version.tar.bz2
Source1: msghack.py
Source2: %name-po-mode-start.el

Patch1: %name-0.10.35-jbj.patch
Patch2: %name-0.10.35-aclocaldir.patch
Patch3: %name-0.10.38-INSTOBJEXT.patch

Requires: %name-base = %version-%release

# Automatically added by buildreq on Thu Mar 14 2002
BuildRequires: bison emacsen-startscripts flex gcc-c++ gcc-java libgcj libstdc++-devel tetex tetex-dvips

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

%package base
Summary: GNU %name utility for producing multi-lingual messages
Group: System/Base
Requires: %libintl = %version-%release

%package devel
Summary: GNU libraries and utilities for producing multi-lingual messages
Group: Development/Other
Requires: %name = %version-%release, %libintl-devel = %version-%release

%description
The GNU %name package provides a set of tools and documentation for producing
multi-lingual messages in programs. Tools include a set of conventions about
how programs should be written to support message catalogs, a directory and
file naming organization for the message catalogs, a runtime library which
supports the retrieval of translated messages, and stand-alone programs for
handling the translatable and the already translated strings. Gettext provides
an easy to use library and tools for creating, using, and modifying natural
language catalogs and is a powerful and simple method for internationalizing
programs.

If you would like to internationalize or incorporate multi-lingual messages
into programs that you're developing, you should install %name.

%description -n %libintl
This package contains the dynamic %libintl library.

%description -n %libintl-devel
This package contains development %libintl library.

%description -n %libintl-devel-static
This package contains static %libintl library.

%description base
The base package which includes the %name binary.

%description devel
Header files, used when the libc does not provide code of handling
multi-lingual messages.

%prep
%setup -q
%patch1 -p1
%patch2 -p1
%patch3 -p1

%build
autoconf
%configure --enable-shared --with-included-gettext
%make_build

%install
%makeinstall \
	lispdir=$RPM_BUILD_ROOT%_datadir/emacs/site-lisp \
	aclocaldir=$RPM_BUILD_ROOT%_datadir/aclocal \
	gettextsrcdir=$RPM_BUILD_ROOT%_datadir/%name/intl

mv $RPM_BUILD_ROOT%_datadir/%name/intl/ABOUT-NLS $RPM_BUILD_ROOT%_datadir/%name

mkdir -p $RPM_BUILD_ROOT%_datadir/%name/po
install -p -m644 po/Makefile.in.in $RPM_BUILD_ROOT%_datadir/%name/po

install -p -m755 %SOURCE1 $RPM_BUILD_ROOT%_bindir/msghack
install -pD -m644 %SOURCE2 $RPM_BUILD_ROOT%_sysconfdir/emacs/site-start.d/%name.el

mkdir -p $RPM_BUILD_ROOT/{bin,lib}
mv $RPM_BUILD_ROOT%_bindir/%name $RPM_BUILD_ROOT/bin
ln -s ../../bin/%name $RPM_BUILD_ROOT%_bindir/%name

mkdir -p $RPM_BUILD_ROOT/lib
mv $RPM_BUILD_ROOT%_libdir/libintl*.so.* $RPM_BUILD_ROOT/lib

for f in `find $RPM_BUILD_ROOT%_libdir -type l -name libintl\*`; do
        ln -sf ../../lib/"$(basename "$(/bin/ls -l "$f" |awk '{print $11}')")" "$f"
done

mkdir -p $RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d
echo libintl >$RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/%libintl
echo libintl-devel >$RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel
echo libintl-devel-static >$RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel-static
chmod 644 $RPM_BUILD_ROOT%_sysconfdir/buildreqs/packages/substitute.d/*

%find_lang %name

%post
%install_info %name.info

%preun
%uninstall_info %name.info

%post -n %libintl -p /sbin/ldconfig
%postun -n %libintl -p /sbin/ldconfig

%files -n %libintl
%config %_sysconfdir/buildreqs/packages/substitute.d/%libintl
/lib/*.so.*

%files -n %libintl-devel
%config %_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel
%_libdir/libintl*.so
%_libdir/libintl*.la

%files -n %libintl-devel-static
%config %_sysconfdir/buildreqs/packages/substitute.d/%libintl-devel-static
%_libdir/libintl*.a

%files base -f %name.lang
/bin/%name
%_bindir/%name

%files
%_bindir/msg*
%_bindir/?gettext
%_infodir/*.info*
%_datadir/aclocal/*
%_datadir/emacs/site-lisp/*.el
%config(noreplace) %_sysconfdir/emacs/site-start.d/*.el
#%config(noreplace) %_libdir/charset.alias
%doc BUGS NEWS README TODO

%files devel
%_bindir/gettextize
%_datadir/%name
%_mandir/man?/*

%changelog
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
