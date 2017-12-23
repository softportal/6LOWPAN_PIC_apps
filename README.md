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
    

_Depura la aplicación con la herramienta Simulator. Puedes partir de las pruebas su-_
_geridas en esta sección y probar otras opciones que vayas descubriendo. Luego, de-_
_pura la aplicación en la placa, siguiendo instrucciones similares (en este caso, ten-_
_drás que reconfigurar el proyecto para que utilice como herramienta hardware el_
_PICkit-3)._

Project_properties -> Elegimos Simulator.
Tools->options->Embedded->Debug startud[Halt at Main].

El programa se para justo antes de empezar la ejecución y podemos avanzar libremente, en el primer ejemplo
lo único que haremos será poner los bits determinados a 1 para que se encienda el led.

También se nos presenta la opción de _Halt at Reset Vector_ esto es muy util para cuando nos interesa
para el sistema justo cuando el core se resetea.

_¿Cuál es y dónde se invoca la función que inicializa los LEDs? Analiza a fondo tan-_
_to la función genérica de Contiki como la función específica de la placa con la que_

_estamos trabajando._
_Analiza la función ledstoggle hasta encontrar el código de escritura en el registro de_
_entrada/salida conectado con el LED1 y relaciónalo con el código Bare Metal de la_
_sección anterior. Si lo necesitas, puedes ayudarte del depurador, como se explica en_
_la sección siguiente._

leds_init es una funcion de contiki definida en core/dev/leds.h.
La implementación de estas funciones se suelen encontrar en el fichero leds_arch.v de 
cada plataforma. Ya que la función leds_init se dedicará a llamar a la función
específica dependiendo de la plataforma en la que nos encontremos.

core/dev/leds.h

        void leds_init(void);

core/dev/leds.c

```c
void
leds_init(void)
{
  leds_arch_init(); --> mikro-e
  leds = 0;
}
```

En el caso de mikro-e si nos vamos al fichero inicial: contiki-mikro-e-main.c que es
como su nombre indica el primer punto de ejecución de código para esta plataforma
encontramos en el main:

```c
int
main(int argc, char **argv)
{
  int32_t r = 0;

  pic32_init();
  watchdog_init();
  clock_init();
  leds_init();
  platform_init();
```

La función leds_toggle funciona de la misma manera que leds_init

        $ grep -R leds_toggle | grep leds.h
        core/dev/leds.h:void leds_toggle(unsigned char leds);

```c
void
leds_toggle(unsigned char ledv)
{
  show_leds(leds ^ ledv);
}
```

```c
show_leds(unsigned char new_leds)
{
  ...

  ...

  leds_arch_set(leds); --> mikro-e
}

```

volvemos a la funcion arch específica para nuestra plataforma:

```c
void
leds_arch_set(unsigned char leds)
{
  if(leds & LED1) {
    GPIO_SET(LED1_PORT, LED1_PIN);
  } else {
    GPIO_CLR(LED1_PORT, LED1_PIN);
  }

  if(leds & LED2) {
    GPIO_SET(LED2_PORT, LED2_PIN);
  } else {
    GPIO_CLR(LED2_PORT, LED2_PIN);
  }
}
...
...

Correspondientes macros: /cpu/pic32/lib/pic32_gpio.h
#define __GPIO_SET(port, pin)                                     \
  do {                                                            \
    LAT##port##SET = _LAT##port##_LAT##port##pin##_MASK;          \
  } while(0)

```

_¿Qué tendrías que modificar en el fichero test-motion.c para que sólo se ilumine el
LED de la derecha al detectar movimiento?_

Bastaría con intercambiar la llamada a la función leds_on(LEDS_ALL) por
leds_toggle(LED1); o leds_on(LED1);


Describe las acciones que desata, en el ejemplo de test-motion.c, un cambio en el va-
lor del puerto D0 del PIC32.



Porque los botones son un puerto de entrada, por lo tanto nos vale con leer su valor
y comprobar como esta el estado de tensión.


El fichero relay-click.c tiene un pequeño bug que ha-
ce que uno de los dos relés no funcione. Trata de encontrar y solucionar la
errata. A continuación, implementa una nueva función en ese mismo fichero
relay-click.c) que intercambie el estado del relé 1 por el del relé 2 y vice-
versa; es decir, si el relé 1 está cerrado y el 2 abierto, el 1 pasará a estar abierto
y el 2 cerrado. Prueba esta nueva función modificando el ejemplo disponible en
platform/mikro-e/apps/test-relay/test-relay.c.


La errata la encontramos en la definicion de los enumerados de los Relés
el segundo relé esta mal definido, por lo tanto no funcionará ninguna
funcionalidad ya que al llegar a los switch case siempre que queramos
preguntar por el estará undefined


Función:

```c
void change()
{

    if (GPIO_VALUE(RELAY1_PORT, RELAY1_PIN) == GPIO_VALUE(RELAY2_PORT, RELAY2_PIN))
        return;

      GPIO_INV(RELAY1_PORT, RELAY1_PIN);
      GPIO_INV(RELAY2_PORT, RELAY2_PIN);
}
```






