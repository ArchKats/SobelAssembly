        section .text
        global bmptogray_conversion

; --------bmptogray_conversion----------
;|                                      |
;| Converts an array of rgb struct array|
;| to a grayscale array (int values).   |
;| It has 2 floating points accuracy.   |
;|                                      |
; --------------------------------------
;|ebp+8 = height                        |
;|ebp+12= width                         |
;| ebp+16= image (struct)               |
;|ebp+20= gray_image (int)              |
; --------------------------------------

bmptogray_conversion:

        enter   0,              0                       ;       create stack frame

        push    esi                                     ;       "save" last states of ebx, esi and edi registers
        push    edi
        push    ebx

        mov     esi,            [ebp+16]                ;       esi= *image
        mov     edi,            [ebp+20]                ;       edi= *gray_image
                                                        ;       for loop 
        mov     ecx,            [ebp+12]                ;       for(int i=0; i<width; i++)      

.wloop:
        
        push    ecx                                     
                                                        ;       outer loop body
        mov     ecx,            [ebp+8]                 ;       for(int j=0; j<height; j++)

        push    edi                                     
        push    esi
        
.hloop:
        
        push    ecx
                                                        ;       inner loop body
        mov     ebx,            0
        ;mov    edx,            0                       ;       works fine without this for some reason :/
        mov     eax,            0                       ;       RED=(rgbRed*0.29~~~)*100        
        mov     al,             [esi+2]
        mov     ecx,            29
        mul     ecx
        add     ebx,            eax

        mov     eax,            0                       ;       GREEN=(rgbGreen*0.58~~~)*100
        mov     al,             [esi+1]
        mov     ecx,            58
        mul     ecx
        add     ebx,            eax

        mov     eax,            0                       ;       BLUE=(rgbBlue*0.11~~~)*100
        mov     al,             [esi]
        mov     ecx,            11
        mul     ecx
        add     ebx,            eax
        
        mov     eax,            ebx                     ;       GRAY= (RED+BLUE+GREEN)/100
        mov     ecx,            100
        div     ecx

        mov     [edi],          eax                     ;       insert value to gray_image array

                                                        ;       next cell
        add     esi,            4
        add     edi,            4
                                                        ;       end of inner loop body
        pop     ecx
        
        loop    .hloop

        pop     esi
        pop     edi

        add     esi,            2048*4                  ;       offset the (array) indexes by one column
        add     edi,            2048*4
                                                        ;       end of outer loop body
        pop     ecx

        loop    .wloop

        pop     ebx
        pop     edi
        pop     esi

        leave                                           ;       destroy stack frame
        
        mov     eax,            0

        ret

