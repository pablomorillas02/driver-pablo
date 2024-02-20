#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> // Para memset

unsigned hash_pjw(const void *str)
{
    const char *s = str;
    unsigned int g, h = 1234567u;
    while (*s != 0)
    {
        h = (h << 4) + *s++;
        if ((g = h & (unsigned int)0xf0000000) != 0)
            h = (h ^ (g >> 24)) ^ g;
    }

    return h;
}

int main()
{
    char buf[11];
    int fd;

    memset(buf, 0, sizeof(buf));

    fd = open("/dev/DriverDePablo0", O_RDONLY);
    if (fd < 0)
    {
        perror("Error al abrir el dispositivo");
        return 1;
    }

    ssize_t bytes_read = read(fd, buf, sizeof(buf) - 1);
    if (bytes_read < 0)
    {
        perror("Error al leer del dispositivo");
        close(fd);
        return 1;
    }
    else if (bytes_read == 0)
    {
        printf("No se leyeron datos. El buffer está vacío.\n");
    }
    else
    {
        printf("Teclas presionadas: %s\n", buf);
        printf("Número generado: %u\n", hash_pjw(&buf));
    }

    close(fd);
    return 0;
}