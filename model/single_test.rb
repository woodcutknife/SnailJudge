require_relative '../global_const'
require_relative '../util'

module SnailJudge
  class SingleTest
    extend HashInitializer

    hash_initialize :full_score, :time_limit, :memory_limit, :input_file_path, :output_file_path, :checker_path

    def judge(src_file_name, lang)
      system(
        SnailJudge::EXECUTOR_PATH, src_file_name, lang,
        "--time_limit=#{@time_limit}",
        "--memory_limit=#{@memory_limit}",
        "--output_limit=#{SnailJudge::OUTPUT_LIMIT}",
        "--not_compile?=true",
        "--input_file=#{@input_file_path}"
      )
      judge_log = get_hash_from_file('log')
      judge_log[:result] = judge_log[:result][0]
      judge_log[:time] = judge_log[:time][0].to_i
      judge_log[:memory] = judge_log[:memory][0].to_i
      system('rm', 'log')
      if judge_log[:result] == 'No_Error'
        if system(@checker_path, @input_file_path, '_.out', @output_file_path, '.checker_log')
          judge_log[:result] = 'Accepted'
          judge_log[:score] = @full_score
        else
          judge_log[:result] = 'Wrong_Answer'
          if Dir.exist?('.part_log')
            judge_log[:score] = (File.open('.part_log', 'r').gets.to_f * @full_score).round
            system('rm', '.part_log')
          else
            judge_log[:score] = 0
          end
        end
        judge_log[:checker_log] = File.open('.checker_log', 'r').gets
        system('rm', '.checker_log')
      end
      system('rm', '_.out')
      if judge_log[:time] > @time_limit + 100
        judge_log[:result] = 'Time_Limit_Exceeded'
        judge_log[:score] = 0
      end
      judge_log
    end
  end
end
