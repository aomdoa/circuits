require 'sqlite3'
require 'rubygems'
require 'json'
require 'eventmachine'


module DataServer
  def post_init
    puts "-- connection"
    @timer = EventMachine::Timer.new 10, proc {
	close_connection
    }
  end

  def receive_data data
    insert_stmt = "INSERT INTO temperatures (sample_date, sample, sensor_1, sensor_2, sensor_3, sensor_4) VALUES (datetime('now'), ?, ?, ?, ?, ?)"
    puts "you sent #{data}"
    begin
      probe_data = JSON.parse(data)
      db = SQLite3::Database.new "temperatures.db"
      db.execute(insert_stmt, probe_data["sample"], probe_data["s"][0]["t"], probe_data["s"][1]["t"], probe_data["s"][2]["t"], probe_data["s"][3]["t"])
      db.close
      close_connection
    rescue JSON::ParserError => e
      db.close if db
    rescue Exception => e
      p e
      db.close if db
      close_connection
    end
  end
end

EventMachine::run {
  EventMachine::start_server "0.0.0.0", 2001, DataServer
  puts "started server"
}

