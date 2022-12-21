ORG 0x7C00
BITS 16

xor ax, ax
mov si, msg
ploop:
	lodsb
	or al, al
	jz halt
	mov ah, 0Eh
	int 10h
	jmp ploop
halt:
	cli
	hlt
	jmp halt
	
msg db "This disk is not bootable",0xA,0xD,"Formatted with MkImg", 0
