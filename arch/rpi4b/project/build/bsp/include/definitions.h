#pragma once

#define LINE_SIZE 64

#if !defined(APP)
#error "APP needs to be defined"
#endif

#define WRITE 1
#define WRITEX6 2
#define WRITEX16 3
#define WRITENEON 4
#define READ 5
#define READNEON 6

#if APP == WRITE
#define ACCESS_METHOD(TARGET)   \
    *(volatile uint8_t *)(TARGET) = 0x00U
#elif APP == READ
#define ACCESS_METHOD(TARGET)   \
    *(volatile uint8_t *)(TARGET)
#elif APP == WRITEX6
#define ACCESS_METHOD(TARGET)   \
    __asm__("                   \
        strb    w0, [%0]        \n\
        add     %0, %0, #%1     \n\
        strb    w0, [%0]        \n\
        add     %0, %0, #%1     \n\
        strb    w0, [%0]        \n\
        add     %0, %0, #%1     \n\
        strb    w0, [%0]        \n\
        add     %0, %0, #%1     \n\
        strb    w0, [%0]        \n\
        add     %0, %0, #%1     \n\
        strb    w0, [%0]        \
            "                   \
        :                       \ 
        :"r"(TARGET),           \
         "i"(LINE_SIZE)         \
        :"x0")           
#elif APP == WRITEX16
#define ACCESS_METHOD(TARGET)   \
    __asm__("                   \
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \n\
        strb    w0, [%0], #%1   \
            "                   \
        :                       \ 
        :"r"(TARGET),           \
         "i"(LINE_SIZE)         \
        :"x0")           
#elif APP == WRITENEON
#define ACCESS_METHOD(TARGET)   \
    __asm__("                   \
        st4     {V0.2D, V1.2D, V2.2D, V3.2D}, [%0]        \
            "                   \
        :                       \ 
        :"r"(TARGET)            \
        :)           
#elif APP == READNEON
#define ACCESS_METHOD(TARGET)   \
    __asm__("                   \
        ld4     [%0], {V0.2D, V1.2D, V2.2D, V3.2D}         \
            "                   \
        :                       \ 
        :"r"(TARGET)            \
        :)           
#else
#error "Specify the access method"
#endif

/*
 *  PREPROCESSOR LOGIC
 */
#if defined(VICTIM)
#define HEADER register uint32_t j = 0 ; j < ITERATIONS ; j++
#else
#define HEADER ; ;
#endif

#define INIT()      \ 
    __asm__ (       \
    "       \
    mrs x0, PMCNTENSET_EL0          \n\
    and x0, x0, #0x7FFFFFC0         \n\
    orr x0, x0, #0x80000000         \n\
    msr PMCNTENSET_EL0, x0          \n\
    "                               \
    :::"x0")

            

#define PREP()      \
    __asm__ (       \
    "       \
    mrs x0, PMCR_EL0                \n\
    and x0, x0, #0xFFFFFFC0         \n\
    mov x1, #0x5                    \n\
    orr x0, x0, x1                  \n\
    msr PMCR_EL0, x0                \n\
    "                               \
    :::"x0", "x1")

#define RETRIEVE()  \
    {               \
	register u64 value=0;   \
	__asm__ ("mrs %0, PMCCNTR_EL0" : "=r" (value));     \
    uart_int(value); uart_nl();    \
    }

#define END()       \
    {               \
    uart_str("Fin"); uart_nl();\
    }
