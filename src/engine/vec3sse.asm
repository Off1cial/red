global SSE_FloatsAdd

; rdi, rsi, rdx, rcx

section .text
  SSE_FloatsAdd: ; please let count be a multiple of 4 or i will shit myself
  ; dst, floats0, floats1, count

  .loop:
    test rcx, rcx
    jz .finish
    movups xmm0, [rsi]
    movups xmm1, [rdx]
    addps  xmm0, xmm1
    movups [rdi],  xmm0

    ; advance
    add rsi, 0x10
    add rdx, 0x10
    add rdi, 0x10
  
    sub rcx, 0x4
  
    jmp .loop

  .finish:
    ret


   


