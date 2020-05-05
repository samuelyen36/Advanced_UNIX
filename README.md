# Advanced_UNIX

Homework of Advanced UNIX programming.

## Homework1
Practice the use of directory related API, write an tool which has the similar function with netstat
### detail implementation
search /proc/$pid/fd first
找到socket number
去對/proc/net/tcp，找出src IP and des IP
### related link
[/proc/net/tcp](https://guanjunjian.github.io/2017/11/09/study-8-proc-net-tcp-analysis/)
[readdir](https://www.itread01.com/content/1510067052.html)


## Homework2
Practice injecting the shared library. Monitor the parameter that is fed into these shared libraries.

### related link
[source_code_of_libs](https://code.woboq.org/)


