#include <iostream>

using namespace std;

class indentbuf : public streambuf {

public:

	indentbuf(streambuf* sbuf) : m_sbuf(sbuf), m_indent(4), m_need(true) {}

	int indent() const { return m_indent; }
	void indent() { m_indent += 4; }
	void deindent() { if (m_indent >= 4) m_indent -= 4; }

protected:

	virtual int_type overflow(int_type c) {

		if (traits_type::eq_int_type(c, traits_type::eof()))

			return m_sbuf->sputc(c);

		if (m_need)
		{
			fill_n(ostreambuf_iterator<char>(m_sbuf), m_indent, ' ');
			m_need = false;
		}

		if (traits_type::eq_int_type(m_sbuf->sputc(c), traits_type::eof()))

			return traits_type::eof();

		if (traits_type::eq_int_type(c, traits_type::to_char_type('\n')))

			m_need = true;

		return traits_type::not_eof(c);
	}

	streambuf* m_sbuf;
	int m_indent;
	bool m_need;
};


class IndentStream : public ostream {
public:
	IndentStream(ostream &os) : ib(os.rdbuf()), ostream(&ib) {};

	ostream& indent(ostream& stream) {
		ib.indent();
		return stream;
	}

	friend ostream& deindent(ostream& stream);
	//  ^^^^^^

private:
	indentbuf ib;
};

ostream& deindent(ostream& stream)
{
	IndentStream* pIndentStream = dynamic_cast<IndentStream*>(&stream);
	if (pIndentStream != nullptr)
	{
		pIndentStream->ib.deindent();
	}

	return stream;
}

int main()
{
	IndentStream is(cout);
	is << "31 hexadecimal: " << hex << 31 << endl;
	is << "31 hexadecimal: " << hex << 31 << deindent << endl;
	is << "31 hexadecimal: " << hex << 31 << endl;
	return 0;
}