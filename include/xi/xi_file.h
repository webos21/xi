/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XI_FILE_H_
#define _XI_FILE_H_

/**
 * @brief XI File/Directory Handling API
 *
 * @file xi_file.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"
#include "xi_clock.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN


/**
 * @defgroup xi_file File/Directory Handling API
 * @ingroup XI
 * @{
 */

/**
 * Return values of File/Dir Functions
 */
typedef enum _e_file_rv {
	XI_FILE_RV_OK          = 0,   ///< OK
	XI_FILE_RV_EOF         = 0,   ///< End of File
	XI_FILE_RV_ERR_ARGS    = -1,  ///< Invalid Arguement
	XI_FILE_RV_ERR_PERM    = -2,  ///< Permission Denyed
	XI_FILE_RV_ERR_NF      = -3,  ///< File Not Found
	XI_FILE_RV_ERR_AE      = -4,  ///< File Already Exists
	XI_FILE_RV_ERR_MAX     = -5,  ///< Reached the maximum file open
	XI_FILE_RV_ERR_FD      = -6,  ///< Bad File Descriptor
	XI_FILE_RV_ERR_DIR     = -7,  ///< Not a file, It's a Directory
	XI_FILE_RV_ERR_NOTDIR  = -8,  ///< Not a file, It's a Directory
	XI_FILE_RV_ERR_OVER    = -9,  ///< Over-Flow Occurred
	XI_FILE_RV_ERR_NOMEM   = -10, ///< No Memory
	XI_FILE_RV_ERR_INTR    = -11, ///< Interrupted
	XI_FILE_RV_ERR_BIG     = -12, ///< Big
	XI_FILE_RV_ERR_BUSY    = -13, ///< File is used
	XI_FILE_RV_ERR_NOTEMPT = -14, ///< Not empty
	XI_FILE_RV_ERR_IO      = -15  ///< I/O Error
} xi_file_re;


/**
 * File Mode
 */
typedef enum _e_file_mode {
	XI_FILE_MODE_READ       = 0x00001,  ///< For Reading
	XI_FILE_MODE_WRITE      = 0x00002,  ///< For Writing
	XI_FILE_MODE_CREATE     = 0x00004,  ///< Create file, if not exists
	XI_FILE_MODE_APPEND     = 0x00008,  ///< Append end of the file
	XI_FILE_MODE_TRUNCATE   = 0x00010,  ///< Truncate the file when open
	XI_FILE_MODE_EXCL       = 0x00020,  ///< Open should fail if CREATE and file exists
	XI_FILE_MODE_DIRECT     = 0x00040,  ///< Direct I/O (not use the internal buffer)
	XI_FILE_MODE_LARGEFILE  = 0x00080,  ///< Use Large file (> 2GB)
	XI_FILE_MODE_NONBLOCK   = 0x00100   ///< Non-Block Access
} xi_file_mode_e;


/**
 * File Type
 */
typedef enum _e_file_type {
	XI_FILE_TYPE_INIT      = 0,  ///< Initial Value
	XI_FILE_TYPE_REG       = 0x0100000,  ///< Regular File
	XI_FILE_TYPE_DIR       = 0x0040000,  ///< Directory
	XI_FILE_TYPE_CHR       = 0x0020000,  ///< Character Device File
	XI_FILE_TYPE_BLK       = 0x0060000,  ///< Block Device File
	XI_FILE_TYPE_PIPE      = 0x0010000,  // Pipe
	XI_FILE_TYPE_LNK       = 0x0120000,  // Symbolic Link
	XI_FILE_TYPE_SOCK      = 0x0140000,  // Socket
	XI_FILE_TYPE_UNKNOWN   = -1  // Unknown File
} xi_file_type_e;


/**
 * File Permission  -- OCTET
 */
typedef enum _e_file_perm {
	XI_FILE_PERM_CTL_STICKY = 01000, ///< Stick Bit
	XI_FILE_PERM_CTL_GID    = 02000, ///< Set GID
	XI_FILE_PERM_CTL_UID    = 04000, ///< Set UID
	XI_FILE_PERM_USR_EXEC   = 00100, ///< User Executable
	XI_FILE_PERM_USR_WRITE  = 00200, ///< User Writable
	XI_FILE_PERM_USR_READ   = 00400, ///< User Readable
	XI_FILE_PERM_USR_ALL    = 00700, ///< User gets all permission
	XI_FILE_PERM_GRP_EXEC   = 00010, ///< Group Executable
	XI_FILE_PERM_GRP_WRITE  = 00020, ///< Group Writable
	XI_FILE_PERM_GRP_READ   = 00040, ///< Group Readable
	XI_FILE_PERM_GRP_ALL    = 00070, ///< Group gets all permission
	XI_FILE_PERM_OTH_EXEC   = 00001, ///< Other Executable
	XI_FILE_PERM_OTH_WRITE  = 00002, ///< Other Writable
	XI_FILE_PERM_OTH_READ   = 00004, ///< Other Readable
	XI_FILE_PERM_OTH_ALL    = 00007  ///< Other gets all permission
} xi_file_perm_e;


/**
 * File Seek Mode
 */
typedef enum _e_file_seek {
	XI_FILE_SEEK_SET       = 0,  ///< Absolute Position
	XI_FILE_SEEK_CUR       = 1,  ///< Relative Position from Current
	XI_FILE_SEEK_END       = 2   ///< Relative Position from End
} xi_file_seek_e;


/**
 * The structure represents a IO Vector.
 */
typedef struct _st_file_iovec {
    xvoid  *iov_base;    ///< Starting address
    xint32  iov_len;     ///< Number of bytes to transfer
} xi_file_iovec_t;


/**
 * File Status Information
 */
typedef struct _st_file_stat {
	xint32          type;        ///< Type (xi_file_type_e)
	xint32          perm;        ///< Permission (xi_file_perm_e)
	xint64          size;        ///< Size
	xint64          blocks;      ///< Allocated Blocks
	xint64          created;     ///< Creation Time (msec)
	xint64          accessed;    ///< Last accessed Time (msec)
	xint64          modified;    ///< Last modified Time (msec)
	xchar           pathname[XCFG_PATHNAME_MAX];  ///< File Path and Name
	xchar           filename[XCFG_PATHNAME_MAX];  ///< File Name Only
} xi_file_stat_t;


/**
 * Disk Space Information
 */
typedef struct _st_fs_space {
	xint64   total;     ///< Total Disk Space
	xint64   avail;     ///< Available Disk Space
	xint64   free;      ///< Free Disk Space
} xi_fs_space_t;


/**
 * structure of the resource-path
 */
typedef struct _st_pathname {
	xchar drive[XCFG_DRVNAME_MAX];     ///< Drive Character (win32 only)
	xchar dirname[XCFG_PATHNAME_MAX];  ///< Directory Path
	xchar basename[XCFG_PATHNAME_MAX]; ///< FileName + Suffix
	xchar filename[XCFG_PATHNAME_MAX]; ///< FileName
	xchar suffix[XCFG_PATHNAME_MAX];   ///< Suffix
} xi_pathname_t;


/**
 * file handle
 */
typedef xfd              xi_file_t;


/**
 * directory handle
 */
typedef struct _xi_dir   xi_dir_t;


/**
 * Open the specified file.
 *
 * @param pathname The full path to the file (using / on all systems)
 * @param mode The flag of opening the file. (xi_file_mode_e)
 * @param perm Access permissions for file.  (xi_file_perm_e)
 * @return the number of file-descriptor.
 */
xint32       xi_file_open(const xchar *pathname, xint32 mode, xint32 perm);


/**
 * Peek the read-available data size
 *
 * @param fd The file descriptor to read from.
 * @return the number of bytes can be read; otherwise error(<0).
 */
xint32       xi_file_rpeek(xint32 fd);


/**
 * Read data from the specified file.
 *
 * @param fd The file descriptor to read from.
 * @param buf The buffer to store the data to.
 * @param buflen The length of buffer and the number of bytes to read.
 * @return the number of bytes read.
 */
xssize       xi_file_read(xint32 fd, xvoid *buf, xsize buflen);


/**
 * Write data to the specified file.
 *
 * @param fd The file descriptor to write to.
 * @param buf The buffer which contains the data.
 * @param buflen The length of buffer and the number of bytes to write.
 * @return the number of bytes written.
 */
xssize       xi_file_write(xint32 fd, const xvoid *buf, xsize buflen);


/**
 * Read vector data from the specified file.
 *
 * @param fd The file descriptor to read from.
 * @param iov The structure of IOVEC.
 * @param iovlen The length of structure of IOVEC.
 * @return the number of bytes read.
 */
xssize       xi_file_readv(xint32 fd, const xi_file_iovec_t *iov, xint32 iovlen);


/**
 * Write vector data to the specified file.
 *
 * @param fd The file descriptor to write to.
 * @param iov The structure of IOVEC.
 * @param iovlen The length of structure of IOVEC.
 * @return the number of bytes written.
 */
xssize       xi_file_writev(xint32 fd, const xi_file_iovec_t *iov, xint32 iovlen);


/**
 * Move the read/write file offset to a specified byte within a file.
 *
 * @param fd The file descriptor.
 * @param pos The offset to move the pointer to.
 * @param whence How to move the pointer. (xi_file_seek_e)
 * @return The offset of moved position.
 */
xoff64       xi_file_seek(xint32 fd, xoff64 pos, xint32 whence);


/**
 * Truncate the file from current position to length.
 *
 * @param fd The file descriptor.
 * @param len The length from current position to be truncated.
 * @return On success, zero is returned.  On error, -1 is returned.
 */
xint32       xi_file_ftruncate(xint32 fd, xoff64 len);


/**
 * Lock the specific file area.
 *
 * @param fd    The file descriptor.
 * @param spos  The position to lock.
 * @param len   The length to lock.
 * @param bexcl The lock mode - exclusive?
 * @param bwait The lock mode - wait?
 */
xi_file_re   xi_file_lock(xint32 fd, xoff64 spos, xoff64 len, xint32 bexcl, xint32 bwait);


/**
 * Unlock the specific file area.
 *
 * @param fd   The file descriptor.
 * @param spos The position to unlock.
 * @param len  The length to unlock.
 */
xi_file_re   xi_file_unlock(xint32 fd, xoff64 spos, xoff64 len);


/**
 * Synchronize the specified file.
 *
 * @param fd The file descriptor to synchronize.
 */
xi_file_re   xi_file_sync(xint32 fd);


/**
 * Synchronize the specified file.
 *
 * @param fd The file descriptor to synchronize.
 */
xi_file_re   xi_file_pipe(xint32 fd[2]);


/**
 * Close the specified file.
 *
 * @param fd The file descriptor to close.
 */
xi_file_re   xi_file_close(xint32 fd);


/**
 * open standard input.
 *
 * @return The file descriptor of standard input.
 */
xint32       xi_file_get_stdin();


/**
 * open standard output.
 *
 * @return The file descriptor of standard output.
 */
xint32       xi_file_get_stdout();


/**
 * open standard error.
 *
 * @return The file descriptor of standard error.
 */
xint32       xi_file_get_stderr();


/**
 * Read symbolic link.
 *
 * @param pathname The symbolic link path.
 * @param buf The buffer to be filled result.
 * @param buflen The length of buffer.
 * @result The length of result string.
 */
xssize       xi_file_readlink(const xchar *pathname, xchar *buf, xsize buflen);


/**
 * Set the specified file's permission bits.
 *
 * @param pathname The file (name) to apply the permissions to.
 * @param perm The permission bits to apply to the file. (xi_file_perm_e)
 */
xi_file_re   xi_file_chmod(const xchar *pathname, xint32 perm);


/**
 * Rename or Move the specified file.
 *
 * @param frompath The full path to the original file (using / on all systems)
 * @param topath The full path to the new file (using / on all systems)
 *
 * @warning If a file exists at the new location, then it will be
 * overwritten.  Moving files or directories across devices may not be
 * possible.
 */
xi_file_re   xi_file_rename(const xchar *frompath, const xchar *topath);


/**
 * Delete the specified file.
 *
 * @param pathname The full path to the file (using / on all systems)
 */
xi_file_re   xi_file_remove(const xchar *pathname);


/**
 * get the specified file's stats.  The file is specified by filename,
 * instead of using a pre-opened file.
 *
 * @param pathname The name of the file to stat.
 * @param s Where to store the information about the file, which is never touched if the call fails.
 */
xi_file_re   xi_file_stat(const xchar* pathname, xi_file_stat_t *s);


/**
 * get the specified file's stats using a pre-opened file.
 *
 * @param fd The file descriptor returned from xi_file_open.
 * @param s Where to store the information about the file, which is never touched if the call fails.
 */
xi_file_re   xi_file_fstat(xint32 fd, xi_file_stat_t *s);


/**
 * File system space information.
 *
 * @param pathname The name of the mount point.
 * @param s Where to store the information about the file system, which is never touched if the call fails.
 */
xi_file_re   xi_file_fsspace(const xchar* pathname, xi_fs_space_t *s);


/**
 * Open the specified directory.
 *
 * @param pathname The full path to the directory (use / on all systems)
 * @return The directory descriptor
 */
xi_dir_t    *xi_dir_open(const xchar *pathname);


/**
 * Read the next entry from the specified directory.
 *
 * @param dfd the directory descriptor returned from xi_dir_open.
 * @param stat the file info structure and filled in by xi_dir_read.
 */
xint32       xi_dir_read(xi_dir_t *dfd, xi_file_stat_t *stat);


/**
 * Rewind the directory to the first entry.
 *
 * @param dfd the directory descriptor returned from xi_dir_open.
 */
xi_file_re   xi_dir_rewind(xi_dir_t *dfd);


/**
 * close the specified directory.
 *
 * @param dfd the directory descriptor to close.
 */
xi_file_re   xi_dir_close(xi_dir_t *dfd);


/**
 * Create a new directory on the file system.
 *
 * @param pathname the path for the directory to be created. (use / on all systems)
 * @param perm Permissions for the new directory. (xi_file_perm_e)
 */
xi_file_re   xi_dir_make(const xchar *pathname, xint32 perm);


/**
 * Creates a new directory on the file system, but behaves like
 * 'mkdir -p'. Creates intermediate directories as required. No error
 * will be reported if PATH already exists.
 *
 * @param pathname the path for the directory to be created. (use / on all systems)
 * @param perm Permissions for the new directory. (xi_file_perm_e)
 */
xi_file_re   xi_dir_make_force(const xchar *pathname,  xint32 perm);


/**
 * Delete the specified directory.
 *
 * @param pathname The directory path to be deleted.
 */
xi_file_re   xi_dir_remove(const xchar *pathname);


/**
 * Get the current working directory.
 * same as 'pwd' command.
 *
 * @param pathbuf The buffer to be filled with CWD.
 * @param pblen The length of buffer.
 */
xi_file_re   xi_pathname_get(xchar *pathbuf, xuint32 pblen);   // getcwd


/**
 * Set the current working directory.
 * same as 'cd path-to-changed' command.
 *
 * @param path The path to be changed to.
 */
xi_file_re   xi_pathname_set(const xchar *path);               // chdir


/**
 * Get the absolute path of given path
 *
 * @param pathbuf The buffer to be filled with absolute path. (it must have enough length)
 * @param pblen The length of buffer.
 * @param pathname The give path to be interpreted.
 */
xi_file_re   xi_pathname_absolute(xchar *pathbuf, xuint32 pblen, const xchar *pathname);


/**
 * Get the filename from a specific path.
 *
 * @param path The path that contains filename.
 */
xchar       *xi_pathname_basename(const xchar *path);


/**
 * Split a search path into separate components
 *
 * @param path the paths to be split
 * @param pathname the path info structure to be filled
 */
xi_file_re   xi_pathname_split(xi_pathname_t *pathname, const xchar *path);


/**
 * Merge additional file path onto the previously processed by xi_pathname_split
 *
 * @param pathbuf the merged paths returned
 * @param pblen the length of the pathbuf
 * @param pathname the path info structure to be merged
 */
xi_file_re   xi_pathname_merge(xchar *pathbuf, xuint32 pblen, xi_pathname_t pathname);

/**
 * @}  // end of xi_file
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_FILE_H_
