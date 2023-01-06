ORG 0x7C00
BITS 16
dw 0
xor ax, ax
mov es, ax
mov si, msg
mov al, BYTE [es:0x7C01]
add si, 2
add si, ax
xor ax, ax
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
