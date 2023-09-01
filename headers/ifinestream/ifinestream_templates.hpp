template <typename T>
int GetAny(T& DATA) {
	uchar BYTES[sizeof(T)];
	int ERR{ 0 };
	for (int I = sizeof(T) - 1; I >= 0; I--) {
		ERR = GetByte(BYTES[I]);
	}
	const T* DATAPTR = reinterpret_cast<const T*>(BYTES);
	DATA = *DATAPTR;   // will BYTES[] memory be released after exiting this function?
	return ERR;
}

template <typename T>
int GetAnyReversed(T& DATA) {  // is it really reversed? or is normal?
	uchar BYTES[sizeof(T)];
	int ERR{ 0 };
	for (int I = 0, SIZE = sizeof(T); I < SIZE; I++) {
		ERR = GetByte(BYTES[I]);
	}
	const T* DATAPTR = reinterpret_cast<const T*>(BYTES);
	DATA = *DATAPTR;
	return ERR;
}

template <int N>
fsm::ifinestream& operator >> (std::bitset <N>& BSLINE) {
	if (N <= BRLAST_BYTE.BITSN) {
		BSLINE = (BRLAST_BYTE.MoveToRight().UCBYTE >> (BRLAST_BYTE.BITSN - N));
		BRLAST_BYTE.BITSN -= N;
		BRLAST_BYTE.ClearMargins();
	}
	else {
		(BSLINE <<= BRLAST_BYTE.BITSN) |= BRLAST_BYTE.MoveToRight().UCBYTE;
		int BSSIZE = N - BRLAST_BYTE.BITSN;
		BRLAST_BYTE.Clear(); // don't forget to clear, other functions such as GetByte() can depend on BRLAST_BYTE
		for (char INPUT_CBYTE; BSSIZE > CHB1; BSSIZE -= CHB) {
			FILE_STREAM.get(INPUT_CBYTE);
			(BSLINE <<= CHB) |= (uchar)INPUT_CBYTE;
		}
		if (BSSIZE) {
			int NEW_REMEDY_SIZE = CHB - BSSIZE;
			uchar INPUT_UCBYTE = GetByte();
			(BSLINE <<= BSSIZE) |= (INPUT_UCBYTE >> NEW_REMEDY_SIZE);
			BRLAST_BYTE = { INPUT_UCBYTE,  NEW_REMEDY_SIZE, false };  // old remedy will be erased by .ClearMargins() here
		}
	}
	return *this;
}

template <typename T>
fsm::ifinestream& operator >> (T& DATA) {
	if constexpr (container <T>) {
		for (auto& ELEMENT : DATA) {
			*this >> ELEMENT;
		}
	}
	else if constexpr (is_array_v <T>) {
		for (int I = 0, SIZE = sizeof(DATA) / sizeof(DATA[0]); I < SIZE; I++) {
			*this >> DATA[I];
		}
	}
	else if constexpr (container_adaptor <T>) {
		typename T::value_type ELEMENT;
		while (!FILE_STREAM.eof()) {
			*this >> ELEMENT;
			DATA.push(ELEMENT);
		}
	}
	else if constexpr (sizeof(T) == 1) {
		DATA = (T)GetByte();
	}
	else if constexpr (is_arithmetic_v <T>) {
		if (IsLittleEndian())
			GetAny(DATA);
		else
			GetAnyReversed(DATA);
	}
	else {
		std::cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << std::endl;
		if (IsLittleEndian())
			GetAny(DATA);
		else
			GetAnyReversed(DATA);
	}
	return *this;
}