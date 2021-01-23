/** MIT License
 *
 * Copyright (c) 2020 Qv Junping
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* libc/src/string/strerror.c
 * 返回错误码对应描述
 */

#include <errno.h>
#include <string.h>

char *strerror(int errno)
{
    char *error;
    switch (errno)
    {
    case 0:
        error = "Success";
        break;
#ifdef E2BIG
    case E2BIG:
        error = "Argument list too long";
        break;
#endif
#ifdef EACCES
    case EACCES:
        error = "Permission denied";
        break;
#endif
#ifdef EADDRINUSE
    case EADDRINUSE:
        error = "Address already in use";
        break;
#endif
#ifdef EADDRNOTAVAIL
    case EADDRNOTAVAIL:
        error = "Address not available";
        break;
#endif
#ifdef EAFNOSUPPORT
    case EAFNOSUPPORT:
        error = "Address family not supported";
        break;
#endif
#ifdef EAGAIN
    case EAGAIN:
        error = "Resource temporarily unavailable";
        break;
#endif
#ifdef EALREADY
    case EALREADY:
        error = "Connection already in progress";
        break;
#endif
#ifdef EBADF
    case EBADF:
        error = "Bad file descriptor";
        break;
#endif
#ifdef EBADMSG
    case EBADMSG:
        error = "Bad message";
        break;
#endif
#ifdef EBUSY
    case EBUSY:
        error = "Device or resource busy";
        break;
#endif
#ifdef ECANCELED
    case ECANCELED:
        error = "Operation canceled";
        break;
#endif
#ifdef ECHILD
    case ECHILD:
        error = "No child processes";
        break;
#endif
#ifdef ECONNABORTED
    case ECONNABORTED:
        error = "Connection aborted";
        break;
#endif
#ifdef ECONNREFUSED
    case ECONNREFUSED:
        error = "Connection refused";
        break;
#endif
#ifdef ECONNRESET
    case ECONNRESET:
        error = "Connection reset";
        break;
#endif
#ifdef EDEADLK
    case EDEADLK:
        error = "Resource deadlock avoided";
        break;
#endif
#ifdef EDESTADDRREQ
    case EDESTADDRREQ:
        error = "Destination address required";
        break;
#endif
#ifdef EDOM
    case EDOM:
        error = "Mathematics argument out of domain of function";
        break;
#endif
#ifdef EDQUOT
    case EDQUOT:
        error = "Disk quota exceeded";
        break;
#endif
#ifdef EEXIST
    case EEXIST:
        error = "File exists";
        break;
#endif
#ifdef EFAULT
    case EFAULT:
        error = "Bad address";
        break;
#endif
#ifdef EFBIG
    case EFBIG:
        error = "File too large";
        break;
#endif
#ifdef EHOSTUNREACH
    case EHOSTUNREACH:
        error = "Host is unreachable";
        break;
#endif
#ifdef EIDRM
    case EIDRM:
        error = "Identifier removed";
        break;
#endif
#ifdef EILSEQ
    case EILSEQ:
        error = "Invalid or incomplete multibyte or wide character";
        break;
#endif
#ifdef EINPROGRESS
    case EINPROGRESS:
        error = "Operation in progress";
        break;
#endif
#ifdef EINTR
    case EINTR:
        error = "Interrupted function call";
        break;
#endif
#ifdef EINVAL
    case EINVAL:
        error = "Invalid argument";
        break;
#endif
#ifdef EIO
    case EIO:
        error = "Input/output error";
        break;
#endif
#ifdef EISCONN
    case EISCONN:
        error = "Socket is connected";
        break;
#endif
#ifdef EISDIR
    case EISDIR:
        error = "Is a directory";
        break;
#endif
#ifdef ELOOP
    case ELOOP:
        error = "Too many levels of symbolic links";
        break;
#endif
#ifdef EMFILE
    case EMFILE:
        error = "Too many open files";
        break;
#endif
#ifdef EMLINK
    case EMLINK:
        error = "Too many links";
        break;
#endif
#ifdef EMSGSIZE
    case EMSGSIZE:
        error = "Message too long";
        break;
#endif
#ifdef EMULTIHOP
    case EMULTIHOP:
        error = "Multihop attempted";
        break;
#endif
#ifdef ENAMETOOLONG
    case ENAMETOOLONG:
        error = "Filename too long";
        break;
#endif
#ifdef ENETDOWN
    case ENETDOWN:
        error = "Network is down";
        break;
#endif
#ifdef ENETRESET
    case ENETRESET:
        error = "Connection aborted by network";
        break;
#endif
#ifdef ENETUNREACH
    case ENETUNREACH:
        error = "Network unreachable";
        break;
#endif
#ifdef ENFILE
    case ENFILE:
        error = "Too many open files in system";
        break;
#endif
#ifdef ENOBUFS
    case ENOBUFS:
        error = "No buffer space available";
        break;
#endif
#ifdef ENODEV
    case ENODEV:
        error = "No such device";
        break;
#endif
#ifdef ENOENT
    case ENOENT:
        error = "No such file or directory";
        break;
#endif
#ifdef ENOEXEC
    case ENOEXEC:
        error = "Exec format error";
        break;
#endif
#ifdef ENOLCK
    case ENOLCK:
        error = "No locks available";
        break;
#endif
#ifdef ENOLINK
    case ENOLINK:
        error = "Link has been severed";
        break;
#endif
#ifdef ENOMEM
    case ENOMEM:
        error = "Not enough space/cannot allocate memory";
        break;
#endif
#ifdef ENOMSG
    case ENOMSG:
        error = "No message of the desired type";
        break;
#endif
#ifdef ENOPROTOOPT
    case ENOPROTOOPT:
        error = "Protocol not available";
        break;
#endif
#ifdef ENOSPC
    case ENOSPC:
        error = "No space left on device";
        break;
#endif
#ifdef ENOSYS
    case ENOSYS:
        error = "Function not implemented";
        break;
#endif
#ifdef ENOTCONN
    case ENOTCONN:
        error = "The socket is not connected";
        break;
#endif
#ifdef ENOTDIR
    case ENOTDIR:
        error = "Not a directory";
        break;
#endif
#ifdef ENOTEMPTY
    case ENOTEMPTY:
        error = "Directory not empty";
        break;
#endif
#ifdef ENOTRECOVERABLE
    case ENOTRECOVERABLE:
        error = "State not recoverable";
        break;
#endif
#ifdef ENOTSOCK
    case ENOTSOCK:
        error = "Not a socket";
        break;
#endif
#ifdef ENOTSUP
    case ENOTSUP:
        error = "Operation not supported";
        break;
#endif
#ifdef ENOTTY
    case ENOTTY:
        error = "Inappropriate I/O control operation";
        break;
#endif
#ifdef ENXIO
    case ENXIO:
        error = "No such device or address";
        break;
#endif
#ifdef EOPNOTSUPP
    case EOPNOTSUPP:
        error = "Operation not supported on socket";
        break;
#endif
#ifdef EOVERFLOW
    case EOVERFLOW:
        error = "Value too large to be stored in data type";
        break;
#endif
#ifdef EOWNERDEAD
    case EOWNERDEAD:
        error = "Owner died";
        break;
#endif
#ifdef EPERM
    case EPERM:
        error = "Operation not permitted";
        break;
#endif
#ifdef EPIPE
    case EPIPE:
        error = "Broken pipe";
        break;
#endif
#ifdef EPROTO
    case EPROTO:
        error = "Protocol error";
        break;
#endif
#ifdef EPROTONOSUPPORT
    case EPROTONOSUPPORT:
        error = "Protocol not supported";
        break;
#endif
#ifdef EPROTOTYPE
    case EPROTOTYPE:
        error = "Protocol wrong type for socket";
        break;
#endif
#ifdef ERANGE
    case ERANGE:
        error = "Result too large";
        break;
#endif
#ifdef EROFS
    case EROFS:
        error = "Read-only filesystem";
        break;
#endif
#ifdef ESPIPE
    case ESPIPE:
        error = "Invalid seek";
        break;
#endif
#ifdef ESRCH
    case ESRCH:
        error = "No such process";
        break;
#endif
#ifdef ESTALE
    case ESTALE:
        error = "Stale file handle";
        break;
#endif
#ifdef ETIMEDOUT
    case ETIMEDOUT:
        error = "Connection timed out";
        break;
#endif
#ifdef ETXTBSY
    case ETXTBSY:
        error = "Text file busy";
        break;
#endif
#ifdef EWOULDBLOCK
    case EWOULDBLOCK:
        error = "Operation would block";
        break;
#endif
#ifdef EXDEV
    case EXDEV:
        error = "Improper link";
        break;
#endif
    default:
        error = "Unknown error";
        break;
    }
    return error;
}