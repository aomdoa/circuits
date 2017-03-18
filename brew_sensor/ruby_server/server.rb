require 'socket'
require 'sqlite3'
require 'rubygems'
require 'json'

insert_stmt = "INSERT INTO temperatures (sample_date, sample, sensor_1, sensor_2, sensor_3, sensor_4) VALUES (datetime('now'), ?, ?, ?, ?, ?)"
server = TCPServer.open(2001)
loop {
  client = server.accept
  loop {
    begin
      json_data = client.gets
      if json_data.nil?
        break
      end
      p json_data

      db = SQLite3::Database.new "temperatures.db"
      probe_data = JSON.parse(json_data)
      
      db.execute(insert_stmt, probe_data["sample"], probe_data["s"][0]["t"], probe_data["s"][1]["t"], probe_data["s"][2]["t"], probe_data["s"][3]["t"])
      db.close
      break
    rescue JSON::ParserError => e
      p e
    rescue Exception => e
      p e
      db.close if db
      break
    end
  }
  client.close
}
