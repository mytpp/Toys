#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'Personal Network Storage'

__author__ = 'mytpp'



import sys
import os
import argparse
import yaml
import asyncio

# handle Ctrl+C
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

# for Windows, use iocp
if sys.platform == 'win32':
    asyncio.set_event_loop(asyncio.ProactorEventLoop())


def start_daemon(conf):
    # read config from *.yaml
    workingDirectory = os.path.split(os.path.realpath(__file__))[0]
    yamlPath = os.path.join(workingDirectory, conf)
    f = open(yamlPath, 'r', encoding='utf-8')
    content = f.read()
    config = yaml.load(content)
    print(config)





def cp(src, dst):
    print('cp')
    print(src)
    print(dst)
    pass

def ln(src, dst):
    pass

def ls(dst):
    pass

def md(dst):
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
    elif cmd == 'md':
        md(*args)
    elif cmd == 'mv':
        mv(*args)
    elif cmd == 'rm':
        rm(*args)

def main():
    parser = argparse.ArgumentParser(description='Personal Network Storage')
    parser.add_argument('-m', '--mode', dest='mode',
                        help='mode can be shell or daemon')
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
    elif args.mode == 'daemon':
        start_daemon(args.config)
        print('daemon')
    else:
        parser.print_help()


if __name__ == '__main__':
    main()
