#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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
void convertir_binario(unsigned int valor, int numero_bits) {
    for (int j = numero_bits - 1; j >= 0; j--) {
        unsigned int bits = (valor >> j) & 1;
        printf("%u", bits);
    }
    printf("\n");
}

//defino la funcion que inserta en el TLB
void insertar_en_TLB(unsigned int *tlb, unsigned int direccion, unsigned int pagina, unsigned int desplazamiento, int *entradas) {

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
    *(ptr + 4) = 1;

    // Ahora tenemos una entrada más guardada en el TLB
    (*entradas)++;

    printf("Entrada nueva guardada en el TLB:\n");
    printf("Dirección de memoria: %d\n", *ptr);
    printf("Página: %d\n", *(ptr + 1));
    printf("Desplazamiento: %d\n", *(ptr + 2));
    //Se llama a la función para obtener el numero de la pagina en binario.
    printf("Pagina en binario: ");
    convertir_binario(*(ptr + 1), bits_pagina);
    //se llama a la función para obtener el numero del desplazamiento en binario.
    printf("Desplazamiento en binario: ");
    convertir_binario(*(ptr + 2), bits_desplazamiento);
    //Quiero ver el valor del contador.
    printf("El valor del contador sera: %d\n", *(ptr + 3));
    //Quiero ver el valor la bandera.
    printf("El valor de la bandera sera: %d\n", *(ptr + 4));
    printf(" - Guardado en dirección de memoria: %p\n", (void*)ptr);
}

//defino la función de busqueda.
int busqueda(unsigned int *tlb, unsigned int direccion) {
    for (int i = 0; i < 5; i++) {
        unsigned int *apuntador = tlb + (i * 5); //Aquí se posicionara en el inicio de cada entrada.
        if (*(apuntador + 4) == 1) { //Verifica si es valido.
            if (*apuntador == direccion) {
                *(apuntador + 3) = contador;
                contador++;
                return 1; //Indicando que la dirección ya esta en el tlb
            }
        }
    }
    return 0; //Indicando que la dirección no esta en el tlb.
}

void reemplazo(unsigned int *tlb, unsigned int direccion, unsigned int numero_pagina, unsigned int desplazamiento) {
    unsigned int posicion = 0;
    unsigned int contador_pequeno = 0xFFFFFFFF; //Usamos este valor para hacer que contador_pequeño sea un valor muy grande.
    for (int i = 0; i < 5; i++) {
        unsigned int *apuntador2 = tlb + (i * 5); //Esta se va a parar en cada una de las posiciones iniciales de cada entrada.
        if (*(apuntador2 + 3) < contador_pequeno) {
            contador_pequeno = *(apuntador2 + 3); //Hacemos esto para obtener al valor mas pequeño.
            posicion = i;
        }
    }

    unsigned int *entrada_reemplazada = tlb + (posicion * 5); //Obtenemos la entrada que va a ser reemplazada.
    //Debemos imprimir dicho valor indicando que fue reemplazada.
    printf("La dirección que ha sido reemplazada es la siguiente: %p\n", (void *)entrada_reemplazada);

    //Ahora debemos modificar los valores de dicha entrada.
    *(entrada_reemplazada) = direccion;
    *(entrada_reemplazada + 1) = numero_pagina;
    *(entrada_reemplazada + 2) = desplazamiento;
    *(entrada_reemplazada + 3) = contador;
    contador++;
    *(entrada_reemplazada + 4) = 1;

    printf("La dirección de memoria virtual es: %d\n", direccion);
    printf("Numero de pagina: %d\n", numero_pagina);
    printf("El desplazamiento: %d\n", desplazamiento);
    printf("Contador: %d\n", contador);
    printf("Bandera de validez: %d\n", *(entrada_reemplazada + 4));

}

int main() {
    unsigned int *tlb = (unsigned int *)malloc(25 * sizeof(unsigned int));
    //Verificamos que el tlb no haya estado vacio.
    if (tlb == NULL) {
        perror("El traducto no existe.");
    }

    //El tlb comienza lleno compleatamente de ceros.
    for (int i = 0; i < 25; i++) {
        *(tlb + i) = 0;
    }

    //Inicializamos el numero de entradas.
    int entradas = 0;
    char tecla_ingresada[100];

    printf("Hola bienvenido a nuestro traductor de direcciones.\n");

    while (true) {
        printf("Ingresa alguna direccion de memoria:");
        if (fgets(tecla_ingresada, sizeof(tecla_ingresada), stdin) == NULL) {
            break;
        }
        tecla_ingresada[strcspn(tecla_ingresada, "\n")] = 0;

        if (strlen(tecla_ingresada) == 0) {
            continue;
        }

        if ((strcmp(tecla_ingresada, "s") == 0) || (strcmp(tecla_ingresada, "S")) == 0) {
            printf("Terminando Programa....\n");
            break;
        }

        bool numerico = true;
        for (int i = 0; tecla_ingresada[i] != '\0'; i++) {
            if (!isdigit(tecla_ingresada[i])) {
                numerico = false;
                break;
            }
        }

        if (!numerico) {
            printf("Entrada no valida.\n");
            printf("Page fault\n");
            continue;
        }

        unsigned int direccion = (unsigned int)strtoul(tecla_ingresada, NULL, 10); //Definimos la variable que va a guardar la dirrecion que le pasamos a las funciones.
        //scanf("%u", &direccion);
        unsigned int numero_pagina = calcular_num_pagina(direccion);
        unsigned int desplazamiento = calcular_desplazamiento(direccion);
        //Llamo a la función que va a insertar los valores en el TLB.
        int verificador = busqueda(tlb, direccion);
        if (verificador == 1) {
            printf("TLB Hit\n");
            printf("No hay reemplazo: 0x0\n");
            //Aquí nos esta indicando que la dirección ingresada ya esta guardada en el TLB.
        } else {
            printf("TLB Miss\n");
            if (entradas < 5) { //Si esta se cumple se ingresan los valores normalmente.
                insertar_en_TLB(tlb, direccion, numero_pagina, desplazamiento, &entradas);
                printf("No hay reemplazo: 0x0.\n");
            } else {
                reemplazo(tlb, direccion, numero_pagina, desplazamiento);
            }
        }
        //Se arma la parte de la busqueda.
        printf("\nInicio del TLB: %p y fin del TLB: %p\n", (void *)tlb, (void *)(tlb + 24));
    }
    free(tlb);
}