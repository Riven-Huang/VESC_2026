/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_RISC_V_EXTENSIONS_H
#define FREERTOS_RISC_V_EXTENSIONS_H

#if defined(__riscv_flen)
#define portasmADDITIONAL_CONTEXT_SIZE 32 /* Must be even number on 32-bit cores. */
#else
#define portasmADDITIONAL_CONTEXT_SIZE 0 /* Must be even number on 32-bit cores. */
#endif

#ifdef __ASSEMBLER__
.macro portasmSAVE_ADDITIONAL_REGISTERS
#if defined(__riscv_flen)
    addi sp, sp, -(portasmADDITIONAL_CONTEXT_SIZE * FPREGBYTES)
    fsw f0, 1 * FPREGBYTES(sp)
    fsw f1, 2 * FPREGBYTES(sp)
    fsw f2, 3 * FPREGBYTES(sp)
    fsw f3, 4 * FPREGBYTES(sp)
    fsw f4, 5 * FPREGBYTES(sp)
    fsw f5, 6 * FPREGBYTES(sp)
    fsw f6, 7 * FPREGBYTES(sp)
    fsw f7, 8 * FPREGBYTES(sp)
    fsw f8, 9 * FPREGBYTES(sp)
    fsw f9, 10 * FPREGBYTES(sp)
    fsw f10, 11 * FPREGBYTES(sp)
    fsw f11, 12 * FPREGBYTES(sp)
    fsw f12, 13 * FPREGBYTES(sp)
    fsw f13, 14 * FPREGBYTES(sp)
    fsw f14, 15 * FPREGBYTES(sp)
    fsw f15, 16 * FPREGBYTES(sp)
    fsw f16, 17 * FPREGBYTES(sp)
    fsw f17, 18 * FPREGBYTES(sp)
    fsw f18, 19 * FPREGBYTES(sp)
    fsw f19, 20 * FPREGBYTES(sp)
    fsw f20, 21 * FPREGBYTES(sp)
    fsw f21, 22 * FPREGBYTES(sp)
    fsw f22, 23 * FPREGBYTES(sp)
    fsw f23, 24 * FPREGBYTES(sp)
    fsw f24, 25 * FPREGBYTES(sp)
    fsw f25, 26 * FPREGBYTES(sp)
    fsw f26, 27 * FPREGBYTES(sp)
    fsw f27, 28 * FPREGBYTES(sp)
    fsw f28, 29 * FPREGBYTES(sp)
    fsw f29, 30 * FPREGBYTES(sp)
    fsw f30, 31 * FPREGBYTES(sp)
    fsw f31, 32 * FPREGBYTES(sp)
#endif
    .endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
#if defined(__riscv_flen)
    flw f0, 1 * FPREGBYTES(sp)
    flw f1, 2 * FPREGBYTES(sp)
    flw f2, 3 * FPREGBYTES(sp)
    flw f3, 4 * FPREGBYTES(sp)
    flw f4, 5 * FPREGBYTES(sp)
    flw f5, 6 * FPREGBYTES(sp)
    flw f6, 7 * FPREGBYTES(sp)
    flw f7, 8 * FPREGBYTES(sp)
    flw f8, 9 * FPREGBYTES(sp)
    flw f9, 10 * FPREGBYTES(sp)
    flw f10, 11 * FPREGBYTES(sp)
    flw f11, 12 * FPREGBYTES(sp)
    flw f12, 13 * FPREGBYTES(sp)
    flw f13, 14 * FPREGBYTES(sp)
    flw f14, 15 * FPREGBYTES(sp)
    flw f15, 16 * FPREGBYTES(sp)
    flw f16, 17 * FPREGBYTES(sp)
    flw f17, 18 * FPREGBYTES(sp)
    flw f18, 19 * FPREGBYTES(sp)
    flw f19, 20 * FPREGBYTES(sp)
    flw f20, 21 * FPREGBYTES(sp)
    flw f21, 22 * FPREGBYTES(sp)
    flw f22, 23 * FPREGBYTES(sp)
    flw f23, 24 * FPREGBYTES(sp)
    flw f24, 25 * FPREGBYTES(sp)
    flw f25, 26 * FPREGBYTES(sp)
    flw f26, 27 * FPREGBYTES(sp)
    flw f27, 28 * FPREGBYTES(sp)
    flw f28, 29 * FPREGBYTES(sp)
    flw f29, 30 * FPREGBYTES(sp)
    flw f30, 31 * FPREGBYTES(sp)
    flw f31, 32 * FPREGBYTES(sp)
    addi sp, sp, (portasmADDITIONAL_CONTEXT_SIZE * FPREGBYTES)
#endif
    .endm
#endif

#endif // FREERTOS_RISC_V_EXTENSIONS_H
