require 'mkmf'

$LIBS += ' -Wl,-rpath=${AMQ_HOME}/lib -lamq_wireapi -lamq_common -lsmt -licl -lipr -lasl -lapr -laprutil -lpcre'

dir_config("wireapi","#{ENV['AMQ_HOME']}/include","#{ENV['AMQ_HOME']}/lib")

unless ENV.include?('AMQ_HOME')
  raise "Missing environment variable AMQ_HOME"
end

puts ["amq_wireapi", "amq_client_connection_auth_plain", "#{ENV['AMQ_HOME']}/lib"].join(',')
unless find_library("amq_wireapi", "amq_client_connection_auth_plain", "#{ENV['AMQ_HOME']}/lib")
  raise "missing libamq_wireapi"
end


[
 "amq_client_agent.h"      ,
 "amq_client_connection.h" , 
 "amq_client_session.h"    ,
 "asl.h"                   ,
 "icl.h"                   ,
 "wireapi.h"               ,
].each do |header_file|
  unless find_header(header_file, "#{ENV['AMQ_HOME']}/include")
    raise "missing #{header_file}"
  end
end


create_makefile("WireAPI")
