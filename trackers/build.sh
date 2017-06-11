#!/bin/bash

if [ -d "work" ]; then
  echo "Cleaning up old build"
  rm work -rf
fi
echo "Starting build from scratch.."

mkdir work
cd work

cmake -G "CodeBlocks - Ninja" ../
ninja
