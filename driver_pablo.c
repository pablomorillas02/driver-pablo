#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/notifier.h>
#include <linux/keyboard.h>
#include <linux/input.h>

#define DRIVER_NAME "DriverDePablo"
#define DRIVER_CLASS "DriverDePabloClass"
#define NUM_DEVICES 3 // Dispositivos
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL"); // OBLIGATORIO

// BUFFER
static char key_buffer[BUFFER_SIZE];
static int head = 0;
static int tail = 0;

static inline bool is_buffer_empty(void)
{
    return head == tail;
}

static void add_to_buffer(char key)
{
    key_buffer[head] = key;
    head = (head + 1) % BUFFER_SIZE;
    // Si el buffer esta lleno
    if (head == tail)
    {
        tail = (tail + 1) % BUFFER_SIZE;
    }
}

static bool read_from_buffer(char *key)
{
    if (!is_buffer_empty())
    {
        *key = key_buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        return true;
    }
    return false;
}

// SECUENCIAS
// #1 S(n) = 1 - 2 + 3 - 4 + 5 - ... + n
int seq_1(unsigned int n)
{
    if (n % 2)
        return (n + 1) / 2;
    else
        return -(n / 2);
}

// #2 S(n) = 1 + 2 + 3 + ... + n
int seq_2(unsigned int n)
{
    return (n * (n + 1)) / 2;
}

// #3 S(n) = 1² + 2² + 3² + ... + n²
int seq_3(unsigned int n)
{
    return (n * (n + 1) * (2 * n + 1)) / 6;
}

// TECLADO
static int keyboard_notifier_fn(struct notifier_block *nb, unsigned long action, void *data)
{
    struct keyboard_notifier_param *param = data;
    unsigned int keycode = param->value;

    if (action == KBD_KEYCODE && param->down) {
        if (keycode >= KEY_Q && keycode <= KEY_P) {
            char key = keycode - KEY_Q + 'a';
            add_to_buffer(key);            
        }
        if (keycode >= KEY_A && keycode <= KEY_L) {
            char key = keycode - KEY_A + 'a';
            add_to_buffer(key);
        }
        if (keycode >= KEY_Z && keycode <= KEY_M) {
            char key = keycode - KEY_Z + 'a';
            add_to_buffer(key);
        }
    }

    return NOTIFY_OK;
}

static struct notifier_block keyboard_nb =
    {
        .notifier_call = keyboard_notifier_fn
    };

// FILE OPERATIONS
static int open_op(struct inode *inode, struct file *file)
{
    pr_info("Open! (%u)", iminor(inode));
    return 0;
}
static ssize_t read_op(struct file *file, char __user *buffer, size_t count, loff_t *f_pos)
{
    ssize_t bytes_read = 0;
    char key;

    while (count > 0 && read_from_buffer(&key))
    {
        if (copy_to_user(buffer + bytes_read, &key, 1))
        {
            return -EFAULT;
        }
        count--;
        bytes_read++;
    }

    return bytes_read;
}
static ssize_t write_op(struct file *file, const char __user *buffer, size_t count,
                        loff_t *f_pos)
{
    // Generación número aleatorio
    unsigned int n;
    get_random_bytes(&n, sizeof(n));

    // Resultado de la sucesión
    int s;

    // Elección de operación
    unsigned int n_device = iminor(file->f_inode);
    switch (n_device)
    {
    case 0: // seq_1
        s = seq_1(n);
        // Uso del número aleatorio
        pr_info("Sucesión (#1) de %u: %d", n, s);
        break;
    case 1: // seq_2
        s = seq_2(n);
        pr_info("Sucesión (#2) de %u: %d", n, s);
        break;
    case 2: // seq_3
        s = seq_3(n);
        pr_info("Sucesión (#3) de %u: %d", n, s);
        break;
    default:
        pr_info("Esto nunca debería de aparecer!");
        break;
    }

    return count;
}
static int release_op(struct inode *inode, struct file *file)
{
    return 0;
}

// DICCIONARIO
static const struct file_operations driver_pablo_fops = {
    .owner = THIS_MODULE,
    .open = open_op,
    .read = read_op,
    .write = write_op,
    .release = release_op,
};

static dev_t major_minor = -1;
static struct cdev driver_pablo_cdev[NUM_DEVICES];
static struct class *driver_pablo_class = NULL;

// Función de carga
static int __init init_driver(void)
{
    int n_device;
    dev_t id_device;

    // Teclado
    register_keyboard_notifier(&keyboard_nb);

    if (alloc_chrdev_region(&major_minor, 0, NUM_DEVICES, DRIVER_NAME) < 0)
    {
        pr_err("Major number assignment failed");
        goto error;
    }

    pr_info("%s driver assigned %d major number\n", DRIVER_NAME, MAJOR(major_minor));

    if ((driver_pablo_class = class_create(DRIVER_CLASS)) == NULL)
    {
        pr_err("Class device registering failed");
        goto error;
    }

    pr_info("/sys/class/%s class driver registered\n", DRIVER_CLASS);

    for (n_device = 0; n_device < NUM_DEVICES; n_device++)
    {
        cdev_init(&driver_pablo_cdev[n_device], &driver_pablo_fops);
        id_device = MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device);

        if (cdev_add(&driver_pablo_cdev[n_device], id_device, 1) == -1)
        {
            pr_err("Device node creation failed");
            goto error;
        }

        if (device_create(driver_pablo_class, NULL, id_device, NULL, DRIVER_NAME "%d", n_device) == NULL)
        {
            pr_err("Device node creation failed");
            goto error;
        }

        pr_info("Device node /dev/%s%d created\n", DRIVER_NAME, n_device);
    }

    pr_info("'Driver de Pablo' initialized and loaded\n");
    return 0;

error:
    if (driver_pablo_class)
        class_destroy(driver_pablo_class);

    if (major_minor != -1)
        unregister_chrdev_region(major_minor, NUM_DEVICES);

    return -1;
}

// Función de descarga
static void __exit exit_driver(void)
{
    int n_device;

    // Teclado
    unregister_keyboard_notifier(&keyboard_nb);

    for (n_device = 0; n_device < NUM_DEVICES; n_device++)
    {
        device_destroy(driver_pablo_class, MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device));
        cdev_del(&driver_pablo_cdev[n_device]);
    }

    class_destroy(driver_pablo_class);

    unregister_chrdev_region(major_minor, NUM_DEVICES);

    pr_info("'Driver de Pablo' unloaded\n");
}

module_init(init_driver);
module_exit(exit_driver);