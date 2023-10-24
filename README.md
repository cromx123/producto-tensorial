# producto-tensorial

Es un algoritmo paralelo en memoria compartida para calcular el producto tensorial de dos matrices Am×k y Bk×n

Debe descargar el archivo tensproduct.c y data.txt
Para poder ejecutar el programa debe tener el archivo data.txt en el mismo repositorio que el tensproduct.c

La forma de compilación para Windows es:
 gcc tensproduct.c -o tensproduct.exe

y la ejecución:
 ./t1.exe K -P -O < data.txt

 K = numero de threads (hilos)
 P = {V: particion vertical, H: particion horizontal}
 O = {S (Silent), V (Verbose)}
 
La forma de compilación para Linux es:
 gcc tensproduct.c -o tensproduct

y la ejecución:
 ./t1 K -P -O < data.txt

 K = numero de threads (hilos)
 P = {V: particion vertical, H: particion horizontal}
 O = {S (Silent), V (Verbose)}

*De no ejecutarlo de la manera correcta mandará un mensaje de como ejecutarlo.*

*****************************************
*    Cristóbal Gallardo                 *
*                                       *
*    Producto Tensorial con Hilos       *
*                                       *
*    Última actualización: 24-10-2023   *
*****************************************
