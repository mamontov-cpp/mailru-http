require 'net/http'
threads = []
data = []
for curthread in 1..20
	data << [0, 0]
end
for curthread in 1..20
	threads << Thread.new(curthread - 1) do |threadid| 
		for i in 1..100
			uri = URI('http://127.0.0.1:8080/main.cpp')
			res = Net::HTTP.get_response(uri)
			if res.code == "200"
				data[threadid][0] += 1;
			else
				data[threadid][1] += 1;
			end
		end
	end
end

threads.each {|thr| thr.join }
data.each { |dt|  print dt[0].to_s + "," + dt[1].to_s + "\n"  }