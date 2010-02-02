#ifdef __MORPHOS__
#include <errno.h>
#include <caf/errors.h>

static char *g_noerrno = "Error code not found in errno table";

#define MAX_ERRNO 80
static struct ErrnoEntry ErrnoTable[MAX_ERRNO] = 
{
	{ EPERM,  "Operation not permitted" },
	{ ENOENT,  "No such file or directory" },
	{ ESRCH,  "No such process" },
	{ EINTR,  "Interrupted system call" },
	{ EIO,  "Input/output error" },
	{ ENXIO,  "Device not configured" },
	{ E2BIG,  "Argument list too long" },
	{ ENOEXEC,  "Exec format error" },
	{ EBADF,  "Bad file descriptor" },
	{ ECHILD,  "No child processes" },
	{ EDEADLK,  "Resource deadlock avoided" },
	{ ENOMEM,  "Cannot allocate memory" },
	{ EACCES,  "Permission denied" },
	{ EFAULT,  "Bad address" },
	{ ENOTBLK,  "Block device required" },
	{ EBUSY,  "Device busy" },
	{ EEXIST,  "File exists" },
	{ EXDEV,  "Cross-device link" },
	{ ENODEV,  "Operation not supported by device" },
	{ ENOTDIR,  "Not a directory" },
	{ EISDIR,  "Is a directory" },
	{ EINVAL,  "Invalid argument" },
	{ ENFILE,  "Too many open files in system" },
	{ EMFILE,  "Too many open files" },
	{ ENOTTY,  "Inappropriate ioctl for device" },
	{ ETXTBSY,  "Text file busy" },
	{ EFBIG,  "File too large" },
	{ ENOSPC,  "No space left on device" },
	{ ESPIPE,  "Illegal seek" },
	{ EROFS,  "Read-only file system" },
	{ EMLINK,  "Too many links" },
	{ EPIPE,  "Broken pipe" },
	{ EDOM,  "Numerical argument out of domain" },
	{ ERANGE,  "Result too large" },
	{ EAGAIN,  "Resource temporarily unavailable" },
	{ EWOULDBLOCK,  "Operation would block" },
	{ EINPROGRESS,  "Operation now in progress" },
	{ EALREADY,  "Operation already in progress" },
	{ ENOTSOCK,  "Socket operation on non-socket" },
	{ EDESTADDRREQ,  "Destination address required" },
	{ EMSGSIZE,  "Message too long" },
	{ EPROTOTYPE,  "Protocol wrong type for socket" },
	{ ENOPROTOOPT,  "Protocol not available" },
	{ EPROTONOSUPPORT,  "Protocol not supported" },
	{ ESOCKTNOSUPPORT,  "Socket type not supported" },
	{ EOPNOTSUPP,  "Operation not supported on socket" },
	{ EPFNOSUPPORT,  "Protocol family not supported" },
	{ EAFNOSUPPORT,  "Address family not supported by protocol family" },
	{ EADDRINUSE,  "Address already in use" },
	{ EADDRNOTAVAIL,  "Can't assign requested address" },
	{ ENETDOWN,  "Network is down" },
	{ ENETUNREACH,  "Network is unreachable" },
	{ ENETRESET,  "Network dropped connection on reset" },
	{ ECONNABORTED,  "Software caused connection abort" },
	{ ECONNRESET,  "Connection reset by peer" },
	{ ENOBUFS,  "No buffer space available" },
	{ EISCONN,  "Socket is already connected" },
	{ ENOTCONN,  "Socket is not connected" },
	{ ESHUTDOWN,  "Can't send after socket shutdown" },
	{ ETOOMANYREFS,  "Too many references: can't splice" },
	{ ETIMEDOUT,  "Connection timed out" },
	{ ECONNREFUSED,  "Connection refused" },
	{ ELOOP,  "Too many levels of symbolic links" },
	{ ENAMETOOLONG,  "File name too long" },
	{ EHOSTDOWN,  "Host is down" },
	{ EHOSTUNREACH,  "No route to host" },
	{ ENOTEMPTY,  "Directory not empty" },
	{ EPROCLIM,  "Too many processes" },
	{ EUSERS,  "Too many users" },
	{ EDQUOT,  "Disc quota exceeded" },
	{ ESTALE,  "Stale NFS file handle" },
	{ EREMOTE,  "Too many levels of remote in path" },
	{ EBADRPC,  "RPC struct is bad" },
	{ ERPCMISMATCH,  "RPC version wrong" },
	{ EPROGUNAVAIL,  "RPC prog. not avail" },
	{ EPROGMISMATCH,  "Program version wrong" },
	{ EPROCUNAVAIL,  "Bad procedure for program" },
	{ ENOLCK,  "No locks available" },
	{ ENOSYS,  "Function not implemented" },
	{ EFTYPE,  "Inappropriate file type or format" }
};

char *GetErrnoDesc(int err_code)
{
	int i;
	
	for (i = 0; i < MAX_ERRNO; i++) {
		if (ErrnoTable[i].error_code == err_code)
			return ErrnoTable[i].error_desc;
	}
	
	return g_noerrno;
}
#endif
