require 'WireAPI'

wireapi=WireAPI.new()
auth_data=wireapi.auth_plain("guest","guest")
connection = wireapi.connection_new("localhost", nil, auth_data, "my_client",0,10000)
session=connection.client_session_new


session.queue_declare("my_queue",false,false, false,false)
session.queue_bind("my_queue","amq.direct","default",nil)


session.basic_consume("my_queue","my_consumer",false,true,false)
while connection.alive
  session.wait(2*1000)
  content=session.basic_arrived
  unless content.nil?
    puts "received [#{content.message_id}]  #{content.body}"
  end
 
  content.destroy  # not really needed
end
session.basic_cancel("my_consumer")

