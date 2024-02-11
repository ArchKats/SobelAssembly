section .text
global border_pixel_calculation


; -------border-pixel-calculation-------------------
;| Implements a function that calculates through    |
;| duplication the border pixels of an image        |
; -------------------------------------------------- 
;| [ebp+8] -> height                                |
;| [ebp+12]-> width                                 |
;| [ebp+16]-> ee_image                              |
; --------------------------------------------------
border_pixel_calculation:
	enter 	0, 		0 			; 	create stack frame
	push 	ebx

	call  	verloop 				; 	call first loop (vertical borders)

	call 	horloop 				; 	call second loop (horizontal borders)

	pop 	ebx

	leave
	ret

verloop:
	mov 	esi, 		[ebp+16] 		; 	y=1; 	esi = ee_image[0][1];
	add 	esi, 		1
	
	mov 	edx, 		0 			; 	since I move the data 8 cells per time
	mov 	eax, 		[ebp+8]  		; 	I divide the height (no. iterations) by 8
	sub 	eax, 		2
	mov 	ecx, 		8
	div 	ecx
	mov 	ecx, 		eax
	inc 	ecx 					; 	loops/8
.loop:
	push 	ecx
	
	mov 	ebx, 		2048 			; 	because it isn't a widthxheight array but a 2048x2048 array...........
	
							; 	ee_image[0][y]=ee_image[1][y]
	movq 	mm0, 		[esi+1*ebx] 		; 	I just use mmx as a 64 processor for data transfer...	
	movq 	[esi], 		mm0
	
	mov 	eax, 		[ebp+12] 		
	mul 	ebx
							; 	ee_image[width-1][y]=ee_image[width-2][y]
	movq 	mm1, 		[esi+eax-2]
	movq 	[esi+eax-1], 	mm1

	pop 	ecx
	add 	esi, 		8

	loop 	.loop

	ret


horloop:
	mov 	esi, 		[ebp+16] 		; 	esi=*ee_image[0][0]	

	mov 	ecx, 		[ebp+12]

.loop:
							; 	Since the horizontal lines arent in series, I can't use mmx 
	push 	ecx

	mov 	cl, 		[esi+1] 	 	
	mov 	[esi], 		cl  			; 	ee_image[x][0]= ee_image[x][1]

	mov 	ebx, 		[ebp+8] 		
	sub 	ebx, 		2
	mov 	cl, 		[esi+ebx] 		; 	ee_image[x][height-1]= ee_image[x][height-2]

	add 	ebx, 		1
	mov 	[esi+ebx], 	cl;ecx

	add 	esi, 		2048 	

	pop 	ecx

	loop 	.loop

	ret





