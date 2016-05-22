require 'WireAPI'

wireapi=WireAPI.new()
auth_data=wireapi.auth_plain("guest","guest")
connection = wireapi.connection_new("localhost", nil, auth_data, "my_client",0,10000)
session=connection.client_session_new

h=1
while connection.alive
  h+=1
  content=WireAPI::AMQ_ContentBasic.new
  content.body="Hello"
  content.message_id="my #{h}"
  session.basic_publish(content,"amq.direct","default",false,false)
  puts "alive :#{connection.alive}"
  content.destroy
end
