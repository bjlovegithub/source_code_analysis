# RedHat-style .spec file for Xapian
# xapian-core.spec.  Generated from xapian-core.spec.in by configure.

# Tell SuSE's build scripts not to build as root:
# norootforbuild

Summary: The Xapian Probabilistic Information Retrieval Library
Name: xapian-core
Version: 1.1.4
Release: 1
License: GPL
Vendor: xapian.org
Group: Applications/Databases
URL: http://xapian.org/
Requires: %{name}-libs = %{version}
Source: http://www.oligarchy.co.uk/xapian/%{version}/%{name}-%{version}.tar.gz
BuildRequires: gcc-c++ zlib-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
Xapian is an Open Source Probabilistic Information Retrieval Library. It
offers a highly adaptable toolkit that allows developers to easily add advanced
indexing and search facilities to applications.

%package libs
Summary: Xapian search engine libraries.
Group: System Environment/Libraries

%description libs
Xapian is an Open Source Probabilistic Information Retrieval framework. It
offers a highly adaptable toolkit that allows developers to easily add advanced
indexing and search facilities to applications. This package provides the
libraries for applications using Xapian functionality.

%package devel
Group: Development/Libraries
Summary: Files needed for building packages which use Xapian.
Requires: %{name}-libs = %{version}

%description devel
Xapian is an Open Source Probabilistic Information Retrieval framework. It
offers a highly adaptable toolkit that allows developers to easily add advanced
indexing and search facilities to applications. This package provides the
files needed for building packages which use Xapian.

%prep
%setup -q -n %{name}-%{version}

%build
# As of 1.1.0, Xapian uses libtool 2.2.x which allows us to override libtool's
# sometimes conservative take on which directories are in the default dynamic
# linker search path, so we no longer incorrectly try to set rpath for
# /usr/lib64.  Hence there's no longer a need to run "autoreconf --force" here
# and it's better not to as it avoids having to cope with incompatibilities
# with older versions of the autotools which older distros have.
#autoreconf --force
%configure
make

%install
[ "%{buildroot}" != "/" ] && rm -rf %{buildroot}
mkdir -p %{buildroot}
# makeinstall doesn't work properly with libtool built libraries
make DESTDIR=%{buildroot} install
# Move the docs to the right place
mv %{buildroot}%{_datadir}/doc/%{name} %{buildroot}%{_datadir}/doc/%{name}-devel-%{version}
# Copy HACKING now, as "%doc HACKING" would overwrite everything
cp HACKING %{buildroot}%{_datadir}/doc/%{name}-devel-%{version}
# Copy the rest while we are in this directory
mkdir -p %{buildroot}%{_datadir}/doc/%{name}-%{version}
cp AUTHORS ChangeLog ChangeLog.examples COPYING NEWS PLATFORMS README %{buildroot}%{_datadir}/doc/%{name}-%{version}

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig

%clean
[ "%{buildroot}" != "/" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/xapian-tcpsrv
%{_bindir}/xapian-progsrv
%{_bindir}/quest
%{_bindir}/delve
%{_bindir}/copydatabase
%{_bindir}/simpleindex
%{_bindir}/simplesearch
%{_bindir}/simpleexpand
%{_bindir}/xapian-check
%{_bindir}/xapian-compact
%{_bindir}/xapian-inspect
%doc %{_datadir}/doc/%{name}-%{version}
# man pages may be gzipped, hence the trailing wildcard.
%{_mandir}/man1/xapian-tcpsrv.1*
%{_mandir}/man1/xapian-progsrv.1*
%{_mandir}/man1/quest.1*
%{_mandir}/man1/delve.1*
%{_mandir}/man1/copydatabase.1*
%{_mandir}/man1/xapian-check.1*
%{_mandir}/man1/xapian-compact.1*
%{_mandir}/man1/xapian-inspect.1*

%files libs
%defattr(-, root, root)
%{_libdir}/libxapian*.so.*

%files devel
%defattr(-, root, root)
%{_bindir}/xapian-config
%{_includedir}/xapian
%{_includedir}/xapian.h
%{_libdir}/libxapian*.so
%{_libdir}/libxapian*.a
%{_libdir}/libxapian*.la
%{_datadir}/aclocal/xapian*.m4
%doc %{_datadir}/doc/%{name}-devel-%{version}
# man pages may be gzipped, hence the trailing wildcard.
%{_mandir}/man1/xapian-config.1*
