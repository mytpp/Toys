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
    this_ip = socket.gethostbyname(socket.gethostname())
    config['root'] = config['root'].rstrip('/')
    config['port'] = str(config['port'])
    config['ip'] = this_ip
    if 'tracker' in config:
        tracker_addr = config['tracker'].split(':')
        config['tracker_ip'] = tracker_addr[0]
        config['tracker_port'] = tracker_addr[1]

    logging.info('Load config successfully')
    # print(config)


#---------------------------------Daemon Side----------------------------------#
def root_to_physical(localpath):
    return localpath.split(config['root'])[1]

def load_path(root, cursor, localhost = True):
    global config
    path_list = [root + subpath for subpath in os.listdir(root)]
    for path in path_list:
        logging.info('loading ' + path)
        ctime_stamp = os.path.getctime(path)
        mtime_stamp = os.path.getmtime(path)
        ctime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ctime_stamp))
        mtime = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(mtime_stamp))
        cursor.execute('''
            insert into filesystem
            (physical_path, category, ctime, mtime, size, host_addr, host_name)
            values  (?, ?, ?, ?, ?, ?, ?)
            ''', 
            (root_to_physical(path) , os.path.isfile(path), 
            ctime, mtime, os.path.getsize(path), 
            config['ip'] + ':' + config['port'], config['name'])
        )
        metaDB.commit()
        # recursive load path
        if(os.path.isdir(path)):
            load_path(path + '/', cursor)
        
def update_db():
    global config, metaDB
    cursor = metaDB.cursor()
    cursor.execute('delete from filesystem where host_addr = ?', 
                    (config['ip'] + ':' + config['port'],))
    # insert physical paths in this host
    load_path(config['root'] + '/', cursor)
    cursor.close()

def init_db():
    global config, metaDB
    metaDB = sqlite3.connect('pns.sqlite3')
    cursor = metaDB.cursor()
    # Both directory and file paths don't end with '/',
    # except logical root path
    cursor.execute('''
    create table if not exists filesystem (
        logical_path  varcahr,     -- all logical path is link
        physical_path varcahr,     -- is null if logical_path is not linked
        category      int,         -- 0: path, 1: file, 2: link
        ctime         varchar,
        mtime         varchar,
        size          int,         -- zero for dir
        host_addr     varchar,     -- ip and port
        host_name     varchar
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
            (logical_path, category, ctime, mtime, size)
            values ('/', 2, ?, ?, 0)
            ''', (now, now)
        )
        metaDB.commit()

    update_db()
    # debug
    for row in cursor.execute('select * from filesystem'):
        print(row)
    cursor.close()


# src is like '//137.0.0.1/local/path'
# dst is a logical path, like '/dir/a/file'
async def echo_ln(src, dst, host_name, writer):
    dst.rstrip('/')
    src = src.split('/', 3)
    addr = src[2]
    path = src[3]
    if path[1] != ':':  # if path is not like 'c:/dir/f'
        path = '/' + path
    #is_file = path.endswith('/')
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    global metaDB
    cursor = metaDB.cursor()
    cursor.execute('select * from filesystem where logical_path = ?', (dst,))
    # if we're linking to an existing logical path
    if cursor.fetchone()[0] == 0:
        cursor.execute('insert into filesystem values (?, ?, ?, ?, ?, ?, ?, ?)',
                        (dst, path, 2, now, now, 0, addr, host_name))
    else:
        cursor.execute('''
            update into filesystem 
            set physical_path = ?, mtime = ?, host_addr = ?, host_name = ?
            where logical_path = ?
            ''', (path, now, addr, host_name, dst)
        )
    cursor.execute('delete from filesystem where physical_path = ?', (path,))
    cursor.close()
    metaDB.commit()

    writer.write(b'E: 200 OK\n\n')
    await writer.drain()


# dst is a logical path, like '/dir/a/file', 
# or a physical path, like '//137.0.0.1/local/path'
#                      or  '//h2/local/path'
async def echo_ls(dst, writer):
    global metaDB
    cursor = metaDB.cursor()
    file_list = []
    
    dst.rstrip('/')
    if dst.startswith('//'): # physical path
        dst = dst.split('/', 3)
        location = dst[2]
        path = '' if len(dst) <= 3 else dst[3]
        if len(path) < 1 or path[1] != ':':  # if path is not like 'c:/dir/f'
            path = '/' + path
        if location.find('.') != -1: # if location denotes an ip
            cursor.execute('''
                select * from filesystem 
                where host_addr like ? and physical_path like ?
            ''', (location + '%%', path + '%%'))
        else: # if location denotes an name
            cursor.execute('''
                select * from filesystem 
                where host_name = ? and physical_path like ?
            ''', (location, path + '%%'))
            
        results = cursor.fetchall()
        if len(results) == 0:
            writer.write(b'E: 404 Path Not Found\n\n')
            await writer.drain()
            return
        for record in results:
            item = {
                'name' : record[1],
                'type' : 'f' if record[2] else 'd',
                'ctime': record[3],
                'mtime': record[4],
                'size' : record[5],
                'host' : record[6]  # ip
            }
            file_list.append(item)
    else: # logical path
        cursor.execute('select * from filesystem where logical_path like ?', 
                        (dst + '%%', ))
        results = cursor.fetchall()
        if len(results) == 0:
            writer.write(b'E: 404 Path Not Found\n\n')
            await writer.drain()
            return
        for record in results:
            item = {
                'name' : record[0],
                'type' : record[1], # the physical path for this logical path
                'ctime': record[3],
                'mtime': record[4],
                'size' : record[5],
                'host' : record[6]  # ip
            }
        file_list.append(item)
    cursor.close()
    metaDB.commit()
    data = b'E: 200 OK\n\n' + json.dumps(file_list).encode('utf-8')
    # logging.info(data)
    writer.write(data) # need to add 'L' field in header?
    writer.write_eof()
    await writer.drain()


# dst is logical path, like '/logical/path'
async def echo_md(dst, writer):
    global metaDB
    cursor = metaDB.cursor()

    dst.rstrip('/')
    cursor.execute('select count(*) from filesystem where logical_path = ?', 
                    (dst,))
    if cursor.fetchone()[0] == 0: 
        now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        cursor.execute('''
            insert into filesystem
            (logical_path, category, ctime, mtime, size)
            values (?, 2, ?, ?, 0)
            ''', (dst, now, now)
        )
        metaDB.commit()
        writer.write(b'E: 200 OK\n\n')
        await writer.drain()
    else: # error
        writer.write(b'E: 406 Path Already Exists\n\n')
        await writer.drain()
    cursor.close()


# dst is logical path, like '/logical/path'
async def echo_rm(dst, writer):
    global metaDB
    cursor = metaDB.cursor()

    dst.rstrip('/')
    cursor.execute('select physical_path from filesystem where logical_path = ?',
                    (dst,))
    if cursor.fetchone()[0] == None:
        cursor.execute('delete from filesystem where logical_path = ?', (dst,))
    else:
        cursor.execute('''
            update into filesystem
            set logical_path = null
            where logical_path = ?
        ''', (dst,))
    writer.write(b'E: 200 OK\n\n')
    await writer.drain()


async def echo_cp(src, dst, writer):
    pass

async def echo_mv(src, dst, writer):
    pass

async def echo_illegal_command(writer):
    logging.info('Reciive illegal command')
    writer.write(b'E: 400 Illegal Command\n\n')
    await writer.drain()

async def echo_request(reader, writer):
    data = await reader.readuntil(b'\n\n')
    header_str = data.decode('utf-8').split('\n')
    # remove the ending '' (empty string)
    header_str.pop()
    header_str.pop()
    print(f"Received header:\n{header_str!r}")

    header = {}
    for line in header_str:
        # word_list = line.split(': ')
        # if word_list < 2
        #    continue
        key, value = line.split(': ')
        header[key] = value

    # check required fields
    if not 'V' in header:
        writer.write(b'E: 400 No Version Field\n\n')
        await writer.drain()
        return
    if not 'A' in header:
        writer.write(b'E: 400 No Authorization Field\n\n')
        await writer.drain()
        return
    if not 'C' in header:
        writer.write(b'E: 400 No Command Field\n\n')
        await writer.drain()
        return

    # check authorization
    global config
    sha1 = hashlib.sha1()
    sha1.update(config['secret'].encode('utf-8'))
    sha1.update(header['C'].encode('utf-8'))
    if sha1.hexdigest() != header['A']: # wrong password
        writer.write(b'E: 401 Unauthorized\n\n')
        await writer.drain()
        return
    

    # handle authorized request
    host_name = header['V'].split(' ')[0]
    cmd = header['C'].split(' ')
    if cmd[0] == 'ln':
        if len(cmd) < 3:
            await echo_illegal_command(writer)
            return
        await echo_ln(cmd[1], cmd[2], host_name, writer)
    elif cmd[0] == 'ls':
        if len(cmd) < 2:
            await echo_illegal_command(writer)
            return
        await echo_ls(cmd[1], writer)
    elif cmd[0] == 'md':
        if len(cmd) < 2:
            await echo_illegal_command(writer)
            return
        await echo_md(cmd[1], writer)
    elif cmd[0] == 'rm':
        if len(cmd) < 2:
            await echo_illegal_command(writer)
            return
        await echo_rm(cmd[1], writer)
    elif cmd[0] == 'cp':
        if len(cmd) < 3:
            await echo_illegal_command(writer)
            return
        await echo_cp(cmd[1], cmd[2], writer)
    elif cmd[0] == 'mv':
        if len(cmd) < 3:
            await echo_illegal_command(writer)
            return
        await echo_mv(cmd[1], cmd[2], writer)
    else:
        await echo_illegal_command(writer)
        return

    logging.info('Echo client successfully')



async def start_daemon():
    global config
    # if we're starting tracker, initiate meta database
    if not 'tracker' in config: 
        init_db()
    
    server = await asyncio.start_server(
        echo_request, config['ip'], config['port'])
    addr = server.sockets[0].getsockname()
    print(f'Serving on {addr}')
    async with server:
        await server.serve_forever()

    # if this is tracker, close database
    if not 'tracker' in config:
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
    header  = 'V: ' + config['name'] + ' ' + config['port'] + '\n'
    header += 'A: ' + sha1.hexdigest() + '\n'
    header += 'C: ' + cmd + '\n'
    if length > 0:
        header += 'L: ' + str(length) + '\n'
    header += '\n'
    return header

# get error code
async def get_error(reader, writer):
    error = await reader.readuntil(b'\n\n')
    err_msg = error.decode('utf-8').strip()
    logging.info(err_msg)
    if err_msg.split(' ', 2)[1] != '200': # error occurs
        writer.close()
        return True

def local_to_physical(path):
    global config
    path = path.strip()
    return '//' + config['ip'] + '/' + path


async def cp(src, dst):
    # judge if the path is in this host
    def path_in_this_host(path):
        # os.path.exists(path) ?
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
            config['tracker_ip'], config['tracker_port'])
        if src_is_here:
            header = make_header('ls ' + dst)
        else:
            header = make_header('ls ' + src)
        print(f'Send: {header!r}')
        writer.write(header.encode('utf-8'))
        
        if await get_error(reader, writer):
            return
        
        # get json data
        data = await reader.read()
        response = json.loads(data)
        print(response)
        writer.close()


async def ln(src, dst):
    reader, writer = await asyncio.open_connection(
            config['tracker_ip'], config['tracker_port'])
    header = make_header('ln ' + local_to_physical(src) + ' ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))
    
    if await get_error(reader, writer):
        return
    print('Link successfully')
    writer.close()


async def ls(dst):
    reader, writer = await asyncio.open_connection(
            config['tracker_ip'], config['tracker_port'])
    header = make_header('ls ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))

    if await get_error(reader, writer):
        return
    
    # get json data
    data = await reader.read()
    response = json.loads(data)
    print(response)
    writer.close()


async def md(dst):
    reader, writer = await asyncio.open_connection(
            config['tracker_ip'], config['tracker_port'])
    header = make_header('md ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))
    
    if await get_error(reader, writer):
        return
    print('Make directory successfully')
    writer.close()


async def mv(src, dst):
    pass

async def rm(dst):
    reader, writer = await asyncio.open_connection(
            config['tracker_ip'], config['tracker_port'])
    header = make_header('md ' + dst)
    print(f'Send: {header!r}')
    writer.write(header.encode('utf-8'))
    
    if await get_error(reader, writer):
        return
    print('Remove successfully')
    writer.close()


def do_command(cmd, *args):
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
            do_command(*(args.cmd))
    elif args.mode == 'daemon':
        logging.info('Starting daemon...')
        # for Windows, use iocp
        # if sys.platform == 'win32':
        #     asyncio.set_event_loop(asyncio.ProactorEventLoop())
        asyncio.run(start_daemon()) 
    else:
        parser.print_help()


if __name__ == '__main__':
    main()
