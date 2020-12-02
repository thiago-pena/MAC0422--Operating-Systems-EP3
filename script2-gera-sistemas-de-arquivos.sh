#!/bin/bash

# script

make

rm fs
./ep3 < "./input_testes/input-cria-1a-vazio"
mv fs ./sistemas_de_arquivos_testes/1a

./ep3 < "./input_testes/input-cria-1b-10MB"
mv fs ./sistemas_de_arquivos_testes/1b

./ep3 < "./input_testes/input-cria-1c-50MB"
mv fs ./sistemas_de_arquivos_testes/1c

./ep3 < "./input_testes/input-cria-7a-vazio"
mv fs ./sistemas_de_arquivos_testes/7a

./ep3 < "./input_testes/input-cria-7b-10MB"
mv fs ./sistemas_de_arquivos_testes/7b

./ep3 < "./input_testes/input-cria-7c-50MB"
mv fs ./sistemas_de_arquivos_testes/7c

./ep3 < "./input_testes/input-cria-8a-vazio"
mv fs ./sistemas_de_arquivos_testes/8a

./ep3 < "./input_testes/input-cria-8b-10MB"
mv fs ./sistemas_de_arquivos_testes/8b

./ep3 < "./input_testes/input-cria-8c-50MB"
mv fs ./sistemas_de_arquivos_testes/8c
