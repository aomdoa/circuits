#!/usr/bin/python
import socket
import sys
import json
import argparse
from datetime import datetime
import os.path
import sqlite3

parser = argparse.ArgumentParser(description="Brew data listener")
parser.add_argument('--address', nargs='?', help="Address to listen on", default='0.0.0.0')
parser.add_argument('--port', nargs='?', type=int, help="Port to listen on", default=2001)
parser.add_argument('--database', nargs='?', help="Database file for storing data", default="temperatures.db")
args = parser.parse_args()

if not os.path.exists(args.database):
    print("The specified database path {} doesn't exist".format(args.database))
    sys.exit()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (args.address, args.port)
print('starting up on %s port %s' % server_address, file=sys.stderr)
sock.bind(server_address)
sock.listen(1)

while True:
    # Wait for a connection
    print('waiting for a connection', file=sys.stderr)
    connection, client_address = sock.accept()
    try:
        print('connection from %s - %s' % client_address, file=sys.stderr)
        data = connection.recv(512).decode().strip()
        try:
            print('received "%s"' % data, file=sys.stderr)
            parsed = json.loads(data)
        except:
            print('bad data, skipping', file=sys.stderr)
            continue

        insert_stmt = "INSERT INTO temperatures (sample_date, sample, sensor_1, sensor_2, sensor_3, sensor_4) VALUES ('{}', {}, {}, {}, {}, {})".format(datetime.now(), parsed['sample'], parsed['s'][0]['t'], parsed['s'][1]['t'], parsed['s'][2]['t'], parsed['s'][3]['t'])

        conn = sqlite3.connect(args.database)
        c = conn.cursor()
        c.execute(insert_stmt)
        conn.commit()
        conn.close()
    finally:
        connection.close()

