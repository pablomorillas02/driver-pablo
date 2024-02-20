
# Driver

Este driver ha sido desarrollado por mí para la asignatura **programación hardware** de la universidad de Jáen.

## Puesta en marcha

Una vez clonado el repositorio tendremos que usar la herramienta _make_ para crear los archivos necesarios:

```console
make
```

Una vez ejecutado esta orden, se nos crearan una serie de archivos en la carpeta. Para crear e instalar los dispositivos del driver seguiremos los siguientes pasos:

1. Instalar los dispositivos:

```console
sudo insmod driver_pablo.ko
```

2. Otorgar permisos usando el script _set_permissions.sh_:

```console
sudo ./set_permissions.sh
```

Y ya estaría todo listo para poder probar el driver. Para poder ver si todo está en orden, ejecuta la siguiente orden:

```console
ls -l /dev/DriverDePablo*
```

Deberían de aparecer 3 dispositivos con todos los permisos a excepción del de ejecución.

## Ejecución de pruebas

Vamos a ejecutar pruebas para ver las operaciones de lectura y escritura que hemos implementado para el driver. Estas operaciones no tienen un uso en concreto, son meramente experimentales.

### Lectura

Este driver lee las letras que se presionan en el teclado y las almacena en un buffer para su posterior uso en otros programas a nivel de usuario. Para poder comprobarlo de manera sencilla, tendremos que compilar el archivo _read_test.c_ usando la herramienta _gcc_:

```console
gcc read_test.c -o read_test
```

Luego ejecutamos el programa compilado:

```console
./read_test
```

Obtendremos por la consola las últimas letras presionadas además de un número aleatorio generado con dichas letras.

### Escritura

Para la escritura se ha implementado un mecanismo que computa el resultado de una sucesión matemática tras generar un número aleatorio. La sucesión matemática a ejecutar vendrá dada por el dispositivo al que se le realice la escritura.

Dado un número _n_ las sucesiones que se calculan son:

- Para el primer dispositivo: 
> 1 - 2 + 3 - 4 + ... + n si n es par
> 1 - 2 + 3 - 4 + ... - n si n es impar 

- Para el segundo dispositivo: 1 + 2 + 3 + 4 + ... + n

- Para el tercer dispositivo: 1^2  + 2^2 + 3^2 + ... + n^2 

Para poder comprobar el funcionamiento podemos realizar una operación de escritura arbitraria usado la orden _echo_:

```console
sudo echo "tonto el que lo lea" > /dev/DriverDePablo0
```

O podemos hacer uso del script _write_test.sh_:

```console
sudo ./write_test.sh
```