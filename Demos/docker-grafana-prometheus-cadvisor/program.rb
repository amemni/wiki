#!/usr/bin/env ruby
require "net/http"
require "uri"
require "yaml"

sleep(5)

uri = URI.parse("http://candidateexercise.s3-website-eu-west-1.amazonaws.com/exercise1.yaml")

http = Net::HTTP.new(uri.host, uri.port)
request = Net::HTTP::Get.new(uri.request_uri)

response = http.request(request)

if response.code == "200"
  result = YAML.load(response.body)

  puts 'provider "aws" {'
  puts '  region = "us-west-2"'
  puts '}'
  puts

  result["machines"].each do |key, value|
    puts "resource \"aws_key_pair\" \"deployer_#{value["name"]}\" {"
    puts "  public_key = \"#{value["sshkey"]}\""
    puts "}"
    puts 
    puts "resource \"aws_instance\" \"#{value["name"]}\" {"
    puts "  ami           = \"#{value["ami"]}\""
    puts "  instance_type = \"#{value["type"]}\""
    puts "  keyname       = \"aws_key_pair\.deployer_#{value["name"]}.name\""
    puts "}"
    puts
  end  
else
  puts "ERROR!!!"
end
