$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) args.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) backup.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) comments.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) globs.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) indent.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) io.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) lexi.c
$ cc/prefix=all/define=HAVE_CONFIG_H/warnings=(disable=LONGEXTERN) parse.c
$ link/exe=indent.exe -
args, backup, comments, globs, indent, io, lexi, parse
