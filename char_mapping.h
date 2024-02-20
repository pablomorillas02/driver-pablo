#ifndef CHAR_MAPPING_H
#define CHAR_MAPPING_H

#include <linux/keyboard.h>
#include <linux/input.h>

char keycode_to_char(int keycode)
{
    switch (keycode)
    {
    case KEY_A:
        return 'a';
    case KEY_B:
        return 'b';
    case KEY_C:
        return 'c';
    case KEY_D:
        return 'd';
    case KEY_E:
        return 'e';
    case KEY_F:
        return 'f';
    case KEY_G:
        return 'g';
    case KEY_H:
        return 'h';
    case KEY_I:
        return 'i';
    case KEY_J:
        return 'j';
    case KEY_K:
        return 'k';
    case KEY_L:
        return 'l';
    case KEY_M:
        return 'm';
    case KEY_N:
        return 'n';
    case KEY_O:
        return 'o';
    case KEY_P:
        return 'p';
    case KEY_Q:
        return 'q';
    case KEY_R:
        return 'r';
    case KEY_S:
        return 's';
    case KEY_T:
        return 't';
    case KEY_U:
        return 'u';
    case KEY_V:
        return 'v';
    case KEY_W:
        return 'w';
    case KEY_X:
        return 'x';
    case KEY_Y:
        return 'y';
    case KEY_Z:
        return 'z';
    default:
        return ' ';
    }
}

#endif /* CHAR_MAPPING_H */