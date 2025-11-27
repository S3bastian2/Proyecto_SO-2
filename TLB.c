#include <stdio.h>
#include <stdlib.h>

//defino la funcion que inserta en el TLB
void insertar_en_TLB(unsigned int *tlb, unsigned int pagina,
                     unsigned int desp, int *entradas) {

    // Si ya metimos 5 páginas se llena.
    // todavia sin reemplazo lru por facilisidad
    if (*entradas >= 5) {
        printf("TLB lleno, aún no hay reemplazo implementado.\n");
        return;
    }

    // Cada entrada ocupa 2 unsigned int:
    // [0] página
    // [1] desplazamiento
    //
    // Por ejemplo:
    // Entrada 0 → tlb + 0*2 → posiciones 0 y 1
    // Entrada 1 → tlb + 1*2 → posiciones 2 y 3
    // Entrada 2 → posiciones 4 y 5 ... y así
    unsigned int *ptr = tlb + (*entradas * 2);

    // Guardamos la página en la primera posición de la entrada
    *ptr = pagina;

    // Guardamos el desplazamiento en la segunda posición
    *(ptr + 1) = desp;

    // Ahora tenemos una entrada más guardada en el TLB
    (*entradas)++;

    printf("Entrada nueva guardada en el TLB:\n");
    printf(" - Página: %u\n", pagina);
    printf(" - Desplazamiento: %u\n", desp);
    printf(" - Guardado en dirección de memoria: %p\n", (void*)ptr);
}

//Defino la función para calcular el numero de pagina.
unsigned int calcular_num_pagina(unsigned int direccion) {
    unsigned int num_pagina = direccion / 4096;
    return num_pagina;
}

//Defino la función para calcular la dirección.
unsigned int calcular_desplazamiento(unsigned int direccion) {
    unsigned int despla = direccion % 4096;
    return despla;
}

//Se debe definir la función que me va a convertir tanto al numero de pagina como al de desplazamiento en binario.
void convertir_binario(unsigned int valor, int numero_bits) {
    for (int j = numero_bits - 1; j >= 0; j--) {
        unsigned int bits = (valor >> j) & 1;
        printf("%u", bits);
    }
}

int main() {
    printf("Hola bienvenido a nuestro traductor de direcciones.\n");
    printf("Ingresa alguna direccion de memoria:\n ");
    unsigned int direccion; //Definimos la variable que va a guardar la dirrecion que le pasamos a las funciones.
    scanf("%u", &direccion);
    unsigned int numero_pagina = calcular_num_pagina(direccion);
    printf("El numero de pagina sera: %u\n", numero_pagina);
    //printf("Tamaño de la variable desplazamiento: %zu\n", sizeof(numero_pagina)); Al ser char unicamente ocupa un byte de almacenamiento.
    unsigned int desplazamiento = calcular_desplazamiento(direccion);
    printf("El desplazamiento sera: %u\n", desplazamiento);
    //Convertimos el numero de pagina en binario. 
    convertir_binario(numero_pagina, 20);
    //Convertimos el numero del desplazamiento en binario.
    convertir_binario(desplazamiento, 12);
    unsigned int *tlb = (unsigned int *)malloc(300);
    printf("\nInicio del TLB: %p y fin del TLB: %p\n", (void *)tlb, (void *)(tlb + 299));
    free(tlb);
}
