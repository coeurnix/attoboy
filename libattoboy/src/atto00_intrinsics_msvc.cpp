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

__declspec(naked) unsigned long long __aullshr(unsigned long long value, int shift) {
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

}

#endif
