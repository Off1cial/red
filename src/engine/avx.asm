global AVX_AddArrays
global AVX_Integrate
global AVX_IntegrateFMA3

global AVX_Add

; TODO : Add tailing to finish the remaining < 4 values to avoid the C cleanup

section .text
  
AVX_Add:
; rdi = dst
; rsi = a
; xmm0 = val
; rdx = count
; I think?
  vbroadcastss ymm0, xmm0 ; Fill ymm0 with val
  .avxloop:
    cmp rdx, 0x8
    jl .sseloop

    vmovups ymm1, [rsi] ; mov a into ymm1
    vaddps ymm1, ymm1, ymm0 ; a = a + val

    vmovups [rdi], ymm1 ; mov a into dst
    
    ; advance 32 bytes (8 floats, 8 x 4 = 32)
    add rsi, 0x20
    add rdi, 0x20
    sub rdx, 0x8
    ; Do i need to advance ymm0 or xmm0?
    jmp .avxloop

  .sseloop: ; Same as above but with just 4 floats
    cmp rdx, 0x4
    jl .tail

    movups xmm1, [rsi]
    addps xmm1, xmm0
    movups [rdi], xmm1

    ; advance 16 bytes
    add rsi, 0x10
    add rdi, 0x10
    sub rdx, 0x4
    jmp .sseloop
  
  ; Linear finish
  .tail:
    test rdx, rdx
    jz .done

    movss xmm1, [rsi]
    addss xmm1, xmm0
    movss [rdi], xmm1

    ; advance
    add rsi, 0x4
    add rdi, 0x4
    dec rdx
    jmp .tail

  .done:
    vzeroupper
    mov rax, rdx
    ret


AVX_AddArrays:
; rdi = dst
; rsi = a
; rdx = b
; rcx = count
  
  .avxloop:
    cmp rcx, 0x8
    jl .sseloop



    vmovups ymm0, [rsi]
    vmovups ymm1, [rdx]

    vaddps ymm0, ymm0, ymm1

    vmovups [rdi], ymm0

    ; advance
    add rsi, 0x20
    add rdx, 0x20
    add rdi, 0x20
    
    sub rcx, 0x8
    jmp .avxloop

  .sseloop:
    cmp rcx, 0x4
    jl .tail
    
    movups xmm0, [rsi]
    movups xmm1, [rdx]

    addps xmm0, xmm1
    movups [rdi], xmm0

    ; advance
    add rsi, 0x10
    add rdx, 0x10
    add rdi, 0x10
  
    sub rcx, 0x4

    jmp .sseloop
  
  .tail:
    test rcx, rcx
    jz .done

    movss xmm0, [rsi]
    movss xmm1, [rdx]

    addss xmm0, xmm1
    movss [rdi], xmm0

    ;advance
    add rsi, 0x4
    add rdx, 0x4
    add rdi, 0x4
    dec rcx
    jmp .tail
  

  .done:
    vzeroupper
    mov rax, rcx
    ret

AVX_Integrate:
; rdi = dst
; rsi = a;
; rdx = b;
; rcx = count
; xmm0 = m
  vbroadcastss ymm0, xmm0
  .avxloop: ; Integrate 8 floats
    cmp rcx,0x8 
    jl .sseloop
    ; No FMA3 Support
    vmovups ymm1, [rsi]
    vmovups ymm2, [rdx]

    vmulps ymm2, ymm2, ymm0 ; b = b * m
    vaddps ymm1, ymm1, ymm2

    vmovups [rdi], ymm1

    ; advance
    add rsi, 0x20
    add rdx, 0x20
    add rdi, 0x20

    sub rcx, 0x8
    jmp .avxloop

  .sseloop: ; Integrate 4 floats
    cmp rcx, 0x4
    jl .tail

    movups xmm1, [rsi]
    movups xmm2, [rdx]

    mulps xmm2, xmm0
    addps xmm1, xmm2

    movups [rdi], xmm1

    ;adavnce
    add rsi, 0x10
    add rdx, 0x10
    add rdi, 0x10
    
    sub rcx, 0x4
    jmp .sseloop

  .tail:
    test rcx, rcx
    jz .done

    movss xmm1, [rsi]
    movss xmm2, [rdx]

    mulss xmm2, xmm0
    addss xmm1, xmm2
    
    movss [rdi], xmm1

    ; advance 4 bytes
    add rsi, 0x4
    add rdx, 0x4
    add rdi, 0x4

    dec rcx
    jmp .tail

  .done:
    vzeroupper
    mov rax, rcx
    ret

AVX_IntegrateFMA3:
; rdi = dst
; rsi = a;
; rdx = b;
; rcx = count
; xmm0 = m
  vbroadcastss ymm0, xmm0 ; ymm0 is just m
  .avxloop:
    ; dst = a + (m * b)
    ; Assume FMA3 support?
    cmp rcx, 0x8
    jl .sseloop 
    ; Move a and b
    vmovups ymm1, [rsi]
    vmovups ymm2, [rdx]

    ; FMA3 instruction
    vfmadd231ps ymm1, ymm2, ymm0
    ; Store result
    vmovups [rdi], ymm1
  
    ;advance
    add rsi, 0x20
    add rdi, 0x20
    add rdx, 0x20

    sub rcx, 0x8
    jmp .avxloop

  .sseloop:
    cmp rcx, 0x4
    jl .tail

    movups xmm1, [rsi]
    movups xmm2, [rdx]

    ; 4 Floats can use the same instruction
    vfmadd231ps xmm1, xmm2, xmm0
    vmovups [rdi], xmm1

    ;advance
    add rsi, 0x10
    add rdx, 0x10
    add rdi, 0x10

  .tail:
    test rcx, rcx
    jz .done
    movss xmm1, [rsi]
    movss xmm2, [rdx]

    vfmadd231ss xmm1, xmm2, xmm0
    movss [rdi], xmm1

    ; advance 4 bytes, 1 float
    add rsi, 0x4
    add rdx, 0x4
    add rdi, 0x4
  
    dec rcx
    jmp .tail

  .done:
    vzeroupper
    mov rax, rcx
    ret
    

