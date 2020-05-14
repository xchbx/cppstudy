#!/bin/sh

version=1.1.1
arm-hisiv600-linux-g++ Server.c -o svr

g++ Client.c -o cli
