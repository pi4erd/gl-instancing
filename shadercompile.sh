#!/bin/zsh

for file in $1/*.vert $1/*.frag
do
    glslangValidator -G $file -o $file.spv
done
