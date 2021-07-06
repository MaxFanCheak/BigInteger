#define BASE 10000
#define DIG 4

#include "LN.hpp"
#include <iostream>
#include <cmath>
#include <charconv>

int checkInsignificantZeros(string_view digits, bool neg);

LN::LN(bool number) {
	cntDigits_ = 1;
	notPositive_ = false;
	digits_ = new int[1];
	digits_[0] = number;
	NaN_ = false;
}

LN::LN(long long number) {
	if (number < 0) {
		notPositive_ = true;
		number *= -1;
	}
	cntDigits_ = ceil(log10((double) number + 0.1) / DIG);
	digits_ = new int[cntDigits_];
	for (long long i = (int) (cntDigits_) - 1; i > -1; --i) {
		digits_[i] = (int) (number % BASE);
		number /= BASE;
	}
	NaN_ = false;
}

LN::LN(string_view number) {
	int i{0};
	unsigned int shift;
	if (number[0] == '-') {
		notPositive_ = true;
		++i;
		shift = (number.size() == 2 && number[1] != '0') ? 0 : checkInsignificantZeros(number, true);
	} else {
		shift = (number.size() == 1 && number[0] != '0') ? 0 : checkInsignificantZeros(number, false);
	}
	if (shift == -1) {
		cntDigits_ = 1;
		notPositive_ = false;
		digits_ = new int[cntDigits_];
		digits_[0] = 0;
		NaN_ = false;
		return;
	}
	unsigned int del;
	unsigned int k{i + shift};
	del = ((unsigned int) number.size() - k) % DIG;
	cntDigits_ = ((number.size() - k) / DIG) + (del > 0);
	digits_ = new int[cntDigits_];
	if (del) {
		from_chars(number.data() + k, number.data() + del + k, digits_[0]);
		for (int j{1}; j < cntDigits_; j++) {
			from_chars(number.data() + k + j * DIG - (DIG - del), number.data() + k + j * DIG + del,
					digits_[j]);
		}
	} else {
		for (int j{0}; j < cntDigits_; j++) {
			from_chars(number.data() + k + j * DIG, number.data() + k + (j + 1) * DIG, digits_[j]);
		}
	}
	NaN_ = false;
}

LN::LN(const char *number) : LN(string_view(number)) {}

LN::LN(const LN &copy) {
	NaN_ = copy.NaN_;
	if (NaN_) {
		return;
	}
	cntDigits_ = copy.cntDigits_;
	notPositive_ = copy.notPositive_;
	digits_ = new int[cntDigits_];
	memmove(digits_, copy.digits_, cntDigits_ * sizeof(int));
}

LN::LN(LN &&copy) noexcept {
	NaN_ = copy.NaN_;
	if (NaN_) {
		return;
	}
	notPositive_ = copy.notPositive_;
	cntDigits_ = copy.cntDigits_;
	digits_ = copy.digits_;
	copy.digits_ = nullptr;
}

LN::~LN() {
	delete[] digits_;
}

LN::operator string() const {
	if (NaN_) return "NaN";
	string number;
	number += to_string(digits_[0]);
	for (int i{1}; i < cntDigits_; ++i) {
		if (digits_[i] >= BASE / 10) {
			number += to_string(digits_[i]);
		} else {
			int del = DIG - (digits_[i] > 0 ? ceil(log10(digits_[i] + 0.1)) : 1);
			while (del-- > 0) {
				number += '0';
			}
			number += to_string(digits_[i]);
		}
	}
	if (notPositive_) number = "-" + number;
	return number;
}

LN::operator bool() const {
	if (!NaN_ && cntDigits_ == 1 && digits_[0] == 0) return false;
	return true;
}

LN &LN::operator=(const LN &copy) {
	if (this == &copy) {
		return *this;
	}
	if (copy.NaN_) {
		NaN_ = true;
		return *this;
	}
	delete[] digits_;
	notPositive_ = copy.notPositive_;
	cntDigits_ = copy.cntDigits_;
	digits_ = new int[cntDigits_];
	memmove(digits_, copy.digits_, cntDigits_ * sizeof(int));
	return *this;
}

LN &LN::operator=(LN &&copy) noexcept {
	if (copy.NaN_) {
		NaN_ = true;
		return *this;
	}
	delete[] digits_;
	notPositive_ = copy.notPositive_;
	cntDigits_ = copy.cntDigits_;
	digits_ = copy.digits_;
	copy.digits_ = nullptr;
	return *this;
}

LN LN::operator+(const LN &number) const {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return res;
	}
	return LN(*this) += number;
}

LN LN::operator-(const LN &number) const {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return res;
	}
	return LN(*this) -= number;
}

LN LN::operator*(const LN &number) const {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return res;
	}
	return LN(*this) *= number;
}

LN LN::operator/(const LN &number) const {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return res;
	}
	return LN(*this) /= number;
}

LN LN::operator%(const LN &number) const {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return res;
	}
	return LN(*this) %= number;
}

LN &LN::operator+=(const LN &number) {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return *this = res;
	}
	if (number == LN()) return *this;
	if (notPositive_ == number.notPositive_) digits_ = add(number);
	else digits_ = sub(number);
	removeZeros();
	return *this;
}

LN &LN::operator-=(const LN &number) {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return *this = res;
	}
	if (number == LN()) return *this;
	if (notPositive_ == number.notPositive_) digits_ = sub(number);
	else digits_ = add(number);
	removeZeros();
	return *this;
}

LN &LN::operator*=(const LN &number) {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return *this = res;
	}
	int *res = new int[cntDigits_ + number.cntDigits_];
	for (int i = 0; i < cntDigits_; ++i) {
		int carry = 0, j = 0;
		for (; j < number.cntDigits_; ++j) {
			int cur = res[i + j] + digits_[i] * number.digits_[j] + carry;
			res[i + j] = cur % BASE;
			carry = cur / BASE;
		}
	}
	notPositive_ ^= number.notPositive_;
	delete[] digits_;
	digits_ = res;
	cntDigits_ += number.cntDigits_;
	removeZeros();
	return *this;

}

LN &LN::operator/=(const LN &number) {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return *this = res;
	}
	if (abstractCompare(number) < 0) return *this = LN(false);
	int *res = new int[cntDigits_ - number.cntDigits_];
	memset(res, 0, (cntDigits_ - number.cntDigits_) * sizeof(int));
	size_t p = number.cntDigits_ - 1;
	notPositive_ = false;
	LN divisor;
	delete[] divisor.digits_;
	divisor.digits_ = new int[cntDigits_];
	memset(divisor.digits_, 0, cntDigits_ * sizeof(int));
	memcpy(divisor.digits_ + cntDigits_ - number.cntDigits_, number.digits_, number.cntDigits_ * sizeof(int));
	divisor.cntDigits_ = cntDigits_;
	for (size_t i = cntDigits_ - 1; i >= p; --i) {
		int temp;
		int max = 10000;
		int min = 0;
		do {
			temp = (min + max) / 2;
			if (abstractCompare(divisor * LN((long long) temp)) >= 0) min = temp;
			else max = temp;
		} while (max - min != 1);
		*this -= divisor * LN((long long) min);
		res[i - p] = min;
		if (i == p)break;
		divisor.cntDigits_--;
		memcpy(divisor.digits_ + i - p - 1, number.digits_, number.cntDigits_ * sizeof(int));
	}
	cntDigits_ -= p;
	delete[] digits_;
	digits_ = res;
	notPositive_ ^= number.notPositive_;
	cntDigits_ = cntDigits_ - number.cntDigits_;
	removeZeros();
	return *this;
}

LN &LN::operator%=(const LN &number) {
	if (NaN_ || number.NaN_) {
		LN res;
		res.NaN_ = true;
		return *this = res;
	}
	if (abstractCompare(number) < 0) return *this;
	size_t p = number.cntDigits_ - 1;
	LN divisor;
	delete[] divisor.digits_;
	divisor.digits_ = new int[cntDigits_];
	memset(divisor.digits_, 0, cntDigits_ * sizeof(int));
	memcpy(divisor.digits_ + cntDigits_ - number.cntDigits_, number.digits_, number.cntDigits_ * sizeof(int));
	divisor.cntDigits_ = cntDigits_;
	for (size_t i = cntDigits_ - 1; i >= p; --i) {
		int temp;
		int max = 10000;
		int min = 0;
		do {
			temp = (min + max) / 2;
			if (abstractCompare(divisor * LN((long long) temp)) >= 0) min = temp;
			else max = temp;
		} while (max - min != 1);
		digits_ = sub(divisor * LN((long long) min));
		if (i == p)break;
		divisor.cntDigits_--;
		memcpy(divisor.digits_ + i - p - 1, number.digits_, number.cntDigits_ * sizeof(int));
	}
	removeZeros();
	return *this;
}

LN LN::operator~() const {
	if (NaN_ || *this < LN(0ll)) {
		LN res;
		res.NaN_ = true;
		return res;
	}
	if (*this == LN(0ll)) return *this;
	LN res(1ll);
	LN right("1" + string((cntDigits_ + 1) / 2 * DIG, '0'));
	LN one(true);
	LN two(2ll);
	while (right - res != one) {
		LN temp = (right + res) / two;
		if (*this >= temp * temp) res = temp;
		else right = temp;
	}
	return res;
}

LN LN::operator-() const {
	LN a(*this);
	a.notPositive_ = !a.notPositive_;
	return a;
}

bool LN::operator>(const LN &number) const {
	if (NaN_ || number.NaN_) return true;
	return compare(number) > 0;
}

bool LN::operator==(const LN &number) const {
	if (NaN_ || number.NaN_) return true;
	return compare(number) == 0;
}

bool LN::operator<(const LN &number) const {
	if (NaN_ || number.NaN_) return true;
	return compare(number) < 0;
}

bool LN::operator>=(const LN &number) const {
	if (NaN_ || number.NaN_) return true;
	return compare(number) >= 0;
}

bool LN::operator<=(const LN &number) const {
	if (NaN_ || number.NaN_) return true;
	return compare(number) <= 0;
}

bool LN::operator!=(const LN &number) const {
	if (NaN_ || number.NaN_) return true;
	return compare(number) != 0;
}

LN::operator long long() const {
	long long res{0};
	for (int i{0}; i < cntDigits_; i++) {
		res += digits_[i] * ((long long) pow(BASE, cntDigits_ - i - 1));
	}
	return notPositive_ ? -res : res;
}

int checkInsignificantZeros(string_view digits, bool neg) {
	int i{neg};
	while (i != digits.size() - 1 && digits[i++] == '0');
	if (i == digits.size() - 1 && digits[i] == '0') {
		return -1;
	}
	if (neg) {
		return i - 2;
	}
	return i - 1;
}

int LN::compare(const LN &number) const {
	if (notPositive_ != number.notPositive_) {
		if (notPositive_) return -1;
		return 1;
	}
	if (notPositive_) return -abstractCompare(number);
	return abstractCompare(number);
}

int LN::abstractCompare(const LN &number) const {
	if (cntDigits_ > number.cntDigits_) return 1;
	if (cntDigits_ < number.cntDigits_) return -1;
	for (int i = cntDigits_ - 1; i >= 0; --i) {
		if (digits_[i] < number.digits_[i]) return -1;
		if (digits_[i] > number.digits_[i]) return 1;
	}
	return 0;
}


int *LN::add(const LN &number) {
	int p = 0;
	int size = max(cntDigits_, number.cntDigits_);
	int *res = new int[size];
	if (cntDigits_ < number.cntDigits_) {
		for (int i = 0; i < cntDigits_; ++i) {
			p += digits_[i] + number.digits_[i];
			res[i] = p % BASE;
			p /= BASE;
		}
		for (int i = cntDigits_; i < number.cntDigits_; ++i) {
			p += number.digits_[i];
			res[i] = p % BASE;
			p /= BASE;
		}
	} else {
		for (int i = 0; i < number.cntDigits_; ++i) {
			p += digits_[i] + number.digits_[i];
			res[i] = p % BASE;
			p /= BASE;
		}
		for (int i = number.cntDigits_; i < cntDigits_; ++i) {
			p += digits_[i];
			res[i] = p % BASE;
			p /= BASE;
		}
	}
	if (p) {
		int *newRes = new int[size + 1];
		newRes[size] = p;
		memmove(newRes, res, size * sizeof(int));
		delete[] res;
		res = newRes;
		size++;
	}
	cntDigits_ = size;
	delete[] digits_;
	return res;
}

int *LN::sub(const LN &number) {
	int p = 0;
	int size = max(cntDigits_, number.cntDigits_);
	int *res = new int[size];
	if (abstractCompare(number) >= 0) {
		for (int i = 0; i < number.cntDigits_; i++) {
			p = digits_[i] - number.digits_[i] - p;
			res[i] = (p + BASE) % BASE;
			p = p < 0 ? 1 : 0;
		}
		for (int i = number.cntDigits_; i < cntDigits_; ++i) {
			p = digits_[i] - p;
			res[i] = (p + BASE) % BASE;
			p = p < 0 ? 1 : 0;
		}
	} else {
		notPositive_ = !notPositive_;
		for (int i = 0; i < cntDigits_; i++) {
			p = number.digits_[i] - digits_[i] - p;
			res[i] = ((p + BASE) % BASE);
			p = p < 0 ? 1 : 0;
		}
		for (int i = cntDigits_; i < number.cntDigits_; ++i) {
			p = number.digits_[i] - p;
			res[i] = (p + BASE) % BASE;
			p = p < 0 ? 1 : 0;
		}
	}
	cntDigits_ = size;
	delete[] digits_;
	return res;
}

void LN::removeZeros() {
	int i = cntDigits_;
	if (i == 0) {
		*this = LN();
		return;
	}
	while (i > 1 && digits_[i - 1] == 0) {
		i--;
	}
	if (i == 1 && digits_[0] == 0) {
		notPositive_ = false;
	}
	if (i == cntDigits_) {
		return;
	}
	auto *res = new int[i];
	memcpy(res, digits_, i * sizeof(int));
	cntDigits_ = i;
	delete[] digits_;
	digits_ = res;
}