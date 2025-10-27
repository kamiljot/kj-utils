#pragma once
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>
#include <limits>

namespace kj::io {

	/**
	 * @brief Fast buffered input reader for competitive programming.
	 *
	 * Provides zero-allocation, branch-lean methods to read integers, strings,
	 * and raw bytes from stdin using a large static buffer. Works in C++17+.
	 *
	 * Usage:
	 * @code
	 * kj::io::FastInput in;
	 * int n; in.read(n);
	 * long long x; in.read(x);
	 * std::string s; in.read(s);
	 * @endcode
	 */
	class FastInput {
	public:
		FastInput() : data_(buf_), ptr_(buf_), end_(buf_) {}

		/**
		 * @brief Reads a signed/unsigned integer type.
		 * @tparam T integral type (e.g., int, long long, unsigned, etc.)
		 * @param out destination reference
		 * @return true if a token was read, false on EOF
		 */
		template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
		bool read(T& out) {
			if (!skip_ws_()) return false;
			bool neg = false;
			using U = std::make_unsigned_t<T>;
			U v = 0;

			if constexpr (std::is_signed<T>::value) {
				if (*ptr_ == '-') { neg = true; ++ptr_; ensure_(); }
			}
			while (ensure_() && std::isdigit(static_cast<unsigned char>(*ptr_))) {
				v = v * 10 + (*ptr_ - '0');
				++ptr_;
			}
			out = neg ? static_cast<T>(-static_cast<std::make_signed_t<U>>(v)) : static_cast<T>(v);
			return true;
		}

		/**
		 * @brief Reads a single non-whitespace token into std::string.
		 */
		bool read(std::string& s) {
			if (!skip_ws_()) return false;
			s.clear();
			while (ensure_() && !std::isspace(static_cast<unsigned char>(*ptr_))) {
				s.push_back(*ptr_++);
			}
			return true;
		}

		/**
		 * @brief Reads a character (skips whitespace by default).
		 * @param c destination character
		 * @param skip_ws if true, skip whitespace before reading
		 */
		bool read_char(char& c, bool skip_ws = true) {
			if (skip_ws) {
				if (!skip_ws_()) return false;
			}
			else if (!ensure_()) {
				return false;
			}
			c = *ptr_++;
			return true;
		}

		/**
		 * @brief Reads an array/vector of integrals.
		 */
		template <class T>
		bool read_vec(std::vector<T>& v, std::size_t n) {
			v.resize(n);
			for (std::size_t i = 0; i < n; ++i) if (!read(v[i])) return false;
			return true;
		}

	private:
		static constexpr std::size_t kBuf = 1 << 16; // 64 KiB
		char buf_[kBuf];
		char* data_;
		char* ptr_;
		char* end_;

		inline bool refill_() {
			std::size_t r = std::fread(buf_, 1, kBuf, stdin);
			data_ = buf_;
			ptr_ = buf_;
			end_ = buf_ + r;
			return r != 0;
		}

		inline bool ensure_() {
			if (ptr_ < end_) return true;
			return refill_();
		}

		inline bool skip_ws_() {
			while (true) {
				if (!ensure_()) return false;
				if (!std::isspace(static_cast<unsigned char>(*ptr_))) return true;
				++ptr_;
			}
		}
	};

	/**
	 * @brief Fast buffered output writer for competitive programming.
	 *
	 * Uses a large internal buffer and flushes on destruction or explicit flush().
	 * Supports printing integers, characters, strings and lines.
	 */
	class FastOutput {
	public:
		FastOutput() : ptr_(buf_) {}
		~FastOutput() { flush(); }

		void flush() {
			std::size_t n = static_cast<std::size_t>(ptr_ - buf_);
			if (n) std::fwrite(buf_, 1, n, stdout);
			ptr_ = buf_;
		}

		FastOutput& put_char(char c) {
			if (ptr_ == buf_ + kBuf) flush();
			*ptr_++ = c;
			return *this;
		}

		FastOutput& write_str(const char* s) {
			while (*s) put_char(*s++);
			return *this;
		}

		FastOutput& write_str(const std::string& s) {
			for (char c : s) put_char(c);
			return *this;
		}

		template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
		FastOutput& write_int(T x) {
			if (x == 0) { put_char('0'); return *this; }
			if constexpr (std::is_signed<T>::value) {
				if (x < 0) { put_char('-'); x = -x; }
			}
			char tmp[32]; int i = 0;
			while (x > 0) { tmp[i++] = char('0' + (x % 10)); x /= 10; }
			while (i--) put_char(tmp[i]);
			return *this;
		}

		FastOutput& write_line() { return put_char('\n'); }

	private:
		static constexpr std::size_t kBuf = 1 << 16; // 64 KiB
		char  buf_[kBuf];
		char* ptr_;
	};

} // namespace kj::io
