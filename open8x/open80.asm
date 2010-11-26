;Open80 - A free firmware for the Sinclair ZX80
;Copyright (C) 1979 Nine Tiles Networks Ltd.

;This program is free software; you can redistribute it and/or
;modify it under the terms of the GNU General Public License
;as published by the Free Software Foundation; either version 2
;of the License, or (at your option) any later version.

;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.

;You should have received a copy of the GNU General Public License
;along with this program; if not, write to the Free Software
;Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


; z80dasm 1.1.2
; command line: z80dasm -g 0x0000 -l -b blockfile -o open80.asm zx80.rom
; If you'd like to repeat this disassembly then here's the blockfile data:
; l006c: start 0x006c end 0x013c type bytedata
; l0372: start 0x0372 end 0x0382 type bytedata
; l0726: start 0x0726 end 0x0727 type bytedata
; l0752: start 0x0752 end 0x07be type bytedata
; l0836: start 0x0836 end 0x083d type bytedata
; l0922: start 0x0922 end 0x0923 type bytedata
; l092f: start 0x092f end 0x0930 type bytedata
; l0964: start 0x0964 end 0x0965 type bytedata
; l0971: start 0x0971 end 0x0972 type bytedata
; l09d0: start 0x09d0 end 0x09d1 type bytedata
; l0aa3: start 0x0aa3 end 0x0aad type bytedata
; l0bbf: start 0x0bbf end 0x0bc0 type bytedata
; l0bc0: start 0x0bc0 end 0x0bed type bytedata
; l0cd2: start 0x0cd2 end 0x0cd3 type bytedata
; l0d1f: start 0x0d1f end 0x0d39 type bytedata
; l0d43: start 0x0d43 end 0x0d44 type bytedata
; l0e00: start 0x0e00 end 0x1000 type bytedata
; 
; md5: e03bd725f98be45aac39c85f7fd41ce4  open80.rom

	org	00000h

l0000h:
	ld hl,07fffh
	ld a,03fh
	jp l0261h
	pop hl	
	ld l,(hl)	
	bit 7,(iy+000h)
	jr l0013h
	jp l0560h
l0013h:
	ret z	
	ld (iy+000h),l
	ret	
	jr l0052h
sub_001ah:
	ld hl,(04026h)
	ld a,(hl)	
l001eh:
	and a	
	ret nz	
	call l0052h
	jr l001eh
sub_0025h:
	call sub_0055h
	call sub_001ah
	ld b,000h
	jp l09e1h
	call sub_094fh
	ret nc	
	push bc	
	jp l0cf3h
	dec c	
	jp nz,l0045h
	pop hl	
	dec b	
	ret z	
	set 3,c
l0041h:
	ld r,a
	ei	
	jp (hl)	
l0045h:
	pop de	
	ret z	
	jr l0041h
sub_0049h:
	call sub_0025h
	ld a,(hl)	
l004dh:
	cp 0d9h
	jp nz,l08aeh
l0052h:
	ld hl,(04026h)
sub_0055h:
	inc hl	
	ld (04026h),hl
	ld a,(hl)	
	cp 0b0h
	ret nz	
	ld (04004h),hl
	bit 7,(iy+019h)
	jr z,sub_0055h
	set 2,(iy+001h)
	jr sub_0055h

; BLOCK 'l006c' (start 0x006c end 0x013c)
l006c_start:
	defb 03fh
	defb 03dh
	defb 028h
	defb 03bh
	defb 026h
	defb 038h
	defb 029h
	defb 02bh
	defb 02ch
	defb 036h
	defb 03ch
	defb 02ah
	defb 037h
	defb 039h
	defb 01dh
	defb 01eh
	defb 01fh
	defb 020h
	defb 021h
	defb 01ch
	defb 025h
	defb 024h
	defb 023h
	defb 022h
	defb 035h
	defb 034h
	defb 02eh
	defb 03ah
	defb 03eh
	defb 076h
	defb 031h
	defb 030h
	defb 02fh
	defb 02dh
	defb 000h
	defb 01bh
	defb 032h
	defb 033h
	defb 027h
	defb 00eh
	defb 0d7h
	defb 00fh
	defb 0dfh
	defb 009h
	defb 008h
	defb 006h
	defb 007h
	defb 00bh
	defb 002h
	defb 003h
	defb 004h
	defb 005h
	defb 00ah
	defb 0dbh
	defb 0e0h
	defb 0d5h
	defb 0d6h
	defb 072h
	defb 077h
	defb 074h
	defb 073h
	defb 070h
	defb 071h
	defb 0deh
	defb 0d9h
	defb 0dah
	defb 00dh
	defb 001h
	defb 075h
	defb 0e3h
	defb 0ddh
	defb 0dch
	defb 0e2h
	defb 00ch
	defb 0d8h
	defb 0e4h
	defb 0e5h
	defb 0e1h
l00bah:
	defb 0d4h
	defb 08fh
	defb 081h
	defb 039h
	defb 02dh
	defb 02ah
	defb 0b3h
	defb 039h
	defb 0b4h
	defb 099h
	defb 09ah
	defb 091h
	defb 090h
	defb 033h
	defb 034h
	defb 0b9h
	defb 092h
	defb 093h
	defb 094h
	defb 095h
	defb 026h
	defb 033h
	defb 0a9h
	defb 034h
	defb 0b7h
	defb 014h
	defb 094h
	defb 096h
	defb 097h
	defb 098h
	defb 031h
	defb 02eh
	defb 038h
	defb 0b9h
	defb 037h
	defb 02ah
	defb 039h
	defb 03ah
	defb 037h
	defb 0b3h
	defb 028h
	defb 031h
	defb 0b8h
	defb 029h
	defb 02eh
	defb 0b2h
	defb 038h
	defb 026h
	defb 03bh
	defb 0aah
	defb 02bh
	defb 034h
	defb 0b7h
	defb 02ch
	defb 034h
	defb 000h
	defb 039h
	defb 0b4h
	defb 035h
	defb 034h
	defb 030h
	defb 0aah
	defb 02eh
	defb 033h
	defb 035h
	defb 03ah
	defb 0b9h
	defb 037h
	defb 026h
	defb 033h
	defb 029h
	defb 034h
	defb 032h
	defb 02eh
	defb 038h
	defb 0aah
	defb 031h
	defb 02ah
	defb 0b9h
	defb 08fh
	defb 08fh
	defb 033h
	defb 02ah
	defb 03dh
	defb 0b9h
	defb 035h
	defb 037h
	defb 02eh
	defb 033h
	defb 0b9h
	defb 08fh
	defb 033h
	defb 02ah
	defb 0bch
	defb 037h
	defb 03ah
	defb 0b3h
	defb 038h
	defb 039h
	defb 034h
	defb 0b5h
	defb 028h
l0120h:
	defb 034h
	defb 033h
	defb 039h
	defb 02eh
	defb 033h
	defb 03ah
	defb 0aah
	defb 02eh
	defb 0abh
	defb 02ch
	defb 034h
	defb 000h
	defb 038h
	defb 03ah
	defb 0a7h
	defb 031h
	defb 034h
	defb 026h
	defb 0a9h
	defb 028h
	defb 031h
	defb 02ah
	defb 026h
	defb 0b7h
	defb 037h
	defb 02ah
	defb 0b2h
	defb 08fh
l006c_end:
	call sub_01adh
sub_013fh:
	ld b,008h
l0141h:
	djnz l0141h
	ld hl,(0401eh)
	inc hl	
	ld (0401eh),hl
	ld hl,0ffffh
	ld b,0feh
	ld c,b	
	in a,(c)
	or 001h
l0154h:
	or 0e0h
	ld d,a	
	cpl	
	cp 001h
	sbc a,a	
	or b	
	and l	
	ld l,a	
	ld a,h	
	and d	
	ld h,a	
	rlc b
	in a,(c)
	jr c,l0154h
	rra	
	rl h
	rla	
	rla	
	rla	
	sbc a,a	
	and 018h
	add a,020h
	ld (04023h),a
	ld bc,(04026h)
	ld (04026h),hl
	ld a,b	
	add a,002h
	sbc hl,bc
	ex de,hl	
	ld hl,04022h
	ld a,(hl)	
	or d	
	or e	
	ret z	
	ld a,b	
	cp 0feh
	sbc a,a	
	ld b,01fh
	or (hl)	
	and b	
	rra	
	ld (hl),a	
	dec b	
l0194h:
	djnz l0194h
	out (0ffh),a
	ld a,0ech
	ld b,019h
	ld hl,(0400ch)
	set 7,h
	call sub_01adh
	ld a,0f3h
	inc b	
	dec hl	
	dec (iy+023h)
	jr l006c_end
sub_01adh:
	ld c,(iy+023h)
	ld r,a
	ld a,0ddh
	ei	
	jp (hl)	
	pop de	
	ld de,012cbh
l01bah:
	ld a,07fh
	in a,(0feh)
	rra	
	jr nc,l0203h
l01c1h:
	djnz l01c1h
	dec de	
	ld a,d	
	or e	
	jr nz,l01bah
	ld hl,04000h
l01cbh:
	ld de,0f808h
l01ceh:
	rlc (hl)
	sbc a,a	
	and 005h
	add a,004h
	ld c,a	
l01d6h:
	out (0ffh),a
	ld b,024h
l01dah:
	djnz l01dah
	ld a,07fh
	in a,(0feh)
	ld b,023h
l01e2h:
	djnz l01e2h
	dec c	
	jr nz,l01d6h
	ld b,d	
l01e8h:
	nop	
	djnz l01e8h
	ld d,0feh
	dec e	
	jr nz,l01ceh
	rra	
	jr nc,l0203h
	call sub_01f8h
	jr l01cbh
sub_01f8h:
	inc hl	
	ex de,hl	
	ld hl,(0400ah)
	scf	
	sbc hl,de
	ex de,hl	
	ret nc	
	pop hl	
l0203h:
	jp l0283h
	pop de	
l0207h:
	ld de,05712h
l020ah:
	ld a,07fh
	in a,(0feh)
	rra	
	jr nc,l0203h
	rla	
	rla	
	jr c,l0207h
	dec de	
	ld a,d	
	or e	
	jr nz,l020ah
	inc (iy+00bh)
	ld hl,04000h
l0220h:
	ld e,008h
l0222h:
	ld a,07fh
	in a,(0feh)
	rra	
	jr nc,l024dh
	rla	
	rla	
	jr nc,l0222h
	ld c,094h
l022fh:
	ld b,01ah
l0231h:
	dec c	
	in a,(0feh)
	rla	
	bit 7,c
	ld a,c	
	jr c,l022fh
	djnz l0231h
	jr nz,l0242h
	cp 056h
	jr nc,l0222h
l0242h:
	ccf	
	rl (hl)
	dec e	
	jr nz,l0222h
	call sub_01f8h
	jr l0220h
l024dh:
	dec d	
	jp p,l0000h
	dec (iy+00bh)
	jr l0203h
	res 7,b
	res 6,b
	ld (04006h),bc
	pop bc	
	jr l0283h
l0261h:
	ld (hl),001h
	dec hl	
	cp h	
	jr nz,l0261h
l0267h:
	inc hl	
	dec (hl)	
	jr z,l0267h
	ld sp,hl	
	push af	
	ld a,00eh
	ld i,a
	im 1
	ld iy,04000h
	ld hl,04028h
	ld (04008h),hl
	ld (hl),080h
	inc hl	
	ld (0400ah),hl
l0283h:
	ld hl,(0400ah)
	ld (hl),0b0h
	inc hl	
	ld (hl),076h
	inc hl	
	ld (0400ch),hl
	ld (iy+012h),002h
l0293h:
	call sub_0747h
	ex de,hl	
	ld a,b	
	sub (iy+012h)
	jr c,l02f7h
	inc a	
	ld b,a	
	exx	
	ld hl,(04006h)
	ld de,(04013h)
	sbc hl,de
	ex de,hl	
	jr nc,l02b0h
	add hl,de	
	ld (04013h),hl
l02b0h:
	call sub_060ah
	ld e,000h
l02b5h:
	call sub_04f7h
	jr c,l02b5h
	dec e	
	jr nz,l02f0h
	push hl	
	ld hl,(04006h)
	call sub_060ah
	pop hl	
	and a	
	sbc hl,de
	ld hl,04013h
	jr nc,l02d8h
	ex de,hl	
	ld a,(hl)	
	inc hl	
	ldi
	ld (de),a	
l02d3h:
	jr l0293h
	ld hl,04006h
l02d8h:
	ld e,(hl)	
	inc hl	
	ld d,(hl)	
	push hl	
	ex de,hl	
	inc hl	
	call sub_060ah
	call sub_03c2h
	pop hl	
l02e5h:
	bit 5,(iy+019h)
	jr nz,l02f7h
	ld (hl),d	
	dec hl	
	ld (hl),e	
	jr l0293h
l02f0h:
	call sub_05c2h
	ld (0400eh),de
l02f7h:
	ld (iy+001h),001h
	ld hl,(0400ah)
	call l0752_end
	ld de,(0400eh)
	ld b,(iy+012h)
	ld c,001h
	exx	
	ld hl,(0400ah)
	call sub_0512h
	jr c,l031dh
	ld hl,04012h
	inc (hl)	
	ld a,018h
	cp (hl)	
	jr nc,l02d3h
	ld (hl),a	
l031dh:
	call sub_05c2h
	call sub_013fh
	sra b
	sbc a,a	
	or 026h
	ld l,005h
	sub l	
l032bh:
	add a,l	
	scf	
	rr c
	jr c,l032bh
	inc c	
	jr nz,l02f7h
	ld c,b	
	dec l	
	ld l,001h
	jr nz,l032bh
	ld hl,0006bh
	ld e,a	
	add hl,de	
	ld a,(hl)	
	bit 2,(iy+001h)
	jr z,l034dh
	add a,0c0h
	cp 0e6h
	jr nc,l034dh
	ld a,(hl)	
l034dh:
	cp 0c0h
	jp pe,l035eh
	ld hl,(04004h)
	ld bc,l0000h+1
	call sub_05d5h
	ld (de),a	
l035ch:
	jr l02f7h
l035eh:
	ld e,a	
	ld hl,00292h
	add hl,de	
	add hl,de	
	ld c,(hl)	
	inc hl	
	ld b,(hl)	
	push bc	
	ld hl,(04004h)
	ret	
sub_036ch:
	ld bc,l0000h+1
	jp l0666h

; BLOCK 'l0372' (start 0x0372 end 0x0382)
l0372_start:
	defb 0a9h
	defb 003h
	defb 0d5h
	defb 002h
	defb 082h
	defb 003h
	defb 087h
	defb 003h
	defb 0b9h
	defb 003h
	defb 0cbh
	defb 003h
	defb 008h
	defb 004h
	defb 095h
	defb 003h
l0372_end:
	call sub_039eh
	dec hl	
	dec hl	
	inc hl	
	ld a,(hl)	
	cp 076h
	jr z,l03a7h
	ld (hl),0b0h
	ld hl,(04004h)
	ld (hl),a	
	jr l035ch
	call sub_039eh
	dec hl	
	call sub_036ch
	jr l035ch
sub_039eh:
	ld de,(0400ah)
	ld a,(de)	
	cp 0b0h
	ret nz	
	pop de	
l03a7h:
	jr l035ch
	ld hl,(04006h)
	call sub_060ah
	ex de,hl	
	call sub_03c2h
l03b3h:
	ld hl,04007h
	jp l02e5h
	ld de,l0000h
	jr l03b3h
l03beh:
	ex de,hl	
	ld de,003bah
sub_03c2h:
	ld a,(hl)	
	and 0c0h
	jr nz,l03beh
	ld d,(hl)	
	inc hl	
	ld e,(hl)	
	ret	
	ld c,000h
	ld de,(0400ah)
	exx	
	ld hl,(04006h)
	call sub_060ah
	call sub_03c2h
	ld a,d	
	or e	
	jp z,l0283h
	dec hl	
	call sub_06bfh
	dec hl	
	call sub_0624h
	inc hl	
	inc hl	
	dec bc	
	dec bc	
	exx	
	push de	
	exx	
	pop de	
	ld a,0b0h
	ld (de),a	
	inc de	
	push hl	
	ld hl,00022h
	add hl,de	
	add hl,bc	
	sbc hl,sp
	jr nc,l03a7h
	pop hl	
	ldir
	ld (0400ch),de
	jp l0293h
	ld hl,(04015h)
	ld a,h	
	or l	
	jr nz,l03a7h
	ld hl,(04004h)
	call sub_036ch
	ld hl,(0400ah)
	ld (04026h),hl
	call sub_001ah
	bit 5,(iy+019h)
	jr nz,l043ch
	call sub_0679h
	exx	
	ld a,h	
	or l	
	jp nz,l04bah
	dec hl	
	dec hl	
	ld (04002h),hl
	call sub_0747h
	exx	
	ld a,(hl)	
	cp 076h
	jp z,l0283h
l043ch:
	ld (iy+000h),0ffh
	ld (iy+001h),088h
l0444h:
	call l0752_end
	call sub_0d0ah
	ld de,(04002h)
	ld hl,04019h
	bit 5,(hl)
	jr z,l0458h
	res 5,(hl)
	inc de	
l0458h:
	bit 7,(iy+000h)
	jr z,l0488h
	ld hl,04001h
	bit 3,(hl)
	res 3,(hl)
	ld hl,(04026h)
	inc hl	
	jr z,l0474h
	ex de,hl	
	ld a,h	
	and 0c0h
	jr nz,l0488h
	call sub_060ah
l0474h:
	ld a,(hl)	
	and 0c0h
	jr nz,l0488h
	ld d,(hl)	
	inc hl	
	ld e,(hl)	
	ld (04002h),de
	inc hl	
	ld a,07fh
	in a,(0feh)
	rra	
	jr c,l0444h
l0488h:
	call sub_06e0h
	call sub_05c2h
	ld bc,l0120h
	exx	
	ld a,(04000h)
	ld bc,(04002h)
	inc a	
	jr z,l04a8h
	cp 009h
	jr nz,l04a1h
	inc bc	
l04a1h:
	ld (04017h),bc
	jr nz,l04a8h
	dec bc	
l04a8h:
	call sub_0556h
	ld a,015h
	rst 10h	
	call sub_06a1h
	call sub_05c2h
	call sub_013fh
	jp l0283h
l04bah:
	ld (04006h),hl
	exx	
	ex de,hl	
	call sub_0747h
	sbc hl,de
	exx	
	call sub_060ah
	push hl	
	jr nz,l04d1h
	call sub_0624h
	call l0666h
l04d1h:
	exx	
	inc hl	
	ld b,h	
	ld c,l	
	ld a,l	
	sub 003h
	or h	
	call nz,sub_094fh
	pop hl	
	jr nc,l04f4h
	push bc	
	dec hl	
	call sub_05d5h
	inc de	
	ld hl,(0400ch)
	dec hl	
	pop bc	
	dec bc	
	lddr
	ld hl,(04006h)
	ex de,hl	
	ld (hl),d	
	inc hl	
	ld (hl),e	
l04f4h:
	jp l0283h
sub_04f7h:
	ld bc,(04006h)
	call sub_061ch
	ld d,097h
	jr z,l0507h
	ld de,l0000h
	rl e
l0507h:
	ld a,(hl)	
	cp 040h
	call c,sub_06bfh
	ret nc	
	inc hl	
	ld a,d	
	rst 10h	
	ret nc	
sub_0512h:
	set 0,(iy+001h)
l0516h:
	ld bc,(04015h)
	and a	
	sbc hl,bc
	jr nz,l0523h
	ld a,0b8h
	rst 10h	
	ret z	
l0523h:
	add hl,bc	
	ld a,(hl)	
	inc hl	
	cp 0b0h
	jr z,l053ch
	cp 0c0h
	jp pe,l0559h
	jr c,l0536h
	call sub_0584h
	jr l0539h
l0536h:
	call l0559h
l0539h:
	ret nc	
	jr l0516h
l053ch:
	bit 2,(iy+001h)
	jr nz,l0543h
	inc a	
l0543h:
	rst 10h	
	jr l0539h
l0546h:
	ld a,e	
	rlca	
	rrca	
	ret c	
	jr l055ch
sub_054ch:
	xor a	
l054dh:
	add hl,bc	
	inc a	
	jr c,l054dh
	sbc hl,bc
	dec a	
	jr z,l0546h
sub_0556h:
	ld e,01ch
	add a,e	
l0559h:
	and a	
	jr z,l0560h
l055ch:
	res 0,(iy+001h)
l0560h:
	exx	
	ld h,a	
	rla	
	rla	
	dec c	
	jr nc,l0569h
	ld c,000h
l0569h:
	jp m,l0574h
	jr c,l057ch
	jr nz,l057ch
	ld a,076h
	ld (de),a	
	inc de	
l0574h:
	jr c,l0578h
	ld c,020h
l0578h:
	and a	
	dec b	
	jr z,l0582h
l057ch:
	ld l,b	
	call sub_0958h
	ld (de),a	
	inc de	
l0582h:
	exx	
	ret	
sub_0584h:
	call sub_05a8h
	jr nc,l0592h
	bit 0,(iy+001h)
	jr nz,l0592h
	xor a	
	rst 10h	
	ret nc	
l0592h:
	ld a,(bc)	
	and 03fh
	call l0559h
	ret nc	
	ld a,(bc)	
	inc bc	
	add a,a	
	jr nc,l0592h
	cp 038h
	ret c	
	xor a	
	set 0,(iy+001h)
	jr l0560h
sub_05a8h:
	push hl	
	ld hl,l00bah
	sub (hl)	
	inc hl	
	jr c,l05b9h
	inc a	
	ld b,a	
l05b2h:
	bit 7,(hl)
	inc hl	
	jr z,l05b2h
	djnz l05b2h
l05b9h:
	ld b,h	
	ld c,l	
	pop hl	
	ld a,(bc)	
	and 03fh
	add a,0e4h
	ret	
sub_05c2h:
	exx	
	xor a	
	cp b	
	jr z,l05d0h
	cp c	
	ld a,076h
	jr z,l05ceh
l05cch:
	ld (de),a	
	inc de	
l05ceh:
	djnz l05cch
l05d0h:
	ld (04010h),de
	ret	
sub_05d5h:
	call sub_05dfh
	ld hl,(04010h)
	ex de,hl	
	lddr
	ret	
sub_05dfh:
	push af	
	push hl	
	ld hl,04008h
	ld a,005h
l05e6h:
	ld e,(hl)	
	inc hl	
	ld d,(hl)	
	ex (sp),hl	
	and a	
	sbc hl,de
	add hl,de	
	ex (sp),hl	
	jr nc,l05fah
	push de	
	ex de,hl	
	add hl,bc	
	ex de,hl	
	ld (hl),d	
	dec hl	
	ld (hl),e	
	inc hl	
	pop de	
l05fah:
	inc hl	
	dec a	
	jr nz,l05e6h
	ex de,hl	
	pop de	
	pop af	
	and a	
	sbc hl,de
	ld b,h	
	ld c,l	
	inc bc	
	add hl,de	
	ex de,hl	
	ret	
sub_060ah:
	push hl	
	ld hl,04028h
	ld d,h	
	ld e,l	
l0610h:
	pop bc	
	ex de,hl	
	call sub_061ch
	ret nc	
	push bc	
	call sub_0624h
	jr l0610h
sub_061ch:
	ld a,(hl)	
	cp b	
	ret nz	
	inc hl	
	ld a,(hl)	
	dec hl	
	cp c	
	ret	
sub_0624h:
	push hl	
	ld a,(hl)	
	add a,a	
	jp m,l0635h
	jr c,l0643h
	inc hl	
	ld a,076h
l062fh:
	inc hl	
	ld b,a	
	cpir
	jr l0652h
l0635h:
	ld bc,l0000h+2
	jr c,l063bh
	ld c,b	
l063bh:
	rla	
l063ch:
	rla	
	inc hl	
	ld a,(hl)	
	jr nc,l063ch
	jr l064fh
l0643h:
	and 040h
	ld a,001h
	jr z,l062fh
	inc hl	
	ld a,(hl)	
	inc hl	
	ld b,000h
	ld c,a	
l064fh:
	inc bc	
	add hl,bc	
	add hl,bc	
l0652h:
	pop de	
sub_0653h:
	and a	
	sbc hl,de
	ld b,h	
	ld c,l	
	add hl,de	
	ex de,hl	
	ret	
l065bh:
	ld hl,(0400ah)
	dec hl	
	ld de,(04008h)
l0663h:
	call sub_0653h
l0666h:
	push bc	
	ld a,b	
	cpl	
	ld b,a	
	ld a,c	
	cpl	
	ld c,a	
	inc bc	
	call sub_05dfh
	ex de,hl	
	pop hl	
	add hl,de	
	push de	
	ldir
	pop hl	
	ret	
sub_0679h:
	ld a,(hl)	
	exx	
	ld hl,l0000h
	ld b,h	
l067fh:
	sub 01ch
	jr c,l069ah
	cp 00ah
	jr nc,l069ah
	ld c,a	
	ld a,00dh
	cp h	
	jr nc,l068eh
	ld h,a	
l068eh:
	ld d,h	
	ld e,l	
	add hl,hl	
	add hl,hl	
	add hl,de	
	add hl,hl	
	add hl,bc	
	exx	
	rst 18h	
	exx	
	jr l067fh
l069ah:
	ld a,h	
	ld (04022h),hl
	exx	
	rla	
	ret	
sub_06a1h:
	push de	
	push hl	
	ld h,b	
	ld l,c	
	bit 7,b
	jr z,l06b5h
	ld a,012h
	call l0559h
	jr nc,l06ddh
	ld hl,l0000h+1
	sbc hl,bc
l06b5h:
	ld e,0ffh
	ld bc,0d8f0h
	call sub_054ch
	jr l06c8h
sub_06bfh:
	push de	
	ld d,(hl)	
	inc hl	
	ld e,(hl)	
	push hl	
	ex de,hl	
	ld e,000h
	scf	
l06c8h:
	ld bc,0fc18h
	call c,sub_054ch
	ld bc,0ff9ch
	call c,sub_054ch
	ld c,0f6h
	call c,sub_054ch
	ld a,l	
	call c,sub_0556h
l06ddh:
	pop hl	
	pop de	
	ret	
sub_06e0h:
	bit 7,(iy+001h)
	pop hl	
	ret z	
	exx	
	ld de,(0400eh)
	ld bc,(04024h)
	exx	
	jp (hl)	
sub_06f1h:
	bit 7,(iy+000h)
	ret z	
	call sub_06e0h
	ld hl,(04022h)
	bit 6,(iy+001h)
	jr z,l070ch
	ld b,h	
	ld c,l	
	call sub_06a1h
	jr l0723h
l0709h:
	rst 10h	
l070ah:
	jr nc,l0725h
l070ch:
	ld a,(hl)	
	inc hl	
	cp 001h
	jr z,l073ah
	bit 6,a
	jr z,l0709h
	call sub_0584h
	jr l070ah
l071bh:
	call sub_06e0h
	ld a,076h
	call l0559h
l0723h:
	jr c,l073ah
l0725h:
	rst 8	

; BLOCK 'l0726' (start 0x0726 end 0x0727)
l0726_start:
	defb 004h
l0726_end:
	call sub_06e0h
	set 0,(iy+001h)
l072eh:
	xor a	
	rst 10h	
	jr nc,l0725h
	exx	
	ld a,c	
	exx	
	dec a	
	and 007h
	jr nz,l072eh
l073ah:
	exx	
	ex de,hl	
l073ch:
	ld (04024h),bc
	ld (0400eh),hl
	ld (04010h),hl
	ret	
sub_0747h:
	ld hl,(0400ch)
	ld (hl),076h
	inc hl	
	ld bc,01721h
	jr l073ch

; BLOCK 'l0752' (start 0x0752 end 0x07be)
l0752_start:
	defb 04fh
	defb 02ch
	defb 064h
	defb 03fh
	defb 059h
	defb 02bh
	defb 017h
	defb 04bh
	defb 036h
	defb 04eh
	defb 010h
	defb 05eh
	defb 05dh
	defb 02ah
	defb 02dh
	defb 05ah
	defb 061h
	defb 03bh
	defb 018h
	defb 04dh
	defb 00dh
	defb 011h
	defb 044h
	defb 04ch
	defb 031h
	defb 050h
	defb 001h
	defb 0e3h
	defb 002h
	defb 006h
	defb 000h
	defb 034h
	defb 009h
	defb 006h
	defb 0d5h
	defb 005h
	defb 0b9h
	defb 008h
	defb 006h
	defb 000h
	defb 043h
	defb 009h
	defb 000h
	defb 02eh
	defb 009h
	defb 000h
	defb 065h
	defb 009h
	defb 004h
	defb 0e3h
	defb 006h
	defb 0d6h
	defb 005h
	defb 0c4h
	defb 008h
	defb 004h
	defb 000h
	defb 0f9h
	defb 008h
	defb 005h
	defb 072h
	defb 009h
	defb 001h
	defb 000h
	defb 09ah
	defb 009h
	defb 004h
	defb 0dah
	defb 006h
	defb 0d9h
	defb 000h
	defb 0d3h
	defb 00ch
	defb 005h
	defb 04ah
	defb 008h
	defb 003h
	defb 03dh
	defb 009h
	defb 003h
	defb 056h
	defb 002h
	defb 006h
	defb 0d8h
	defb 005h
	defb 0d1h
	defb 009h
	defb 003h
	defb 023h
	defb 009h
	defb 000h
	defb 006h
	defb 002h
	defb 000h
	defb 0b6h
	defb 001h
	defb 000h
	defb 030h
	defb 009h
	defb 000h
	defb 05bh
	defb 006h
	defb 000h
	defb 047h
	defb 007h
	defb 005h
	defb 044h
	defb 008h
l0752_end:
	dec hl	
	ld (04026h),hl
	ld hl,l0000h
	nop	
	ld (04015h),hl
	ld hl,04019h
	bit 5,(hl)
	jr z,l07d7h
	res 7,(hl)
	ld b,(hl)	
	rst 18h	
	jp l0889h
l07d7h:
	set 7,(hl)
	rst 20h	
	call sub_0679h
	jr c,l07e5h
	exx	
	ld de,0d8f0h
	add hl,de	
	exx	
l07e5h:
	call c,l08aeh
l07e8h:
	call sub_001ah
	res 7,(iy+019h)
	ld bc,l0000h
	ld (04022h),bc
	cp 076h
	ret z	
	ld c,a	
	rst 20h	
	ld a,c	
	sub 0e6h
	jr c,l07e5h
	ld c,a	
	ld hl,l0752_start
	add hl,bc	
	ld c,(hl)	
	add hl,bc	
	jr l080ch
l0809h:
	ld hl,(0401ah)
l080ch:
	ld a,(hl)	
	inc hl	
	ld (0401ah),hl
	ld bc,l0809h
	push bc	
	ld c,a	
	rla	
	jr c,l0826h
	ld hl,l0836_start
	ld b,000h
	add hl,bc	
	ld c,(hl)	
	add hl,bc	
	push hl	
	call sub_001ah
	ret	
l0826h:
	call sub_001ah
	cp 0d5h
	jr nz,l0831h
	set 7,(iy+019h)
l0831h:
	cp c	
	jr nz,l08aeh
	rst 20h	
	ret	

; BLOCK 'l0836' (start 0x0836 end 0x083d)
l0836_start:
	defb 01fh
	defb 033h
	defb 04dh
	defb 017h
	defb 064h
	defb 01bh
	defb 06ch
l0836_end:
	bit 7,(iy+001h)
	ret nz	
	pop bc	
l0843h:
	ld a,(hl)	
	cp 076h
	call nz,l08aeh
	ld a,(hl)	
l084ah:
	cp 076h
	ret z	
	rst 20h	
	jr l084ah
	cp 076h
	call nz,sub_08a8h
	cp a	
	pop bc	
	call z,l0836_end
	ex de,hl	
	ld hl,(0401ah)
	ld c,(hl)	
	inc hl	
	ld b,(hl)	
	ex de,hl	
l0862h:
	push bc	
	ld bc,(04022h)
	ld a,b	
	or c	
	ret	
	call sub_0d14h
	jr nc,l08aeh
	bit 7,(iy+001h)
	jp z,l0aa3_end
	ld (04020h),hl
	res 7,(iy+001h)
	call l0aa3_end
	set 7,(iy+001h)
	ret	
	pop bc	
	ld b,(iy+001h)
l0889h:
	push bc	
	rst 28h	
	pop de	
	ld bc,l0c3dh
	ld a,(04001h)
	bit 7,a
	jr nz,l0862h
	xor d	
	and 040h
	call nz,l08aeh
	jr l0843h
	ld (04020h),hl
	call sub_0d14h
	jr nc,l08aeh
	rst 18h	
	ret	
sub_08a8h:
	rst 28h	
	bit 6,(iy+001h)
	ret nz	
l08aeh:
	ld a,(04015h)
	or (iy+016h)
	ret nz	
	ld (04015h),hl
	ret	
	jr nz,l08c1h
	bit 7,(iy+001h)
	jr nz,l084ah
l08c1h:
	jp l07e8h
	push bc	
	call sub_08a8h
	pop bc	
	call l0836_end
	ld hl,(04022h)
	push hl	
	call l0c3dh
	pop bc	
	bit 7,(iy+000h)
	ret z	
	push bc	
	dec hl	
	bit 7,(hl)
	set 7,(hl)
	inc hl	
	inc hl	
	jr nz,l08eah
	ld bc,00004h
	inc hl	
	call sub_05d5h
l08eah:
	inc hl	
	pop de	
	ld (hl),e	
	inc hl	
	ld (hl),d	
	inc hl	
	ld de,(04002h)
	inc de	
	ld (hl),e	
	inc hl	
	ld (hl),d	
	ret	
	ld hl,(04020h)
	call sub_0b3bh
	bit 7,(iy+000h)
	ret z	
	ex de,hl	
	dec hl	
	dec hl	
	bit 7,(hl)
	jr z,l0921h
	inc de	
	inc hl	
	ld (hl),e	
	inc hl	
	ld (hl),d	
	inc hl	
	ld c,(hl)	
	inc hl	
	ld b,(hl)	
	push bc	
	ex (sp),hl	
	call sub_0dcdh
	pop hl	
	ret c	
	inc hl	
	ld c,(hl)	
	inc hl	
	ld b,(hl)	
	jr l0934h
l0921h:
	rst 8	

; BLOCK 'l0922' (start 0x0922 end 0x0923)
l0922_start:
	defb 000h
l0922_end:
	jr nz,l0929h
	ld bc,(0401eh)
l0929h:
	ld (0401ch),bc
	ret	
	rst 8	

; BLOCK 'l092f' (start 0x092f end 0x0930)
l092f_start:
	defb 008h
l092f_end:
	ld bc,(04017h)
l0934h:
	ld (04002h),bc
	set 3,(iy+001h)
	ret	
	call l0934h
	jp l065bh
	ld hl,(04002h)
	inc hl	
	ex (sp),hl	
	push hl	
	call l0934h
	ld bc,00006h
sub_094fh:
	ld hl,(04010h)
	add hl,bc	
	ex de,hl	
	ld hl,(04025h)
	ld h,a	
sub_0958h:
	ld a,013h
	add a,l	
	ld l,a	
	ld a,h	
	ld h,000h
	add hl,de	
	sbc hl,sp
	ret c	
	rst 8	

; BLOCK 'l0964' (start 0x0964 end 0x0965)
l0964_start:
	defb 003h
l0964_end:
	pop hl	
	pop bc	
	push hl	
	ld a,b	
	cp 03fh
	jr nz,l0934h
	pop hl	
	push bc	
	push hl	
	rst 8	

; BLOCK 'l0971' (start 0x0971 end 0x0972)
l0971_start:
	defb 006h
l0971_end:
	ld a,(hl)	
	cp 076h
	jp z,l071bh
l0978h:
	sub 0d8h
	adc a,000h
	jr z,l0991h
	rst 28h	
	call sub_06f1h
	call sub_001ah
	sub 0d8h
	adc a,000h
	jr z,l0991h
	call l0836_end
	jp l071bh
l0991h:
	call nc,l0726_end
	rst 20h	
	cp 076h
	ret z	
	jr l0978h
	bit 7,(iy+003h)
	jr nz,l09cfh
	pop hl	
	ld hl,04019h
	set 5,(hl)
	res 6,(hl)
	ld a,(04001h)
	and 040h
	ld bc,l0000h+2
	jr nz,l09b4h
	ld c,004h
l09b4h:
	or (hl)	
	ld (hl),a	
	rst 30h	
	ret nc	
	ld (hl),076h
	ld a,c	
	rrca	
	rrca	
	jr c,l09c2h
	ld (de),a	
	dec hl	
	ld (hl),a	
l09c2h:
	dec hl	
	ld (hl),0b0h
	ld a,(04025h)
	inc a	
	ld (04012h),a
	jp l02f7h
l09cfh:
	rst 8	

; BLOCK 'l09d0' (start 0x09d0 end 0x09d1)
l09d0_start:
	defb 007h
l09d0_end:
	push bc	
	rst 28h	
	pop de	
	call l0836_end
	ld a,(04022h)
	bit 7,(iy+000h)
	ret z	
	ld (de),a	
	ret	
l09e1h:
	ld c,b	
	push bc	
l09e3h:
	call sub_0d18h
	jr c,l0a24h
	ld bc,00900h
	ld d,c	
	ld e,c	
	sub 0dch
	jr z,l0a17h
	dec de	
	ld b,004h
	inc a	
	jr z,l0a17h
	inc a	
	jr z,l0a1ch
	cp 027h
	jr nz,l0a0eh
	res 6,(iy+001h)
	inc hl	
	ld (04022h),hl
l0a06h:
	rst 18h	
	dec a	
	jr z,l0a21h
	cp 075h
	jr nz,l0a06h
l0a0eh:
	call l08aeh
	exx	
	ld bc,l0000h
	jr l0a4ch
l0a17h:
	push de	
	push bc	
l0a19h:
	rst 20h	
	jr l09e3h
l0a1ch:
	call sub_0049h
	jr l0a37h
l0a21h:
	rst 18h	
	jr l0a37h
l0a24h:
	cp 026h
	jr c,l0a2dh
	call l0aa3_end
	jr l0a37h
l0a2dh:
	call sub_0679h
	call c,l08aeh
	set 6,(iy+001h)
l0a37h:
	call sub_001ah
	exx	
	ld bc,l0000h
	sub 0dch
	jr c,l0a4ch
	cp 00ah
	jr nc,l0a4ch
	ld c,a	
	ld hl,l0aa3_start
	add hl,bc	
	ld b,(hl)	
l0a4ch:
	pop de	
	ld a,d	
	cp b	
	jr c,l0a88h
	and a	
	exx	
	ret z	
	exx	
	bit 7,(iy+001h)
	jr z,l0a6fh
	ld d,000h
	ld hl,l0d1f_start
	add hl,de	
	add hl,de	
	ld e,(hl)	
	inc hl	
	ld d,(hl)	
	ld hl,l0a7fh
	ex (sp),hl	
	push de	
	ld de,(04022h)
	ret	
l0a6fh:
	ld a,e	
	cp 00ah
	rra	
	rra	
	xor (iy+001h)
	and 040h
	exx	
	call nz,l08aeh
	exx	
	pop hl	
l0a7fh:
	ld (04022h),hl
	set 6,(iy+001h)
	jr l0a4ch
l0a88h:
	push de	
	ld a,c	
	bit 6,(iy+001h)
	jr nz,l0a9ah
	add a,003h
	ld c,a	
	cp 00ah
	exx	
	call c,l08aeh
	exx	
l0a9ah:
	ld hl,(04022h)
	push hl	
	push bc	
	exx	
	jp l0a19h

; BLOCK 'l0aa3' (start 0x0aa3 end 0x0aad)
l0aa3_start:
	defb 006h
	defb 006h
	defb 008h
	defb 007h
	defb 003h
	defb 002h
	defb 00ah
	defb 005h
	defb 005h
	defb 005h
l0aa3_end:
	push hl	
	ld hl,04001h
	res 5,(hl)
	set 6,(hl)
	rst 18h	
	cp 00dh
	jp z,l0b30h
	cp 0dah
	jp z,l0b2bh
l0ac0h:
	call sub_0d18h
	jr nc,l0ac8h
	rst 18h	
	jr l0ac0h
l0ac8h:
	cp 0dah
	jr z,l0ad6h
	cp 00dh
	jp nz,l0b35h
	rst 18h	
	cp 0dah
	jr nz,l0b27h
l0ad6h:
	ld de,l0bbf_start
l0ad9h:
	pop hl	
	push hl	
l0adbh:
	ld c,(hl)	
	call sub_0055h
	inc de	
	ld a,(de)	
	cp c	
	jr z,l0adbh
	and 03fh
	cp c	
	jr nz,l0aeeh
	ld a,0dah
	cp (hl)	
	jr z,l0af9h
l0aeeh:
	ld a,(de)	
	and a	
	jr z,l0b27h
	inc de	
	rla	
	jr nc,l0aeeh
	inc de	
	jr l0ad9h
l0af9h:
	push de	
	call sub_0049h
	pop de	
	ex (sp),hl	
	ld hl,04001h
	ld a,(de)	
	xor (hl)	
	and 040h
	jr nz,l0b27h
	set 5,(hl)
	set 6,(hl)
	ld a,(de)	
	and 03fh
	cp 00dh
	jr nz,l0b15h
	res 6,(hl)
l0b15h:
	bit 7,(hl)
	pop hl	
	ret z	
	ld hl,l0bbah
	push hl	
	ex de,hl	
	inc hl	
	ld e,(hl)	
	inc hl	
	ld d,(hl)	
	push de	
	ld hl,(04022h)
	ret	
l0b27h:
	pop hl	
	jp l08aeh
l0b2bh:
	call sub_0049h
	jr l0b35h
l0b30h:
	res 6,(iy+001h)
	rst 18h	
l0b35h:
	pop hl	
	bit 7,(iy+001h)
	ret z	
sub_0b3bh:
	ld c,(hl)	
	inc hl	
	ld a,(hl)	
	push hl	
	cp 0dah
	jr nz,l0b5ch
	push bc	
	ld bc,(04026h)
	push bc	
	call sub_0025h
	pop hl	
	ld (04026h),hl
	pop bc	
	ld hl,04000h
	bit 7,(hl)
	jr nz,l0b6bh
	ld (hl),002h
	pop hl	
	ret	
l0b5ch:
	res 5,c
	cp 00dh
	jr z,l0b6bh
	set 6,c
	call sub_0d18h
	jr c,l0b6bh
	set 5,c
l0b6bh:
	ld hl,(04008h)
l0b6eh:
	ld a,(hl)	
	and 07fh
	jp z,l0cd0h
	cp c	
	jr nz,l0b93h
	rla	
	add a,a	
	jp m,l0ba4h
	jr nc,l0bb8h
	pop de	
	push de	
	push hl	
l0b81h:
	inc hl	
	ld a,(de)	
	inc de	
	cp (hl)	
	jr z,l0b81h
	or 080h
	cp (hl)	
	jr nz,l0b92h
	ld a,(de)	
	call sub_0d18h
	jr nc,l0b9bh
l0b92h:
	pop hl	
l0b93h:
	push bc	
	call sub_0624h
	ex de,hl	
	pop bc	
	jr l0b6eh
l0b9bh:
	pop de	
l0b9ch:
	pop de	
l0b9dh:
	inc hl	
	ld e,(hl)	
	inc hl	
	ld d,(hl)	
	ex de,hl	
	jr l0bbah
l0ba4h:
	jr c,l0b9ch
	ex (sp),hl	
	ld hl,(04022h)
	rlc h
	pop de	
	jr nz,l0bbeh
	inc de	
	ld a,(de)	
	cp l	
	jr c,l0bbeh
	add hl,hl	
	add hl,de	
	jr l0b9dh
l0bb8h:
	pop de	
	inc hl	
l0bbah:
	ld (04022h),hl
	ret	
l0bbeh:
	rst 8	

; BLOCK 'l0bbf' (start 0x0bbf end 0x0bc0)
l0bbf_start:
	defb 002h
l0bbf_end:

; BLOCK 'l0bc0' (start 0x0bc0 end 0x0bed)
l0bc0_start:
	defb 035h
	defb 02ah
	defb 02ah
	defb 0f0h
	defb 024h
	defb 00ch
	defb 028h
	defb 02dh
	defb 037h
	defb 0cdh
	defb 028h
	defb 00ch
	defb 028h
	defb 034h
	defb 029h
	defb 0aah
	defb 024h
	defb 00ch
	defb 037h
	defb 033h
	defb 0e9h
	defb 0edh
	defb 00bh
	defb 039h
	defb 031h
	defb 08dh
	defb 038h
	defb 00ch
	defb 03ah
	defb 038h
	defb 0f7h
	defb 0f0h
	defb 006h
	defb 038h
	defb 039h
	defb 037h
	defb 0cdh
	defb 010h
	defb 00ch
	defb 026h
	defb 027h
	defb 0f8h
	defb 0f2h
	defb 00dh
	defb 000h
l0bc0_end:
	push hl	
	ld hl,(0401ch)
	ld de,l004dh
	ld a,h	
	or l	
	jr z,l0c03h
	call sub_0d55h
	and a	
	sbc hl,bc
	jr nc,l0c05h
	inc hl	
	jr l0c05h
l0c03h:
	sbc hl,de
l0c05h:
	ld (0401ch),hl
	pop de	
	call sub_0d55h
	ld h,b	
	ld l,c	
	inc hl	
	ret	
	exx	
	ld bc,00007h
	rst 30h	
	jr nc,l0c34h
	push de	
	exx	
	ld b,h	
	ld c,l	
	call sub_06a1h
	exx	
	ld a,001h
	ld (de),a	
l0c22h:
	pop hl	
	ret	
	ld l,(hl)	
	ld h,000h
	ret	
	ld bc,l0000h+2
	ld a,l	
	rst 30h	
	jr nc,l0c34h
	ld (hl),001h
	dec hl	
	ld (hl),a	
	ret	
l0c34h:
	ld hl,00c30h
	ret	
	ld a,(hl)	
	dec a	
	ret z	
	inc hl	
	ret	
l0c3dh:
	bit 7,(iy+000h)
	ret z	
	push bc	
	ld hl,(04020h)
	call sub_0b3bh
	ld hl,04000h
	ld a,(hl)	
	cp 002h
	jr z,l0c22h
	rla	
	bit 6,(iy+001h)
	jr c,l0c93h
	ld (hl),0ffh
	jr z,l0ca3h
	ld hl,(04020h)
	ld bc,l0000h+2
l0c62h:
	inc bc	
	inc hl	
	ld a,(hl)	
	call sub_0d18h
	jr c,l0c62h
	cp 0dah
	jr z,l0cd0h
	rst 30h	
	jr nc,l0c22h
	push de	
	ld hl,(04020h)
	dec bc	
	dec bc	
	dec bc	
	dec de	
	ld a,b	
	or c	
	ld a,040h
	jr z,l0c87h
	ldir
	ld a,(hl)	
	or 080h
	ld (de),a	
	ld a,060h
l0c87h:
	pop hl	
	call sub_0cb9h
	ex de,hl	
	dec de	
l0c8dh:
	pop hl	
	ex de,hl	
	ld (hl),d	
	dec hl	
	ld (hl),e	
	ret	
l0c93h:
	jr nz,l0c8dh
	pop hl	
	call sub_0ca4h
	ld hl,(04022h)
	dec hl	
	call sub_0624h
	jp l0666h
l0ca3h:
	pop hl	
sub_0ca4h:
	ld a,001h
	ld bc,l0000h+1
l0ca9h:
	cp (hl)	
	inc hl	
	inc bc	
	jr nz,l0ca9h
	push hl	
	rst 30h	
	ex de,hl	
	pop hl	
	ret nc	
	lddr
	ex de,hl	
	inc hl	
	ld a,0a0h
sub_0cb9h:
	ex de,hl	
	ld hl,(04020h)
	xor (hl)	
	ex de,hl	
	push af	
	call sub_0d0dh
	pop af	
	dec hl	
	ld (hl),a	
	ld hl,(0400ch)
	ld (0400ah),hl
	dec hl	
	ld (hl),080h
	ret	
l0cd0h:
	pop hl	
	rst 8	

; BLOCK 'l0cd2' (start 0x0cd2 end 0x0cd3)
l0cd2_start:
	defb 001h
l0cd2_end:
	and b	
	jp nz,l0bbeh
	push bc	
	ld h,b	
	ld l,c	
	inc hl	
	inc hl	
	add hl,hl	
	ld b,h	
	ld c,l	
	rst 30h	
	jp nc,l0c22h
	dec hl	
	ld d,h	
	ld e,l	
	dec de	
	dec bc	
	dec bc	
	ld (hl),000h
	lddr
	pop bc	
	ld (hl),c	
	ld a,080h
	jr sub_0cb9h
l0cf3h:
	ld hl,(0400ah)
	push hl	
	ld hl,(0400ch)
	dec hl	
	call sub_05d5h
	inc hl	
	inc hl	
	pop bc	
	ld (0400ah),bc
	pop bc	
	ex de,hl	
	inc hl	
	scf	
	ret	
sub_0d0ah:
	ld hl,(0400ch)
sub_0d0dh:
	ld de,(0400ah)
	jp l0663h
sub_0d14h:
	cp 026h
	jr l0d1ah
sub_0d18h:
	cp 01ch
l0d1ah:
	ccf	
	ret nc	
	cp 040h
	ret	

; BLOCK 'l0d1f' (start 0x0d1f end 0x0d39)
l0d1f_start:
	defb 039h
	defb 00dh
	defb 03eh
	defb 00dh
	defb 044h
	defb 00dh
	defb 090h
	defb 00dh
	defb 0b5h
	defb 00dh
	defb 0bch
	defb 00dh
	defb 070h
	defb 00dh
	defb 0c3h
	defb 00dh
	defb 0cch
	defb 00dh
	defb 0cdh
	defb 00dh
	defb 0d9h
	defb 00dh
	defb 0dfh
	defb 00dh
	defb 0deh
	defb 00dh
l0d1f_end:
	and a	
	sbc hl,de
	jr l0d41h
	and a	
	adc hl,de
l0d41h:
	ret po	
l0d42h:
	rst 8	

; BLOCK 'l0d43' (start 0x0d43 end 0x0d44)
l0d43_start:
	defb 005h
l0d43_end:
	call sub_0dedh
	push bc	
	ex af,af'	
	call sub_0d55h
	jr nz,l0d8dh
l0d4eh:
	pop bc	
	ex af,af'	
	rra	
	ret nc	
	jp l0df6h
sub_0d55h:
	ld b,h	
	ld c,l	
	ld a,010h
	ld hl,l0000h
l0d5ch:
	add hl,hl	
	rl c
	rl b
	jr nc,l0d67h
	add hl,de	
	jr nc,l0d67h
	inc bc	
l0d67h:
	dec a	
	jr nz,l0d5ch
	ld a,h	
	and 080h
	or b	
	or c	
	ret	
	bit 7,d
	jr nz,l0d42h
	xor a	
	call sub_0df2h
	and e	
	ex af,af'	
	push bc	
	ld b,d	
	ld c,e	
	ex de,hl	
	ld hl,l0000h+1
l0d81h:
	dec bc	
	bit 7,b
	jr nz,l0d4eh
	push bc	
	call sub_0d55h
	pop bc	
	jr z,l0d81h
l0d8dh:
	pop bc	
	jr l0d42h
	ld a,d	
	or e	
	jr z,l0d42h
	call sub_0dedh
	push bc	
	rra	
	adc hl,hl
	ld a,h	
	ld c,l	
	ld hl,l0000h
	ld b,010h
l0da2h:
	adc hl,hl
	sbc hl,de
	jr nc,l0da9h
	add hl,de	
l0da9h:
	rl c
	rla	
	djnz l0da2h
	ld h,a	
	ld l,c	
	inc hl	
	pop bc	
	ret c	
	jr l0df6h
	ld a,h	
	and d	
	ld h,a	
	ld a,l	
	and e	
	ld l,a	
	ret	
	ld a,h	
	or d	
	ld h,a	
	ld a,l	
	or e	
	ld l,a	
	ret	
	and a	
	sbc hl,de
l0dc6h:
	ld hl,0ffffh
	ret z	
	inc hl	
	ret	
	ex de,hl	
sub_0dcdh:
	and a	
	sbc hl,de
	ld a,h	
	rla	
	jp po,l0dd6h
	ccf	
l0dd6h:
	sbc hl,hl
	ret	
	call sub_0de4h
	jr l0dc6h
	ex de,hl	
	call sub_0de4h
	jr l0dd6h
sub_0de4h:
	ld a,(de)	
	cp (hl)	
	ret nz	
	dec a	
	ret z	
	inc de	
	inc hl	
	jr sub_0de4h
sub_0dedh:
	xor a	
	call sub_0df1h
sub_0df1h:
	ex de,hl	
sub_0df2h:
	bit 7,h
	ret z	
	inc a	
l0df6h:
	ex af,af'	
	ld a,h	
	cpl	
	ld h,a	
	ld a,l	
	cpl	
	ld l,a	
	inc hl	
	ex af,af'	
	ret	

; BLOCK 'l0e00' (start 0x0e00 end 0x1000)
l0e00_start:
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 014h
	defb 014h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 0ffh
	defb 0ffh
	defb 0ffh
	defb 0ffh
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 00fh
	defb 00fh
	defb 00fh
	defb 00fh
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 00fh
	defb 00fh
	defb 00fh
	defb 00fh
	defb 00fh
	defb 00fh
	defb 00fh
	defb 00fh
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0f0h
	defb 0aah
	defb 055h
	defb 0aah
	defb 055h
	defb 0aah
	defb 055h
	defb 0aah
	defb 055h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 0aah
	defb 055h
	defb 0aah
	defb 055h
	defb 0aah
	defb 055h
	defb 0aah
	defb 055h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 01eh
	defb 021h
	defb 078h
	defb 020h
	defb 020h
	defb 07fh
	defb 000h
	defb 000h
	defb 008h
	defb 03eh
	defb 048h
	defb 03eh
	defb 009h
	defb 03eh
	defb 008h
	defb 000h
	defb 000h
	defb 000h
	defb 008h
	defb 000h
	defb 000h
	defb 008h
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 006h
	defb 008h
	defb 000h
	defb 008h
	defb 000h
	defb 000h
	defb 004h
	defb 008h
	defb 008h
	defb 008h
	defb 008h
	defb 004h
	defb 000h
	defb 000h
	defb 010h
	defb 008h
	defb 008h
	defb 008h
	defb 008h
	defb 010h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 03eh
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 008h
	defb 008h
	defb 03eh
	defb 008h
	defb 008h
	defb 000h
	defb 000h
	defb 000h
	defb 02ah
	defb 01ch
	defb 008h
	defb 01ch
	defb 02ah
	defb 000h
	defb 000h
	defb 000h
	defb 002h
	defb 004h
	defb 008h
	defb 010h
	defb 020h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 03eh
	defb 000h
	defb 03eh
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 010h
	defb 008h
	defb 004h
	defb 008h
	defb 010h
	defb 000h
	defb 000h
	defb 000h
	defb 004h
	defb 008h
	defb 010h
	defb 008h
	defb 004h
	defb 000h
	defb 000h
	defb 000h
	defb 008h
	defb 000h
	defb 000h
	defb 008h
	defb 008h
	defb 010h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 008h
	defb 008h
	defb 010h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 000h
	defb 00ch
	defb 00ch
	defb 000h
	defb 000h
	defb 01ch
	defb 022h
	defb 041h
	defb 041h
	defb 022h
	defb 01ch
	defb 000h
	defb 000h
	defb 00ch
	defb 014h
	defb 004h
	defb 004h
	defb 004h
	defb 01eh
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 001h
	defb 03eh
	defb 040h
	defb 07fh
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 006h
	defb 001h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 00ch
	defb 014h
	defb 024h
	defb 044h
	defb 07fh
	defb 004h
	defb 000h
	defb 000h
	defb 07fh
	defb 040h
	defb 07eh
	defb 001h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 03eh
	defb 040h
	defb 07eh
	defb 041h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 07fh
	defb 001h
	defb 002h
	defb 004h
	defb 008h
	defb 008h
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 03eh
	defb 041h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 041h
	defb 03fh
	defb 001h
	defb 03eh
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 041h
	defb 07fh
	defb 041h
	defb 041h
	defb 000h
	defb 000h
	defb 07eh
	defb 041h
	defb 07eh
	defb 041h
	defb 041h
	defb 07eh
	defb 000h
	defb 000h
	defb 01eh
	defb 021h
	defb 040h
	defb 040h
	defb 021h
	defb 01eh
	defb 000h
	defb 000h
	defb 07ch
	defb 042h
	defb 041h
	defb 041h
	defb 042h
	defb 07ch
	defb 000h
	defb 000h
	defb 07fh
	defb 040h
	defb 07ch
	defb 040h
	defb 040h
	defb 07fh
	defb 000h
	defb 000h
	defb 07fh
	defb 040h
	defb 07ch
	defb 040h
	defb 040h
	defb 040h
	defb 000h
	defb 000h
	defb 01eh
	defb 021h
	defb 040h
	defb 047h
	defb 021h
	defb 01eh
	defb 000h
	defb 000h
	defb 041h
	defb 041h
	defb 07fh
	defb 041h
	defb 041h
	defb 041h
	defb 000h
	defb 000h
	defb 03eh
	defb 008h
	defb 008h
	defb 008h
	defb 008h
	defb 03eh
	defb 000h
	defb 000h
	defb 002h
	defb 002h
	defb 002h
	defb 042h
	defb 022h
	defb 01ch
	defb 000h
	defb 000h
	defb 042h
	defb 044h
	defb 078h
	defb 044h
	defb 042h
	defb 041h
	defb 000h
	defb 000h
	defb 040h
	defb 040h
	defb 040h
	defb 040h
	defb 040h
	defb 07fh
	defb 000h
	defb 000h
	defb 041h
	defb 063h
	defb 055h
	defb 049h
	defb 041h
	defb 041h
	defb 000h
	defb 000h
	defb 061h
	defb 051h
	defb 049h
	defb 045h
	defb 043h
	defb 041h
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 041h
	defb 041h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 07eh
	defb 041h
	defb 041h
	defb 07eh
	defb 040h
	defb 040h
	defb 000h
	defb 000h
	defb 03eh
	defb 041h
	defb 041h
	defb 049h
	defb 045h
	defb 03eh
	defb 000h
	defb 000h
	defb 07eh
	defb 041h
	defb 041h
	defb 07eh
	defb 044h
	defb 042h
	defb 000h
	defb 000h
	defb 03eh
	defb 040h
	defb 03eh
	defb 001h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 07fh
	defb 008h
	defb 008h
	defb 008h
	defb 008h
	defb 008h
	defb 000h
	defb 000h
	defb 041h
	defb 041h
	defb 041h
	defb 041h
	defb 041h
	defb 03eh
	defb 000h
	defb 000h
	defb 041h
	defb 041h
	defb 041h
	defb 022h
	defb 014h
	defb 008h
	defb 000h
	defb 000h
	defb 041h
	defb 041h
	defb 041h
	defb 049h
	defb 055h
	defb 022h
	defb 000h
	defb 000h
	defb 021h
	defb 012h
	defb 00ch
	defb 00ch
	defb 012h
	defb 021h
	defb 000h
	defb 000h
	defb 041h
	defb 022h
	defb 01ch
	defb 008h
	defb 008h
	defb 008h
	defb 000h
	defb 000h
	defb 07fh
	defb 002h
	defb 004h
	defb 008h
	defb 010h
	defb 07fh
	defb 000h
l0e00_end:
