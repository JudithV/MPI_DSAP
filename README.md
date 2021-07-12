# MPI_DSAP

Este repositorio contiene prácticas realizadas para la asignatura Desarrollo de Software en Arquitecturas Paralelas, cursada en la Universidad de Alicante. Los ejercicios se encuentran cada uno en su correspondiente carpeta, la cual contiene el ejercicio en sí (en un archivo *.c*, ya que C es el lenguaje utilizado en estos ejercicios) y un archivo makefile. Dicho archivo makefile está compuesto por *compile*, para compilar el archivo del ejercicio, *run* para ejecutarlo (a tener en cuenta que run tiene asociado el comando "mpirun -np 4 ejercicio", ya que elegí 4 como número arbitrario de procesadores. Modifíquese de ser necesario). Los ejercicios que contiene este repositorio son:

 - **Anillo** : Consiste en el paso de un mensaje entre procesadores, siguiendo la estructura de anillo (el mensaje es pasado siempre al siguiente procesador).
 - **Matriz vector** : Consiste en el cálculo del producto matriz vector utilizando MPI, para llevar a cabo el cálculo de manera paralela entre los procesadores.
 - **Pi** : Consiste en el cálculo del número pi utilizando MPI, para llevar a cabo el cálculo de manera paralela entre los procesadores.
 - **Producto escalar** : Consiste en el cálculo del producto escalar utilizando MPI, para llevar a cabo el cálculo de manera paralela entre los procesadores.
