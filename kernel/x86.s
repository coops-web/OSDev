BITS 32

VIDEO_MEMORY equ 0xB8000
WHITE_ON_BLACK equ 0x0F ; video memory works as: 0xB8000 = first character, 0xB8001 = color for first character, and then loops

SECTION _text:


GLOBAL _x86_Video_WriteCharTeletype
_x86_Video_WriteCharTeletype:

     PUSHA
     MOV EBX, VIDEO_MEMORY      ; We want to display to here
     ADD EBX,  [ESP + 44]        ; look at comment below to get explanation as to why we add 44

; ESP + 4 is the character we want to display. ESP is the stack pointer, it points to the return address since this function is
; being called. But, since we're using CDECL, (GCC automatically uses it), and stacks are LIFO, if we add 4 to the memory address
; that ESP points to, we get the character we want to display. (Each 'thing' in the stack takes up 4 bytes, that's why we add 4).
; CORRECTION: You may have seen the explanation above and asked why we're adding 44 and 36 instead. That's because the first line
; we call is PUSHA, which pushes a bunch of general purpose registers into the stack, so we have to add even MORE to the ESP to 
; make it work.

     MOVZX EAX, BYTE [ESP + 36]  ; EAX is just the 32-bit version of the 16-bit register AX. Literally. AX represents the lower
                                 ; 16 bits of EAX.
     MOV AH, WHITE_ON_BLACK
     MOV [EBX], AX
     POPA          ; IMPORTANT, because ESP is pointing to some random GPR, not the return address.
     RET
