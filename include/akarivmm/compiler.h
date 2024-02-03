#ifndef AKARIVMM_COMPILER_H
#define AKARIVMM_COMPILER_H

#define __unused        __attribute__((unused))
#define __fallthrough   __attribute__((fallthrough))
#define __packed        __attribute__((packed))
#define __aligned(n)    __attribute__((aligned(n)))
#define __noreturn      __attribute__((noreturn))
#define __section(s)    __attribute__((section(s)))
#define __used          __attribute__((used))

#define likely(cond)    __builtin_expect(!!(cond), 1)
#define unlikely(cond)  __builtin_expect(!!(cond), 0)

#endif	/* AKARIVMM_COMPILER_H */
