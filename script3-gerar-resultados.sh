#!/bin/bash

# script
rm fs
echo Sistema de arquivos vazio
echo ---------------------- FS vazio \(1 e 4\) ----------------------
filename='./input_testes/input-itens-1-4'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1a fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS vazio \(2 e 5\) ----------------------
filename='./input_testes/input-itens-2-5'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1a fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS vazio \(3 e 6\) ----------------------
filename='./input_testes/input-itens-3-6'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1a fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS vazio \(7\) ----------------------
filename='./input_testes/input-itens-7-e-8'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/7a fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS vazio \(8\) ----------------------
filename='./input_testes/input-itens-7-e-8'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/8a fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ============================================================
echo Sistema de arquivos com 10MB ocupados
echo ---------------------- FS 10MB \(1 e 4\) ----------------------
filename='./input_testes/input-itens-1-4'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1b fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 10MB \(2 e 5\) ----------------------
filename='./input_testes/input-itens-2-5'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1b fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 10MB \(3 e 6\) ----------------------
filename='./input_testes/input-itens-3-6'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1b fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 10MB \(7\) ----------------------
filename='./input_testes/input-itens-7-e-8'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/7b fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 10MB \(8\) ----------------------
filename='./input_testes/input-itens-7-e-8'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/8b fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ============================================================
echo Sistema de arquivos com 50MB ocupados
echo ---------------------- FS 50MB \(1 e 4\) ----------------------
filename='./input_testes/input-itens-1-4'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1c fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 50MB \(2 e 5\) ----------------------
filename='./input_testes/input-itens-2-5'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1c fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 50MB \(3 e 6\) ----------------------
filename='./input_testes/input-itens-3-6'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/1c fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 50MB \(7\) ----------------------
filename='./input_testes/input-itens-7-e-8'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/7c fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done

echo ---------------------- FS 50MB \(8\) ----------------------
filename='./input_testes/input-itens-7-e-8'
for i in `seq 1 30`
do
    echo "$i";
    cp ./sistemas_de_arquivos_testes/8c fs
    ./ep3 < $filename | grep "microsegundos"
    # ./ep3 < $filename
    rm fs
done
