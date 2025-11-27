#include <stdio.h>
#include <stdlib.h>
#define bits_pagina 20
#define bits_desplazamiento 12

//Aquí se define la variable contador.
unsigned int contador = 0;

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
unsigned int convertir_binario(unsigned int valor, int numero_bits) {
    for (int j = numero_bits - 1; j >= 0; j--) {
        unsigned int bits = (valor >> j) & 1;
        printf("%u", bits);
    } 
}

//defino la funcion que inserta en el TLB
void insertar_en_TLB(unsigned int *tlb, unsigned int direccion, unsigned int pagina, unsigned int desplazamiento, int *entradas) {

    // Si ya metimos 5 páginas se llena.
    // todavia sin reemplazo lru por facilisidad
    if (*entradas >= 5) {
        printf("TLB lleno, aún no hay reemplazo implementado.\n");
        return;
    }

    // Cada entrada ocupa 5 unsigned int:
    // [0] dirección
    // [1] página
    // [2] desplazamiento
    // [3] pagina binario
    // [4] desplazamiento binario
    //
    // Por ejemplo:
    // Entrada 0 → tlb + 0*5 → posiciones 0 y 4
    // Entrada 1 → tlb + 1*5 → posiciones 5 y 9
    // Entrada 2 → tlb + 2*5 -> posiciones 10 y 14... Y así sucesivamente.
    unsigned int *ptr = tlb + (*entradas * 5);

    // Guardamos la dirección en la primera posición de la entrada
    *ptr = direccion;

    // Guardamos la pagina en la segunda posición
    *(ptr + 1) = pagina;

    // Guardamos la dirección en la tercera posición
    *(ptr + 2) = desplazamiento;

    //Guardamos la variable contador la cual no va a permitir saber cual entrada es las mas antigua.
    *(ptr + 3) = contador; //Cada vez que esta se guarde en una entrada se aumenta.
    contador++;

    //Ahora hay que definir la bandera que nos dira si los valores que ingresamos son correctos.
    unsigned int bandera_validez = 0;
    if (contador > 0) {
        bandera_validez = 1;
    }

    *(ptr + 4) = bandera_validez;

    // Ahora tenemos una entrada más guardada en el TLB
    (*entradas)++;

    printf("Entrada nueva guardada en el TLB:\n");
    printf("Dirección de memoria: %d\n", *ptr);
    printf("Página: %d\n", *(ptr + 1));
    printf("Desplazamiento: %d\n", *(ptr + 2));
    //Se llama a la función para obtener el numero de la pagina en binario.
    convertir_binario(*(ptr + 1), bits_pagina);
    //se llama a la función para obtener el numero del desplazamiento en binario.
    convertir_binario(*(ptr + 2), bits_desplazamiento);
    //Quiero ver el valor del contador.
    printf("El valor del contador sera: %d\n", *(ptr + 3));
    //Quiero ver el valor la bandera.
    printf("El valor de la bandera sera: %d\n", *(ptr + 4));
    printf(" - Guardado en dirección de memoria: %p\n", (void*)ptr);
}

int main() {
    unsigned int *tlb = (unsigned int *)malloc(300);
    printf("Hola bienvenido a nuestro traductor de direcciones.\n");
    printf("Ingresa alguna direccion de memoria:\n ");
    unsigned int direccion; //Definimos la variable que va a guardar la dirrecion que le pasamos a las funciones.
    scanf("%u", &direccion);
    printf("\nInicio del TLB: %p y fin del TLB: %p\n", (void *)tlb, (void *)(tlb + 299));
    unsigned int numero_pagina = calcular_num_pagina(direccion);
    unsigned int desplazamiento = calcular_desplazamiento(direccion);
    insertar_en_TLB(tlb, direccion, numero_pagina, desplazamiento, 0);
    //Llamo a la función que va a insertar los valores en el TLB.
    //insertar_en_TLB(tlb, direccion, numero_pagina, desplazamiento, 0);
    free(tlb);
}