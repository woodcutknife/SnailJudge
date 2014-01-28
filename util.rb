module HashInitializer
  def hash_initialize(*fields)
    define_method(:initialize) do |h|
      h.each { |key, val| instance_variable_set("@#{key}", val) }
    end
  end
end

def get_hash_from_file(file_path)
  h = {}
  File.open(file_path, 'r') do |f|
    while true
      str = f.gets
      break unless str
      key, cnt = str.split(':').map { |x| x.strip }
      val = []
      cnt.to_i.times { val << f.gets.strip }
      h[key.downcase.to_sym] = val
    end
  end
  h
end
