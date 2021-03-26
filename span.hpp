
// Generic Span Template

template <typename ctype> class span
{
public:
	typedef typename ctype::iterator iterator;

	typedef typename ctype::pointer pointer;

	span(ctype &data) : m_b(data.begin()), m_e(data.end())
	{
	}

	span(iterator const &b, iterator const &e) : m_b(b), m_e(e)
	{
	}

	span(iterator const &b, size_t e) : m_b(b), m_e(b+e)
	{
	}

	iterator const & begin(void) const
	{
		return m_b;
	}

	iterator const & end(void) const
	{
		return m_e;
	}

	size_t size(void) const
	{
		return m_e - m_b;
	}

	bool empty(void) const
	{
		return m_b == m_e;
	}

	pointer data(void) const
	{
		return &*m_b;
	}

protected:
	iterator m_b;
	iterator m_e;
};

// Vector Span Template

template <typename dtype> class vspan : public span<vector<dtype>>
{
public:
	typedef vspan<dtype> self;
	
	typedef span<vector<dtype>> base;

	void strip_head(size_t n)
	{
		m_b += n;
	}

	void strip_tail(size_t n)
	{
		m_e -= n;
	}

	using base::base;
	using base::iterator;
	using base::pointer;
	using base::m_b;
	using base::m_e;
};

// End of File
