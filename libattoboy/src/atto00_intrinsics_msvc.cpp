#ifdef _MSC_VER

extern "C" {

__declspec(naked) long long __allmul(long long a, long long b) {
  __asm {
    push ebx

    mov eax, [esp+8]    ; a.lo
    mov ecx, [esp+16]   ; b.lo

    mov ebx, eax
    mul ecx             ; EDX:EAX = a.lo * b.lo

    imul ecx, [esp+12]  ; ECX = b.lo * a.hi
    imul ebx, [esp+20]  ; EBX = a.lo * b.hi
    add ecx, ebx        ; ECX = (b.lo * a.hi) + (a.lo * b.hi)

    add edx, ecx        ; EDX = high part
                        ; EAX = low part (already set)

    pop ebx
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
