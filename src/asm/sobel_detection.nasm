        section .data 
        S1:     dq      -1.0, 0, 1.0, -2.0, 0, 2.0, -1.0, 0, 1.0 
        S2:     dq      -1.0, -2.0, -1.0, 0, 0, 0, 1.0, 2.0, 1.0

        section .text
        global sobel_detection

; ---------sobel-detection----------------
;| A function that uses sobel operator    |
;| for edge detection of a gray scaled    |
;| image                                  |
;|                                        |
;|                                        |
;|                                        |
; ----------------------------------------
;| ebp+8 = height                         |
;| ebp+12= width                          |
;| ebp+16= gray_image (int)               |
;| ebp+20= output array (unsigned char)   |
; ----------------------------------------
sobel_detection:
        enter   4,              0                       ;       create stack frame, use 1 double word local variable for moving data between the FPU and the CPU

        push    esi                                     ;       save esi, edi and ebx in the stack
        push    edi
        push    ebx
        
        mov     esi,            [ebp+16]                ;       esi=*gray_image ( int == double word)
        mov     edi,            [ebp+20]                ;       edi=*ee_image ( char == byte)

        mov     ecx,            [ebp+12]                ;       for ( int x=0; x<width-2; x++)
        sub     ecx,            2
.wloop:
        push    ecx                                     
                                                        ;       outer loop body 
        push    esi                                     
        push    edi
 
        mov     ecx,            [ebp+8]
        sub     ecx,            2                       ;       for ( int y=0; y<height-2; y++)
.hloop:
        push    ecx
                                                        ;       inner loop body

        finit                                           ;       initializing the fpu after every loop (makes a difference!!!)

        mov     ebx,            S1
        call    kernel_multiplication                   ;       calculating GX, at the end of the procedure it is in ST0

        fmul    st0,            st0                     ;       ST0 = GX^2 

        mov     ebx,            S2
        call    kernel_multiplication                   ;       calculating GY, at the end of the procedure it is in ST0

        fmul    st0,            st0                     ;       ST0 = GY^2 ; ST1 == GX^2

        faddp                                           ;       ST0 = ST0 + ST1
        fsqrt                                           ;       calculate the e variable 

        fist    dword           [ebp-4]                 ;       convert st0's value to an integer and store it  
        mov     eax,            [ebp-4]                 ;       in eax

        cmp     eax,            THRESHOLD       
        jb      .ponyo                                  ;       if e > THRESHOLD

        mov     [edi+1+2048],   byte 255                ;       do ee_image[x+1][y+1]=255       
        jmp     .ponyop

.ponyo:
        mov     [edi+1+2048],   byte 0                  ;       else do ee_image[x+1][y+1]=0
.ponyop:

                                                        
        add     esi,            4                       ;       move indexes to the next element(s)
        add     edi,            1

                                                        ;       end of inner loop body
        pop     ecx
        loop    .hloop

        pop     edi
        pop     esi

        add     esi,            2048*4                  ;       move indexes to the start of the next column 
        add     edi,            2048
                                                        ;       end of outer loop body
        pop     ecx
        loop    .wloop

        pop     ebx
        pop     edi
        pop     esi

        leave                                           ;       destroy stack frame

        mov     eax,            0                       ;       return 0

        ret


; ----------Kernel-Multiplication-----------
;| Multiplies two 3x3 matrixes, A and B     |
;| A is a matrix of double float indexed ebx|
;| B is a part of a matrix of integers      |
;|   original matrix is (doesnt matter)x2048|
; ------------------------------------------
kernel_multiplication:
                                                        ;       no stack frame \(OωO )/
        push    esi

        mov     eax,            0                       ;       push 0 to the fpu stack of registers (initial)
        push    eax
        fild    dword           [esp]
        pop     eax

        mov     ecx,            3                       ;       for (int i=0; i<3 ; i++)
.iloop:
        push    ecx
                                                        
        mov     ecx,            3                       ;       for (int j=0; j<3 ; j++)
        push    esi
.jloop:
        push    ecx

        fld     qword           [ebx]                   ;       load [ebx] to st0
        fild    dword           [esi]                   ;       convert [esi] from an integer to a double and then load it to st0
        fmulp                                           ;       st0 = st0 * st1 ; st0 = [ebx] * (double)[esi]
        faddp                                           ;       sum the product with the previous one (initial 0)
        

        add     esi,            4                       ;       move indexes to the next elements
        add     ebx,            8

                                                        ;       end jloop
        pop     ecx
        loop    .jloop

        pop     esi
        
        add     esi,            2048*4                  ;       move index to the next column, not doing it for ebx cause its elements are in series
        
        pop     ecx
        loop    .iloop

        pop     esi

        ret
        
