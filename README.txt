Autores: Ana Gener Coll, Arión Sintes Sintes

Mejoras realizadas:
- Se han introducido los programas mi_cp y mi_cp_r para hacer copias y copias recursivas respectivamente
- El programa mi_rn para renombrar tanto directorios como ficheros
- Un programa mi_mv de manera que se puedan mover ficheros de un directorio a otro
- A parte de mi_rm que es para eliminar archivos, se ha añadido también mi_rm_r para borrar ficheros de forma recursiva
- Se ha añadido un parser para mejorar la lectura de los argumentos de forma que sea más dinámica y mejorar la legibilidad del código
- En mi_mkdir se ha implentado la creación de directorios recursivos con -p. mi_mkdir funciona tanto con o sin / final (como en linux)
- Todos los tipos de cache están disponibles (FIFO, LRU, LAST_RW) 
- Un programa mi_tree para ver el árbol de directorios y ficheros desde la ruta especificada
- En mi_ls se puede utilizar el argumento -l para listar los detalles de los ficheros y directorios. También se puede utilizar sobre ficheros para ver su información 

