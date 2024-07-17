 // Include pour les benchmarks du pilier 1

#ifndef TAILLE
//#define TAILLE 16777216 // 128 MB
//#define TAILLE 8388608 // 64 MB
//#define TAILLE 4194304 // 32 MB
#define TAILLE 1048576 // 8 MB
#endif

#ifndef TAILLE_MIN
#define TAILLE_MIN 2048 // 16 kB
#endif

#ifndef TAILLE_MAX
#define TAILLE_MAX 524288 // 4 MB
#endif

#ifndef STRIDE_MAX
//#define STRIDE_MAX 512
#define STRIDE_MAX 2048
//#define STRIDE_MAX 4096
//#define STRIDE_MAX 8192
//#define STRIDE_MAX 16384
//#define STRIDE_MAX 32768
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846    /* pi */
#endif

#ifndef MICROBENCH_INT
#define MICROBENCH_INT
int microbench_int ( int ite, int mode ) ;
#endif

#ifndef FACTO_PREMIER_INT
#define FACTO_PREMIER_INT
int factoPremier_int ( int value, int mode ) ;
#endif

#ifndef MICROBENCH_LONG
#define MICROBENCH_LONG
long microbench_long ( int ite, int mode ) ;
#endif

#ifndef FACTO_PREMIER_LONG
#define FACTO_PREMIER_LONG
long factoPremier_long ( long  value, int mode ) ;
#endif

#ifndef MICROBENCH_FLOAT
#define MICROBENCH_FLOAT
float  microbench_float ( int ite, int mode ) ;
#endif

#ifndef COMPUTE_PI_FLOAT
#define COMPUTE_PI_FLOAT
float computePI_float (int iter, int mode ) ;
#endif

#ifndef MICROBENCH_DOUBLE
#define MICROBENCH_DOUBLE
double microbench_double( int ite, int mode ) ;
#endif

#ifndef COMPUTE_PI_DOUBLE
#define COMPUTE_PI_DOUBLE
double computePI_double (int iter, int mode ) ;
#endif

#ifndef MICROBENCH_LOAD_CONSTANT_SIZE
#define MICROBENCH_LOAD_CONSTANT_SIZ
long microbench_load_constant_size ( long  *tab, int lg, int stride, int mode ) ;
#endif

#ifndef MICROBENCH_LOAD_CONSTANT_STRIDE
#define MICROBENCH_LOAD_CONSTANT_STRIDE
long microbench_load_constant_stride(long * tab, int lg, int size, int stride, int mode ) ;
#endif

#ifndef MICROBENCH_LOAD_LOCALLY_RANDOMIZED_STRIDE
#define MICROBENCH_LOAD_LOCALLY_RANDOMIZED_STRIDE
int microbench_load_locally_randomized_stride (long * tab, int size, int radius, int mode ) ;
#endif

#ifndef MICROBENCH_LOAD_FULLY_RANDOMIZED_STRIDE
#define MICROBENCH_LOAD_FULLY_RANDOMIZED_STRIDE
int microbench_load_fully_randomized_stride (long * tab, int size, int mode ) ;
#endif

#ifndef MICROBENCH_LOAD_CONSTANT_SIZE_WITH_INDEX_CHASING
#define MICROBENCH_LOAD_CONSTANT_SIZE_WITH_INDEX_CHASING
int microbench_load_constant_size_with_index_chasing (long *tab, int lg, int stride, int mode ) ;
#endif

#ifndef MICROBENCH_STORE_CONSTANT_SIZE
#define MICROBENCH_STORE_CONSTANT_SIZE
long microbench_store_constant_size(long  *tab, int lg, int stride, int mode) ;
#endif

#ifndef MICROBENCH_STORE_CONSTANT_STRIDE
#define MICROBENCH_STORE_CONSTANT_STRIDE
long microbench_store_constant_stride(long * tab, int lg, int size, int stride, int mode) ;
#endif

#ifndef MICROBENCH_LOADSTORE_CONSTANT_SIZE
#define MICROBENCH_LOADSTORE_CONSTANT_SIZE
long microbench_loadstore_constant_size(long * tab, int lg, int stride, int mode) ;
#endif

#ifndef MICROBENCH_LOADSTORE_CONSTANT_STRIDE
#define MICROBENCH_LOADSTORE_CONSTANT_STRIDE
long microbench_loadstore_constant_stride(long * tab, int lg, int size, int stride, int mode ) ;
#endif

#ifndef MICROBENCH_NULL
#define MICROBENCH_NULL
void microbench_null(int mode) ;
#endif
