#!/bin/bash

echo "esto es una prueba para el primer dispositivo" > /dev/DriverDePablo0
echo "esto es una prueba para el segundo dispositivo" > /dev/DriverDePablo1
echo "esto es una prueba para el tercer dispositivo" > /dev/DriverDePablo2

dmesg | tail -n 10