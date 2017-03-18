require 'sinatra'
require 'json'
require 'sqlite3'

SELECT_ALL = "SELECT sample_date, sensor_1 AS sensor_one, sensor_2 AS sensor_two, sensor_3 AS board, sensor_4 AS air FROM temperatures ORDER BY sample_date DESC LIMIT 600"
SELECT_COUNT = "SELECT count(*) FROM temperatures"
SELECT_LAST = "SELECT sample_date, sensor_1 AS sensor_one, sensor_2 AS sensor_two, sensor_3 AS board, sensor_4 AS air FROM temperatures ORDER BY sample_date DESC LIMIT 1"
SELECT_TENMIN = "SELECT avg(sensor_1) AS sensor_one, avg(sensor_2) AS sensor_two, avg(sensor_3) AS board, avg(sensor_4) AS air, count(*) AS count FROM temperatures WHERE sample_date > datetime('now', '-10 minutes')"
SELECT_HOUR = "SELECT avg(sensor_1) AS sensor_one, avg(sensor_2) AS sensor_two, avg(sensor_3) AS board, avg(sensor_4) AS air, count(*) AS count FROM temperatures WHERE sample_date > datetime('now', '-%d hour') AND sample_date < datetime('now', '-%d hour')"

get '/' do
  content_type :json
  db = SQLite3::Database.open "temperatures.db"
  results = db.execute( "SELECT * FROM temperatures ORDER BY sample_date DESC LIMIT 600" )
  total = db.get_first_value( "SELECT count(*) FROM temperatures" )
  db.close
  { :results => results, :page => 0, :total => total }.to_json
end

get '/status' do
  content_type :json
  db = SQLite3::Database.open "temperatures.db"
  db.results_as_hash = true
  last_hours = Array.new
  (0..5).each do |hour|
    query = SELECT_HOUR % [ hour + 1, hour];
    result = db.get_first_row(query)
    last_hours.push(result)
  end
  last = db.get_first_row(SELECT_LAST)
  tenmin = db.get_first_row(SELECT_TENMIN)
  db.close
  { :last_sample => last["sample_date"], :last => last, :tenmin => tenmin, :houravg => last_hours }.to_json
end

