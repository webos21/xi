_text   SEGMENT

; char _InterlockedCompareExchange8(volatile char*, char NewValue, char Expected) 
;      - RCX, RDX, R8

_InterlockedCompareExchange8  PROC

    mov    eax,r8d
    lock cmpxchg [rcx],dl
    ret

_InterlockedCompareExchange8  ENDP

_text  ENDS

       END
