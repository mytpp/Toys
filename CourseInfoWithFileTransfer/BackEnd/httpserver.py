import signal
import os
import logging
import asyncio
from aiohttp import web
import aiohttp_cors

async def hello(request):
    return web.Response(text="Hello, world")

# get existing files' list
async def get_files_handler(request):
    file_names = list(filter(
        lambda filename: filename != os.path.split(os.path.realpath(__file__))[1],
        os.listdir('.')))
    file_sizes = [os.path.getsize(os.path.join('.', path)) for path in file_names]
    files_info = list(map(lambda name, size: {'name': name, 'size': size}, 
                            file_names, file_sizes))
    return web.json_response(files_info)

# upload file
async def post_file_handler(request):
    reader = await request.multipart()
    # reader.next() will `yield` the fields of your form
    field = await reader.next()
    if not field.name == 'filename':
        return web.Response(text='No file uploaded')
    filename = (await field.read(decode=True)).decode('utf-8')

    field = await reader.next()
    if not field.name == 'file':
        return web.Response(text='No file uploaded')

    # I found that aiohttp can't correctly handle uploaded file name
    # eg. "C:\tmp\test.txt" => "C:\tmptest.txt"
    # print(filename) # debug
    size = 0
    with open(os.path.join('.', filename), 'wb') as f:
        while True:
            chunk = await field.read_chunk()  # 8192 bytes by default.
            if not chunk:
                break
            size += len(chunk)
            f.write(chunk)

    return web.Response(text='{filename} sized of {size} successfully stored')

def init_func(argv):
    app = web.Application()
    app.add_routes([web.get('/', hello),
                    web.get('/files', get_files_handler),
                    web.post('/files', post_file_handler),
                    web.static('/files', '')])
    # set CORS rules, allows all origins with all CORS features
    cors = aiohttp_cors.setup(app, defaults={
            "*": aiohttp_cors.ResourceOptions(
                    allow_credentials=True,
                    expose_headers="*",
                    allow_headers="*",
                )
            })
    # allows all resources (routes)
    for route in app.router.routes():
        cors.add(route)
    # enable aiohttp's logging
    logging.basicConfig(level=logging.DEBUG)
    return app

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    web.run_app(init_func(None), port=8081)
