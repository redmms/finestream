template <typename T>  // if you know how to safely use reference type parameter here - commit it
inline void PutAny(T DATA) {
	uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
	for (int I = 0, SIZE = sizeof(DATA); I < SIZE; I++) {
		PutByte(BYTE_PTR[I]);
	}
}

template <typename T>
inline void PutAnyReversed(T DATA) {
	uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
	for (int I = sizeof(DATA) - 1; I >= 0; I--) {
		PutByte(BYTE_PTR[I]);
	}
}

template <int N> // N - int operations occur
fsm::ofinestream& operator << (const std::bitset <N>& BSLINE) {
	int LPZSIZE = CHB - BRLAST_BYTE.BITSN;  // left puzzle size
	if (N <= LPZSIZE) {
		//(BRLAST_BYTE.MoveToRight().UCBYTE <<= N) |= (char) BSLINE.to_ulong();
		//BRLAST_BYTE.BITSN += N;
		//BRLAST_BYTE.MoveToLeft();
		//if (BRLAST_BYTE.BITSN == CHB) {
		//	FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
		//	BRLAST_BYTE.Clear();
		//}
		PutByte((bitremedy)BSLINE);  // use my functions, if you want to shorten code, no more than 5% slower but harder to make mistake and shorter
	}
	else {
		std::bitset <N> MASK((2u << CHB) - 1);
		MASK <<= max(N - CHB, 0);
		if (BRLAST_BYTE.BITSN) {
			PutByte({ (BSLINE & MASK) >> (N - LPZSIZE), LPZSIZE, false });  //? N-LPZSIZE?
			MASK >>= LPZSIZE;
		}
		int BSSIZE = N - LPZSIZE % CHB;
		for (; BSSIZE > CHB1; BSSIZE -= CHB) {
			FILE_STREAM.put((char)((BSLINE & MASK) >> (BSSIZE - CHB)).to_ulong());
			MASK >>= CHB;
		}
		if (BSSIZE > 0) {
			PutByte({ BSLINE & MASK, BSSIZE, false });
		}
	}
	return *this;
}

template <typename T>
fsm::ofinestream& operator << (const T& DATA) {
	if constexpr (container <T>) {
		for (const auto& ELEMENT : DATA) {
			*this << ELEMENT;
		}
	}
	else if constexpr (is_array_v <T>) {
		for (int I = 0, SIZE = sizeof(DATA) / sizeof(DATA[0]); I < SIZE; I++) {
			*this << DATA[I];
		}
	}
	else if constexpr (container_adaptor <T>) {
		if constexpr (is_same_v < T, queue<typename T::value_type> >) {
			auto DATA_COPY{ DATA };
			while (!DATA_COPY.empty()) {
				*this << DATA_COPY.front();
				DATA_COPY.pop();
			}
		}
		else {
			auto DATA_COPY{ DATA };
			while (!DATA_COPY.empty()) {
				*this << DATA_COPY.top();
				DATA_COPY.pop();
			}
		}
	}
	else if constexpr (sizeof(T) == 1) {
		PutByte(DATA);
	}
	else if constexpr (is_arithmetic_v <T>) {
		if (IsLittleEndian())
			PutAnyReversed(DATA);
		else
			PutAny(DATA);
	}
	else {
		if (IsLittleEndian())
			PutAnyReversed(DATA);
		else
			PutAny(DATA);
		std::cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << std::endl;
	}
	return *this;
}