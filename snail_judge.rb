#!/usr/bin/env ruby

require_relative 'global_const'
require_relative 'util'
require_relative 'model/single_test'
require_relative 'model/task'

require 'yaml'
require 'socket'

module SnailJudge
  class Judge
    def initialize(port)
      @port = port
    end

    def run
      server = TCPServer.new @port
      loop do
        begin
          client = server.accept

          content = YAML::load(client.recvfrom(131072)[0])
          puzzle_id = content[0]
          language = SnailJudge::RCV_LANG_HASH[content[1]]
          code = content[2]
          mode = content[3]

          task = Task.new
          task.single_tests = []
          File.open("data/#{puzzle_id}/cfg.ini", 'r') do |f|
            lines = f.readlines.map { |x| x.strip }
            lines.each do |line|
              ct = line.split('|')
              task.single_tests << SingleTest.new({
                input_file_path: ct[0],
                output_file_path: ct[1],
                full_score: ct[2].to_f,
                time_limit: ct[3].to_i,
                memory_limit: ct[4].to_i,
                checker_path: ct[5]
              })
            end
          end

          src_file_name = "Main.#{SnailJudge::LANGUAGE_HASH[language]}"
          File.open(src_file_name, 'w') do |f|
            f.write(code)
          end

          judge_log = task.judge(src_file_name, language, mode == SnailJudge::MODE_OI)
          system('rm', src_file_name)

          if mode == SnailJudge::MODE_OI
            judge_val = [
              judge_log[:score],
              judge_log[:time],
              judge_log[:memory],
              judge_log[:test_logs],
              judge_log[:compile_info]
            ]
          else
            judge_val = [
              judge_log[:result],
              judge_log[:time],
              judge_log[:memory],
              judge_log[:compile_info]
            ]
          end
          client.puts YAML::dump(judge_val)

          sleep(0.5)
          client.close
        rescue Exception => e
          puts "Error: #{e.inspect}"
          exit(1)
        end
      end
    end
  end
end

if __FILE__ == $0
  if $*.size != 1
    puts "Usage: #{__FILE__} port"
    exit(1)
  end

  SnailJudge::Judge.new($*[0].to_i).run
end
