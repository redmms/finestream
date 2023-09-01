#pragma once
#include "..\bitremedy\bitremedy.h"
#include <bitset>
#include <fstream>
#include <type_traits>
#include <vector>
#include <iostream>

namespace fsm {
	using uchar = unsigned char;
	constexpr int CHB1 = CHAR_BIT - 1,
				   CHB = CHAR_BIT;
	template <typename T>
	concept container = requires(T STRUCTURE) {
		begin(STRUCTURE);
		end(STRUCTURE);
	};
	template <typename T>
	concept container_adaptor = requires(T STRUCTURE) {
		STRUCTURE.pop();
	};
	inline bool IsLittleEndian() {
		return (std::endian::native == std::endian::little);
	}
	//void ToBytes(ORIGINAL_TYPE& NUMBER, ARRAY_VALUES_TYPE(&BYTES_ARRAY)[N]);
	//void ToBytes(ORIGINAL_TYPE& NUMBER, CONTAINER_TYPE& BYTES_ARRAY);
	//inline void FromBytes(T& NUMBER, CONTAINER_TYPE& BYTES);
	//inline void FromBytes(T& NUMBER, ARRAY_VALUES_TYPE(&BYTES)[N]);
	//constexpr int LeadingN(const T& NUMBER);
	//constexpr inline int NonLeadingN(const T& NUMBER);
	//std::vector <bool> NonLeadingVector(const T& NUMBER);
	//void NonLeadingVector(const T& NUMBER, std::vector <bool>& CONTAINER);
	//inline void FromVector(T& NUMBER, std::vector <bool> VECTOR);
	//constexpr auto NonLeadingBitset = std::bitset<NonLeadingN(ORIGINAL_NUMBER)>(ORIGINAL_NUMBER);
	//inline void FromBitset(T& NUMBER, std::bitset <N> BITSET);
#include "fsm_templates.hpp"


	class finestream {
	protected:
		std::fstream FILE_STREAM;
		bitremedy BRLAST_BYTE;
	public:
		finestream(std::string FILE_PATH);
	public:
		bitremedy LastByte();
		inline int ExtraZerosN();
	};


	class ofinestream : public finestream {
	public:
		ofinestream(std::string FILE);
		~ofinestream();
	public:
		void Flush();
		inline void PutByte(const uchar UCBYTE);
		inline void PutByte(const std::bitset <CHB>& BSBYTE);
		inline void PutByte(const bitremedy& BRBYTE);
		//inline void PutAny(T DATA);
		//inline void PutAnyReversed(T DATA);
	public:
		inline ofinestream& operator << (const std::bitset <CHB>& BSBYTE);
		//inline ofinestream& operator << (const std::bitset <N>& BSLINE);
		ofinestream& operator << (const bitremedy& BRBYTE);
		ofinestream& operator << (const bool BBYTE);
		//ofinestream& operator << (const T& DATA);
#include "..\ofinestream\ofinestream_templates.hpp"
	};


	class ifinestream : public finestream {
	public:
		ifinestream(std::string FILE);
	public:
		inline uchar GetByte();
		inline int GetByte(uchar& UCBYTE);
		inline int GetByte(std::bitset <CHB>& BSBYTE);
		inline int GetByte(bitremedy& BRBYTE);
		//int GetAny(T& DATA);
		//int GetAnyReversed(T& DATA);
		void Flush();
	public:
		ifinestream& operator >> (std::bitset <CHB>& BSBYTE);
		//ifinestream& operator >> (std::bitset <N>& BSLINE);
		ifinestream& operator >> (bitremedy& BRBYTE);
		ifinestream& operator >> (bool& BBYTE);
		ifinestream& operator >> (std::vector <bool>& VB);
		//ifinestream& operator >> (T& DATA);
#include "..\ifinestream\ifinestream_templates.hpp"
	};
}
