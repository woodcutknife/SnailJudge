#!/usr/bin/env ruby

require_relative 'global_const'

if __FILE__ == $0
  if $*.size != 5
    puts 'Usage: auto_configurator task_id time_limit memory_limit checker_name case_score'
  else
    checker_path = "./data/#{$*[0]}/#{$*[3]}"
    begin
      input_file_list = Dir.glob("data/#{$*[0]}/*.in").sort
      output_file_list = Dir.glob("data/#{$*[0]}/*.out").sort

      problem_config = []
      (input_file_list.size).times.with_index do |idx|
        problem_config << "#{input_file_list[idx]}|#{output_file_list[idx]}|#{$*[4]}|#{$*[1]}|#{$*[2]}|#{checker_path}"
      end

      system('cp', "checkers/#{$*[3]}.out", "data/#{$*[0]}/#{$*[3]}")

      File.open("data/#{$*[0]}/cfg.ini", 'w') { |f| f.write(problem_config.join("\n")) }
    rescue Exception => e
      puts "Error: #{e.inspect}"
      exit(1)
    end
  end
end
