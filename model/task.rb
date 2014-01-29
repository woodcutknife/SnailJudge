require_relative '../global_const'
require_relative '../util'

module SnailJudge
  class Task
    attr_accessor :single_tests

    def judge(src_file_name, lang, need_all_logs)
      system(
        SnailJudge::EXECUTOR_PATH, src_file_name, lang,
        "--only_compile?=true"
      )
      judge_log = get_hash_from_file('log')
      system('rm', 'log')
      judge_log[:time] = 0
      judge_log[:memory] = 0
      judge_log[:score] = 0
      if judge_log[:result]
        judge_log[:result] = judge_log[:result][0]
      end
      if judge_log[:result] == 'Compile_Error'
        return judge_log
      end
      judge_log[:test_logs] = [] if need_all_logs
      @single_tests.each do |single_test|
        test_log = single_test.judge(src_file_name, lang)
        judge_log[:time] += test_log[:time]
        judge_log[:memory] = [judge_log[:memory], test_log[:memory]].max
        judge_log[:score] += test_log[:score]
        judge_log[:result] = test_log[:result]
        break if judge_log[:result] != 'Accepted' and not need_all_logs
        judge_log[:test_logs] << test_log if need_all_logs
      end
      system('rm', 'Main')
      judge_log
    end
  end
end
