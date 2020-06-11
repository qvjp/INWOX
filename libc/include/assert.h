#ifdef __cplusplus
extern "C" {
#endif
#undef assert

#ifdef NDEBUG
#define assert(ignore) ((void) 0)
#else
__attribute__((__noreturn__)) void __assert(
    const char*, const char*, unsigned int, const char*);

#define assert(e) ((e) ? (void) 0 : __assert(#e, __FILE__, __LINE__, __func__))

#endif

#ifdef __cplusplus
}
#endif
