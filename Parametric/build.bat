@echo off

tools\make\make.exe -f windows.mk clean 
tools\make\make.exe -f windows.mk package
