%define name	lbreakout2
%define version	2.4.1
%define release 1

Summary:	A breakout-style arcade game for Linux
Name:		%{name}
Version:	%{version}
Release:	%{release}
Group:		Amusements/Games
Copyright:	GPL
URL:		http://www.xiph.org/
Vendor:		Michael Speck <kulkanie@gmx.net>
Source:		http://ftp.sourceforge.net/lgames/%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-root

%description
A breakout-style arcade game for Linux. I guess all of you know how
to play breakout basically. Ball bounces around --> paddle keeps ball
in game -> all bricks destroyed --> next level ;-D

%prep
%setup -q -n %{name}-%{realversion}

%build
%configure
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
mkdir -p ${RPM_BUILD_ROOT}/var/lib/games
make DESTDIR=$RPM_BUILD_ROOT install

%clean 
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING NEWS README TODO
%doc %{_prefix}/doc/lbreakout2
%attr(2555, root, games) %{_bindir}/lbreakout2
%{_datadir}/games/lbreakout2

%defattr(664,games,games)
%config %{_localstatedir}/lib/games/lbreakout2.hscr
