
#pragma  once

////////////////////////////////////////////////////////////////////////////////
//	
// GcLib Class Library
//
// Copyright (c) 2018 Granby Consulting LLC
//
// Placed in the public domain.
//

////////////////////////////////////////////////////////////////////////////////
//	
// Formatted String
//

class CPrintf : public string
{
public:
	// This Class

	typedef CPrintf self;

	// Base Class

	typedef string base;

	// Inherited Types

	using typename base::const_pointer;
	using typename base::pointer;
	using typename base::value_type;

	// Constructors

	CPrintf(void) noexcept : base()
	{
	}

	CPrintf(base &&That) noexcept : base(That)
	{
	}

	CPrintf(base const &That) : base(That)
	{
	}

	CPrintf(const_pointer p, ...) : base()
	{
		va_list v;

		va_start(v, p);

		VPrintf(p, v);

		va_end(v);
	}

	// Conversion

	operator char const * (void) const
	{
		return c_str();
	}

	// Formatted Output

	void Printf(const_pointer p, ...)
	{
		va_list v;

		va_start(v, p);

		VPrintf(p, v);

		va_end(v);
	}

	void VPrintf(const_pointer p, va_list v)
	{
		if( *p ) {

			size_t a = 2 * strlen(p);

			for( ;;) {

				resize(a, ' ');

				size_t const r = a - 1;

				size_t const w = vsnprintf(pointer(c_str()), r, p, v);

				if( w >= 0 && w < r ) {

					resize(w);

					break;
				}

				a <<= 1;
			}

			return;
		}

		clear();
	}
};

// End of File
