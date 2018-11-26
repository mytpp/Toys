#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'Personal Network Storage'

__author__ = 'mytpp'



import sys
import os
import argparse
import yaml
import json
import asyncio
import hashlib

# handle Ctrl+C
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)


config = None # to be read from *.yaml


async def echo_request(reader, writer):
    data = await reader.readuntil(b'\n\n')
    header_str = data.decode('utf-8').split('\r\n')
    header_str.pop() # remove the ending '\n'
    print(f"Received header:\n{header_str!r}")
    header = {}
    for line in header_str:
        # word_list = line.split(': ')
        # if word_list < 2
        #    continue
        key, value = line.split(': ')
        header[key] = value

    # check authorization
    global config
    sha1 = hashlib.sha1()
    sha1.update(config['secret'].encode('utf-8'))
    sha1.update(header['C'].encode('utf-8'))
    if not 'A' in header or sha1.hexdigest() != header['A']: # wrong password
        writer.write(b'401 Unauthorized\n\n')
        await writer.drain()
        return
    
    # code to handle authorized request
    # ...


async def start_daemon(conf):
    # read config from *.yaml
    wd = os.path.split(os.path.realpath(__file__))[0]
    yaml_path = os.path.join(wd, conf)
    global config 
    with open(yaml_path, 'r', encoding='utf-8') as f:
        content = f.read()
    config = yaml.load(content)
    print(type(config))
    print(config)
    
    server = await asyncio.start_server(
        echo_request, '127.0.0.1', config['port'])
    addr = server.sockets[0].getsockname()
    print(f'Serving on {addr}')
    async with server:
        await server.serve_forever()




# construct a request header
def make_header(cmd, length = 0):
    global config
    sha1 = hashlib.sha1()
    sha1.update(config['secret'].encode('utf-8'))
    sha1.update(cmd.encode('utf-8'))
    header  = 'V: ' + config['name'] + ' 1.0\r\n'
    header += 'A: ' + sha1.hexdigest() + '\r\n'
    header += 'C: ' + cmd + '\r\n'
    if length > 0:
        header += 'L: ' + str(length) + '\r\n'
    header += '\n'
    return header


async def cp(src, dst):
    print('cp')
    print(src)
    print(dst)
    pass

async def ln(src, dst):
    pass

async def ls(dst):
    reader, writer = await asyncio.open_connection(
            '127.0.0.1', config['port'])
    header = make_header('ls ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))

    # get error code
    error = await reader.readline()
    err_msg = error.decode('utf-8').split(b' ')
    print(err_msg[2])
    if err_msg[1] != b'200':
        return
    
    # get json data
    data = await reader.read()
    response = json.loads(data)
    print(response)


async def md(dst):
    pass

async def mv(src, dst):
    pass

async def rm(dst):
    pass 


def parse_command(cmd, *args):
    if cmd == 'cp':
        asyncio.run(cp(*args))
    elif cmd == 'ln':
        asyncio.run(ln(*args))
    elif cmd == 'ls':
        asyncio.run(ls(*args))
    elif cmd == 'md':
        asyncio.run(md(*args))
    elif cmd == 'mv':
        asyncio.run(mv(*args))
    elif cmd == 'rm':
        asyncio.run(mv(*args))
    else:
        print('Unknown Command')

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
        print('daemon')
        # for Windows, use iocp
        # if sys.platform == 'win32':
            # asyncio.set_event_loop(asyncio.ProactorEventLoop())
        asyncio.run(start_daemon(args.config)) 
    else:
        parser.print_help()


if __name__ == '__main__':
    main()
