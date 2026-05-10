Autores: Ana Gener Coll, Arión Sintes Sintes

En esta entrega se han implementado como mejoras la cache FIFO y un programa mi_tree para ver todos los archivos y directorios de forma recursiva dentro de una ruta. Adicionalmente el funcionamiento de las rutas en mi_mkdir es similar al de linux de forma que no es necesario acabarla con /. Finalmente se ha procurado también hacer más leíbles los permisos dentro del código.

Mejoras sin implementar pensadas para la proxima entrega:
- mkdir recursivo con -p
- los otros dos tipos de cache mencionados en el nivel 9
- un parser para los argumentos de forma que sean más flexibles: por ejemplo que -l o -p se puedan posicionar donde sea en el comando sin que tengan una posición fija como argv[2]
