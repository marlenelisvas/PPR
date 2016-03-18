## Ejercicios Propuestos
En esta primera práctica, seguiremos los ejercicios propuestos en el [tutorial](https://lsi.ugr.es/jmantas/ppr/tutoriales/) y realizaremos las siguientes cuatro tareas, que toman como base algunas de las soluciones propuestas en el tutorial.
1. Modificar el programa solución del ejercicio 3.2 Send Receive del tutorial para que el proceso 0 difunda su identificador de proceso (0) al resto de procesos con identificadores pares, siguiendo un anillo de procesos pares, y el proceso 1 haga lo mismo con los procesos impares.
Se deben tener en cuenta soluciones con cualquier número de procesos.

2. Modificar el programa solucción del cálculo paralelo d número π (3.3 Cálculo de PI) para que los subintervalos de trabajo sean distribuidos por bloques en lugar de cíclicamente entre los procesos. Por ejemplo,  si tuvieramos  3 procesos y n = 11 (número de subintervalos).
el proceso 0 deber ía aproximar las integrales numéricas en los primeros 4 subintervalos
consecutivos (1,2,3,4), el proceso 1 calcularía las integrales en los siguientes 4 subinterva-los (5,6,7,8,) y el proceso 2 calcuría los últimoss tres (9,10,11). Se recomienda  empezara derivando automamaticamente   un método general para repartir por bloques n subintervalos entre P procesos para cualquier n entero positivo. Modificarlo también para que la aproximación  a n se  obtrenga en todos los procesos.

3. Modificar el programa solución del cálculo del producto escalar de dos vectores (4.1 Producto Escalar) para que cada proceso inicialice por su cuenta su parte correspondiente del vector B de forma local, de tal forma que no haya necesidad de inicializar todo el vector B en el proceso 0 y repartir sus bloques entre los procesos.

4. Modificar el programa solución del ejercicio 4.4 Comunicadores para que también se realice un Scatter de un vector de enteros desde el proceso 1 del comunicador global a todos los procesos impares de dicho comunicador. Los valores de este vector se escogen arbitrariamente en el proceso 0 (ojo, en el proceso con rango 0 del comunicador de rangos impares que es el proceso 1 de MPI Comm World), pero su tamaño debe ser igual al número de procesos impares en el comunicador global.El reparto asignará un elemento de dicho vector a cada proceso impar del comunicador global. Se recomienda usar el comunicador de impares para realizar esta tarea.
