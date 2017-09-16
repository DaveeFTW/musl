#ifndef FD_H
#define FD_H

#define MAX_OPEN_FILES 256

typedef enum
{
    VITA_DESCRIPTOR_FILE,
    VITA_DESCRIPTOR_SOCKET,
    VITA_DESCRIPTOR_PIPE,
    VITA_DESCRIPTOR_TTY
} DescriptorTypes;

typedef struct
{
    int sce_uid;
    DescriptorTypes type;
    int ref_count;
    int flags;
} DescriptorTranslation;

int __vita_acquire_descriptor(void);
int __vita_release_descriptor(int fd);
int __vita_duplicate_descriptor(int fd);
int __vita_descriptor_ref_count(int fd);
DescriptorTranslation *__vita_fd_grab(int fd);
int __vita_fd_drop(DescriptorTranslation *fdmap);

#endif // FD_H
