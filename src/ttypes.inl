/*
 * types.inl
 *
 * Define special types needed by TPTLib
 *
 * $Id$
 *
 */

namespace ExtendedTypes {

template<typename T>
ChrSet<T>::ChrSet (T* set)
{
	T last(0);
	while (*set)
	{
		if (*set == '-')
		{
			++set;
			if (*set != '-')	// if "--" insert a dash
				while (++last < *set)
					names.insert(last);
		}
		names.insert(last = *set);
		++set;
	}
}


template<typename T>
ChrRange<T>::operator ChrSet<T> ()
{
	T set[4];
	set[0] = lower;
	set[1] = '-';
	set[2] = upper;
	set[3] = '\0';
	ChrSet<T> s(set);
	return s;
}

} // end namespace ExtendedTypes
