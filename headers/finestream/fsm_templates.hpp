template <typename ORIGINAL_TYPE, typename ARRAY_VALUES_TYPE, size_t N>
	requires (sizeof(ARRAY_VALUES_TYPE) == 1)
void ToBytes(ORIGINAL_TYPE& NUMBER, ARRAY_VALUES_TYPE(&BYTES_ARRAY)[N]) {
	if (N != sizeof(ORIGINAL_TYPE)) {
		std::cerr << "Error: use array of same size as original data in ToBytes()" << std::endl;
		throw std::out_of_range("Error: use array of same size as original data in ToBytes()");
	}
	ARRAY_VALUES_TYPE* BYTE_PTR = reinterpret_cast<ARRAY_VALUES_TYPE*>(&NUMBER);
	for (int I = 0, SIZE = sizeof(NUMBER); I < SIZE; I++) {
		BYTES_ARRAY[I] = BYTE_PTR[I];
	}
}

template <typename ORIGINAL_TYPE, typename CONTAINER_TYPE>
	requires fsm::container<CONTAINER_TYPE> && (sizeof(typename CONTAINER_TYPE::value_type) == 1)
void ToBytes(ORIGINAL_TYPE& NUMBER, CONTAINER_TYPE& BYTES_ARRAY) {
	if (!BYTES_ARRAY.empty()) {
		std::cerr << "Warning: in ToBytes(): you are adding NUMBER representation to "
			"nonempty container" << std::endl;
	}
	typename CONTAINER_TYPE::value_type* BYTE_PTR = reinterpret_cast<typename
		CONTAINER_TYPE::value_type*>(&NUMBER);
	for (int I = 0, SIZE = sizeof(NUMBER); I < SIZE; I++) {
		BYTES_ARRAY.emplace(BYTES_ARRAY.end(), BYTE_PTR[I]);
	}
}

template <typename T, typename CONTAINER_TYPE>
	requires fsm::container<CONTAINER_TYPE> && (sizeof(typename CONTAINER_TYPE::value_type) == 1)
inline void FromBytes(T& NUMBER, CONTAINER_TYPE& BYTES) {
	const T* NUMBERPTR = reinterpret_cast<const T*>(BYTES.data());
	NUMBER = *NUMBERPTR;
	//NUMBER = reinterpret_cast<T>(*DATA);
}

template <typename T, typename ARRAY_VALUES_TYPE, size_t N>
	requires (sizeof(ARRAY_VALUES_TYPE) == 1)
inline void FromBytes(T& NUMBER, ARRAY_VALUES_TYPE(&BYTES)[N]) {
	const T* NUMBERPTR = reinterpret_cast<const T*>(BYTES);
	NUMBER = *NUMBERPTR;
	//NUMBER = reinterpret_cast<T>(*DATA);
}

template <typename T, typename MASK_TYPE = typename std::remove_const<T>::type>
constexpr int LeadingN(const T& NUMBER) {  // return number of leading zeros in a bit representation
	int		  BITSN{ sizeof(T) * CHB }, 
			  I{ BITSN };
	MASK_TYPE MASK{ 1u << BITSN - 1 };
	for (; I && !(NUMBER & MASK); I--, MASK >>= 1) {}
	return BITSN - I;
}

template <typename T>
constexpr inline int NonLeadingN(const T& NUMBER) {
	return sizeof (T) * CHB - LeadingN(NUMBER);
}

template <typename T, typename MASK_TYPE = typename std::make_unsigned<typename std::remove_const<T>::type>::type>
std::vector <bool> NonLeadingVector(const T& NUMBER) {
	std::vector <bool> CONTAINER;
	MASK_TYPE MASK{ MASK_TYPE(1) << sizeof(T) * CHB - 1 };
	while (!(NUMBER & MASK)) {
		MASK >>= 1;
	};
	while (MASK) {
		CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
		MASK >>= 1;
	}
	return CONTAINER;
}

template <typename T, typename MASK_TYPE = typename std::remove_const<T>::type>
void NonLeadingVector(const T& NUMBER, std::vector <bool>& CONTAINER) {
	MASK_TYPE MASK{ MASK_TYPE(1) << sizeof(T) * CHB - 1 };
	while (!(NUMBER & MASK)) {
		MASK >>= 1;
	};
	while (MASK) {
		CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
		MASK >>= 1;
	}
}

template <typename T>
inline void FromVector(T& NUMBER, std::vector <bool> VECTOR) {
	NUMBER = 0;
	for (size_t BIT_IDX = VECTOR.size() - 1; BIT_IDX != SIZE_MAX; BIT_IDX--) {
		NUMBER <<= 1;
		NUMBER |= VECTOR[BIT_IDX];
	}
}

template<auto ORIGINAL_NUMBER>
constexpr auto NonLeadingBitset = bitset<NonLeadingN(ORIGINAL_NUMBER)>(ORIGINAL_NUMBER);

template <typename T, size_t N>
inline void FromBitset(T& NUMBER, std::bitset <N> BITSET) {
	NUMBER = (T)BITSET.to_ullong();
}