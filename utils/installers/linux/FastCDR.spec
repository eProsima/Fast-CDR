Name: FastCDR
Version: VERSION
Release: 1%{?dist}
Summary: A library for serialize using CDR serialization
License: LGPLv3	
URL: http://eprosima.com
Source0: %{name}_%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: autoconf automake libtool

%description
eProsima FastCDR library provides two serialization mechanisms. One is the standard CDR serialization mechanism,
while the other is a faster implementation of it.

%prep
%setup -q -n %{name}

%build
%configure
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc
%{_includedir}/*
%{_libdir}/libfastcdr*
%{_datadir}/*

%changelog
