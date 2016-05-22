require 'rake'

spec = Gem::Specification.new do |s| 
  s.name = "WireAPI"
  s.version = "0.6"
  s.author = "Jurgen Van Ham"
  s.email = "juvanham@rubyforge.org"
  s.homepage = "http://rubyforge.org/projects/rbwireapi/"
  s.platform = Gem::Platform::RUBY
  s.summary = "Ruby binding to OpenAMQ WireAPI"
  s.files = FileList['./*.c', './*.rb', './*.h'].to_a
  s.require_path = "."
  s.extensions << './extconf.rb'
  s.description = <<-EOF
    This binding allows the use of the www.OpenAMQ.org WireAPI to be used
	for communication via a www.AMQP.org compatible messaging middleware.
  EOF
end
