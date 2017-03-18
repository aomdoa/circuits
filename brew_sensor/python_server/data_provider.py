#!/usr/bin/python
import json
import sqlite3
import falcon

DATABASE = "/opt/brew_sensor/temperatures.db"
SELECT_LAST = "SELECT sample_date, sensor_1 AS sensor_one, sensor_2 AS sensor_two, sensor_3 AS board, sensor_4 AS air FROM temperatures ORDER BY sample_date DESC LIMIT 1"
SELECT_TENMIN = "SELECT avg(sensor_1) AS sensor_one, avg(sensor_2) AS sensor_two, avg(sensor_3) AS board, avg(sensor_4) AS air, count(*) AS count FROM temperatures WHERE sample_date > datetime('now', '-10 minutes', 'localtime')"
SELECT_HOUR = "SELECT avg(sensor_1) AS sensor_one, avg(sensor_2) AS sensor_two, avg(sensor_3) AS board, avg(sensor_4) AS air, count(*) AS count FROM temperatures WHERE sample_date > datetime('now', '-{} hour', 'localtime') AND sample_date < datetime('now', '-{} hour', 'localtime')"

def dict_factory(cursor, row):
    d = {}
    for idx, col in enumerate(cursor.description):
        d[col[0]] = row[idx]
    return d

class BaseResource:
    def on_get(self, req, resp):
        conn = sqlite3.connect(DATABASE)
        cur = conn.cursor()
        cur.execute("SELECT * FROM temperatures ORDER BY sample_date DESC LIMIT 1000")
        results = cur.fetchall()
        conn.close()
        resp.content_type = "application/json"
        resp.body = json.dumps(results)

class StatusResource:
    def on_get(self, req, resp):
        conn = sqlite3.connect(DATABASE)
        conn.row_factory = dict_factory
        cur = conn.cursor()
        last_hours = []
        for hour in range(0, 5):
            cur.execute(SELECT_HOUR.format(hour + 1, hour))
            last_hours.append(cur.fetchone())

        cur.execute(SELECT_LAST)
        last = cur.fetchone()
        cur.execute(SELECT_TENMIN)
        resp.body = SELECT_TENMIN
        tenmin = cur.fetchone()
        conn.close()
        resp.content_type = "application/json"
        resp.body = json.dumps({"last_sample": last["sample_date"], "last": last, "tenmin": tenmin, "houravg": last_hours})


application = falcon.API()
base = BaseResource()
status = StatusResource()
application.add_route('/', base)
application.add_route('/status', status)
