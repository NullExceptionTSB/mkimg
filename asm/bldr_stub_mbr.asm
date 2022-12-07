ORG 0x7C00
BITS 16

xor ax, ax
mov si, msg
ploop:
	lodsb
	or al, al
	jz hlt
	mov ah, 0Eh
	int 10h
	jmp ploop
hlt:
cli
hlt
msg db "This disk is not bootable",0xA,0xD,"Formatted with MkImg", 0
