# ANIOT_P3 

### Alumnos

Sergio Semedi Barranco 
Lucas Segarra Fernández


## Memoria


_Analiza los ficheros xc.h y proc/p32mx470f512h.h, tratando de entender cómo_

_se realiza el acceso a los registros de Entrada/Salida. Puedes utilizar el pro-_
_pio editor de MPLAB, que permite navegar sencillamente a través de los fi-_
_cheros de tu proyecto. También puede resultarte útil analizar el código des-_
_ensambaldo por medio del depurador, opción que se explica más adelan-_
_te en este documento. Por último, te puede resultar útil la siguiente página:_

http://www.johnloomis.org/microchip/pic32/bitfields/bitfield3.html

xc.h tiene definidas las macros que definen la interfaz para el direccionamiento de memoria:

```c
#define _get_byte(addr, errp)   (*(volatile unsigned char *)(addr))
#define _get_half(addr, errp)   (*(volatile unsigned short *)(addr))
#define _get_word(addr, errp)   (*(volatile unsigned int *)(addr))
#define _get_dword(addr, errp)  (*(volatile unsigned long long *)(addr))

#define _put_byte(addr, v)      (*(volatile unsigned char *)(addr)=(v), 0)
#define _put_half(addr, v)      (*(volatile unsigned short *)(addr)=(v), 0)
#define _put_word(addr, v)      (*(volatile unsigned int *)(addr)=(v), 0)
#define _put_dword(addr, v)     (*(volatile unsigned long long *)(addr)=(v), 0)
#endif
```
proc/p32mx470f512h.h Nos proporciona las estrucruas necesarias de PIC32, en el podemos encontrar el acceso a los bits para los diferentes modulos como puede ser un Timer:


```c
extern volatile unsigned int        T1CON __attribute__((section("sfrs")));
typedef union {
  struct {
    unsigned :1;
    unsigned TCS:1;
    unsigned TSYNC:1;
    unsigned :1;
    unsigned TCKPS:2;
    unsigned :1;
    unsigned TGATE:1;
    unsigned :3;
    unsigned TWIP:1;
    unsigned TWDIS:1;
    unsigned SIDL:1;
    unsigned :1;
    unsigned ON:1;
  };
  struct {
    unsigned :4;
    unsigned TCKPS0:1;
    unsigned TCKPS1:1;
  };
  struct {
    unsigned :13;
    unsigned TSIDL:1;
    unsigned :1;
    unsigned TON:1;
  };
  struct {
    unsigned w:32;
  };
} __T1CONbits_t;
```

Esta union (no memory fragment) nos permite acceder a los diferentes campos de bits, así si por ejemplo queremos activar el timer lo que hariamos sería:
      T1CONbits.TON = 1;
    



