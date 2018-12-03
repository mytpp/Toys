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
import sqlite3
import datetime
import time
import socket


import logging
logging.basicConfig(level=logging.INFO)

# handle Ctrl+C
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)


config = None # to be read from *.yaml
metaDB = None # meta database used only in tracker

def parse_config(file_name):
    # read config from *.yaml
    wd = os.path.split(os.path.realpath(__file__))[0]
    yaml_path = os.path.join(wd, file_name)
    global config 
    with open(yaml_path, 'r', encoding='utf-8') as f:
        content = f.read()
    config = yaml.load(content)
    this_ip = socket.gethostbyname(socket.getfqdn(socket.gethostname()))
    config['root'] = os.path.normpath(config['root'])
    config['ip'] = this_ip
    print('Load config successfully')
    print(config)



#---------------------------------Daemon Side----------------------------------#
def local_to_physical(localpath):
    relative_path = localpath.split(config['root'])[1]
    return '//' + config['name'] + '/' + relative_path[1:]

def load_path(path_list, cursor, localhost = True):
    global config
    for path in path_list:
        logging.info(path)
        ctime_stamp = os.path.getctime(path)
        mtime_stamp = os.path.getmtime(path)
        ctime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ctime_stamp))
        mtime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(mtime_stamp))
        cursor.execute('''
            insert into filesystem
            (phsical_path, category, ctime, mtime, size, hostip)
            values  (?, ?, ?, ?, ?, ?)
            ''', 
            (local_to_physical(path) , os.path.isfile(path), 
            ctime, mtime, os.path.getsize(path), config['ip'])
        )
        metaDB.commit()
        # recursive load path
        if(os.path.isdir(path)):
            load_path([path + '\\' + subpath for subpath in os.listdir(path)], cursor)
        

def init_db():
    global config, metaDB
    
    metaDB = sqlite3.connect('pns.sqlite3')
    cursor = metaDB.cursor()
    cursor.execute('''
    create table if not exists filesystem (
        logical_path varcahr(255),
        phsical_path varcahr(255), -- is null if logical_path is not linked
        category     int,          -- 0: file, 1: path, 2: link
        ctime        text,
        mtime        text,
        size         int,          -- zero for dir
        hostip       varchar(20)   -- where is this path
    )
    ''')
    
    # if the database is empty, 
    # this is the first time we start the tracker
    cursor.execute('select count(*) from filesystem')
    if cursor.fetchone()[0] == 0:
        now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        # insert logical root
        cursor.execute('''
            insert into filesystem
            (logical_path, category, ctime, mtime, size, hostip)
            values ('/', 0, ?, ?, 0, ?)
            ''', (now, now, config['ip'])
        )
        metaDB.commit()
        # insert physical paths in this host
        load_path([config['root']], cursor)
    
    # debug
    for row in cursor.execute('select * from filesystem'):
        print(row)
    cursor.close()


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


async def start_daemon():
    global config
    # if we're starting tracker, initiate meta database
    if not 'tracker' in config: 
        init_db()
    
    server = await asyncio.start_server(
        echo_request, '127.0.0.1', config['port'])
    addr = server.sockets[0].getsockname()
    print(f'Serving on {addr}')
    async with server:
        await server.serve_forever()

    # close database
    global metaDB
    metaDB.commit()
    metaDB.close()



#------------------------------Shell Side--------------------------------#

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


# get error code
async def get_error(reader):
    error = await reader.readline()
    err_msg = error.decode('utf-8').split(b' ')
    print(err_msg[2])
    if err_msg[1] != b'200': # error occurs
        print("Error! " + err_msg[2])
        return True


async def cp(src, dst):
    # judge if the path is in this host
    def path_in_this_host(path):
        if not path.startwith('//'): # not a physical path
            return False
        global config
        location = path.split('/', 3)[2]
        if config['ip'] == location or config['name'] == location:
            return True
        return False

    src_is_here = path_in_this_host(src)
    dst_is_here = path_in_this_host(dst)
    if src_is_here and dst_is_here:
        # use local filesystem
        pass
    elif not src_is_here and not dst_is_here:
        print("Either src or dst should be in this host!")
    else:
        reader, writer = await asyncio.open_connection(
            config['tracker'], config['port'])
        if src_is_here:
            header = make_header('ls ' + dst)
        else:
            header = make_header('ls ' + src)
        print(f'Send: {header!r}')
        writer.write(header.encode('utf-8'))
        
        if await get_error(reader):
            return
        
        # get json data
        data = await reader.read()
        response = json.loads(data)



async def ln(src, dst):
    reader, writer = await asyncio.open_connection(
            config['tracker'], config['port'])
    header = make_header('ln ' + src + ' ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))
    
    if await get_error(reader):
        return
    print('Link successfully')


async def ls(dst):
    reader, writer = await asyncio.open_connection(
            config['tracker'], config['port'])
    header = make_header('ls ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))

    if await get_error(reader):
        return
    
    # get json data
    data = await reader.read()
    response = json.loads(data)
    print(response)


async def md(dst):
    reader, writer = await asyncio.open_connection(
            config['tracker'], config['port'])
    header = make_header('md ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))
    
    if await get_error(reader):
        return
    print('Make directory successfully')


async def mv(src, dst):
    pass

async def rm(dst):
    reader, writer = await asyncio.open_connection(
            config['tracker'], config['port'])
    header = make_header('md ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))
    
    if await get_error(reader):
        return
    print('Remove successfully')


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

    if args.config:
        parse_config(args.config)
    else:
        print('No config file detected!')

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
        asyncio.run(start_daemon()) 
    else:
        parser.print_help()


if __name__ == '__main__':
    main()
