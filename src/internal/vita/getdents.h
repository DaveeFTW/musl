#ifndef GETDENTS_H
#define GETDENTS_H

struct dirent;

int __vita_getdents(unsigned int fd, struct dirent *dirp, unsigned int count);

#endif // GETDENTS_H
