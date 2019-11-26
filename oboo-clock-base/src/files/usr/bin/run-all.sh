#!/bin/sh

for file in $1/*
do
  sh "$file" &
done
