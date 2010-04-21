/* Automatically generated header! Do not edit! */

#ifndef _INLINE_USERGROUP_H
#define _INLINE_USERGROUP_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef USERGROUP_BASE_NAME
#define USERGROUP_BASE_NAME UserGroupBase
#endif /* !USERGROUP_BASE_NAME */

#define crypt(___key, ___salt) \
	AROS_LC2(char *, crypt, \
	AROS_LCA(const char *, (___key), A0), \
	AROS_LCA(const char *, (___salt), A1), \
	struct Library *, USERGROUP_BASE_NAME, 29, /* s */)

#define endgrent() \
	AROS_LC0(void, endgrent, \
	struct Library *, USERGROUP_BASE_NAME, 28, /* s */)

#define endpwent() \
	AROS_LC0(void, endpwent, \
	struct Library *, USERGROUP_BASE_NAME, 23, /* s */)

#define endutent() \
	AROS_LC0(void, endutent, \
	struct Library *, USERGROUP_BASE_NAME, 40, /* s */)

#define getcredentials(___task) \
	AROS_LC1(struct UserGroupCredentials *, getcredentials, \
	AROS_LCA(struct Task *, (___task), A0), \
	struct Library *, USERGROUP_BASE_NAME, 43, /* s */)

#define getegid() \
	AROS_LC0(gid_t, getegid, \
	struct Library *, USERGROUP_BASE_NAME, 13, /* s */)

#define geteuid() \
	AROS_LC0(uid_t, geteuid, \
	struct Library *, USERGROUP_BASE_NAME, 9, /* s */)

#define getgid() \
	AROS_LC0(gid_t, getgid, \
	struct Library *, USERGROUP_BASE_NAME, 12, /* s */)

#define getgrent() \
	AROS_LC0(struct group *, getgrent, \
	struct Library *, USERGROUP_BASE_NAME, 27, /* s */)

#define getgrgid(___gid) \
	AROS_LC1(struct group *, getgrgid, \
	AROS_LCA(gid_t, (___gid), D0), \
	struct Library *, USERGROUP_BASE_NAME, 25, /* s */)

#define getgrnam(___name) \
	AROS_LC1(struct group *, getgrnam, \
	AROS_LCA(const char *, (___name), A1), \
	struct Library *, USERGROUP_BASE_NAME, 24, /* s */)

#define getgroups(___ngroups, ___groups) \
	AROS_LC2(int, getgroups, \
	AROS_LCA(int, (___ngroups), D0), \
	AROS_LCA(gid_t *, (___groups), A1), \
	struct Library *, USERGROUP_BASE_NAME, 16, /* s */)

#define getlastlog(___uid) \
	AROS_LC1(struct lastlog *, getlastlog, \
	AROS_LCA(uid_t, (___uid), D0), \
	struct Library *, USERGROUP_BASE_NAME, 41, /* s */)

#define getlogin() \
	AROS_LC0(char *, getlogin, \
	struct Library *, USERGROUP_BASE_NAME, 36, /* s */)

#define getpass(___prompt) \
	AROS_LC1(char *, getpass, \
	AROS_LCA(const char *, (___prompt), A1), \
	struct Library *, USERGROUP_BASE_NAME, 31, /* s */)

#define getpgrp() \
	AROS_LC0(pid_t, getpgrp, \
	struct Library *, USERGROUP_BASE_NAME, 35, /* s */)

#define getpwent() \
	AROS_LC0(struct passwd *, getpwent, \
	struct Library *, USERGROUP_BASE_NAME, 22, /* s */)

#define getpwnam(___name) \
	AROS_LC1(struct passwd *, getpwnam, \
	AROS_LCA(const char *, (___name), A1), \
	struct Library *, USERGROUP_BASE_NAME, 19, /* s */)

#define getpwuid(___uid) \
	AROS_LC1(struct passwd *, getpwuid, \
	AROS_LCA(uid_t, (___uid), D0), \
	struct Library *, USERGROUP_BASE_NAME, 20, /* s */)

#define getuid() \
	AROS_LC0(uid_t, getuid, \
	struct Library *, USERGROUP_BASE_NAME, 8, /* s */)

#define getumask() \
	AROS_LC0(mode_t, getumask, \
	struct Library *, USERGROUP_BASE_NAME, 33, /* s */)

#define getutent() \
	AROS_LC0(struct utmp *, getutent, \
	struct Library *, USERGROUP_BASE_NAME, 39, /* s */)

#define initgroups(___name, ___basegroup) \
	AROS_LC2(int, initgroups, \
	AROS_LCA(const char *, (___name), A1), \
	AROS_LCA(gid_t, (___basegroup), D0), \
	struct Library *, USERGROUP_BASE_NAME, 18, /* s */)

#define setgid(___id) \
	AROS_LC1(int, setgid, \
	AROS_LCA(gid_t, (___id), D0), \
	struct Library *, USERGROUP_BASE_NAME, 15, /* s */)

#define setgrent() \
	AROS_LC0(void, setgrent, \
	struct Library *, USERGROUP_BASE_NAME, 26, /* s */)

#define setgroups(___ngroups, ___groups) \
	AROS_LC2(int, setgroups, \
	AROS_LCA(int, (___ngroups), D0), \
	AROS_LCA(const gid_t *, (___groups), A1), \
	struct Library *, USERGROUP_BASE_NAME, 17, /* s */)

#define setlastlog(___uid, ___name, ___host) \
	AROS_LC3(int, setlastlog, \
	AROS_LCA(uid_t, (___uid), D0), \
	AROS_LCA(char *, (___name), A0), \
	AROS_LCA(char *, (___host), A1), \
	struct Library *, USERGROUP_BASE_NAME, 42, /* s */)

#define setlogin(___buffer) \
	AROS_LC1(int, setlogin, \
	AROS_LCA(const char *, (___buffer), A1), \
	struct Library *, USERGROUP_BASE_NAME, 37, /* s */)

#define setpwent() \
	AROS_LC0(void, setpwent, \
	struct Library *, USERGROUP_BASE_NAME, 21, /* s */)

#define setregid(___real, ___eff) \
	AROS_LC2(int, setregid, \
	AROS_LCA(gid_t, (___real), D0), \
	AROS_LCA(gid_t, (___eff), D1), \
	struct Library *, USERGROUP_BASE_NAME, 14, /* s */)

#define setreuid(___real, ___eff) \
	AROS_LC2(int, setreuid, \
	AROS_LCA(uid_t, (___real), D0), \
	AROS_LCA(uid_t, (___eff), D1), \
	struct Library *, USERGROUP_BASE_NAME, 10, /* s */)

#define setsid() \
	AROS_LC0(pid_t, setsid, \
	struct Library *, USERGROUP_BASE_NAME, 34, /* s */)

#define setuid(___id) \
	AROS_LC1(int, setuid, \
	AROS_LCA(uid_t, (___id), D0), \
	struct Library *, USERGROUP_BASE_NAME, 11, /* s */)

#define setutent() \
	AROS_LC0(void, setutent, \
	struct Library *, USERGROUP_BASE_NAME, 38, /* s */)

#define ug_GetErr() \
	AROS_LC0(int, ug_GetErr, \
	struct Library *, USERGROUP_BASE_NAME, 6, /* s */)

#define ug_GetSalt(___user, ___buffer, ___size) \
	AROS_LC3(char *, ug_GetSalt, \
	AROS_LCA(const struct passwd *, (___user), A0), \
	AROS_LCA(char *, (___buffer), A1), \
	AROS_LCA(ULONG, (___size), D0), \
	struct Library *, USERGROUP_BASE_NAME, 30, /* s */)

#define ug_SetupContextTagList(___pname, ___taglist) \
	AROS_LC2(int, ug_SetupContextTagList, \
	AROS_LCA(const UBYTE*, (___pname), A0), \
	AROS_LCA(struct TagItem *, (___taglist), A1), \
	struct Library *, USERGROUP_BASE_NAME, 5, /* s */)

#ifndef NO_INLINE_STDARG
#define ug_SetupContextTags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; ug_SetupContextTagList((a0), (struct TagItem *)_tags);})
#endif /* !NO_INLINE_STDARG */

#define ug_StrError(___code) \
	AROS_LC1(const char *, ug_StrError, \
	AROS_LCA(LONG, (___code), D1), \
	struct Library *, USERGROUP_BASE_NAME, 7, /* s */)

#define umask(___mask) \
	AROS_LC1(mode_t, umask, \
	AROS_LCA(mode_t, (___mask), D0), \
	struct Library *, USERGROUP_BASE_NAME, 32, /* s */)

#endif /* !_INLINE_USERGROUP_H */
