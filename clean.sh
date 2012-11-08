#!/bin/bash

make clean
echo rm -rf $(find "$(dirname $0)" -name \*.pyc)
rm -rf $(find "$(dirname $0)" -name \*.pyc)
