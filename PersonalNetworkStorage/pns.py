#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'Personal Network Storage'

__author__ = 'mytpp'



import sys, argparse

def cp(src, dst):
    print('cp')
    pass

def ln(src, dst):
    pass

def ls(dst):
    pass

def mkdir(dst):
    pass

def mv(src, dst):
    pass

def rm(dst):
    pass 


def parse_command(cmd, *args):
    if cmd == 'cp':
        cp(*args)
    elif cmd == 'ln':
        ln(*args)
    elif cmd == 'ls':
        ls(*args)
    elif cmd == 'mkdir':
        mkdir(*args)
    elif cmd == 'mv':
        mv(*args)
    elif cmd == 'rm':
        rm(*args)



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Personal Network Storage')
    parser.add_argument('-m', '--mode', dest='mode',
                        help='mode can be shell, agent or tracker')
    parser.add_argument('-c', '--config', dest='config',
                        help='the config file (*.yml)')
    parser.add_argument('cmd', nargs='*',
                        help='(Available in shell mode) the command to be executed')

    args = parser.parse_args()

    if args.mode == 'shell':
        if not args.cmd:
            print('Please enter a command')
        else:
            parse_command(*(args.cmd))
    elif args.mode == 'agent':
        print('agent')
    elif args.mode == 'tracker':
        print('tracker')

