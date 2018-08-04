#include <cstdio>
#include <iostream>



class Log
{
private:
	std::ostream& _out_stream;
	bool         _next_is_begin;

	const std::string _log_header;
	//using endl_type = decltype(std::endl); //This is the key: std::endl is a template function, and this is the signature of that function (For std::ostream).
	using endl_type = std::ostream&(std::ostream&); //This is the key: std::endl is a template function, and this is the signature of that function (For std::ostream).

public:
	static const std::string default_log_header;

	//Constructor: User passes a custom log header and output stream, or uses defaults.
	Log(const std::string& log_header = default_log_header, std::ostream& out_stream = std::cout) : _log_header(log_header), _out_stream(out_stream), _next_is_begin(true) {}

	//Overload for std::endl only:
	Log& operator<<(endl_type endl)
	{
		_next_is_begin = true;

		_out_stream << endl;

		return *this;
	}

	//Overload for anything else:
	template<typename T>
	Log& operator<< (const T& data)
	{
		auto now = std::chrono::system_clock::now();
		auto now_time_t = std::chrono::system_clock::to_time_t(now); //Uhhg, C APIs...
		auto now_tm = std::localtime(&now_time_t); //More uhhg, C style... 

		if (_next_is_begin)
			_out_stream << _log_header << "(" << now_tm->tm_hour << ":" << now_tm->tm_min << ":" << now_tm->tm_sec << "): " << data;
		else
			_out_stream << data;

		_next_is_begin = false;

		return *this;
	}
};

const std::string Log::default_log_header = "Log entry";