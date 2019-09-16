Summary: AWS DynamoDB Library for C and C++
Name: aws_dynamo
Version: 0.0.1
Release: 0
License: LGPL
Group: System Environment/Libraries
Buildroot: %{_tmppath}/%{name}-%{version}-root
Source: %{name}-%{version}.tar.gz

%description
AWS DynamoDB Library for C and C++

%post -p /sbin/ldconfig

%prep
%setup

%build
./autogen.sh
./configure --prefix=/usr --disable-static
make

%install
make install DESTDIR=${RPM_BUILD_ROOT}

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/include/aws_dynamo/*
/usr/lib/libaws_dynamo.*a
/usr/lib/libaws_dynamo.so*
