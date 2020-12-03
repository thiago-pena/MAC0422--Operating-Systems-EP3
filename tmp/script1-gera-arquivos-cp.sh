#!/bin/bash

# script

g++ -o gerador ./arquivos_para_cp/gerador.cpp

{ echo 500000 | ./gerador ;} > ./arquivos_para_cp/arquivo1MB.txt
echo arquivo 1MB criado em ./arquivos_para_cp/arquivo1MB.txt

{ echo 5000000 | ./gerador ;} > ./arquivos_para_cp/arquivo10MB.txt
echo arquivo 10MB criado em ./arquivos_para_cp/arquivo10MB.txt

{ echo 15000000 | ./gerador ;} > ./arquivos_para_cp/arquivo30MB.txt
echo arquivo 30MB criado em ./arquivos_para_cp/arquivo30MB.txt

rm gerador
