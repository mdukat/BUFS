# BUFS
Backup Filesystem

## Purpose
CD/DVD and Tape backups.

## BUFS vs ISOFS
BUFS saves whole file names, saves a lot of space, and is lightweight.

## Usage
```
./bufs [hvxlc] image_file [input_dir]/[output_file]
	 h - Show this help
	 v - Show version
	 x image_file output_file - Extract output_file from image_file
	 l image_file - List files in image
	 c image_file input_dir - Create image from files in input_dir
```

## Examples
 - `bufs l /dev/dvd` - List files on BUFS on CD/DVD drive
 - `bufs c /tmp/image.img /home/SuperCoolUser/things\ to\ backup/` - Creates image which you can write to CD/DVD drive
 - `bufs x /dev/dvd "backup-01.01.2019.tar.xz"` - Extracts file from CD/DVD drive, to current directory

## Compilation
```
./build.sh
```

# Some nerd things you probably dont want to do
## Debugging
Good way to debug things, is to backtrace bufs binary with gdb. Example:
```
#~ gdb bufs
(gdb) run l /dev/dvd
(gdb) bt
```

In case of possible memleak, or errors, use valgrind. Example:
```
#~ valgrind bufs l /dev/dvd
==12925== 
==12925== HEAP SUMMARY:
==12925==     in use at exit: 0 bytes in 0 blocks
==12925==   total heap usage: 236 allocs, 236 frees, 15,304,997 bytes allocated
==12925== 
==12925== All heap blocks were freed -- no leaks are possible
==12925== 
==12925== For counts of detected and suppressed errors, rerun with: -v
==12925== ERROR SUMMARY: 116 errors from 2 contexts (suppressed: 0 from 0)
```
