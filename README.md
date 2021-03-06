# ANIOT_P3 

### Alumnos

Sergio Semedi Barranco

Lucas Segarra Fernández


## Memoria


__Analiza los ficheros xc.h y proc/p32mx470f512h.h, tratando de entender cómo__
__
__se realiza el acceso a los registros de Entrada/Salida. Puedes utilizar el pro-__
__pio editor de MPLAB, que permite navegar sencillamente a través de los fi-__
__cheros de tu proyecto. También puede resultarte útil analizar el código des-__
__ensambaldo por medio del depurador, opción que se explica más adelan-__
__te en este documento. Por último, te puede resultar útil la siguiente página:__

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
    

__Depura la aplicación con la herramienta Simulator. Puedes partir de las pruebas su-__
__geridas en esta sección y probar otras opciones que vayas descubriendo. Luego, de-__
__pura la aplicación en la placa, siguiendo instrucciones similares (en este caso, ten-__
__drás que reconfigurar el proyecto para que utilice como herramienta hardware el__
__PICkit-3).__

Project_properties -> Elegimos Simulator.
Tools->options->Embedded->Debug startud[Halt at Main].

El programa se para justo antes de empezar la ejecución y podemos avanzar libremente, en el primer ejemplo
lo único que haremos será poner los bits determinados a 1 para que se encienda el led.

También se nos presenta la opción de _Halt at Reset Vector_ esto es muy util para cuando nos interesa
para el sistema justo cuando el core se resetea.

__¿Cuál es y dónde se invoca la función que inicializa los LEDs? Analiza a fondo tan-__
__to la función genérica de Contiki como la función específica de la placa con la que__
__
__estamos trabajando.__
__Analiza la función ledstoggle hasta encontrar el código de escritura en el registro de__
__entrada/salida conectado con el LED1 y relaciónalo con el código Bare Metal de la__
__sección anterior. Si lo necesitas, puedes ayudarte del depurador, como se explica en__
__la sección siguiente.__

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

```c
#define __GPIO_SET(port, pin)                                     \
  do {                                                            \
    LAT##port##SET = _LAT##port##_LAT##port##pin##_MASK;          \
  } while(0)

```

__¿Qué tendrías que modificar en el fichero test-motion.c para que sólo se ilumine el__
__LED de la derecha al detectar movimiento?__

Bastaría con intercambiar la llamada a la función leds_on(LEDS_ALL) por
leds_toggle(LED1); o leds_on(LED1);


Describe las acciones que desata, en el ejemplo de test-motion.c, un cambio en el va-
lor del puerto D0 del PIC32.



Porque los botones son un puerto de entrada, por lo tanto nos vale con leer su valor
y comprobar como esta el estado de tensión.


__El fichero relay-click.c tiene un pequeño bug que ha-__
__ce que uno de los dos relés no funcione. Trata de encontrar y solucionar la__
__errata. A continuación, implementa una nueva función en ese mismo fichero__
__relay-click.c) que intercambie el estado del relé 1 por el del relé 2 y vice-__
__versa; es decir, si el relé 1 está cerrado y el 2 abierto, el 1 pasará a estar abierto__
__y el 2 cerrado. Prueba esta nueva función modificando el ejemplo disponible en__
__platform/mikro-e/apps/test-relay/test-relay.c.__


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



__Analiza en detalle la implementación de las fun-__
__ciones motion_click_enable(MIKROBUS_1) y__
__motion_click_attach_callback(MIKROBUS_1, motion_callback).__
__Compara las funciones internas de la librería LetMeCreate con las de la práctica 1, y__
__verás importantes diferencias; por ejemplo, en el modo de acceder a los dispositivos__
__en Contiki, que ya no es a través de ficheros como en OpenWRT, sino escribiendo__
__directamente los registros de Entrada/Salida.__



Vamos a analizar la función enable:

```c
int motion_click_enable(uint8_t mikrobus_index)
{
    uint8_t gpio_pin;

    if (gpio_get_pin(mikrobus_index, TYPE_RST, &gpio_pin) < 0)
        return -1;

    if (!gpio_init(gpio_pin)
    &&  !gpio_set_direction(gpio_pin, GPIO_OUTPUT)
    &&  !gpio_set_value(gpio_pin, 1))
        return 0;

    return -1;
}
```


Observamos que lo primero que hacemos es intentar optener el pin de entrada/salida
Para ello con la inferfaz de letmecreate esta tan fin como pasarle el indice
mikrobus para poder identificarlo, por lo tanto le pasamos un puntero a la función
gpio_get_pin y si retorna 0, tendremos el valor del pin en nuestro puntero.


Una vez sacado el valor del pin llamamos a sus correspondientes funciones de iniciali-
zación.



__Estrictamente, la implementación de este ejemplo viola el estilo de planificación de__
__Contiki. ¿Por qué? Modifica el ejemplo para hacerlo acorde con dicho estilo de pla-__
__nificación.__

En el ejemplo observamos un único proceso que llama a un buble infinito  que no hace 
nada (while(1){}) esto es debido a que con la función:

        motion_click_attach_callback(MIKROBUS_1, motion_callback);

El ejemplo funcionará correctamente debido a que la función motion_callback recibira
los eventos disparados por el sensor y encendera los leds.


Este ejemplo no respeta la filosofia de contiki debido a que contiki esta diseñado
para que sean los propios procesos los que reciban los eventos y segun el dato
pasado como parameto actue de una forma u otra, el ejemplo no sigue esta filosofia
delegando todo en funciones de letmecreate.


Para seguir la filosofía de contiki haría lo siguiente:

```c
static process_event_t e;

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);

void motion_callback(uint8_t event)
{
    process_post(&main_process,e,NULL); //async to p1
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
    PROCESS_BEGIN();
    e = process_alloc_event();
    INIT_NETWORK_DEBUG();
    {
        PRINTF("=====Start=====\n");

        motion_click_enable(MIKROBUS_1);

        motion_click_attach_callback(MIKROBUS_1, motion_callback);
        while(1)
        {
            PROCESS_WAIT_EVENT(); //wait 4 events
            if (ev == e)
                leds_toggle(LED1);
        }

        motion_click_disable(MIKROBUS_1);
    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/

```




### MAP

* Ejercicio Leds + motion + button VANILLA : test-motion-button0
* Ejercicio Leds + motion + button letmecreate : test-motion-button1

