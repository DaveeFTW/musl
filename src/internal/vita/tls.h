#ifndef TLS_H
#define TLS_H

#define __NR_ARM_set_tls    (0xF0005)

int __vita_set_tls(void *p);

#endif // TLS_H
