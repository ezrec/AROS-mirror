//----------------------------------------------------------------------------
// file.c:
//
// File operations
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "file.h"
#include "gui.h"
#include "probe.h"
#include "resource.h"
#include "strop.h"
#include "util.h"

#include <dirent.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#ifdef AMIGA
#include <dos/dos.h>
#include <dos/dosasl.h>
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <workbench/workbench.h>
#endif

//----------------------------------------------------------------------------
// (expandpath <path>)
//     Expands a short path to its full path equivalent
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_expandpath(entry_p contxt)
{
    // We need one argument, a short path.
    if(c_sane(contxt, 1))
    {
        // Short path.
        char *pth = str(CARG(1));

        if(!DID_ERR())
        {
            #ifdef AMIGA
            BPTR lock = (BPTR) Lock(pth, ACCESS_READ);

            if(lock)
            {
                if(NameFromLock(lock, get_buf(), buf_size()))
                {
                    UnLock(lock);
                    RSTR(strdup(get_buf()));
                }
                else
                {
                    ERR(ERR_OVERFLOW, pth);
                    UnLock(lock);
                }
            }
            #else
            RSTR(strdup(pth));
            #endif
        }

        // Return empty string.
        REST;
    }

    // Broken parser
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// Name:        h_confirm
// Description: Ask user for confirmation (proceed / skip / abort).
// Input:       entry_p contxt:     The execution context.
//              const char *hlp:    Help text.
//              const char *msg:    Message format string.
//              ...:                Format string varargs.
// Return:      bool:               If confirmed 'true' else 'false'. Both
//                                  skip and abort will return 'false'.
//----------------------------------------------------------------------------
bool h_confirm(entry_p contxt,
               const char *hlp,
               const char *msg,
               ...)
{
    if(contxt)
    {
        // By setting @yes, @skip or @abort
        // user behaviour can be simulated.
        int yes = get_numvar(contxt, "@yes"),
            skip = get_numvar(contxt, "@skip"),
            abort = get_numvar(contxt, "@abort");

        inp_t rc;
        va_list ap;

        // Format messsage string.
        va_start(ap, msg);
        vsnprintf(get_buf(), buf_size(), msg, ap);
        va_end(ap);

        // If we have any overrides, translate
        // them to the corresponding gui return
        // value.
        if(abort) // || skip || abort)
        {
            rc = G_ABORT;
        }
        else
        if(skip)
        {
            rc = G_FALSE;
        }
        else
        if(yes)
        {
            rc = G_TRUE;
        }
        // No overrides, get confirmation.
        else
        {
            entry_p back = get_opt(contxt, OPT_BACK);

            rc = gui_confirm(get_buf(), hlp, back);

            // Is the back option available?
            if(back)
            {
                // Fake input?
                if(get_numvar(contxt, "@back"))
                {
                    rc = G_ABORT;
                }

                // On abort execute.
                if(rc == G_ABORT)
                {
                    rc = resolve(back) ?
                         G_TRUE : G_ERR;
                }
            }
        }

        // FIXME
        if(rc == G_ABORT || rc == G_EXIT)
        {
            HALT();
        }

        // FIXME
        return rc == G_TRUE;
    }

    // Broken parser.
    PANIC(contxt);
    return false;
}

//----------------------------------------------------------------------------
// Name:        h_exists
// Description: Get file / dir info.
// Input:       entry_p contxt:     The execution context.
//              const char *n:      Path to file / dir.
// Return:      int:                Dir = '2', file = '1' else '0'.
//----------------------------------------------------------------------------
int h_exists(const char *n)
{
    // NULL is a valid argument but
    // this 'file' doesn't exist.
    if(n)
    {
        // Empty string = current dir.
        if(*n)
        {
            #ifdef AMIGA
            int r = 0;
            struct FileInfoBlock *fib = (struct FileInfoBlock *)
                   AllocDosObject(DOS_FIB, NULL);

            if(fib)
            {
                BPTR lock = (BPTR) Lock(n, ACCESS_READ);

                if(lock)
                {
                    if(Examine(lock, fib))
                    {
                        // ST_FILE         -3
                        // ST_LINKFILE     -4
                        if(fib->fib_DirEntryType < 0)
                        {
                            // A file.
                            r = 1;
                        }
                        // ST_ROOT          1
                        // ST_USERDIR       2
                        // ST_SOFTLINK      3
                        // ST_LINKDIR       4
                        else if(fib->fib_DirEntryType > 0)
                        {
                            // A directory.
                            r = 2;
                        }
                        // Return values according to the
                        // CBM installer documentation.
                    }

                    UnLock(lock);
                }

                FreeDosObject(DOS_FIB, fib);
            }

            return r;
            #else
            // This implementation doesn't work on MorphOS.
            // I have no clue why, it works on AROS. Let's
            // use the implementation above on all Amiga
            // like systems for now.
            struct stat fs;
            if(!stat(n, &fs))
            {
                // A file?
                if(S_ISREG(fs.st_mode))
                {
                    // Value according to the CBM
                    // installer documentation.
                    return 1;
                }

                // A directory?
                if(S_ISDIR(fs.st_mode))
                {
                    // Ibid.
                    return 2;
                }
            }
            #endif
        }
        else
        {
            // The current dir exists,
            // and it's a dir, really.
            return 2;
        }
    }

    // Ibid.
    return 0;
}

//----------------------------------------------------------------------------
// Name:        h_fileonly
// Description: Get file part from full path.
// Input:       entry_p contxt:     The execution context.
//              const char *n:      Path to file.
// Return:      const char *:       On success, file part of path, otherwise
//                                  empty string.
//----------------------------------------------------------------------------
static const char *h_fileonly(entry_p contxt,
                              const char *s)
{
    if(s)
    {
        size_t i = strlen(s);

        // Do we have a string that doesn't
        // look like a directory / volume?
        if(i-- &&
           s[i] != '/' &&
           s[i] != ':' )
        {
            // Go backwards until we find a
            // delimiter or the beginning of
            // the string.
            while(i &&
                  s[i - 1] != '/' &&
                  s[i - 1] != ':' )
            {
                i--;
            }

            // Return the new offset.
            return (s + i);
        }

        // Only fail if we're in 'strict' mode.
        if(get_numvar(contxt, "@strict"))
        {
            // Empty string or dir / vol.
            ERR(ERR_NOT_A_FILE, s);
        }
    }
    else
    {
        // Bad input.
        PANIC(contxt);
    }

    // Always return a valid
    // string pointer.
    return "";
}

//----------------------------------------------------------------------------
// Forward declaration needed by h_choices.
static pnode_p h_filetree(entry_p contxt, const char *src, const char *dst,
                          entry_p files, entry_p fonts, entry_p choices,
                          entry_p pattern);
//----------------------------------------------------------------------------
// Name:        h_choices
// Description: Helper fo h_filetree taking care of (choices). Generating a
//              complete file / directory tree with source and destination
//              tuples.
// Input:       entry_p contxt:     The execution context.
//              entry_p choices:    * List of files.
//              entry_p fonts:      * Include fonts.
//
//              * Refer to the Installer.guide.
//
//              const char *src:    Source directory.
//              const char *dst:    Destination directory.
// Return:      entry_p:            A linked list of file and dir pairs.
//----------------------------------------------------------------------------
static pnode_p h_choices(entry_p contxt,
                         entry_p choices,
                         entry_p fonts,
                         const char *src,
                         const char *dst)
{
    if(contxt && choices && src && dst)
    {
        // Unless the parser is broken,
        // we will have >= one child.
        entry_p *e = choices->children;

        // Create head node.
        pnode_p node = DBG_ALLOC(calloc(1, sizeof(struct pnode_t))),
                head = node;

        if(node)
        {
            // We already know the type of the
            // first element; it's a directory.
            node->name = strdup(src);
            node->copy = strdup(dst);
            node->type = 2;

            // Not necessary to check the return value.
            node->next = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));
            node = node->next;

            // Iterate over all files / dirs.
            while(node)
            {
                // Resolve current file.
                char *f_nam = str(*e);

                // Build source <-> dest pair.
                node->name = h_tackon(contxt, src, f_nam);
                node->copy = h_tackon(contxt, dst, h_fileonly(contxt, f_nam));
                node->type = h_exists(node->name);

                // Next file.
                e++;

                // Make sure that the file / dir exists. But only
                // in strict mode, otherwise just go on, missing
                // files will be skipped during file copy anyway.
                if(node->type || !get_numvar(contxt, "@strict"))
                {
                    if(node->type == 2)
                    {
                        #ifndef AMIGA
                        if(strcmp(f_nam, ".") &&
                           strcmp(f_nam, ".."))
                        #endif
                        {
                            // Get tree of subdirectory.
                            // Don't promote (choices).
                            node->next = h_filetree
                            (
                                contxt,
                                node->name,
                                node->copy,
                                NULL,
                                NULL,
                                NULL,
                                NULL
                            );

                            // Fast forward to the end of
                            // the list.
                            while(node->next)
                            {
                                node = node->next;
                            }
                        }
                    }

                    // If there are more files, allocate
                    // memory for the next node.
                    if(*e && *e != end())
                    {
                        // Not necessary to check the return value.
                        node->next = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));
                    }

                    // Copy the font (if any) as well?
                    if(fonts)
                    {
                        // Font = file + .font.
                        snprintf(get_buf(), buf_size(), "%s.font", node->name);

                        // Only if the font is a file.
                        if(h_exists(get_buf()) == 1)
                        {
                            // This might be the last file / dir.
                            // If true, no next element will exist
                            // at this point so we need to create it.
                            if(!node->next)
                            {
                                // Not necessary to check the return value.
                                node->next = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));
                            }

                            // Proceed with font.
                            node = node->next;

                            // Fill out the details.
                            if(node)
                            {
                                // We already have the font name, create the
                                // copy and set type, always a file.
                                node->name = strdup(get_buf());
                                node->copy = h_tackon(contxt, dst, h_fileonly(contxt, get_buf()));
                                node->type = 1;

                                // Unless we ran out of memory, and there are
                                // more files / dirs, create new list element.
                                if(node->name && node->copy
                                   && *e && *e != end())
                                {
                                    // Not necessary to check the return value.
                                    node->next = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));
                                }
                            }
                        }
                    }
                }
                else
                {
                    // File or directory doesn't exist.
                    ERR(ERR_NO_SUCH_FILE_OR_DIR, node->name);
                }

                // Next element.
                node = node ? node->next : node;
            }

            // List complete.
            return head;
        }
    }

    // Unknown error /
    // Out of memory.
    PANIC(contxt);
    return NULL;
}

//----------------------------------------------------------------------------
// Name:        h_filetree
// Description: Generate a complete file / directory tree with source and
//              destination tuples. Used by m_copyfiles.
// Input:       entry_p contxt:     The execution context.
//              const char *src:    Source directory / file.
//              const char *dst:    Destination directory.
//              entry_p files:      * Files only.
//              entry_p fonts:      * Skip fonts.
//              entry_p choices:    * List of files.
//              entry_p pattern:    * File / dir pattern.
//
//              * Refer to the Installer.guide.
//
// Return:      entry_p:            A linked list of file and dir pairs.
//----------------------------------------------------------------------------
static pnode_p h_filetree(entry_p contxt,
                         const char *src,
                         const char *dst,
                         entry_p files,
                         entry_p fonts,
                         entry_p choices,
                         entry_p pattern)
{
    char *n_src = NULL,
         *n_dst = NULL;

    if(src && dst)
    {
        // File enumeration.
        if(choices)
        {
            // No need for recursion. Handle
            // enumerations separately.
            return h_choices(contxt, choices,
                             fonts, src, dst);
        }

        int type = h_exists(src);

        // Is source a directory?
        if(type == 2)
        {
            DIR *dir = opendir(src);

            if(dir)
            {
                // Create head node.
                pnode_p node = DBG_ALLOC(calloc(1, sizeof(struct pnode_t))),
                        head = node;

                if(node)
                {
                    struct dirent *entry = readdir(dir);

                    // We already know the type of the
                    // first element; it's a directory.
                    node->name = strdup(src);
                    node->copy = strdup(dst);
                    node->type = 2;

                    // Iterate over all entries in the source
                    // directory.
                    while(entry)
                    {
                        // Create the source destination tuple
                        n_src = h_tackon(contxt, src, entry->d_name),
                        n_dst = h_tackon(contxt, dst, entry->d_name);

                        if(n_src && n_dst)
                        {
                            if(pattern)
                            {
                                // Use a static buffer, Installer.guide
                                // restricts pattern length to 64. It
                                // seems like MatchPattern can use a lot
                                // of stack if we use long patterns, so
                                // let's not get rid of this limitation.
                                static char pat[BUFSIZ];
                                #ifdef AMIGA
                                LONG w = ParsePattern(str(pattern), pat, sizeof(pat));

                                // Can we parse the pattern?
                                if(w >= 0)
                                {
                                    // Use pattern matching if we have one or more
                                    // wildcards, otherwise use plain strcmp().
                                    if((w && MatchPattern(pat, entry->d_name)) ||
                                       (w && !strcmp(pat, entry->d_name)))
                                    {
                                        // Match, get proper type.
                                        type = h_exists(n_src);
                                    }
                                    else
                                    {
                                        // Not a match, skip this.
                                        type = 0;
                                    }
                                }
                                else
                                {
                                    // We probably had a buffer overflow.
                                    // No more pattern matching today.
                                    ERR(ERR_OVERFLOW, str(pattern));
                                    pattern = NULL;
                                }
                                #else
                                // Get rid of warning.
                                *pat = '\0';
                                type = h_exists(n_src);
                                #endif
                            }
                            else
                            {
                                // File or directory?
                                type = h_exists(n_src);
                            }

                            // If we have a directory, recur.
                            if(type == 2)
                            {
                                // Unless the (files) option is set.
                                if(!files)
                                {
                                    #ifndef AMIGA
                                    if(strcmp(entry->d_name, ".") &&
                                       strcmp(entry->d_name, ".."))
                                    #endif
                                    {
                                        // Get tree of subdirectory.
                                        // Don't promote (choices),
                                        // dirs will be assumed to
                                        // be files and things will
                                        // break.
                                        node->next = h_filetree
                                        (
                                            contxt,
                                            n_src,
                                            n_dst,
                                            files,
                                            fonts,
                                            NULL,
                                            pattern
                                        );

                                        // Fast forward to the end of
                                        // the list.
                                        while(node->next)
                                        {
                                            node = node->next;
                                        }
                                    }
                                }

                                // We don't need to store the names
                                // of directories, release them.
                                free(n_src);
                                free(n_dst);
                            }
                            else
                            {
                                node->next = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));

                                if(node->next)
                                {
                                    node->next->type = type;
                                    node->next->name = n_src;
                                    node->next->copy = n_dst;
                                    node = node->next;
                                }
                                else
                                {
                                    // Out of memory.
                                    PANIC(contxt);

                                    // Free what we have and exit.
                                    free(n_src);
                                    free(n_dst);
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // Out of memory.
                            break;
                        }

                        // Get next entry.
                        entry = readdir(dir);
                    }
                }

                // No more entries.
                closedir(dir);

                // The list is complete.
                return head;
            }

            // Could not read from dir.
            ERR(ERR_READ_DIR, src);
            return NULL;
        }

        // Is source a file?
        if(type == 1)
        {
            pnode_p file = DBG_ALLOC(calloc(1, sizeof(struct pnode_t))),
                    head = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));

            if(head && file)
            {
                n_src = strdup(src);
                n_dst = strdup(dst);

                if(n_src && n_dst)
                {
                    // The destination of the head element
                    // will be a directory even though the
                    // source is a file. We need somewhere
                    // to put the file.
                    head->type = 2;
                    head->next = file;
                    head->name = n_src;
                    head->copy = n_dst;

                    // Create destination file path.
                    n_dst = h_tackon(contxt, dst, h_fileonly(contxt, src));
                    n_src = strdup(src);

                    if(n_src && n_dst)
                    {
                        // The second element in the list
                        // will be the file.
                        file->type = 1;
                        file->name = n_src;
                        file->copy = n_dst;

                        if(fonts)
                        {
                            // Font = file + .font.
                            snprintf(get_buf(), buf_size(), "%s.font", n_src);

                            if(h_exists(get_buf()) == 1)
                            {
                                pnode_p font = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));

                                if(font)
                                {
                                    font->name = strdup(get_buf());
                                    font->copy = h_tackon(contxt, dst, h_fileonly(contxt, get_buf()));

                                    // Add the font to the list.
                                    if(font->name && font->copy)
                                    {
                                        font->type = 1;
                                        file->next = font;

                                        // The list is complete.
                                        return head;
                                    }

                                    // Out of memory.
                                    free(font->name);
                                    free(font->copy);
                                    free(font);
                                }
                            }
                        }
                        else
                        {
                            // The list is complete.
                            return head;
                        }
                    }

                    // Out of memory.
                    free(head->name);
                    free(head->copy);
                }
            }

            // Out of memory.
            free(head);
            free(file);
        }
        else
        // It's neither a directory or a file.
        {
            ERR(ERR_NO_SUCH_FILE_OR_DIR, src);
            return NULL;
        }
    }

    // Out of memory.
    PANIC(contxt);

    // These might leak when OOM
    // if we don't free them.
    free(n_src);
    free(n_dst);

    return NULL;
}

//----------------------------------------------------------------------------
// Name:        h_protect_get
// Description: Utility function used by m_protect and m_copyfiles to get
//              file / dir protection bits.
// Input:       entry_p contxt:     The execution context.
//              const char *file:   File / dir.
//              int32_t *mask:      Pointer to the result.
// Return:      int:                On success '1', else '0'.
//----------------------------------------------------------------------------
static int h_protect_get(entry_p contxt,
                         char *file,
                         int32_t *mask)
{
    if(contxt && mask && file)
    {
        // On non Amiga systems, this is a stub.
        #ifdef AMIGA
        struct FileInfoBlock *fib = (struct FileInfoBlock *)
            AllocDosObject(DOS_FIB, NULL);

        if(fib)
        {
            // Attempt to lock file / dir.
            BOOL done = FALSE;
            BPTR lock = (BPTR) Lock(file, ACCESS_READ);

            // Did we obtain a lock?
            if(lock)
            {
                // Fill up FIB and get bits.
                if(Examine(lock, fib))
                {
                    *mask = fib->fib_Protection;
                    done = TRUE;
                }

                UnLock(lock);
            }

            // Free FIB memory.
            FreeDosObject(DOS_FIB, fib);

            // Did everything above succeed?
            if(!done)
            {
                // No, fail and set impossible mask.
                ERR(ERR_GET_PERM, file);
                *mask = -1;
            }

            // If enabled, write to log file.
            h_log(contxt, tr(S_GMSK), file, *mask);

            // MASK or -1.
            return done;
        }
        #else
        // Always succeed on non Amiga systems.
        return 1;
        #endif
    }

    // Out of memory.
    PANIC(contxt);
    return 0;
}

//----------------------------------------------------------------------------
// Name:        h_protect_set
// Description: Utility function used by m_protect and m_copyfiles to set
//              file / dir protection bits.
// Input:       entry_p contxt:     The execution context.
//              const char *file:   File / dir.
//              LONG mask:          Protection bits
// Return:      int:                On success '1', else '0'.
//----------------------------------------------------------------------------
static int h_protect_set(entry_p contxt,
                         const char *file,
                         LONG mask)
{
    if(contxt && file)
    {
        // On non Amiga systems this is a stub.
        #ifdef AMIGA
        size_t len = strlen(file);

        // Filter out volumes.
        if(len && file[len - 1] != ':')
        {
            if(!SetProtection(file, mask))
            {
                // Only fail if we're in 'strict' mode.
                if(get_numvar(contxt, "@strict"))
                {
                    ERR(ERR_SET_PERM, file);
                    return 0;
                }
            }
        }
        #endif

        // If logging is enabled, write to log.
        h_log(contxt, tr(S_PTCT), file, mask);
        return 1;
    }

    // Bad input.
    PANIC(contxt);
    return 0;
}

#define CF_INFOS        (1 << 0)
#define CF_FONTS        (1 << 1)
#define CF_NOGAUGE      (1 << 2)
#define CF_NOFAIL       (1 << 3)
#define CF_OKNODELETE   (1 << 4)
#define CF_FORCE        (1 << 5)
#define CF_ASKUSER      (1 << 6)
#define CF_NOPOSITION   (1 << 7)
#define CF_SILENT       (1 << 8)

//----------------------------------------------------------------------------
// Name:        h_copyfile
// Description: Copy file. Helper used by m_copyfiles and m_copylib.
// Input:       entry_p contxt:     The execution context.
//              char *src:          Source file.
//              char *dst:          Destination file.
//              int mde:            Copy mode, see CF_*.
//              bool bck:           Enable back mode.
// Return:      inp_t:              G_TRUE / G_FALSE / G_ABORT / G_ERR.
//----------------------------------------------------------------------------
static inp_t h_copyfile(entry_p contxt,
                        char *src,
                        char *dst,
                        bool bck,
                        int mde)
{
    if(contxt && src && dst)
    {
        // GUI return code.
        inp_t rc = G_TRUE;

        // Show GUI unless we're in silent mode.
        if(!(mde & CF_SILENT))
        {
            rc = gui_copyfiles_setcur(src, mde & CF_NOGAUGE, bck);
        }

        // Prepare GUI unless we're in silent mode
        // as used by copylib.
        if(rc == G_TRUE)
        {
            static char buf[BUFSIZ];
            FILE *fs = fopen(src, "r");
            size_t n = fs ? fread(buf, 1, BUFSIZ, fs) : 0;
            int err =
                #ifdef AMIGA
                IoErr();
                #else
                fs ? ferror(fs) : 0;
                #endif

            if(fs && !err)
            {
                // Is there an existing destination
                // file that is write protected?
                if(!access(dst, F_OK) &&
                    access(dst, W_OK))
                {
                    // No need to ask if only (force).
                    if((mde & CF_FORCE) &&
                      !(mde & CF_ASKUSER))
                    {
                        // Unprotect file.
                        chmod(dst, S_IRWXU);
                    }
                    else
                    // Ask for confirmation if (askuser).
                    if(mde & CF_ASKUSER)
                    {
                        // Unless we're running in novice mode
                        // and use (force) at the same time.
                        if((mde & CF_FORCE) ||
                            get_numvar(contxt, "@user-level"))
                        {
                            if(h_confirm(contxt, "", tr(S_OWRT), dst))
                            {
                                // Unprotect file.
                                chmod(dst, S_IRWXU);
                            }
                            else
                            {
                                // Skip file or abort.
                                fclose(fs);
                                return DID_HALT() ?
                                       G_ABORT : G_TRUE;
                            }
                        }
                    }
                }

                // Create / overwrite file.
                FILE *fd = fopen(dst, "w");

                if(fd)
                {
                    // Read and write until there is nothing more
                    // to read.
                    while(n)
                    {
                        if(fwrite(buf, 1, n, fd) == n)
                        {
                            // Update GUI unless we're in silent mode.
                            if(!(mde & CF_SILENT))
                            {
                                rc = gui_copyfiles_setcur(NULL, mde & CF_NOGAUGE, bck);
                            }

                            if(rc == G_TRUE)
                            {
                                n = fread(buf, 1, sizeof(buf), fs);
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            ERR(ERR_WRITE_FILE, dst);
                            rc = G_FALSE;
                            break;
                        }
                    }

                    // Close input and output files.
                    fclose(fs);
                    fclose(fd);

                    // The number of bytes read and not written
                    // should be zero.
                    if(!n)
                    {
                        // Write to the log file (if logging is
                        // enabled).
                        h_log(contxt, tr(S_CPYD), src, dst);

                        // Are we going to copy the icon as well?
                        if(mde & CF_INFOS)
                        {
                            // The source icon.
                            static char is[PATH_MAX];
                            snprintf(is, sizeof(is), "%s.info", src);

                            // Only if it exists, it's not an error
                            // if it's missing.
                            if(h_exists(is) == 1)
                            {
                                static char id[PATH_MAX];

                                // The destination icon.
                                snprintf(id, sizeof(id), "%s.info", dst);

                                // Recur without info set.
                                rc = h_copyfile(contxt, is, id, bck, mde & ~CF_INFOS);

                                // Reset icon position?
                                if(rc == G_TRUE && mde & CF_NOPOSITION)
                                {
                                    #ifdef AMIGA
                                    struct DiskObject *obj = (struct DiskObject *)
                                        GetDiskObject(dst);

                                    if(obj)
                                    {
                                        // Reset icon position.
                                        obj->do_CurrentX = NO_ICON_POSITION;
                                        obj->do_CurrentY = NO_ICON_POSITION;

                                        // Save the changes to the .info file.
                                        if(!PutDiskObject(dst, obj))
                                        {
                                            // We failed for some unknown reason.
                                            ERR(ERR_WRITE_FILE, id);
                                            rc = G_FALSE;
                                        }

                                        FreeDiskObject(obj);
                                    }
                                    #endif
                                }
                            }
                        }

                        // Preserve file permissions. On err,
                        // code will be set by h_protect_x().
                        int32_t prm = 0;

                        if(h_protect_get(contxt, src, &prm))
                        {
                            h_protect_set(contxt, dst, prm);
                        }

                        // Reset error codes if necessary.
                        if(DID_ERR())
                        {
                            if(mde & CF_NOFAIL)
                            {
                                // Forget all errors.
                                RESET();
                            }
                            else
                            {
                                // Fail for real.
                                rc = G_ABORT;
                            }
                        }
                    }
                }
                else
                {
                    // The source handle is open.
                    fclose(fs);

                    if((mde & CF_NOFAIL) ||
                       (mde & CF_OKNODELETE))
                    {
                        // Ignore failure.
                        h_log(contxt, tr(S_NCPY), src, dst);
                        rc = G_TRUE;
                    }
                    else
                    {
                        // Fail for real.
                        ERR(ERR_WRITE_FILE, dst);
                        rc = G_FALSE;
                    }
                }
            }
            else
            {
                // The source handle might be open.
                if(fs)
                {
                    // Not true on non Amiga systems.
                    fclose(fs);
                }

                if(mde & CF_NOFAIL)
                {
                    // Ignore failure.
                    h_log(contxt, tr(S_NCPY), src, dst);
                    rc = G_TRUE;
                }
                else
                {
                    // Fail for real.
                    ERR(ERR_READ_FILE, src);
                    rc = G_FALSE;
                }
            }
        }
        else
        {
            HALT();
            h_log(contxt, tr(S_ACPY), src, dst);
        }

        // Unknown status.
        return rc;
    }

    // Unknown error.
    return G_ERR;
}

//----------------------------------------------------------------------------
// Name:        h_makedir
// Description: Create directory / tree of directories.
// Input:       entry_p contxt:     The execution context.
//              const char *dst:    The directory.
//              int mode:           FIXME.
// Return:      int:                On success '1', else '0'.
//----------------------------------------------------------------------------
static int h_makedir(entry_p contxt, const char *dst, int mode)
{
    if(contxt && dst)
    {
        char *dir;

        // Return immediately if directory exists.
        if(h_exists(dst) == 2)
        {
            // Nothing to do.
            h_log(contxt, tr(S_EDIR), dst);
            return 1;
        }

        // Create working copy.
        dir = strdup(dst);

        if(dir)
        {
            int d = 1,
                l = (int) strlen(dir);

            // Get directory depth.
            for(int i = 0; i < l; i++)
            {
                if(dir[i] == '/')
                {
                    d++;
                }
            }

            // Maximum number of retries == depth.
            while(d--)
            {
                // Shrink scope until mkdir works.
                for(int i = l; i >= 0; i--)
                {
                    // Is the current char a delimiter?
                    if(dir[i] == '/' ||
                       dir[i] == '\0')
                    {
                        // Save delimiter.
                        char c = dir[i];

                        // Truncate string.
                        dir[i] = '\0';

                        // Attempt to create path.
                        if(mkdir(dir, 0777) == 0)
                        {
                            // Is this the full path?
                            if(i == l)
                            {
                                free(dir);
                                h_log(contxt, tr(S_CRTD), dst);

                                // Yes, we're done.
                                return 1;
                            }

                            // Not the full path, reinstate
                            // delimiter and start all over
                            // with the full path.
                            dir[i] = c;
                            break;
                        }
                        else
                        {
                            // Reinstate delimiter and shrink
                            // scope.
                            dir[i] = c;
                        }
                    }
                }
            }

            // Free working copy.
            free(dir);

            if(mode)
            {
                // FIXME
                return 1;
            }

            // For some unknown reason, we can't
            // create the directory.
            ERR(ERR_WRITE_DIR, dst);
        }
        else
        {
            // Out of memory.
            PANIC(contxt);
        }
    }

    // Unknown error.
    return 0;
}

//----------------------------------------------------------------------------
// (copyfiles (prompt..) (help..) (source..) (dest..) (newname..) (choices..)
//     (all) (pattern..) (files) (infos) (confirm..) (safe) (optional
//     <option> <option> ...) (delopts <option> <option> ...) (nogauge))
//
//     copy files (and subdir's by default).  files option say NO
//     subdirectories
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_copyfiles(entry_p contxt)
{
    // We need atleast one argument
    if(c_sane(contxt, 1))
    {
        entry_p prompt     = get_opt(contxt, OPT_PROMPT),
                help       = get_opt(contxt, OPT_HELP),
                source     = get_opt(contxt, OPT_SOURCE),
                dest       = get_opt(contxt, OPT_DEST),
                newname    = get_opt(contxt, OPT_NEWNAME),
                choices    = get_opt(contxt, OPT_CHOICES),
                all        = get_opt(contxt, OPT_ALL),
                pattern    = get_opt(contxt, OPT_PATTERN),
                infos      = get_opt(contxt, OPT_INFOS),
                confirm    = get_opt(contxt, OPT_CONFIRM),
                safe       = get_opt(contxt, OPT_SAFE),
                nogauge    = get_opt(contxt, OPT_NOGAUGE),
                fonts      = get_opt(contxt, OPT_FONTS),
                optional   = get_opt(contxt, OPT_OPTIONAL),
                delopts    = get_opt(contxt, OPT_DELOPTS),
                back       = get_opt(contxt, OPT_BACK),
                files      = all ? NULL : get_opt(contxt, OPT_FILES),
                fail       = get_opt(delopts, OPT_FAIL) ?
                             NULL : get_opt(optional, OPT_FAIL),
                nofail     = get_opt(delopts, OPT_NOFAIL) ?
                             NULL : get_opt(optional, OPT_NOFAIL),
                oknodelete = get_opt(delopts, OPT_OKNODELETE) ?
                             NULL : get_opt(optional, OPT_OKNODELETE),
                force      = get_opt(delopts, OPT_FORCE) ?
                             NULL : get_opt(optional, OPT_FORCE),
                askuser    = get_opt(delopts, OPT_ASKUSER) ?
                             NULL : get_opt(optional, OPT_ASKUSER);

        DNUM = 0;

        // The (pattern) (choices) and (all) options
        // are mutually exclusive.
        if((pattern && (choices || all)) ||
           (choices && (pattern || all)) ||
           (all && (choices || pattern)))
        {
            ERR(ERR_OPTION_MUTEX,
                  "pattern/choices/all");
            RCUR;
        }

        // The (fail) (nofail) and (oknodelete) options
        // are mutually exclusive.
        if((fail && (nofail || oknodelete)) ||
           (nofail && (fail || oknodelete)) ||
           (oknodelete && (nofail || fail)))
        {
            ERR(ERR_OPTION_MUTEX,
                  "fail/nofail/oknodelete");
            RCUR;
        }

        // We need a source and a destination dir.
        if(source && dest)
        {
            // Tree of source / destination tuples.
            pnode_p tree;

            const char *src = str(source),
                       *dst = str(dest);

            // If the source is a directory, (all),
            // (choices) or (pattern) must be used.
            if(h_exists(src) == 2 &&
               !all && !choices && !pattern)
            {
                ERR(ERR_MISSING_OPTION, "all/choices/pattern");
                RCUR;
            }

            // A non safe operation in pretend mode
            // always succeeds.
            if(get_numvar(contxt, "@pretend") && !safe)
            {
                RNUM(1);
            }

            // Does the destination already exist?
            if(h_exists(dst) == 2)
            {
                // Don't trust h_exists, this string
                // might be empty.
                size_t dln = strlen(dst);

                // If it's not a volume, set permission
                // so that overwriting can succeed.
                if(dln && dst[dln - 1] != ':')
                {
                    chmod(dst, S_IRWXU);
                }
            }

            // Traverse the source directory and create
            // the corresponding destination strings.
            tree = h_filetree
            (
                contxt,
                src,
                dst,
                files,
                fonts,
                choices,
                pattern
            );

            // Unless we ran out of memory when traversing
            // the source directory, we now have a list of
            // source -> destination tuples.
            if(tree)
            {
                inp_t rc = G_TRUE;
                pnode_p cur = tree;

                if(newname)
                {
                    // Replace file name if single file and
                    // the 'newname' option is set
                    if(cur->next && cur->type == 2 &&
                       cur->next->type == 1 && !cur->next->next)
                    {
                        free(cur->next->copy);
                        cur->next->copy = h_tackon(contxt, dst, str(newname));
                    }
                }

                // Do we need confirmation?
                if(confirm)
                {
                    // The default threshold is expert.
                    int level = get_numvar(contxt, "@user-level"),
                        th = 2;

                    // If the (confirm ...) option contains
                    // something that can be translated into
                    // a new threshold value...
                    if(confirm->children &&
                       confirm->children[0] &&
                       confirm->children[0] != end())
                    {
                        // ...then do so.
                        th = num(confirm);
                    }

                    // If we are below the threshold value, or
                    // user input has been short-circuited by
                    // @yes, skip confirmation.
                    if(level < th ||
                       get_numvar(contxt, "@yes"))
                    {
                        confirm = NULL;
                    }

                    // Make sure that we have the prompt and
                    // help texts that we need if 'confirm'
                    // is set.
                    if(!prompt || !help)
                    {
                        char * m = prompt ? "help" : "prompt";
                        ERR(ERR_MISSING_OPTION, m);
                        cur = NULL;
                    }
                }

                // Initialize GUI, set up file lists, events,
                // and so on.
                if(cur)
                {
                    rc = gui_copyfiles_start
                    (
                        prompt ? str(prompt) : NULL,
                        confirm ? str(help) : NULL,
                        cur,
                        confirm,
                        back
                    );
                }

                // Start copy unless skip / abort / back.
                if(rc == G_TRUE)
                {
                    // Set file copy mode.
                    int md = (infos ? CF_INFOS : 0) |
                             (fonts ? CF_FONTS : 0) |
                             (nogauge ? CF_NOGAUGE : 0) |
                             (nofail ? CF_NOFAIL : 0) |
                             (oknodelete ? CF_OKNODELETE : 0) |
                             (force ? CF_FORCE : 0) |
                             (askuser ? CF_ASKUSER : 0);

                    // For all files / dirs in list, copy / create.
                    for(; cur && rc == G_TRUE; cur = cur->next)
                    {
                        int32_t prm = 0;

                        // Copy file / create dir / skip if zero:ed
                        switch(cur->type)
                        {
                            case 0:
                                continue;

                            case 1:
                                rc = h_copyfile(contxt, cur->name,
                                                cur->copy, back, md);
                                break;

                            case 2:
                                // Make dir and make sure the protection
                                // bits of the new one matches the old.
                                if(!h_makedir(contxt, cur->copy, md) || 
                                   !h_protect_get(contxt, cur->name, &prm) ||
                                   !h_protect_set(contxt, cur->copy, prm))
                                {
                                    rc = G_FALSE;
                                }
                        }
                    }

                    // GUI teardown.
                    gui_copyfiles_end();

                    // Translate return code.
                    DNUM = (rc == G_TRUE) ? 1 : 0;
                }

                // Back return value.
                entry_p br = NULL;

                // FIXME
                if(rc != G_TRUE)
                {
                    // Is the back option available?
                    if(back)
                    {
                        // Fake input?
                        if(get_numvar(contxt, "@back"))
                        {
                            rc = G_ABORT;
                        }

                        // On abort execute.
                        if(rc == G_ABORT)
                        {
                            br = resolve(back);
                        }
                    }

                    // FIXME
                    if(rc == G_ABORT || rc == G_EXIT)
                    {
                        HALT();
                    }
                }

                cur = tree;

                // Free list of files / dirs.
                while(cur)
                {
                    tree = cur;
                    free(cur->name);
                    free(cur->copy);
                    cur = cur->next;
                    free(tree);
                }

                // If we've executed any 'back' code,
                // return its return value instead of
                // our own.
                if(br)
                {
                    return br;
                }
            }
        }
        else
        {
            char *m = !source ? "source" : "dest";
            ERR(ERR_MISSING_OPTION, m);
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // We don't know if we're successsful,
    // at this point, return what we have.
    RCUR;
}

//----------------------------------------------------------------------------
// (copylib (prompt..) (help..) (source..) (dest..) (newname..) (infos)
//     (confirm) (safe) (optional <option> <option> ...) (delopts <option>
//     <option> ...) (nogauge))
//
//     install a library if newer version
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_copylib(entry_p contxt)
{
    // We need atleast one argument
    if(c_sane(contxt, 1))
    {
        entry_p prompt     = get_opt(contxt, OPT_PROMPT),
                help       = get_opt(contxt, OPT_HELP),
                source     = get_opt(contxt, OPT_SOURCE),
                dest       = get_opt(contxt, OPT_DEST),
                newname    = get_opt(contxt, OPT_NEWNAME),
                infos      = get_opt(contxt, OPT_INFOS),
                confirm    = get_opt(contxt, OPT_CONFIRM),
                safe       = get_opt(contxt, OPT_SAFE),
                nogauge    = get_opt(contxt, OPT_NOGAUGE),
                noposition = get_opt(contxt, OPT_NOPOSITION),
                optional   = get_opt(contxt, OPT_OPTIONAL),
                delopts    = get_opt(contxt, OPT_DELOPTS),
                back       = get_opt(contxt, OPT_BACK),
                fail       = get_opt(delopts, OPT_FAIL) ?
                             NULL : get_opt(optional, OPT_FAIL),
                nofail     = get_opt(delopts, OPT_NOFAIL) ?
                             NULL : get_opt(optional, OPT_NOFAIL),
                oknodelete = get_opt(delopts, OPT_OKNODELETE) ?
                             NULL : get_opt(optional, OPT_OKNODELETE),
                force      = get_opt(delopts, OPT_FORCE) ?
                             NULL : get_opt(optional, OPT_FORCE),
                askuser    = get_opt(delopts, OPT_ASKUSER) ?
                             NULL : get_opt(optional, OPT_ASKUSER);

        DNUM = 0;

        // The (fail) (nofail) and (oknodelete) options
        // are mutually exclusive.
        if((fail && (nofail || oknodelete)) ||
           (nofail && (fail || oknodelete)) ||
           (oknodelete && (nofail || fail)))
        {
            ERR(ERR_OPTION_MUTEX,
                "fail/nofail/oknodelete");
            RCUR;
        }

        // We always need a prompt and help since trying
        // to overwrite new files with older ones will
        // force a confirm if we're in expert mode. The
        // CBM installer doesn't behave this way, but the
        // Installer guide says so, and it makes sense, so
        // let's do it this way until we know for sure that
        // this will break existing scripts.
        if(source && dest &&
           prompt && help)
        {
            char *s = str(source),
                 *d = str(dest);

            // Does the source file exist?
            if(h_exists(s) == 1)
            {
                // User level.
                int level = get_numvar(contxt, "@user-level"),

                // Source file version.
                vs = h_getversion(contxt, s),

                // Destination type.
                dt = h_exists(d);

                // Destination file.
                char *f;

                // A non safe operation in pretend mode
                // always succeeds.
                if(get_numvar(contxt, "@pretend") && !safe)
                {
                    RNUM(1);
                }

                if(vs < 0)
                {
                    // Only fail if we're in 'strict' mode.
                    if(get_numvar(contxt, "@strict"))
                    {
                        // Could not find version string.
                        ERR(ERR_NO_VERSION, s);
                        RCUR;
                    }
                }

                if(dt == 1)
                {
                    // Only fail if we're in 'strict' mode.
                    if(get_numvar(contxt, "@strict"))
                    {
                        // Destination is a file, not a dir.
                        ERR(ERR_NOT_A_DIR, d);
                        RCUR;
                    }
                }

                // Do we need confirmation?
                if(confirm)
                {
                    // The default threshold is expert.
                    int th = 2;

                    // If the (confirm ...) option contains
                    // something that can be translated into
                    // a new threshold value...
                    if(confirm->children &&
                       confirm->children[0] &&
                       confirm->children[0] != end())
                    {
                        // ...then do so.
                        th = num(confirm);
                    }

                    // If we are below the threshold value, or
                    // user input has been short-circuited by
                    // @yes, skip confirmation.
                    if(level < th ||
                       get_numvar(contxt, "@yes"))
                    {
                        confirm = NULL;
                    }
                }

                if(!dt)
                {
                    // Directory doesn't exist, create
                    // it. One level deep only.
                    if(!mkdir(d, 0777))
                    {
                        // Log the success.
                        h_log(contxt, tr(S_CRTD), d);
                    }
                    else
                    {
                        // Permission problems or the dir is
                        // more than 1 level deeper than the
                        // existing path.
                        ERR(ERR_WRITE_DIR, d);
                        RCUR;
                    }
                }

                // Are we renaming the file?
                if(newname)
                {
                    // Yes, append the new name to the path.
                    f = h_tackon(contxt, d, str(newname));
                }
                else
                {
                    // No, append the file part of the (possibly)
                    // full source path to the destination path.
                    f = h_tackon(contxt, d, h_fileonly(contxt, s));
                }

                // If we're not out of memory and destination dir
                // and / or destination file is non empty, f will
                // be <> 0.
                if(f)
                {
                    // Get f type info and set copy mode.
                    int ft = h_exists(f),
                        md = CF_SILENT |
                             (infos ? CF_INFOS : 0) |
                             (noposition ? CF_NOPOSITION : 0) |
                             (nogauge ? CF_NOGAUGE : 0) |
                             (nofail ? CF_NOFAIL : 0) |
                             (oknodelete ? CF_OKNODELETE : 0) |
                             (force ? CF_FORCE : 0) |
                             (askuser ? CF_ASKUSER : 0);

                    // Currently there is no way to abort a
                    // file copy, but we need to handle the
                    // GUI return values anyway.
                    inp_t rc = G_FALSE;

                    // Are we overwriting a file?
                    if(!ft)
                    {
                        // No such file, copy source to the
                        // destination directory. If needed
                        // get confirmation.
                        if(confirm)
                        {
                            // Is the version of the source file
                            // unknown?
                            if(vs < 0)
                            {
                                if(h_confirm(
                                    contxt,
                                    "",
                                    "%s\n\n%s: %s\n%s\n\n%s: %s",
                                    str(prompt),
                                    tr(S_VINS),
                                    tr(S_VUNK),
                                    tr(S_NINS),
                                    tr(S_DDRW),
                                    d))
                                {
                                    rc = h_copyfile(contxt, s, f, back, md);
                                }
                            }
                            // The version of the source file
                            // is known.
                            else
                            {
                                if(h_confirm(
                                    contxt,
                                    "",
                                    "%s\n\n%s: %d.%d\n%s\n\n%s: %s",
                                    str(prompt),
                                    tr(S_VINS),
                                    vs >> 16,
                                    vs & 0xffff,
                                    tr(S_NINS),
                                    tr(S_DDRW),
                                    d))
                                {
                                    rc = h_copyfile(contxt, s, f, back, md);
                                }
                            }
                        }
                        else
                        {
                            // No confirmation needed.
                            rc = h_copyfile(contxt, s, f, back, md);
                        }
                    }
                    else
                    // It's a file.
                    if(ft == 1)
                    {
                        // Get version of existing file.
                        int vf = h_getversion(contxt, f);

                        // Is the version of the existing file
                        // unknown?
                        if(vf < 0)
                        {
                            // Only fail if we're in 'strict' mode.
                            if(get_numvar(contxt, "@strict"))
                            {
                                // Could not find version string.
                                ERR(ERR_NO_VERSION, f);
                                vs = vf;
                            }
                            else
                            {
                                // Is the version of the source
                                // file unknown?
                                if(vs < 0)
                                {
                                    // Both target and source file
                                    // have an unknown version.
                                    if(h_confirm(
                                        contxt,
                                        "",
                                        "%s\n\n%s: %s\n%s: %s\n\n%s: %s",
                                        str(prompt),
                                        tr(S_VINS),
                                        tr(S_VUNK),
                                        tr(S_VCUR),
                                        tr(S_VUNK),
                                        tr(S_DDRW),
                                        d))
                                    {
                                        vs = vf + 1;
                                    }
                                    else
                                    {
                                        vs = vf;
                                    }
                                }
                                else
                                {
                                    // The version of the source
                                    // file is known, the target
                                    // version is unknown.
                                    if(h_confirm(
                                        contxt,
                                        "",
                                        "%s\n\n%s: %d.%d\n%s: %s\n\n%s: %s",
                                        str(prompt),
                                        tr(S_VINS),
                                        vs >> 16,
                                        vs & 0xffff,
                                        tr(S_VCUR),
                                        tr(S_VUNK),
                                        tr(S_DDRW),
                                        d))
                                    {
                                        vs = vf + 1;
                                    }
                                    else
                                    {
                                        vs = vf;
                                    }
                                }

                                if(vs > vf)
                                {
                                    confirm = NULL;
                                }
                            }
                        }
                        // The target file version is
                        // known.
                        else
                        {
                            // Is the version of the source
                            // file unknown?
                            if(vs < 0 && h_confirm(
                                contxt,
                                "",
                                "%s\n\n%s: %s\n%s: %d.%d\n\n%s: %s",
                                str(prompt),
                                tr(S_VINS),
                                tr(S_VUNK),
                                tr(S_VCUR),
                                vf >> 16,
                                vf & 0xffff,
                                tr(S_DDRW),
                                d))
                            {
                                vs = vf + 1;
                                confirm = NULL;
                            }
                        }

                        // Did we find a version not equal to
                        // that of the current file?
                        if(vs != vf)
                        {
                            // If we ask for confirmation and get it, copy
                            // the file no matter what version it (and the
                            // existing destination file) has.
                            if(confirm)
                            {
                                if(h_confirm(
                                    contxt,
                                    "",
                                    "%s\n\n%s: %d.%d\n%s: %d.%d\n\n%s: %s",
                                    str(prompt),
                                    tr(S_VINS),
                                    vs >> 16,
                                    vs & 0xffff,
                                    tr(S_VCUR),
                                    vf >> 16,
                                    vf & 0xffff,
                                    tr(S_DDRW),
                                    d))
                                {
                                    rc = h_copyfile(contxt, s, f, back, md);
                                }
                            }
                            else
                            {
                                // If the file to be copied has a higher version
                                // number than the existing one, overwrite.
                                if(vs > vf)
                                {
                                    rc = h_copyfile(contxt, s, f, back, md);
                                }
                                else
                                {
                                    // If the file to be copied has a lower version
                                    // number than the existing one, and we're in
                                    // expert mode, ask the user to confirm. If we
                                    // get a confirmation, overwrite.
                                    if(level == 2)
                                    {
                                        if(h_confirm(
                                            contxt,
                                            "",
                                            "%s\n\n%s: %d.%d\n%s: %d.%d\n\n%s: %s",
                                            str(prompt),
                                            tr(S_VINS),
                                            vs >> 16,
                                            vs & 0xffff,
                                            tr(S_VCUR),
                                            vf >> 16,
                                            vf & 0xffff,
                                            tr(S_DDRW),
                                            d))
                                        {
                                            rc = h_copyfile(contxt, s, f, back, md);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // It's a dir.
                    else
                    {
                        // Only fail if we're in 'strict' mode.
                        if(get_numvar(contxt, "@strict"))
                        {
                            // Dest file exists, but is a directory.
                            ERR(ERR_NOT_A_FILE, f);
                        }
                    }

                    // Free memory allocated by h_tackon.
                    free(f);

                    // Translate return code.
                    DNUM = (rc == G_TRUE) ? 1 : 0;
                }
            }
            else
            {
                // Only fail if we're in 'strict' mode.
                if(get_numvar(contxt, "@strict"))
                {
                    // No file, doesn't exist or is dir.
                    ERR(ERR_NOT_A_FILE, s);
                }
            }
        }
        else
        {
            char *m = !source ? "source" : !dest ? "dest" :
                      !prompt ? "prompt" : "help";

            ERR(ERR_MISSING_OPTION, m);
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success or failure.
    RCUR;
}

//----------------------------------------------------------------------------
// Name:        h_delete_file
// Description: Delete file. Helper used by m_delete.
// Input:       entry_p contxt:     The execution context.
//              const char *file:   File to delete.
// Return:      int:                On success '1', else '0'.
//----------------------------------------------------------------------------
static int h_delete_file(entry_p contxt, const char *file)
{
    if(file)
    {
        entry_p infos    = get_opt(CARG(2), OPT_INFOS),
                optional = get_opt(CARG(2), OPT_OPTIONAL),
                delopts  = get_opt(CARG(2), OPT_DELOPTS),
                force    = get_opt(delopts, OPT_FORCE) ? NULL :
                           get_opt(optional, OPT_FORCE),
                askuser  = get_opt(delopts, OPT_ASKUSER) ? NULL :
                           get_opt(optional, OPT_ASKUSER);

        // If (force) is used, give permissions
        // so that delete can succeed.
        if(force)
        {
            // No need to bother with the return
            // value since errors will be caught
            // below.
            chmod(file, S_IRWXU);
        }
        else
        {
            if(access(file, W_OK))
            {
                // Do we need to ask for confirmation?
                if(askuser)
                {
                    // Only ask for confirmation if we're not
                    // running in novice mode.
                    if(get_numvar(contxt, "@user-level") &&
                       h_confirm(contxt, "", tr(S_DWRT), file))
                    {
                        // Give permissions so that delete
                        // can succeed. No need to bother with
                        // the return value since errors will
                        // be caught below.
                        chmod(file, S_IRWXU);
                    }
                    else
                    {
                        // Halt will be set by h_confirm. Skip
                        // will result in nothing.
                        return 0;
                    }
                }
                else
                {
                    // Fail silently just like the original.
                    return 0;
                }
            }
        }

        // If yes, this must succeed, otherwise we
        // will abort with an error.
        if(!remove(file))
        {
            // The file has been deleted.
            h_log(contxt, tr(S_DLTD), file);

            // Shall we delete the info file as well?
            if(infos)
            {
                // Info = file + .info.
                char *info = get_buf();
                snprintf(info, buf_size(), "%s.info", file);

                if(h_exists(info) == 1)
                {
                    // Set permissions so that delete can
                    // succeed.
                    chmod(info, S_IRWXU);

                    // Delete the info file.
                    if(!remove(info))
                    {
                        // The info file has been deleted.
                        h_log(contxt, tr(S_DLTD), info);
                    }
                    else
                    {
                        ERR(ERR_DELETE_FILE, info);
                        return 0;
                    }
                }
            }

            // All done.
            return 1;
        }

        // Could not delete file.
        ERR(ERR_DELETE_FILE, file);
    }
    else
    {
        // Unknown error.
        PANIC(contxt);
    }
    
    // Failure.
    return 0;
}

//----------------------------------------------------------------------------
// Name:        h_delete_dir
// Description: Delete directory. Helper used by m_delete.
// Input:       entry_p contxt:     The execution context.
//              const char *dir:    Directory to delete.
// Return:      int:                On success '1', else '0'.
//----------------------------------------------------------------------------
static int h_delete_dir(entry_p contxt, const char *dir)
{
    if(dir)
    {
        entry_p infos    = get_opt(CARG(2), OPT_INFOS),
                optional = get_opt(CARG(2), OPT_OPTIONAL),
                delopts  = get_opt(CARG(2), OPT_DELOPTS),
                all      = get_opt(CARG(2), OPT_ALL),
                force    = get_opt(delopts, OPT_FORCE) ? NULL :
                           get_opt(optional, OPT_FORCE),
                askuser  = get_opt(delopts, OPT_FORCE) ? NULL :
                           get_opt(optional, OPT_ASKUSER);

        if(!force && access(dir, W_OK))
        {
            // Do we need to ask for confirmation?
            if(askuser)
            {
                // Only ask for confirmation if we're not
                // running in novice mode.
                if(!get_numvar(contxt, "@user-level") ||
                   !h_confirm(CARG(2), "", tr(S_DWRD), dir))
                {
                    // Halt will be set by h_confirm. Skip
                    // will result in nothing.
                    return 0;
                }
            }
            else
            {
                // Exit silently.
                return 0;
            }
        }

        // Give permissions so that delete can succeed.
        // No need to bother with the return value since
        // errors will be caught below.
        chmod(dir, S_IRWXU);

        if(rmdir(dir))
        {
            if(all)
            {
                DIR *d = opendir(dir);

                // Permission to read?
                if(d)
                {
                    char *w;
                    struct dirent *e = readdir(d);

                    // Find all files in the directory.
                    while(e)
                    {
                        // Create full path.
                        w = h_tackon(contxt, dir, e->d_name);

                        // Is it a file?
                        if(w && h_exists(w) == 1)
                        {
                            // Delete it.
                            h_delete_file(contxt, w);
                        }

                        // Free full path.
                        free(w);

                        // Get next entry.
                        e = readdir(d);
                    }

                    // Restart from the beginning.
                    rewinddir(d);
                    e = readdir(d);

                    // Find all subdirectories in the
                    // directory.
                    while(e)
                    {
                        #ifndef AMIGA
                        // Filter out the magic on non-Amigas.
                        if(strcmp(e->d_name, ".") &&
                           strcmp(e->d_name, ".."))
                        #endif
                        {
                            // Create full path.
                            w = h_tackon(contxt, dir, e->d_name);

                            // Is it a directory?
                            if(w && h_exists(w) == 2)
                            {
                                // Recur into subdirectory.
                                h_delete_dir(contxt, w);
                            }

                            // Free full path.
                            free(w);
                        }

                        // Get next entry.
                        e = readdir(d);
                    }

                    // Close the (by now, hopefully) empty dir.
                    closedir(d);
                }

                if(rmdir(dir))
                {
                    // Fail silently.
                    return 0;
                }
            }
            else
            {
                // Fail silently.
                return 0;
            }
        }

        // Shall we delete the info file as well?
        if(infos)
        {
            // Info = file + .info.
            char *info = get_buf();
            snprintf(info, buf_size(), "%s.info", dir);

            if(h_exists(info) == 1)
            {
                // Set permissions so that delete can
                // succeed.
                chmod(info, S_IRWXU);

                // Delete the info file.
                if(!remove(info))
                {
                    // The info file has been deleted.
                    h_log(contxt, tr(S_DLTD), info);
                }
                else
                {
                    // Fail silently.
                    return 0;
                }
            }
        }

        // Done.
        return 1;
    }

    // Bad input.
    PANIC(contxt);

    // Fail silently.
    return 0;
}

//----------------------------------------------------------------------------
// Name:        h_delete_pattern
// Description: Delete file / dir matching pattern. Helper used by m_delete.
// Input:       entry_p contxt:     The execution context.
//              const char *pat:    Pattern.
// Return:      int:                On success '1', else '0'.
//----------------------------------------------------------------------------
static int h_delete_pattern(entry_p contxt, const char *pat)
{
    if(contxt && pat)
    {
        // Pattern matching is only done on Amiga like OS:es
        #ifdef AMIGA
        struct AnchorPath *ap =
            DBG_ALLOC(calloc(1, sizeof(struct AnchorPath) + PATH_MAX));

        if(ap)
        {
            int err;
            ap->ap_Strlen = PATH_MAX;

            // For all matches, invoke the appropriate
            // function for deletion.
            for(err = MatchFirst(pat, ap); !err;
                err = MatchNext(ap))
            {
                // ST_FILE         -3
                // ST_LINKFILE     -4
                if(ap->ap_Info.fib_DirEntryType < 0)
                {
                    if(!h_delete_file(contxt, ap->ap_Buf))
                    {
                        // Break out on trouble / user
                        // abort.
                        break;
                    }
                }
                else
                // ST_ROOT          1
                // ST_USERDIR       2
                // ST_SOFTLINK      3
                // ST_LINKDIR       4
                if(ap->ap_Info.fib_DirEntryType > 0)
                {
                    if(!h_delete_dir(contxt, ap->ap_Buf))
                    {
                        // Break out on trouble / user
                        // abort.
                        break;
                    }
                }
            }

            // Free all resources.
            MatchEnd(ap);
            free(ap);

            // Is there nothing left?
            if(err == ERROR_NO_MORE_ENTRIES)
            {
                // Done.
                return 1;
            }
            else
            {
                // Please note that 'breaks' will take us
                // here, so will MatchFirst / MatchNext()
                // problems.
                ERR(ERR_DELETE_FILE, pat);
            }
        }
        #else
        // On non Amiga systems this is a stub.
        return 1;
        #endif
    }

    // Bad input.
    PANIC(contxt);
    return 0;
}

//----------------------------------------------------------------------------
// (delete file (help..) (prompt..) (confirm..)
//     (infos) (optional <option> <option> ...) (all)
//     (delopts <option> <option> ...) (safe))
//
//     delete file
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_delete(entry_p contxt)
{
    // We need atleast one argument
    if(c_sane(contxt, 1))
    {
        int wc = 0;
        const char *w = str(CARG(1));

        #ifdef AMIGA
        wc = ParsePattern(w, get_buf(), buf_size());
        #endif

        // Assume failure.
        DNUM = 0;

        // Can we parse the input string?
        if(wc >= 0)
        {
            entry_p help     = get_opt(CARG(2), OPT_HELP),
                    prompt   = get_opt(CARG(2), OPT_PROMPT),
                    confirm  = get_opt(CARG(2), OPT_CONFIRM),
                    safe     = get_opt(CARG(2), OPT_SAFE);

            // Do we need confirmation?
            if(confirm)
            {
                // The default threshold is expert.
                int level = get_numvar(contxt, "@user-level"),
                    th = 2;

                // If the (confirm ...) option contains
                // something that can be translated into
                // a new threshold value...
                if(confirm->children &&
                   confirm->children[0] &&
                   confirm->children[0] != end())
                {
                    // ...then do so.
                    th = num(confirm);
                }

                // If we are below the threshold value,
                // don't care about getting confirmation
                // from the user.
                if(level < th)
                {
                    confirm = NULL;
                }

                // Make sure that we have the prompt and
                // help texts that we need if 'confirm'
                // is set.
                if(!prompt || !help)
                {
                    char *m = prompt ? "help" : "prompt";
                    ERR(ERR_MISSING_OPTION, m);
                    RCUR;
                }
            }

            // If we need confirmation and the user skips
            // or aborts, return. On abort, the HALT will
            // be set by h_confirm.
            if(confirm && !h_confirm(CARG(2),
               str(help), str(prompt)))
            {
                RCUR;
            }

            // Is this a safe operation or are we not
            // running in pretend mode?
            if(safe || !get_numvar(contxt, "@pretend"))
            {
                // Did the input string contain any
                // wildcards?
                if(wc)
                {
                    // Delete everything matching the
                    // wildcard pattern.
                    DNUM = h_delete_pattern(contxt, w);
                }
                else
                // No wildcards, delete directory, file
                // or something that doesn't exist.
                {
                    switch(h_exists(w))
                    {
                        case 2:
                            DNUM = h_delete_dir(contxt, w);
                            break;

                        case 1:
                            DNUM = h_delete_file(contxt, w);
                            break;

                        case 0:
                            h_log(contxt, tr(S_NSFL), w);
                            DNUM = 0;
                    }
                }
            }
            else
            {
                // A non safe operation in pretend
                // mode always succeeds.
                DNUM = 1;
            }
        }
        else
        {
            // We probably had a buffer overflow.
            ERR(ERR_OVERFLOW, w);
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success or broken
    // parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (exists <filename> (noreq))
//     0 if no, 1 if file, 2 if dir
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_exists(entry_p contxt)
{
    if(c_sane(contxt, 1))
    {
        // Supress volume requester?
        if(get_opt(contxt, OPT_NOREQ))
        {
            #ifdef AMIGA
            struct Process *p = (struct Process *)
                FindTask(NULL);

            // Save the current window ptr.
            APTR w = p->pr_WindowPtr;

            // Disable auto request.
            p->pr_WindowPtr = (APTR) -1L;
            #endif

            // Get type (file / dir / 0)
            DNUM = h_exists(str(CARG(1)));

            #ifdef AMIGA
            // Restore auto request.
            p->pr_WindowPtr = w;
            #endif
        }
        else
        {
            // Get type (file / dir / 0)
            DNUM = h_exists(str(CARG(1)));
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success or broken
    // parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (fileonly <path>)
//     return file part of path (see pathonly)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_fileonly(entry_p contxt)
{
    if(c_sane(contxt, 1))
    {
        RSTR(strdup(h_fileonly(contxt, str(CARG(1)))));
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (foreach <dir> <pattern> <statements>)
//     do for entries in directory
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_foreach(entry_p contxt)
{
    if(c_sane(contxt, 3))
    {
        // Pattern matching is not done
        // on non-Amiga systems.
        #ifdef AMIGA
        const char *pt = str(CARG(2));
        #endif

        // Open dir and assume failure.
        const char *dn = str(CARG(1));
        DIR *dir = opendir(dn);
        pnode_p top = NULL;
        int err = 1;

        // Permission to read?
        if(dir)
        {
            // Use global buffer.
            char *cwd = get_buf();
            struct dirent *ent = readdir(dir);

            // Save the current working directory.
            if(getcwd(cwd, buf_size()) == cwd)
            {
                // Enter the directory <drawer name>
                if(!chdir(dn))
                {
                    // Allocate memory for the start node.
                    pnode_p cur;
                    top = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));

                    #ifdef AMIGA
                    // This file info block will be used for all files
                    // and subdirs in the directory.
                    struct FileInfoBlock *fib = (struct FileInfoBlock *)
                           AllocDosObject(DOS_FIB, NULL);

                    // Use cur as a gatekeeper. If set to NULL, nothing
                    // will take place except clean ups.
                    cur = fib ? top : NULL;
                    #else
                    // No pattern matching = no fib.
                    cur = top;
                    #endif

                    // The dir might be empty but that's not an
                    // error.
                    err = 0;

                    // Iterate over all entries in dir.
                    while(ent && cur && !err)
                    {
                        // Name of file / dir.
                        const char *fn = ent->d_name;

                        #ifndef AMIGA
                        // Filter out the magic on non-Amigas.
                        if(strcmp(fn, ".") &&
                           strcmp(fn, ".."))
                        #endif
                        {
                            #ifdef AMIGA
                            // The dir is not empty, we should be able
                            // to go all the way here. Assume failure.
                            err = 1;

                            // Lock and get the information we need
                            // from the current entry
                            BPTR lock = (BPTR) Lock(fn, ACCESS_READ);

                            if(lock)
                            {
                                if(Examine(lock, fib))
                                {
                                    // Please note that if we fail to
                                    // lock and examine cur->name will
                                    // be NULL.
                                    cur->type = fib->fib_DirEntryType;
                                    cur->name = strdup(fn);

                                    // We're probably good. PANIC:s will
                                    // be caught further down.
                                    err = 0;
                                }

                                UnLock(lock);
                            }
                            #else
                            cur->type = h_exists(fn);
                            cur->name = strdup(fn);
                            #endif

                            // An empty name indicates a PANIC only if
                            // we didn't have any locking problems.
                            if(!cur->name)
                            {
                                // Setting cur to NULL will generate a
                                // PANIC further down.
                                if(!err)
                                {
                                    cur = NULL;
                                }

                                // Always bail out.
                                break;
                            }
                        }

                        // Get next entry.
                        ent = readdir(dir);

                        // We need to check for cur->name here, or else the
                        // the filtering of '.' and '..' would not work, we
                        // would get entries without names.
                        if(ent && cur->name)
                        {
                            cur->next = DBG_ALLOC(calloc(1, sizeof(struct pnode_t)));
                            cur = cur->next;
                        }
                    }

                    #ifdef AMIGA
                    // If dir is empty, fib will be NULL.
                    if(fib)
                    {
                        FreeDosObject(DOS_FIB, fib);
                    }
                    #endif

                    if(!cur)
                    {
                        // Out of memory.
                        PANIC(contxt);
                        err = 1;
                    }

                    // Go back where we started.
                    chdir(cwd);
                }
            }

            // Done.
            closedir(dir);
        }

        if(err)
        {
            // We could end up here for a number
            // of reasons. More details perhaps?
            ERR(ERR_READ_DIR, dn);
        }

        // Iterate over the entire list of files / dirs and
        // free entries one by one. If the pattern matches
        // the current entry, set variables and execute what
        // we have in argument 3.
        while(top)
        {
            int skip = err;
            pnode_p old = top;

            // 'Export' name and type info if we have a name.
            if(top->name)
            {
                #ifdef AMIGA
                if(!err)
                {
                    // Use global buffer.
                    char *buf = get_buf();

                    // Parse pattern.
                    switch(ParsePattern(pt, buf, buf_size()))
                    {
                        // If we have any wildcards, try to match.
                        case 1:
                            skip = MatchPattern(buf, top->name) ? 0 : 1;
                            break;

                        // If no wildcards, do a simple string comparsion.
                        case 0:
                            skip = strcmp(top->name, pt);
                            break;

                        // We probably had a buffer overflow.
                        default:
                            ERR(ERR_OVERFLOW, pt);
                            err = 1;
                    }
                }
                #endif

                // We always export, for memory management reasons.
                set_numvar(contxt, "@each-type", (int) top->type);
                set_strvar(contxt, "@each-name", top->name);

                if(!skip)
                {
                    // Execute the code contained in
                    // the third argument.
                    invoke(CARG(3));
                }
            }

            // Free current entry and proceed with the next.
            top = top->next;
            free(old);
        }

        // Set return value.
        DNUM = err ? 0 : 1;
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success or broken
    // parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (makeassign <assign> [<path>] (safe)) ; note: <assign> doesn't need `:''
//     create an assignment
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_makeassign(entry_p contxt)
{
    if(c_sane(contxt, 1))
    {
        entry_p safe = get_opt(contxt, OPT_SAFE);

        // Is this a safe operation or are we not
        // running in pretend mode?
        if(safe || !get_numvar(contxt, "@pretend"))
        {
            // The name of the assign.
            char *asn = str(CARG(1));

            // Assume failure..
            DNUM = 0;

            // Are we going to create an assign?
            if(CARG(2) && CARG(2) != end() &&
               CARG(2)->type != OPTION)
            {
                // The destination.
                char *dst = str(CARG(2));

                #ifdef AMIGA
                BPTR lock = (BPTR) Lock(dst, ACCESS_READ);
                if(lock)
                {
                    // Create the assign. After this,
                    // the lock will be owned by the
                    // system, do not unlock or use.
                    DNUM = AssignLock(asn, lock) ? 1 : 0;
                }
                #else
                DNUM = 1;
                #endif

                // Log the outcome.
                h_log
                (
                    contxt,
                    DNUM ? tr(S_ACRT) : tr(S_ACRE),
                    asn,
                    dst
                );
            }
            else
            {
                #ifdef AMIGA
                // Remove assign.
                DNUM = AssignLock(str(CARG(1)), (BPTR) NULL) ? 1 : 0;
                #else
                DNUM = 2;
                #endif

                // Log the outcome.
                h_log
                (
                    contxt,
                    DNUM ? tr(S_ADEL) : tr(S_ADLE),
                    asn
                );
            }

            if(!DNUM)
            {
                // Could not create / rm assign / get lock.
                ERR(ERR_ASSIGN, str(CARG(1)));
            }
        }
        else
        {
            // Pretend.
            DNUM = 1;
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Return what we have. Could
    // be a failure or a success.
    RCUR;
}

//----------------------------------------------------------------------------
// (makedir <name> (prompt..) (help..) (infos) (confirm..) (safe))
//     make a directory
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_makedir(entry_p contxt)
{
    // We need atleast one argument, the name
    // of the directory to be created.
    if(c_sane(contxt, 1))
    {
        entry_p prompt   = get_opt(CARG(2), OPT_PROMPT),
                help     = get_opt(CARG(2), OPT_HELP),
                infos    = get_opt(CARG(2), OPT_INFOS),
                confirm  = get_opt(CARG(2), OPT_CONFIRM),
                safe     = get_opt(CARG(2), OPT_SAFE);

        DNUM = 0;

        // Do we need confirmation?
        if(confirm)
        {
            // The default threshold is expert.
            int level = get_numvar(contxt, "@user-level"),
                th = 2;

            // If the (confirm ...) option contains
            // something that can be translated into
            // a new threshold value...
            if(confirm->children &&
               confirm->children[0] &&
               confirm->children[0] != end())
            {
                // ...then do so.
                th = num(confirm);
            }

            // If we are below the threshold value,
            // don't care about getting confirmation
            // from the user.
            if(level < th)
            {
                confirm = NULL;
            }

            // Make sure that we have the prompt and
            // help texts that we need if 'confirm'
            // is set.
            if(!prompt || !help)
            {
                char *m = prompt ? "help" : "prompt";
                ERR(ERR_MISSING_OPTION, m);
                RCUR;
            }
        }

        // If we need confirmation and the user skips
        // or aborts, return. On abort, the HALT will
        // be set by h_confirm.
        if(confirm && !h_confirm(CARG(2),
           str(help), str(prompt)))
        {
            RCUR;
        }

        // Is this a safe operation or are we not
        // running in pretend mode?
        if(safe || !get_numvar(contxt, "@pretend"))
        {
            // The name of the directory.
            char *dn = str(CARG(1));

            // Create the directory.
            DNUM = h_makedir(contxt, dn, 0 /* FIXME */);

            // Are we supposed to create an icon
            // as well?
            if(infos && DNUM)
            {
                #ifdef AMIGA
                // Get the default drawer icon from the OS.
                struct DiskObject *obj = (struct DiskObject *)
                    GetDefDiskObject(WBDRAWER);

                // Assume failure.
                DNUM = 0;

                // If we have a default icon, let our newly
                // created directory have it.
                if(obj)
                {
                    // Create new .info.
                    if(PutDiskObject(dn, obj))
                    {
                        // Done.
                        DNUM = 1;
                    }

                    // Free def. icon.
                    FreeDiskObject(obj);
                }

                // If any of the above failed, ioerr
                // will be set. Export that value as
                // a variable.
                if(!DNUM)
                {
                    LONG ioe = IoErr();
                    set_numvar(contxt, "@ioerr", ioe);
                }
                #endif
            }
        }
        else
        {
            // A non safe operation in pretend
            // mode always succeeds.
            DNUM = 1;
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (protect <file> [<string of flags to change>] [<dec. mask>] <parameters>)
//     get/set file protection flags
//
// 8 7 6 5 4 3 2 1  <- Bit number
// h s p a r w e d  <- corresponding protection flag
// ^ ^ ^ ^ ^ ^ ^ ^
// | | | | | | | |
// | | | | | | | +- |
// | | | | | | +--- | 0 = flag set
// | | | | | +----- | 1 = flag clear
// | | | | +------- |
// | | | |
// | | | |
// | | | +--------- |
// | | +----------- | 0 = flag clear
// | +------------- | 1 = flag set
// +--------------- |
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_protect(entry_p contxt)
{
    // A single argument is all we need.
    if(c_sane(contxt, 1))
    {
        char *file = str(CARG(1));
        DNUM = 0;

        if(CARG(2) && CARG(2) != end())
        {
            // Get with option.
            if(CARG(2)->type == CONTXT)
            {
                entry_p override = get_opt(CARG(2), OPT_OVERRIDE);

                if(override)
                {
                    // From user (script).
                    DNUM = num(override);
                }
                else
                {
                    // From file.
                    h_protect_get(contxt, file, &DNUM);
                }
            }
            // Set value.
            else
            {
                // Assume string operation.
                const char *flg = str(CARG(2));
                size_t len = strlen(flg);

                entry_p safe = get_opt(CARG(3), OPT_SAFE),
                        override = get_opt(CARG(3), OPT_OVERRIDE);

                // If any numbers are present in the string,
                // treat it as an absolute mask instead.
                for(size_t i = 0; i < len; i++)
                {
                    if(flg[i] < 58 &&
                       flg[i] > 47)
                    {
                        // We no longer have a string.
                        len = 0;
                        break;
                    }
                }

                // Do we have a string?
                if(len)
                {
                    // Mode (+/-).
                    int m = 0;

                    // Use a real file or override?
                    if(!override)
                    {
                        // Get flags from file.
                        if(!h_protect_get(contxt, file, &DNUM))
                        {
                            // Helper will set proper error
                            RNUM(-1);
                        }
                    }
                    else
                    {
                        // Get flags from user (script).
                        DNUM = num(override);
                    }

                    // Invert 1-4.
                    DNUM ^= 0x0f;

                    // For all flags.
                    for(size_t i = 0; i < len; i++)
                    {
                        // Mask.
                        int b = 0;

                        // Which protection bit?
                        switch(flg[i])
                        {
                            case '+': m = 1; break;
                            case '-': m = 2; break;
                            case 'h': b = 1 << 7; break;
                            case 's': b = 1 << 6; break;
                            case 'p': b = 1 << 5; break;
                            case 'a': b = 1 << 4; break;
                            case 'r': b = 1 << 3; break;
                            case 'w': b = 1 << 2; break;
                            case 'e': b = 1 << 1; break;
                            case 'd': b = 1 << 0; break;
                        }

                        // Adding or subtracting?
                        switch(m)
                        {
                            case 0: DNUM = b; m = 1; break;
                            case 1: DNUM |= b; break;
                            case 2: DNUM &= ~b; break;
                        }
                    }

                    // Invert 1-4.
                    DNUM ^= 0x0f;
                }
                else
                {
                    // Use an absolute mask.
                    DNUM = num(CARG(2));
                }

                if(!override)
                {
                    // Is this a safe operation or are we not
                    // running in pretend mode?
                    if(safe || !get_numvar(contxt, "@pretend"))
                    {
                        // Helper will set error on failure.
                        DNUM = h_protect_set(contxt, file, DNUM);
                    }
                    else
                    {
                        // A non safe operation in pretend
                        // mode always succeeds.
                        DNUM = 1;
                    }
                }
            }
        }
        else
        {
            // Get without options.
            h_protect_get(contxt, file, &DNUM);
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (startup (prompt..) (command..))
//     add a command to the boot scripts (startup-sequence, user-startup)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_startup(entry_p contxt)
{
    // We need atleast two arguments
    if(c_sane(contxt, 2))
    {
        char *cmd = NULL;
        const char *app = str(CARG(1));

        entry_p command  = get_opt(CARG(2), OPT_COMMAND),
                help     = get_opt(CARG(2), OPT_HELP),
                prompt   = get_opt(CARG(2), OPT_PROMPT);

        // Expect failure.
        DNUM = 0;

        // We need a command, a prompt and a help text.
        if(!command || !prompt || !help)
        {
            char *m = !command ? "command" : !prompt ?
                                 "prompt" : "help";
            ERR(ERR_MISSING_OPTION, m);
            RCUR;
        }

        // And somewhere to put the command.
        if(!strlen(app))
        {
            ERR(ERR_INVALID_APP, app);
            RCUR;
        }

        // If we need confirmation and the user skips
        // or aborts, return. On abort, the HALT will
        // be set by h_confirm. Confirmation is needed
        // when user level is not novice or (confirm)
        // is used.
        if(get_opt(CARG(2), OPT_CONFIRM) ||
           get_numvar(contxt, "@user-level") > 0)
        {
            if(!h_confirm(CARG(2),
               str(help), str(prompt)))
            {
                RCUR;
            }
        }

        // We're done if executing in pretend mode.
        if(get_numvar(contxt, "@pretend"))
        {
            RNUM(1);
        }

        // Gather and merge all (command) strings.
        cmd = get_optstr(CARG(2), OPT_COMMAND);

        if(cmd)
        {
            const char *fln = get_strvar(contxt, "@user-startup");
            const size_t len = strlen(";BEGIN ") + strlen(app),
                         ins = strlen(cmd) + 2;

            char *pre = DBG_ALLOC(calloc(len + 1, 1)),
                 *pst = DBG_ALLOC(calloc(len + 1, 1)),
                 *buf = NULL;

            if(pre && pst)
            {
                // We don't need to write yet.
                FILE *fp = fopen(fln, "r");

                // If the file doesn't exist,
                // try to create it.
                if(!fp && !h_exists(fln))
                {
                    fp = fopen(fln, "w");

                    // If successful, close and
                    // reopen as read only.
                    if(fp)
                    {
                        fclose(fp);
                        fp = fopen(fln, "r");
                    }
                }

                if(fp)
                {
                    // Seek to the end so that we
                    // can use ftell below to get
                    // the size of the file.
                    if(!fseek(fp, 0L, SEEK_END))
                    {
                        // Worst case: empty file + 3 NL +
                        // terminating 0 + BEGIN and END
                        // markers + command.
                        size_t osz = (size_t) ftell(fp),
                               nsz = osz + 2 * len + ins;

                        // Allocate enough memory so that
                        // we can keep the old file + any
                        // changes that we need to do in
                        // memory at the same time.
                        buf = DBG_ALLOC(calloc(nsz, 1));

                        if(buf)
                        {
                            // Read the whole file in one go.
                            if(!fseek(fp, 0L, SEEK_SET) &&
                               fread(buf, 1, osz, fp) == osz)
                            {
                                // Prepare the header and footer.
                                snprintf(pre, len + 1, ";BEGIN %s", app);
                                snprintf(pst, len + 1, ";END %s", app);

                                // Do we already have an entry in
                                // the current file?
                                char *beg = strstr(buf, pre),
                                     *end = strstr(buf, pst);

                                // Replace the current entry by inserting
                                // the new one at the same location.
                                if(beg && end)
                                {
                                    // Move the tail of the buffer to the right place.
                                    memmove(beg + len + ins, end, (buf + osz) - end + 1);

                                    // Insert the command string.
                                    memcpy(beg + len + 1, cmd, ins - 2);

                                    // Add surrounding line feeds so that the
                                    // command won't end up being a comment.
                                    beg[len + ins - 1] = beg[len] = '\n';
                                }
                                // No existing entry. Append the new entry.
                                else
                                {
                                    // Header.
                                    memcpy(buf + osz, pre, len);

                                    // Command.
                                    memcpy(buf + osz + len + 1, cmd, ins - 2);

                                    // Footer.
                                    memcpy(buf + osz + len + ins, pst, len);

                                    // Add surrounding line feeds so that the
                                    // command won't end up being a comment.
                                    buf[osz + len + ins - 1] = buf[osz + len] = '\n';

                                    // Add a newline at the end.
                                    buf[osz + len + ins + strlen(pst)] = '\n';
                                }
                            }
                            else
                            {
                                // Read problem, error will be set
                                // further down if buf == NULL.
                                free(buf);
                                buf = NULL;
                            }
                        }
                        else
                        {
                            // Out of memory
                            PANIC(contxt);
                        }
                    }

                    // Reading done.
                    fclose(fp);
                }
            }
            else
            {
                // Out of memory
                PANIC(contxt);
            }

            // We have no longer any use for the
            // header, footer or command string.
            free(pre);
            free(pst);
            free(cmd);

            // If we have a buffer everything wen't fine
            // above. Go ahead and write buffer to file.
            if(buf)
            {
                // Use a temporary file to make sure that we
                // don't mess up the current file if disk space
                // becomes a problem, the system crashes, the
                // power is lost and so on and so forth.
                size_t tln = strlen(fln) + sizeof(".XXXXXX\0");
                char *tmp = DBG_ALLOC(calloc(tln, 1));

                if(tmp)
                {
                    // Create temporary file.
                    snprintf(tmp, tln, "%s.XXXXXX", fln);
                    FILE *fp = fdopen(mkstemp(tmp), "w");

                    if(fp)
                    {
                        // Write everything to the temporary
                        // file at once.
                        if(fputs(buf, fp) != EOF)
                        {
                            // Close file and release buffer.
                            fclose(fp);
                            free(buf);
                            buf = NULL;

                            // Open the target file just to
                            // make sure that we have write
                            // permissions.
                            fp = fopen(fln, "a");

                            if(fp)
                            {
                                // Close it immediately, we're not
                                // going to write directly to it.
                                fclose(fp);

                                // Do a less un-atomic write to
                                // the real file by renaming the
                                // temporary file.
                                if(!rename(tmp, fln))
                                {
                                    // We're done.
                                    free(tmp);
                                    tmp = NULL;
                                    DNUM = 1;
                                }
                            }
                            else
                            {
                                // We aren't allowed to write data
                                // to the target file so we need to
                                // clean up. We don't want old temp
                                // files laying around.
                                if(remove(tmp))
                                {
                                    // This is highly unlikely, but why not?
                                    ERR(ERR_WRITE_FILE, tmp);
                                }
                            }
                        }
                        else
                        {
                            // Failure. Could not write to disk.
                            // The old file is still intact.
                            fclose(fp);
                        }
                    }

                    // If we haven't released tmp by now, we failed
                    // when attempting one of the write operations
                    // above.
                    if(tmp)
                    {
                        free(tmp);
                        ERR(ERR_WRITE_FILE, fln);
                    }
                }
                else
                {
                    // Out of memory
                    PANIC(contxt);
                }

                // We no longer need the buffer holding
                // the new file contents.
                free(buf);
            }
            else
            {
                // No buffer == read problem, or OOM but then
                // we already have a PANIC.
                ERR(ERR_READ_FILE, fln);
            }
        }
        else
        {
            // Broken parser.
            PANIC(contxt);
        }
    }
    else
    {
        // Broken parser.
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (textfile (prompt..) (help..) (dest..) (append) (include..) (confirm..) (safe))
//      create text file from other text files and strings
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//
//----------------------------------------------------------------------------
entry_p m_textfile(entry_p contxt)
{
    // We need atleast one argument
    if(c_sane(contxt, 1))
    {
        entry_p prompt   = get_opt(contxt, OPT_PROMPT),
                help     = get_opt(contxt, OPT_HELP),
                dest     = get_opt(contxt, OPT_DEST),
                append   = get_opt(contxt, OPT_APPEND),
                include  = get_opt(contxt, OPT_INCLUDE),
                confirm  = get_opt(contxt, OPT_CONFIRM),
                safe     = get_opt(contxt, OPT_SAFE);

        DNUM = 0;

        if(dest)
        {
            // Do we need confirmation?
            if(confirm)
            {
                // The default threshold is expert.
                int level = get_numvar(contxt, "@user-level"),
                    th = 2;

                // If the (confirm ...) option contains
                // something that can be translated into
                // a new threshold value...
                if(confirm->children &&
                   confirm->children[0] &&
                   confirm->children[0] != end())
                {
                    // ...then do so.
                    th = num(confirm);
                }

                // If we are below the threshold value,
                // don't care about getting confirmation
                // from the user.
                if(level < th)
                {
                    confirm = NULL;
                }

                // Make sure that we have the prompt and
                // help texts that we need if 'confirm'
                // is set.
                if(!prompt || !help)
                {
                    char *m = prompt ? "help" : "prompt";
                    ERR(ERR_MISSING_OPTION, m);
                    RCUR;
                }
            }


            // If we need confirmation and the user skips
            // or aborts, return. On abort, the HALT will
            // be set by h_confirm.
            if(confirm && !h_confirm(contxt,
               str(help), str(prompt)))
            {
                RCUR;
            }

            // Is this a safe operation or are we not
            // running in pretend mode?
            if(safe || !get_numvar(contxt, "@pretend"))
            {
                // Overwrite existing file.
                const char *fn = str(dest);
                FILE *fp = fopen(fn, "w");

                if(fp)
                {
                    // Assume success.
                    DNUM = 1;

                    // Append to empty file. This is strange
                    // but it's how the CBM installer works.
                    if(append)
                    {
                        // Gather and merge all (append) strings.
                        char *app = get_optstr(contxt, OPT_APPEND);

                        if(app)
                        {
                            // Log operation.
                            h_log(contxt, tr(S_APND), app, fn);

                            if(fputs(app, fp) == EOF)
                            {
                                ERR(ERR_WRITE_FILE, fn);
                                DNUM = 0;
                            }

                            // Free concatenation.
                            free(app);
                        }
                        else
                        {
                            // Out of memory.
                            PANIC(contxt);
                            DNUM = 0;
                        }
                    }

                    // Are we going to include a file at the
                    // end of the new file (append proper)?
                    if(include && DNUM)
                    {
                        // File to copy.
                        const char *fi = str(include);
                        FILE *fs = fopen(fi, "r");

                        if(fs)
                        {
                            // Use the global buffer.
                            char *buf = get_buf();
                            size_t siz = buf_size(),
                                   n = fread(buf, 1, siz, fs);

                            // Log operation.
                            h_log(contxt, tr(S_INCL), fi, fn);

                            // Copy the whole file.
                            while(n)
                            {
                                // Write to destination file.
                                if(fwrite(buf, 1, n, fp) != n)
                                {
                                    ERR(ERR_WRITE_FILE, fn);
                                    DNUM = 0;
                                    break;
                                }

                                // Do we have more data?
                                n = fread(buf, 1, siz, fs);
                            }

                            // We're done reading from
                            // the include file.
                            fclose(fs);
                        }
                    }

                    // We're done writing to the newly
                    // created file.
                    fclose(fp);

                    // We must append and / or include,
                    // or else this doesn't make sense.
                    if(!append && !include)
                    {
                        ERR(ERR_NOTHING_TO_DO, contxt->name);
                        DNUM = 0;
                    }
                }
                else
                {
                    ERR(ERR_WRITE_FILE, fn);
                }
            }
            else
            {
                // A non safe operation in pretend
                // mode always succeeds.
                DNUM = 1;
            }
        }
        else
        {
            ERR(ERR_MISSING_OPTION, "dest");
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (tooltype (prompt..) (help..) (dest..) (settooltype..) (setstack..)
//     (setdefaulttool..) (noposition) (confirm..) (safe))
//
//     modify an icon
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_tooltype(entry_p contxt)
{
    // No arguments needed.
    if(c_sane(contxt, 0))
    {
        entry_p help            = get_opt(contxt, OPT_HELP),
                prompt          = get_opt(contxt, OPT_PROMPT),
                dest            = get_opt(contxt, OPT_DEST),
                settooltype     = get_opt(contxt, OPT_SETTOOLTYPE),
                setdefaulttool  = get_opt(contxt, OPT_SETDEFAULTTOOL),
                setstack        = get_opt(contxt, OPT_SETSTACK),
                noposition      = get_opt(contxt, OPT_NOPOSITION),
                setposition     = get_opt(contxt, OPT_SETPOSITION),
                confirm         = get_opt(contxt, OPT_CONFIRM),
                safe            = get_opt(contxt, OPT_SAFE);

        DNUM = 0;

        // We need something to work with.
        if(dest)
        {
            // Something is 'dest'.info
            char *file = str(dest);

            // Do we need confirmation?
            if(confirm)
            {
                // The default threshold is expert.
                int level = get_numvar(contxt, "@user-level"),
                    th = 2;

                // If the (confirm ...) option contains
                // something that can be translated into
                // a new threshold value...
                if(confirm->children &&
                   confirm->children[0] &&
                   confirm->children[0] != end())
                {
                    // ...then do so.
                    th = num(confirm);
                }

                // If we are below the threshold value,
                // don't care about getting confirmation
                // from the user.
                if(level < th)
                {
                    confirm = NULL;
                }

                // Make sure that we have the prompt and
                // help texts that we need if 'confirm'
                // is set.
                if(!prompt || !help)
                {
                    char *m = prompt ? "help" : "prompt";
                    ERR(ERR_MISSING_OPTION, m);
                    RCUR;
                }
            }

            // The (noposition) and (setposition)
            // options are mutually exclusive.
            if(noposition && setposition)
            {
                ERR(ERR_OPTION_MUTEX,
                    "noposition/setposition");
                RCUR;
            }

            // A non safe operation in pretend mode
            // always succeeds.
            if(get_numvar(contxt, "@pretend") && !safe)
            {
                RNUM(1);
            }

            // Get confirmation if necessary.
            if(!confirm ||
               h_confirm(contxt, str(help), str(prompt)))
            {
                #ifdef AMIGA
                // Get icon information.
                struct DiskObject *obj = (struct DiskObject *)
                    GetDiskObject(file);

                if(obj)
                {
                    // We need to save the current value
                    // of the tool type and default tool
                    // members in order to not trash mem
                    // when free:ing the diskobject.
                    char *odt = obj->do_DefaultTool;
                    char **tts = (char **) obj->do_ToolTypes;

                    // If we're going to set tooltypes the option
                    // must have one or two children.
                    if(settooltype &&
                       c_sane(settooltype, 1))
                    {
                        // The number of tooltypes.
                        size_t n = 0;

                        // Get tooltype and current value (if it exists).
                        char *t = str(settooltype->children[0]),
                             *o = FindToolType(obj->do_ToolTypes, t);

                        // Get size of tooltype array.
                        while(*(tts + n++));

                        // Set value or create tooltype?
                        if(settooltype->children[1] &&
                           settooltype->children[1] != end())
                        {
                            // Resolve tooltype value.
                            const char *v = str(settooltype->children[1]);

                            // If it already exists, we will replace the old
                            // value with the new one.
                            if(o)
                            {
                                // Allocate memory for a new temporary array.
                                obj->do_ToolTypes = DBG_ALLOC(calloc(n, sizeof(char *)));

                                if(obj->do_ToolTypes)
                                {
                                    char **nts = (char **) obj->do_ToolTypes;

                                    // Copy the current set of tooltypes.
                                    memcpy(nts, tts, n  * sizeof(char **));

                                    // Iterate over the current set.
                                    while(*nts)
                                    {
                                        // Is the found value is within the
                                        // bounds of the current string?
                                        if(o >= *nts &&
                                           o <= (*nts + strlen(*nts)))
                                        {
                                            // Create either a new key -> value pair, or a
                                            // naked key.
                                            if(strlen(v))
                                            {
                                                // Tooltype with value.
                                                snprintf(get_buf(), buf_size(), "%s=%s", t, v);
                                            }
                                            else
                                            {
                                                // Naked tooltype.
                                                snprintf(get_buf(), buf_size(), "%s", t);
                                            }

                                            // Overwrite the old tooltype.
                                            *nts = get_buf();
                                            break;
                                        }

                                        // Next tooltype.
                                        nts++;
                                    }
                                }
                                else
                                {
                                    // Out of memory.
                                    PANIC(contxt);
                                }
                            }
                            // It doesn't exist, append new tooltype.
                            else
                            {
                                // Allocate memory for a new temporary array.
                                obj->do_ToolTypes = DBG_ALLOC(calloc(n + 1, sizeof(char *)));

                                if(obj->do_ToolTypes)
                                {
                                    char **nts = (char **) obj->do_ToolTypes;

                                    // Copy the current set of tooltypes.
                                    memcpy(nts, tts, n * sizeof(char **));

                                    if(strlen(v))
                                    {
                                        // Tooltype with value.
                                        snprintf(get_buf(), buf_size(), "%s=%s", t, v);
                                    }
                                    else
                                    {
                                        // Naked tooltype.
                                        snprintf(get_buf(), buf_size(), "%s", t);
                                    }

                                    // Append tooltype.
                                    *(nts + n - 1) = get_buf();
                                }
                                else
                                {
                                    // Out of memory.
                                    PANIC(contxt);
                                }
                            }
                        }
                        // Delete tooltype.
                        else
                        {
                            // Is there anything to delete?
                            if(o && n > 1)
                            {
                                // Allocate memory for a new temporary array.
                                obj->do_ToolTypes = DBG_ALLOC(calloc(n, sizeof(char *)));

                                if(obj->do_ToolTypes)
                                {
                                    char **nts = (char **) obj->do_ToolTypes,
                                         **ots = tts;

                                    // Delete tooltype by copying everything
                                    // except the tooltype to the new array.
                                    while(*ots)
                                    {
                                        if((o < *ots) ||
                                           (o > (*ots + strlen(*ots))))
                                        {
                                            *nts = *ots;
                                            nts++;
                                        }

                                        ots++;
                                    }
                                }
                                else
                                {
                                    // Out of memory.
                                    PANIC(contxt);
                                }
                            }
                        }
                    }

                    // Change the default tool of project?
                    if(setdefaulttool)
                    {
                        // Set temporary string.
                        obj->do_DefaultTool = (char *)
                            str(setdefaulttool);
                    }

                    // Set tool stacksize?
                    if(setstack)
                    {
                        // Is a minimum value a good idea?
                        obj->do_StackSize = num(setstack);
                    }

                    // Reset icon position?
                    if(noposition)
                    {
                        obj->do_CurrentX = NO_ICON_POSITION;
                        obj->do_CurrentY = NO_ICON_POSITION;
                    }

                    // Set icon position?
                    if(setposition &&
                       c_sane(setposition, 2))
                    {
                        obj->do_CurrentX = num(setposition->children[0]);
                        obj->do_CurrentY = num(setposition->children[1]);
                    }

                    // Save all changes to the .info file.
                    if(PutDiskObject(file, obj))
                    {
                        // Done.
                        DNUM = 1;
                    }
                    else
                    {
                        // We failed for some unknown reason.
                        ERR(ERR_WRITE_FILE, file);
                    }

                    // Restore DiskObject, otherwise memory
                    // will be lost / trashed. Refer to the
                    // icon.library documentation.

                    // If we have a new tooltype array, free
                    // it and reinstate the old one.
                    if(tts != (char **) obj->do_ToolTypes)
                    {
                        free(obj->do_ToolTypes);
                        obj->do_ToolTypes = (STRPTR *) tts;
                    }

                    // No need to free the current string,
                    // just overwrite what we have.
                    obj->do_DefaultTool = odt;

                    // Free the DiskObject after restoring it to
                    // the state it was in before our changes.
                    FreeDiskObject(obj);
                }
                else
                {
                    // More information? IoErr() is nice.
                    ERR(ERR_READ_FILE, file);
                }
                #else
                // On non-Amiga systems we always succeed.
                DNUM = 1;

                // For testing purposes only.
                printf("%s%d%d%d",
                        file,
                        settooltype ? 1 : 0,
                        setstack ? 1 : 0,
                        setdefaulttool ? 1: 0);
                #endif
            }
            else
            {
                // The user did not confirm.
                DNUM = 0;
            }
        }
        else
        {
            // We need an icon.
            ERR(ERR_MISSING_OPTION, "dest");
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (transcript <string1> <string2>)
//     puts concatenated strings in log file
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_transcript(entry_p contxt)
{
    // We need atleast one argument, one or
    // more strings to put into the log file.
    if(c_sane(contxt, 1))
    {
        // Concatenate all children.
        char *msg = get_chlstr(contxt);

        // Assume failure.
        DNUM = 0;

        // Did we manage to concatenate something?
        if(msg)
        {
            // If we could resolve all our children,
            // write the result of the concatenation
            // to the log file (unless logging is
            // disabled).
            if(!DID_ERR())
            {
                DNUM = h_log(contxt, "%s\n", msg) ? 1 : 0;
            }

            // Free the temporary buffer and exit.
            free(msg);
            RCUR;
        }
    }

    // OOM / broken parser.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (rename <old> <new> (help..) (prompt..) (confirm..) (safe))
//     rename files
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_rename(entry_p contxt)
{
    // We need atleast two arguments
    if(c_sane(contxt, 2))
    {
        entry_p help    = get_opt(CARG(3), OPT_HELP),
                prompt  = get_opt(CARG(3), OPT_PROMPT),
                confirm = get_opt(CARG(3), OPT_CONFIRM),
                disk    = get_opt(CARG(3), OPT_DISK),
                safe    = get_opt(CARG(3), OPT_SAFE);

        const char *fr  = str(CARG(1)),
                   *to  = str(CARG(2));

        // Do we need confirmation?
        if(confirm)
        {
            // The default threshold is expert.
            int level = get_numvar(contxt, "@user-level");
            int th = 2;

            // If the (confirm ...) option contains
            // something that can be translated into
            // a new threshold value...
            if(confirm->children &&
               confirm->children[0] &&
               confirm->children[0] != end())
            {
                // ...then do so.
                th = num(confirm);
            }

            // If we are below the threshold value,
            // don't care about getting confirmation
            // from the user.
            if(level < th)
            {
                confirm = NULL;
            }

            // Make sure that we have the prompt and
            // help texts that we need if 'confirm'
            // is set.
            if(!prompt || !help)
            {
                char *m = prompt ? "help" : "prompt";
                ERR(ERR_MISSING_OPTION, m);
                RNUM(0);
            }
        }

        // If we need confirmation and the user skips
        // or aborts, return. On abort, the HALT will
        // be set by h_confirm.
        if(confirm && !h_confirm(CARG(3),
           str(help), str(prompt)))
        {
            RNUM(0);
        }

        // Is this a safe operation or are we not
        // running in pretend mode?
        if(safe || !get_numvar(contxt, "@pretend"))
        {
            // Are we going to rename a file/dir?
            if(!disk)
            {
                // Rename if target doesn't exist.
                if(!h_exists(to) && !rename(fr, to))
                {
                    // Success.
                    h_log(contxt, tr(S_FRND), fr, to);
                    RNUM(-1);
                }

                // Fail if target exists.
                ERR(ERR_RENAME_FILE, fr);
                RNUM(0);
            }

            // No, we're going to relabel a volume.
            #ifdef AMIGA
            // Rename volume.
            if(!Relabel(fr, to))
            {
                // Failure.
                RNUM(0);
            }
            #endif

            // Success.
            h_log(contxt, tr(S_FRND), fr, to);
        }

        // Non safe operation in pretend mode
        // or normal successful operation.
        RNUM(-1);
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// Name:        h_log
// Description: Write formatted message to log file.
// Input:       entry_p contxt:     The execution context.
//              const char *fmt:    Message format string.
//              ...:                Format string varargs.
// Return:      int:                Number of characters written to log file
//                                  if logging is enabled. If logging is
//                                  disabled, '1' will always be returned.
//----------------------------------------------------------------------------
int h_log(entry_p contxt, const char *fmt, ...)
{
    if(contxt && fmt)
    {
        // Only if logging is enabled...
        if(get_numvar(contxt, "@log"))
        {
            // Use the log file set in init(..) or by the user.
            const char *lf = get_strvar(contxt, "@log-file");

            // Don't care about existing files, just append.
            FILE *fp = fopen(lf, "a");
            int n = -1;

            if(fp)
            {
                // Line number and function name as prefix.
                n = fprintf(fp, "[%d:%s] ", contxt->id, contxt->name);

                // Append formatted string.
                if(n > 0)
                {
                    // Use whatever format and arguments we get
                    va_list ap;

                    va_start(ap, fmt);
                    n = vfprintf(fp, fmt, ap);
                    va_end(ap);
                }

                fclose(fp);
            }

            // Could we open the file AND write all data to it?
            if(n < 0)
            {
                ERR(ERR_WRITE_FILE, lf);
                n = 0;
            }

            return n;
        }

        // We suceeded doing
        // nothing.
        return 1;
    }

    // We failed.
    return 0;
}
