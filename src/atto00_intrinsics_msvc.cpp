#ifdef _MSC_VER

extern "C" {

__declspec(naked) long long __allmul(long long a, long long b) {
  __asm {
    mov     eax, dword ptr [esp+4]    ; a.lo
    mov     ecx, dword ptr [esp+12]   ; b.lo
    mul     ecx                       ; EDX:EAX = a.lo * b.lo

    push    edx                       ; Save high 32 bits of lo*lo result

    mov     edx, dword ptr [esp+12]   ; a.hi (esp+8 + 4 due to push)
    imul    edx, ecx                  ; EDX = a.hi * b.lo

    mov     ecx, dword ptr [esp+20]   ; b.hi (esp+16 + 4 due to push)
    imul    ecx, dword ptr [esp+8]    ; ECX = b.hi * a.lo (esp+4 + 4 due to push)

    add     edx, ecx                  ; EDX = (a.hi * b.lo) + (b.hi * a.lo)

    pop     ecx                       ; Restore high 32 bits of lo*lo result
    add     edx, ecx                  ; Add to result

    ret
  }
}

__declspec(naked) unsigned long long __aullshr(unsigned long long value,
                                               int shift) {
  __asm {
    mov ecx, [esp+12]   ; ECX = shift count
    mov eax, [esp+4]    ; EAX = value.lo
    mov edx, [esp+8]    ; EDX = value.hi

    cmp cl, 64
    jae return_zero

    cmp cl, 32
    jae more_than_32

    shrd eax, edx, cl
    shr edx, cl
    ret

more_than_32:
    mov eax, edx
    xor edx, edx
    and cl, 31
    shr eax, cl
    ret

return_zero:
    xor eax, eax
    xor edx, edx
    ret
  }
}

__declspec(naked) long long __alldiv(long long dividend, long long divisor) {
  __asm {
    push    edi
    push    esi
    push    ebx

    ; Determine sign of the result (edi = 0 if result positive, non-zero otherwise)
    xor     edi, edi

    ; Determine sign of dividend
    mov     eax, [esp+20]   ; hi word of dividend
    or      eax, eax
    jge     L1
    inc     edi
    mov     edx, [esp+16]   ; lo word of dividend
    neg     eax
    neg     edx
    sbb     eax, 0
    mov     [esp+20], eax
    mov     [esp+16], edx

L1:
    ; Determine sign of divisor
    mov     eax, [esp+28]   ; hi word of divisor
    or      eax, eax
    jge     L2
    inc     edi
    mov     edx, [esp+24]   ; lo word of divisor
    neg     eax
    neg     edx
    sbb     eax, 0
    mov     [esp+28], eax
    mov     [esp+24], edx

L2:
    ; Check if divisor is 0
    or      eax, eax
    jnz     L3
    mov     ecx, [esp+24]
    or      ecx, ecx
    jnz     L3
    ; Divide by zero - should not happen in our context
    xor     eax, eax
    xor     edx, edx
    jmp     L9

L3:
    ; Now do the divide.  First look to see if the divisor
    ; is less or equal to the dividend.
    mov     eax, [esp+20]   ; hi word of dividend
    mov     ecx, [esp+28]   ; hi word of divisor
    cmp     eax, ecx
    ja      short L4        ; if dividend > divisor
    jb      short L7        ; if dividend < divisor
    mov     eax, [esp+16]   ; lo word of dividend
    mov     ecx, [esp+24]   ; lo word of divisor
    cmp     eax, ecx
    jae     short L4        ; if dividend >= divisor

L7:
    ; Dividend < divisor, result = 0
    xor     edx, edx
    xor     eax, eax
    jmp     short L9

L4:
    ; Do the divide
    mov     ebx, [esp+24]   ; divisor lo
    mov     ecx, [esp+28]   ; divisor hi
    mov     edx, [esp+20]   ; dividend hi
    mov     eax, [esp+16]   ; dividend lo

    ; Perform 64-bit division
    div     ecx             ; EAX = quotient lo, EDX = remainder

    ; Adjust signs if necessary
L9:
    dec     edi
    jnz     short L8
    neg     edx
    neg     eax
    sbb     edx, 0

L8:
    pop     ebx
    pop     esi
    pop     edi
    ret     16
  }
}

__declspec(naked) long long __allrem(long long dividend, long long divisor) {
  __asm {
    push    ebx
    push    edi

    ; Determine sign of dividend
    xor     edi, edi
    mov     eax, [esp+16]   ; hi word of dividend
    or      eax, eax
    jge     L1
    inc     edi
    mov     edx, [esp+12]   ; lo word of dividend
    neg     eax
    neg     edx
    sbb     eax, 0
    mov     [esp+16], eax
    mov     [esp+12], edx

L1:
    ; Determine sign of divisor
    mov     eax, [esp+24]   ; hi word of divisor
    or      eax, eax
    jge     L2
    mov     edx, [esp+20]   ; lo word of divisor
    neg     eax
    neg     edx
    sbb     eax, 0
    mov     [esp+24], eax
    mov     [esp+20], edx

L2:
    ; Check if divisor is 0
    or      eax, eax
    jnz     L3
    mov     ecx, [esp+20]
    or      ecx, ecx
    jnz     L3
    ; Divide by zero - return dividend
    mov     eax, [esp+12]
    mov     edx, [esp+16]
    jmp     L5

L3:
    ; Perform the division to get remainder
    mov     eax, [esp+12]   ; dividend lo
    mov     edx, [esp+16]   ; dividend hi
    mov     ebx, [esp+20]   ; divisor lo
    mov     ecx, [esp+24]   ; divisor hi

    ; Do 64-bit modulo
    idiv    ecx             ; EDX:EAX / ECX:EBX -> EDX = remainder hi, EAX = remainder lo

    ; Adjust sign if necessary
    dec     edi
    jnz     short L4
    neg     edx
    neg     eax
    sbb     edx, 0

L4:
    ; Result is in EDX:EAX
L5:
    pop     edi
    pop     ebx
    ret     16
  }
}
}

#endif
